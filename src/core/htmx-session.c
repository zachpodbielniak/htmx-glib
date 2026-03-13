/*
 * htmx-session.c - Server-side session management implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-session.h"
#include "htmx-cookie.h"
#include <libsoup/soup.h>
#include <string.h>

struct _HtmxSession {
	GObject parent_instance;

	GHashTable *sessions;     /* session_id (gchar*) -> data (GHashTable*) */
	gchar      *cookie_name;
	gint64      max_age;
	gboolean    secure;
	gboolean    http_only;
};

G_DEFINE_FINAL_TYPE(HtmxSession, htmx_session, G_TYPE_OBJECT)

static void
session_data_free(gpointer data)
{
	g_hash_table_unref((GHashTable *)data);
}

static GHashTable *
session_data_new(void)
{
	return g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
}

static void
htmx_session_finalize(GObject *object)
{
	HtmxSession *self = HTMX_SESSION(object);

	g_hash_table_unref(self->sessions);
	g_free(self->cookie_name);

	G_OBJECT_CLASS(htmx_session_parent_class)->finalize(object);
}

static void
htmx_session_class_init(HtmxSessionClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);

	object_class->finalize = htmx_session_finalize;
}

static void
htmx_session_init(HtmxSession *self)
{
	self->sessions = g_hash_table_new_full(g_str_hash, g_str_equal,
	                                      g_free, session_data_free);
	self->cookie_name = g_strdup("htmx_sid");
	self->max_age = -1;
	self->secure = FALSE;
	self->http_only = TRUE;
}

HtmxSession *
htmx_session_new(void)
{
	return g_object_new(HTMX_TYPE_SESSION, NULL);
}

void
htmx_session_set_cookie_name(HtmxSession *self, const gchar *name)
{
	g_return_if_fail(HTMX_IS_SESSION(self));
	g_return_if_fail(name != NULL);

	g_free(self->cookie_name);
	self->cookie_name = g_strdup(name);
}

const gchar *
htmx_session_get_cookie_name(HtmxSession *self)
{
	g_return_val_if_fail(HTMX_IS_SESSION(self), NULL);

	return self->cookie_name;
}

void
htmx_session_set_max_age(HtmxSession *self, gint64 max_age)
{
	g_return_if_fail(HTMX_IS_SESSION(self));

	self->max_age = max_age;
}

void
htmx_session_set_secure(HtmxSession *self, gboolean secure)
{
	g_return_if_fail(HTMX_IS_SESSION(self));

	self->secure = secure;
}

gchar *
htmx_session_create(HtmxSession *self)
{
	gchar *session_id;
	GHashTable *data;

	g_return_val_if_fail(HTMX_IS_SESSION(self), NULL);

	session_id = g_uuid_string_random();
	data = session_data_new();

	g_hash_table_insert(self->sessions, g_strdup(session_id), data);

	return session_id;
}

void
htmx_session_destroy(HtmxSession *self, const gchar *session_id)
{
	g_return_if_fail(HTMX_IS_SESSION(self));
	g_return_if_fail(session_id != NULL);

	g_hash_table_remove(self->sessions, session_id);
}

gboolean
htmx_session_exists(HtmxSession *self, const gchar *session_id)
{
	g_return_val_if_fail(HTMX_IS_SESSION(self), FALSE);

	if (session_id == NULL) {
		return FALSE;
	}

	return g_hash_table_contains(self->sessions, session_id);
}

void
htmx_session_set_value(
	HtmxSession *self,
	const gchar *session_id,
	const gchar *key,
	const gchar *value
){
	GHashTable *data;

	g_return_if_fail(HTMX_IS_SESSION(self));
	g_return_if_fail(session_id != NULL);
	g_return_if_fail(key != NULL);

	data = (GHashTable *)g_hash_table_lookup(self->sessions, session_id);
	if (data == NULL) {
		data = session_data_new();
		g_hash_table_insert(self->sessions, g_strdup(session_id), data);
	}

	g_hash_table_insert(data, g_strdup(key), g_strdup(value));
}

const gchar *
htmx_session_get_value(
	HtmxSession *self,
	const gchar *session_id,
	const gchar *key
){
	GHashTable *data;

	g_return_val_if_fail(HTMX_IS_SESSION(self), NULL);
	g_return_val_if_fail(session_id != NULL, NULL);
	g_return_val_if_fail(key != NULL, NULL);

	data = (GHashTable *)g_hash_table_lookup(self->sessions, session_id);
	if (data == NULL) {
		return NULL;
	}

	return (const gchar *)g_hash_table_lookup(data, key);
}

void
htmx_session_remove_value(
	HtmxSession *self,
	const gchar *session_id,
	const gchar *key
){
	GHashTable *data;

	g_return_if_fail(HTMX_IS_SESSION(self));
	g_return_if_fail(session_id != NULL);
	g_return_if_fail(key != NULL);

	data = (GHashTable *)g_hash_table_lookup(self->sessions, session_id);
	if (data != NULL) {
		g_hash_table_remove(data, key);
	}
}

/*
 * Extract session ID from the Cookie request header.
 */
static gchar *
get_session_id_from_request(HtmxRequest *request, const gchar *cookie_name)
{
	SoupServerMessage *msg;
	SoupMessageHeaders *headers;
	const gchar *cookie_header;
	g_autoptr(GHashTable) cookies = NULL;
	const gchar *session_id;

	msg = htmx_request_get_message(request);
	headers = soup_server_message_get_request_headers(msg);
	cookie_header = soup_message_headers_get_one(headers, "Cookie");

	if (cookie_header == NULL) {
		return NULL;
	}

	cookies = htmx_cookie_parse_request(cookie_header);
	session_id = (const gchar *)g_hash_table_lookup(cookies, cookie_name);

	if (session_id != NULL) {
		return g_strdup(session_id);
	}

	return NULL;
}

void
htmx_session_middleware(
	HtmxContext        *context,
	HtmxMiddlewareNext  next,
	gpointer            next_data,
	gpointer            user_data
){
	HtmxSession *self = HTMX_SESSION(user_data);
	HtmxRequest *request;
	HtmxResponse *response;
	g_autofree gchar *session_id = NULL;
	gboolean is_new = FALSE;

	request = htmx_context_get_request(context);

	/* Try to find existing session from cookie */
	session_id = get_session_id_from_request(request, self->cookie_name);

	if (session_id == NULL || !htmx_session_exists(self, session_id)) {
		/* Create new session */
		g_free(session_id);
		session_id = htmx_session_create(self);
		is_new = TRUE;
	}

	/* Store session info on context */
	htmx_context_set_data(context, "htmx_session_id",
	                      g_strdup(session_id), g_free);
	htmx_context_set_data(context, "htmx_session",
	                      g_object_ref(self), g_object_unref);

	/* Continue pipeline */
	next(context, next_data);

	/* After handler: set session cookie on response if new session */
	if (is_new) {
		response = htmx_context_get_response(context);
		if (response != NULL) {
			g_autoptr(HtmxCookie) cookie = NULL;
			g_autofree gchar *cookie_str = NULL;

			cookie = htmx_cookie_new(self->cookie_name, session_id);
			htmx_cookie_set_max_age(cookie, self->max_age);
			htmx_cookie_set_secure(cookie, self->secure);
			htmx_cookie_set_http_only(cookie, self->http_only);

			cookie_str = htmx_cookie_to_set_cookie(cookie);
			htmx_response_add_header(response, "Set-Cookie", cookie_str);
		}
	}
}
