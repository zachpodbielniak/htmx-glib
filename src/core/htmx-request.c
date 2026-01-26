/*
 * htmx-request.c - Parsed HTMX request implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-request.h"
#include "htmx-error.h"
#include <json-glib/json-glib.h>
#include <string.h>

struct _HtmxRequest {
	GObject parent_instance;

	SoupServerMessage  *message;       /* The underlying soup message */
	HtmxMethod          method;        /* Parsed HTTP method */
	gchar              *path;          /* Request path */
	gchar              *query;         /* Query string */
	GHashTable         *query_params;  /* Parsed query parameters */
	HtmxRequestHeaders *htmx_headers;  /* Parsed HTMX headers */
	gchar              *body;          /* Request body as string */
	GBytes             *body_bytes;    /* Request body as bytes */
	GHashTable         *form_data;     /* Parsed form data */
	gchar              *content_type;  /* Content-Type header */
};

G_DEFINE_FINAL_TYPE(HtmxRequest, htmx_request, G_TYPE_OBJECT)

/*
 * Parse query string into hash table
 */
static GHashTable *
parse_query_string(const gchar *query)
{
	GHashTable *params;
	g_auto(GStrv) pairs = NULL;
	gint i;

	params = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);

	if (query == NULL || query[0] == '\0') {
		return params;
	}

	pairs = g_strsplit(query, "&", -1);
	for (i = 0; pairs[i] != NULL; i++) {
		gchar *eq;
		gchar *key;
		gchar *value;

		eq = strchr(pairs[i], '=');
		if (eq != NULL) {
			key = g_uri_unescape_segment(pairs[i], eq, NULL);
			value = g_uri_unescape_string(eq + 1, NULL);
		} else {
			key = g_uri_unescape_string(pairs[i], NULL);
			value = g_strdup("");
		}

		if (key != NULL) {
			g_hash_table_insert(params, key, value != NULL ? value : g_strdup(""));
		} else {
			g_free(value);
		}
	}

	return params;
}

/*
 * Parse form data (application/x-www-form-urlencoded)
 */
static GHashTable *
parse_form_data(const gchar *body)
{
	return parse_query_string(body);
}

/*
 * Parse HTMX headers from message
 */
static HtmxRequestHeaders *
parse_htmx_headers(SoupServerMessage *message)
{
	HtmxRequestHeaders *headers;
	SoupMessageHeaders *msg_headers;
	const gchar *value;

	headers = htmx_request_headers_new();
	msg_headers = soup_server_message_get_request_headers(message);

	/* HX-Request */
	value = soup_message_headers_get_one(msg_headers, "HX-Request");
	headers->is_htmx_request = (value != NULL && g_ascii_strcasecmp(value, "true") == 0);

	/* HX-Boosted */
	value = soup_message_headers_get_one(msg_headers, "HX-Boosted");
	headers->is_boosted = (value != NULL && g_ascii_strcasecmp(value, "true") == 0);

	/* HX-History-Restore-Request */
	value = soup_message_headers_get_one(msg_headers, "HX-History-Restore-Request");
	headers->is_history_restore = (value != NULL && g_ascii_strcasecmp(value, "true") == 0);

	/* HX-Current-URL */
	value = soup_message_headers_get_one(msg_headers, "HX-Current-URL");
	headers->current_url = g_strdup(value);

	/* HX-Trigger */
	value = soup_message_headers_get_one(msg_headers, "HX-Trigger");
	headers->trigger_id = g_strdup(value);

	/* HX-Trigger-Name */
	value = soup_message_headers_get_one(msg_headers, "HX-Trigger-Name");
	headers->trigger_name = g_strdup(value);

	/* HX-Target */
	value = soup_message_headers_get_one(msg_headers, "HX-Target");
	headers->target_id = g_strdup(value);

	/* HX-Prompt */
	value = soup_message_headers_get_one(msg_headers, "HX-Prompt");
	headers->prompt_response = g_strdup(value);

	return headers;
}

/*
 * Parse HTTP method string to enum
 */
