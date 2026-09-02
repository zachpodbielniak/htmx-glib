/*
 * htmx-csrf.c - CSRF protection implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-csrf.h"
#include "htmx-secret.h"
#include <libsoup/soup.h>

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

	/*
	 * From the kernel, not from g_uuid_string_random().
	 *
	 * A CSRF token is printed into the page, so the attacker who wants
	 * to forge one is handed a fresh sample of the generator's output
	 * every time they load it -- and g_uuid_string_random() is GLib's
	 * global GRand, a Mersenne Twister whose state a few hundred
	 * consecutive samples recover.
	 */
	token = htmx_secret_new_hex(HTMX_SECRET_DEFAULT_BYTES);
	htmx_session_set_value(self->session, session_id,
	                       self->session_key, token);

	return htmx_session_get_value(self->session, session_id,
	                              self->session_key);
}

/*
 * The token this session already has, minting one only if it has none.
 *
 * There is one token slot per session, so calling
 * htmx_csrf_generate_token() on every request is not "rotation" -- it is
 * taking the token out of whatever page the reader currently has open.
 * The middleware did exactly that on every safe request, and a page that
 * fetches a favicon, a stylesheet or an hx-get fragment through the same
 * stack invalidates its own form before anybody submits it.
 *
 * htmx_csrf_generate_token() keeps meaning "mint a new one", because
 * that is what an application wants when the identity behind a session
 * changes.  Which of the two the middleware needs is the whole bug, so
 * they are two functions.
 */
static const gchar *
csrf_ensure_token(HtmxCsrf *self, const gchar *session_id)
{
	const gchar *existing;

	existing = htmx_session_get_value(self->session, session_id,
	                                  self->session_key);
	if (existing != NULL) {
		return existing;
	}

	return htmx_csrf_generate_token(self, session_id);
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

	/*
	 * htmx_secret_equals(), which is the same constant-time comparison
	 * this function used to spell out for itself.  One copy, beside the
	 * generator, so a second caller cannot reach for g_str_equal().
	 */
	return htmx_secret_equals(expected, token);
}

/*
 * Whether this request is one of the safe methods, and so needs no token.
 *
 * Asked of the method string rather than of HtmxMethod, because that enum
 * has five members -- GET, POST, PUT, PATCH, DELETE -- and parse_method()
 * answers HTMX_METHOD_GET for every string that is not one of them.  So
 * the old `method == HTMX_METHOD_GET` was not a test for GET: HEAD and
 * OPTIONS passed it, which is right, and so did FROB, PROPFIND and
 * anything else at all, which is a fail-open in the one function whose
 * answer decides whether a credential is required.  Reading the enum
 * cannot be fixed by listing more of its members; the value does not
 * carry the answer.
 *
 * The four names are the safe methods of RFC 9110 section 9.2.1.  A
 * request built by htmx_request_new_for_path() has no message to ask, so
 * it falls back to the enum -- for a synthetic request the caller chose
 * that value directly and it means what they said.
 */
static gboolean
request_is_safe(HtmxRequest *request)
{
	SoupServerMessage *message;
	const gchar *method;

	message = htmx_request_get_message(request);
	if (message == NULL) {
		return htmx_request_get_method(request) == HTMX_METHOD_GET;
	}

	method = soup_server_message_get_method(message);
	if (method == NULL) {
		return FALSE;
	}

	return g_strcmp0(method, "GET") == 0 ||
	       g_strcmp0(method, "HEAD") == 0 ||
	       g_strcmp0(method, "OPTIONS") == 0 ||
	       g_strcmp0(method, "TRACE") == 0;
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
	const gchar *session_id;
	const gchar *token;

	request = htmx_context_get_request(context);

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

	if (request_is_safe(request)) {
		/*
		 * Ensured, not generated.  Generating here replaced the token
		 * in every page the reader already had open, so a second GET
		 * -- a favicon, an hx-get fragment, a second tab -- turned
		 * the next submit into a 403 that no part of the exchange
		 * looks responsible for.
		 */
		token = csrf_ensure_token(self, session_id);
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

	/*
	 * Token valid.  It stays as it is: rotating it here breaks a second
	 * open form for the same reason generating one on a safe request
	 * broke the first, and with one slot per session there is nowhere
	 * for the old one to live while a page still holds it.
	 */
	token = csrf_ensure_token(self, session_id);
	htmx_context_set_data(context, "htmx_csrf_token",
	                      g_strdup(token), g_free);
	next(context, next_data);
}
