/*
 * htmx-csrf.c - CSRF protection implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-csrf.h"
#include <libsoup/soup.h>
#include <string.h>

struct _HtmxCsrf {
	GObject parent_instance;

	HtmxSession *session;
	gchar       *header_name;
	gchar       *form_field;
	gchar       *session_key;
};

G_DEFINE_FINAL_TYPE(HtmxCsrf, htmx_csrf, G_TYPE_OBJECT)

static void
htmx_csrf_finalize(GObject *object)
{
	HtmxCsrf *self = HTMX_CSRF(object);

	g_clear_object(&self->session);
	g_free(self->header_name);
	g_free(self->form_field);
	g_free(self->session_key);

	G_OBJECT_CLASS(htmx_csrf_parent_class)->finalize(object);
}

static void
htmx_csrf_class_init(HtmxCsrfClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);

	object_class->finalize = htmx_csrf_finalize;
}

static void
htmx_csrf_init(HtmxCsrf *self)
{
	self->session = NULL;
	self->header_name = g_strdup("X-CSRF-Token");
	self->form_field = g_strdup("_csrf");
	self->session_key = g_strdup("_csrf_token");
}

HtmxCsrf *
htmx_csrf_new(HtmxSession *session)
{
	HtmxCsrf *self;

	g_return_val_if_fail(HTMX_IS_SESSION(session), NULL);

	self = g_object_new(HTMX_TYPE_CSRF, NULL);
	self->session = g_object_ref(session);

	return self;
}

void
htmx_csrf_set_header_name(HtmxCsrf *self, const gchar *name)
{
	g_return_if_fail(HTMX_IS_CSRF(self));
	g_return_if_fail(name != NULL);

	g_free(self->header_name);
	self->header_name = g_strdup(name);
}

void
htmx_csrf_set_form_field(HtmxCsrf *self, const gchar *field)
{
	g_return_if_fail(HTMX_IS_CSRF(self));
	g_return_if_fail(field != NULL);

	g_free(self->form_field);
	self->form_field = g_strdup(field);
}

const gchar *
htmx_csrf_generate_token(HtmxCsrf *self, const gchar *session_id)
{
	g_autofree gchar *token = NULL;

	g_return_val_if_fail(HTMX_IS_CSRF(self), NULL);
	g_return_val_if_fail(session_id != NULL, NULL);

	token = g_uuid_string_random();
	htmx_session_set_value(self->session, session_id,
	                       self->session_key, token);

	return htmx_session_get_value(self->session, session_id,
	                              self->session_key);
}

gboolean
htmx_csrf_validate_token(
	HtmxCsrf   *self,
	const gchar *session_id,
	const gchar *token
){
	const gchar *expected;

	g_return_val_if_fail(HTMX_IS_CSRF(self), FALSE);

	if (session_id == NULL || token == NULL) {
		return FALSE;
	}

	expected = htmx_session_get_value(self->session, session_id,
	                                  self->session_key);
	if (expected == NULL) {
		return FALSE;
	}

	/* Constant-time comparison to prevent timing attacks */
	if (strlen(expected) != strlen(token)) {
		return FALSE;
	}

	{
		gsize i;
		gsize len = strlen(expected);
		volatile guchar result = 0;

		for (i = 0; i < len; i++) {
			result |= (guchar)expected[i] ^ (guchar)token[i];
		}

		return result == 0;
	}
}

/*
 * Check if the HTTP method is "safe" (doesn't require CSRF validation).
 */
static gboolean
is_safe_method(HtmxMethod method)
{
	return method == HTMX_METHOD_GET;
}

/*
 * Extract the submitted CSRF token from the request.
 * Checks the custom header first, then falls back to form data.
 */
static const gchar *
get_submitted_token(HtmxRequest *request, const gchar *header_name,
                    const gchar *form_field)
{
	SoupServerMessage *msg;
	SoupMessageHeaders *headers;
	const gchar *token;

	msg = htmx_request_get_message(request);
	headers = soup_server_message_get_request_headers(msg);

	/* Check header first */
	token = soup_message_headers_get_one(headers, header_name);
	if (token != NULL) {
		return token;
	}

	/* Fall back to form field */
	return htmx_request_get_form_value(request, form_field);
}

void
htmx_csrf_middleware(
	HtmxContext        *context,
	HtmxMiddlewareNext  next,
	gpointer            next_data,
	gpointer            user_data
){
	HtmxCsrf *self = HTMX_CSRF(user_data);
	HtmxRequest *request;
	HtmxMethod method;
	const gchar *session_id;
	const gchar *token;

	request = htmx_context_get_request(context);
	method = htmx_request_get_method(request);

	/* Get session ID from context (set by session middleware) */
	session_id = (const gchar *)htmx_context_get_data(context,
	                                                  "htmx_session_id");
	if (session_id == NULL) {
		g_warning("CSRF middleware requires session middleware to be "
		          "registered first");
		htmx_context_set_response(context,
			htmx_response_error("Server configuration error"));
		return;
	}

	if (is_safe_method(method)) {
		/* Generate token for safe methods */
		token = htmx_csrf_generate_token(self, session_id);
		htmx_context_set_data(context, "htmx_csrf_token",
		                      g_strdup(token), g_free);
		next(context, next_data);
		return;
	}

	/* Unsafe method — validate token */
	{
		const gchar *submitted;

		submitted = get_submitted_token(request, self->header_name,
		                                self->form_field);

		if (!htmx_csrf_validate_token(self, session_id, submitted)) {
			htmx_context_set_response(context,
				htmx_response_new_with_content(
					"<h1>403 Forbidden</h1>"
					"<p>CSRF token validation failed.</p>"));
			htmx_response_set_status(
				htmx_context_get_response(context), 403);
			return;
		}
	}

	/* Token valid — generate new token and continue */
	token = htmx_csrf_generate_token(self, session_id);
	htmx_context_set_data(context, "htmx_csrf_token",
	                      g_strdup(token), g_free);
	next(context, next_data);
}