static HtmxMethod
parse_method(const gchar *method_str)
{
	if (g_ascii_strcasecmp(method_str, "GET") == 0) {
		return HTMX_METHOD_GET;
	} else if (g_ascii_strcasecmp(method_str, "POST") == 0) {
		return HTMX_METHOD_POST;
	} else if (g_ascii_strcasecmp(method_str, "PUT") == 0) {
		return HTMX_METHOD_PUT;
	} else if (g_ascii_strcasecmp(method_str, "PATCH") == 0) {
		return HTMX_METHOD_PATCH;
	} else if (g_ascii_strcasecmp(method_str, "DELETE") == 0) {
		return HTMX_METHOD_DELETE;
	}

	return HTMX_METHOD_GET;
}

/*
 * Object finalization
 */
static void
htmx_request_finalize(GObject *object)
{
	HtmxRequest *self = HTMX_REQUEST(object);

	g_clear_object(&self->message);
	g_clear_pointer(&self->path, g_free);
	g_clear_pointer(&self->query, g_free);
	g_clear_pointer(&self->query_params, g_hash_table_unref);
	g_clear_pointer(&self->htmx_headers, htmx_request_headers_free);
	g_clear_pointer(&self->body, g_free);
	g_clear_pointer(&self->body_bytes, g_bytes_unref);
	g_clear_pointer(&self->form_data, g_hash_table_unref);
	g_clear_pointer(&self->content_type, g_free);

	G_OBJECT_CLASS(htmx_request_parent_class)->finalize(object);
}

/*
 * Class initialization
 */
static void
htmx_request_class_init(HtmxRequestClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);

	object_class->finalize = htmx_request_finalize;
}

/*
 * Instance initialization
 */
static void
htmx_request_init(HtmxRequest *self)
{
	self->message = NULL;
	self->method = HTMX_METHOD_GET;
	self->path = NULL;
	self->query = NULL;
	self->query_params = NULL;
	self->htmx_headers = NULL;
	self->body = NULL;
	self->body_bytes = NULL;
	self->form_data = NULL;
	self->content_type = NULL;
}

/*
 * Create a new request from a SoupServerMessage
 */
HtmxRequest *
htmx_request_new_from_message(SoupServerMessage *message)
{
	HtmxRequest *self;
	GUri *uri;
	const gchar *method_str;
	SoupMessageHeaders *headers;
	SoupMessageBody *body;

	g_return_val_if_fail(SOUP_IS_SERVER_MESSAGE(message), NULL);

	self = g_object_new(HTMX_TYPE_REQUEST, NULL);
	self->message = g_object_ref(message);

	/* Parse method */
	method_str = soup_server_message_get_method(message);
	self->method = parse_method(method_str);

	/* Parse URI */
	uri = soup_server_message_get_uri(message);
	self->path = g_strdup(g_uri_get_path(uri));
	self->query = g_strdup(g_uri_get_query(uri));
	self->query_params = parse_query_string(self->query);

	/* Parse HTMX headers */
	self->htmx_headers = parse_htmx_headers(message);

	/* Get Content-Type */
	headers = soup_server_message_get_request_headers(message);
	self->content_type = g_strdup(soup_message_headers_get_content_type(headers, NULL));

	/* Get body */
	body = soup_server_message_get_request_body(message);
	if (body != NULL && body->length > 0) {
		self->body_bytes = g_bytes_new(body->data, body->length);
		self->body = g_strndup(body->data, body->length);

		/* Parse form data if applicable */
		if (self->content_type != NULL &&
		    g_str_has_prefix(self->content_type, "application/x-www-form-urlencoded")) {
			self->form_data = parse_form_data(self->body);
		} else {
			self->form_data = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
		}
	} else {
		self->form_data = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
	}

	return self;
}

/*
 * Get the underlying SoupServerMessage
 */
SoupServerMessage *
htmx_request_get_message(HtmxRequest *self)
{
	g_return_val_if_fail(HTMX_IS_REQUEST(self), NULL);

	return self->message;
}

/*
 * Get the HTTP method
 */
HtmxMethod
htmx_request_get_method(HtmxRequest *self)
{
	g_return_val_if_fail(HTMX_IS_REQUEST(self), HTMX_METHOD_GET);

	return self->method;
}

/*
 * Get the request path
 */
const gchar *
htmx_request_get_path(HtmxRequest *self)
{
	g_return_val_if_fail(HTMX_IS_REQUEST(self), NULL);

	return self->path;
}

/*
 * Get the query string
 */
const gchar *
htmx_request_get_query(HtmxRequest *self)
{
	g_return_val_if_fail(HTMX_IS_REQUEST(self), NULL);

	return self->query;
}

/*
 * Get query parameters
 */
GHashTable *
htmx_request_get_query_params(HtmxRequest *self)
{
	g_return_val_if_fail(HTMX_IS_REQUEST(self), NULL);

	return self->query_params;
}

/*
 * Get a specific query parameter
 */
const gchar *
htmx_request_get_query_param(
	HtmxRequest *self,
	const gchar *name
){
	g_return_val_if_fail(HTMX_IS_REQUEST(self), NULL);
	g_return_val_if_fail(name != NULL, NULL);

	return g_hash_table_lookup(self->query_params, name);
}

/*
 * Get HTMX headers
 */
HtmxRequestHeaders *
htmx_request_get_headers(HtmxRequest *self)
{
	g_return_val_if_fail(HTMX_IS_REQUEST(self), NULL);

	return self->htmx_headers;
}

/*
 * Check if this is an HTMX request
 */
gboolean
htmx_request_is_htmx(HtmxRequest *self)
{
	g_return_val_if_fail(HTMX_IS_REQUEST(self), FALSE);

	return htmx_request_headers_is_htmx_request(self->htmx_headers);
}

/*
 * Check if this is a boosted request
 */
gboolean
htmx_request_is_boosted(HtmxRequest *self)
{
	g_return_val_if_fail(HTMX_IS_REQUEST(self), FALSE);

	return htmx_request_headers_is_boosted(self->htmx_headers);
}

/*
 * Get the trigger element ID
 */
const gchar *
htmx_request_get_trigger_id(HtmxRequest *self)
{
	g_return_val_if_fail(HTMX_IS_REQUEST(self), NULL);

	return htmx_request_headers_get_trigger_id(self->htmx_headers);
}

/*
 * Get the target element ID
 */
const gchar *
htmx_request_get_target_id(HtmxRequest *self)
{
	g_return_val_if_fail(HTMX_IS_REQUEST(self), NULL);

	return htmx_request_headers_get_target_id(self->htmx_headers);
}

/*
 * Get the request body as string
 */
const gchar *
htmx_request_get_body(HtmxRequest *self)
{
	g_return_val_if_fail(HTMX_IS_REQUEST(self), NULL);

	return self->body;
}

/*
 * Get the request body as bytes
 */
GBytes *
htmx_request_get_body_bytes(HtmxRequest *self)
{
	g_return_val_if_fail(HTMX_IS_REQUEST(self), NULL);

	return self->body_bytes;
}

/*
 * Get form data
 */
GHashTable *
htmx_request_get_form_data(HtmxRequest *self)
{
	g_return_val_if_fail(HTMX_IS_REQUEST(self), NULL);

	return self->form_data;
}

/*
 * Get a specific form value
 */
const gchar *
htmx_request_get_form_value(
	HtmxRequest *self,
	const gchar *name
){
	g_return_val_if_fail(HTMX_IS_REQUEST(self), NULL);
	g_return_val_if_fail(name != NULL, NULL);

	return g_hash_table_lookup(self->form_data, name);
}

/*
 * Get the request body as JSON
 */
JsonNode *
htmx_request_get_json(
	HtmxRequest *self,
	GError     **error
){
	g_autoptr(JsonParser) parser = NULL;

	g_return_val_if_fail(HTMX_IS_REQUEST(self), NULL);

	if (self->body == NULL || self->body[0] == '\0') {
		g_set_error(error, HTMX_ERROR, HTMX_ERROR_INVALID_REQUEST,
		            "Request body is empty");
		return NULL;
	}

	parser = json_parser_new();
	if (!json_parser_load_from_data(parser, self->body, -1, error)) {
		return NULL;
	}

	return json_node_ref(json_parser_get_root(parser));
}

/*
 * Get the Content-Type header
 */
const gchar *
htmx_request_get_content_type(HtmxRequest *self)
{
	g_return_val_if_fail(HTMX_IS_REQUEST(self), NULL);

	return self->content_type;
}
