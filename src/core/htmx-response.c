/*
 * htmx-response.c - HTMX response builder implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-response.h"

struct _HtmxResponse {
	GObject parent_instance;

	gchar               *content;       /* Response body */
	guint                status_code;   /* HTTP status code */
	gchar               *content_type;  /* Content-Type header */
	GHashTable          *custom_headers;/* Custom headers */
	HtmxResponseHeaders *htmx_headers;  /* HTMX response headers */
};

G_DEFINE_FINAL_TYPE(HtmxResponse, htmx_response, G_TYPE_OBJECT)

/*
 * Object finalization
 */
static void
htmx_response_finalize(GObject *object)
{
	HtmxResponse *self = HTMX_RESPONSE(object);

	g_clear_pointer(&self->content, g_free);
	g_clear_pointer(&self->content_type, g_free);
	g_clear_pointer(&self->custom_headers, g_hash_table_unref);
	g_clear_pointer(&self->htmx_headers, htmx_response_headers_free);

	G_OBJECT_CLASS(htmx_response_parent_class)->finalize(object);
}

/*
 * Class initialization
 */
static void
htmx_response_class_init(HtmxResponseClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);

	object_class->finalize = htmx_response_finalize;
}

/*
 * Instance initialization
 */
static void
htmx_response_init(HtmxResponse *self)
{
	self->content = NULL;
	self->status_code = 200;
	self->content_type = g_strdup("text/html; charset=utf-8");
	self->custom_headers = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
	self->htmx_headers = htmx_response_headers_new();
}

/*
 * Create a new empty response
 */
HtmxResponse *
htmx_response_new(void)
{
	return g_object_new(HTMX_TYPE_RESPONSE, NULL);
}

/*
 * Create a new response with content
 */
HtmxResponse *
htmx_response_new_with_content(const gchar *content)
{
	HtmxResponse *self;

	self = htmx_response_new();
	htmx_response_set_content(self, content);

	return self;
}

/*
 * Set the response content
 */
HtmxResponse *
htmx_response_set_content(
	HtmxResponse *self,
	const gchar  *content
){
	g_return_val_if_fail(HTMX_IS_RESPONSE(self), NULL);

	g_free(self->content);
	self->content = g_strdup(content);

	return self;
}

/*
 * Get the response content
 */
const gchar *
htmx_response_get_content(HtmxResponse *self)
{
	g_return_val_if_fail(HTMX_IS_RESPONSE(self), NULL);

	return self->content;
}

/*
 * Set the HTTP status code
 */
HtmxResponse *
htmx_response_set_status(
	HtmxResponse *self,
	guint         status_code
){
	g_return_val_if_fail(HTMX_IS_RESPONSE(self), NULL);

	self->status_code = status_code;

	return self;
}

/*
 * Get the HTTP status code
 */
guint
htmx_response_get_status(HtmxResponse *self)
{
	g_return_val_if_fail(HTMX_IS_RESPONSE(self), 200);

	return self->status_code;
}

/*
 * Set the Content-Type header
 */
HtmxResponse *
htmx_response_set_content_type(
	HtmxResponse *self,
	const gchar  *content_type
){
	g_return_val_if_fail(HTMX_IS_RESPONSE(self), NULL);

	g_free(self->content_type);
	self->content_type = g_strdup(content_type);

	return self;
}

/*
 * Add a custom header
 */
HtmxResponse *
htmx_response_add_header(
	HtmxResponse *self,
	const gchar  *name,
	const gchar  *value
){
	g_return_val_if_fail(HTMX_IS_RESPONSE(self), NULL);
	g_return_val_if_fail(name != NULL, NULL);

	g_hash_table_insert(self->custom_headers, g_strdup(name), g_strdup(value));

	return self;
}

/*
 * Get the HTMX headers
 */
HtmxResponseHeaders *
htmx_response_get_headers(HtmxResponse *self)
{
	g_return_val_if_fail(HTMX_IS_RESPONSE(self), NULL);

	return self->htmx_headers;
}

/*
 * Set HX-Location
 */
HtmxResponse *
htmx_response_location(
	HtmxResponse *self,
	const gchar  *location
){
	g_return_val_if_fail(HTMX_IS_RESPONSE(self), NULL);

	htmx_response_headers_set_location(self->htmx_headers, location);

	return self;
}

/*
 * Set HX-Push-Url
 */
HtmxResponse *
htmx_response_push_url(
	HtmxResponse *self,
	const gchar  *url
){
	g_return_val_if_fail(HTMX_IS_RESPONSE(self), NULL);

	htmx_response_headers_set_push_url(self->htmx_headers, url);

	return self;
}

/*
 * Set HX-Redirect
 */
HtmxResponse *
htmx_response_redirect(
	HtmxResponse *self,
	const gchar  *url
){
	g_return_val_if_fail(HTMX_IS_RESPONSE(self), NULL);

	htmx_response_headers_set_redirect(self->htmx_headers, url);

	return self;
}

/*
 * Set HX-Refresh
 */
HtmxResponse *
htmx_response_refresh(HtmxResponse *self)
{
	g_return_val_if_fail(HTMX_IS_RESPONSE(self), NULL);

	htmx_response_headers_set_refresh(self->htmx_headers, TRUE);

	return self;
}

/*
 * Set HX-Replace-Url
 */
HtmxResponse *
htmx_response_replace_url(
	HtmxResponse *self,
	const gchar  *url
){
	g_return_val_if_fail(HTMX_IS_RESPONSE(self), NULL);

	htmx_response_headers_set_replace_url(self->htmx_headers, url);

	return self;
}

/*
 * Set HX-Reswap with style
 */
HtmxResponse *
htmx_response_reswap(
	HtmxResponse *self,
	HtmxSwapStyle swap_style
){
	const gchar *swap_str;

	g_return_val_if_fail(HTMX_IS_RESPONSE(self), NULL);

	swap_str = htmx_swap_style_to_string(swap_style);
	htmx_response_headers_set_reswap(self->htmx_headers, swap_str);

	return self;
}

/*
 * Set HX-Reswap with full configuration
 */
HtmxResponse *
htmx_response_reswap_with_config(
	HtmxResponse   *self,
	const HtmxSwap *swap
){
	g_autofree gchar *swap_str = NULL;

	g_return_val_if_fail(HTMX_IS_RESPONSE(self), NULL);
	g_return_val_if_fail(swap != NULL, NULL);

	swap_str = htmx_swap_render(swap);
	htmx_response_headers_set_reswap(self->htmx_headers, swap_str);

	return self;
}

/*
 * Set HX-Retarget
 */
HtmxResponse *
htmx_response_retarget(
	HtmxResponse *self,
	const gchar  *target
){
	g_return_val_if_fail(HTMX_IS_RESPONSE(self), NULL);

	htmx_response_headers_set_retarget(self->htmx_headers, target);

	return self;
}

/*
 * Set HX-Reselect
 */
HtmxResponse *
htmx_response_reselect(
	HtmxResponse *self,
	const gchar  *selector
){
	g_return_val_if_fail(HTMX_IS_RESPONSE(self), NULL);

	htmx_response_headers_set_reselect(self->htmx_headers, selector);

	return self;
}

/*
 * Set HX-Trigger
 */
HtmxResponse *
htmx_response_trigger(
	HtmxResponse *self,
	const gchar  *event
){
	g_return_val_if_fail(HTMX_IS_RESPONSE(self), NULL);

	htmx_response_headers_set_trigger(self->htmx_headers, event);

	return self;
}

/*
 * Set HX-Trigger-After-Settle
 */
HtmxResponse *
htmx_response_trigger_after_settle(
	HtmxResponse *self,
	const gchar  *event
){
	g_return_val_if_fail(HTMX_IS_RESPONSE(self), NULL);

	htmx_response_headers_set_trigger_after_settle(self->htmx_headers, event);

	return self;
}

/*
 * Set HX-Trigger-After-Swap
 */
HtmxResponse *
htmx_response_trigger_after_swap(
	HtmxResponse *self,
	const gchar  *event
){
	g_return_val_if_fail(HTMX_IS_RESPONSE(self), NULL);

	htmx_response_headers_set_trigger_after_swap(self->htmx_headers, event);

	return self;
}

/*
 * Iterator callback for adding custom headers
 */
static void
add_custom_header(gpointer key, gpointer value, gpointer user_data)
{
	SoupMessageHeaders *headers = (SoupMessageHeaders *)user_data;

	soup_message_headers_replace(headers, (const gchar *)key, (const gchar *)value);
}

/*
 * Apply the response to a SoupServerMessage
 */
void
htmx_response_apply(
	HtmxResponse      *self,
	SoupServerMessage *message
){
	SoupMessageHeaders *headers;
	SoupMessageBody *body;

	g_return_if_fail(HTMX_IS_RESPONSE(self));
	g_return_if_fail(SOUP_IS_SERVER_MESSAGE(message));

	/* Set status */
	soup_server_message_set_status(message, self->status_code, NULL);

	/* Get response headers */
	headers = soup_server_message_get_response_headers(message);

	/* Set Content-Type */
	if (self->content_type != NULL) {
		soup_message_headers_replace(headers, "Content-Type", self->content_type);
	}

	/* Add custom headers */
	g_hash_table_foreach(self->custom_headers, add_custom_header, headers);

	/* Apply HTMX headers */
	htmx_response_headers_apply(self->htmx_headers, message);

	/* Set body */
	body = soup_server_message_get_response_body(message);
	if (self->content != NULL) {
		soup_message_body_append(body, SOUP_MEMORY_COPY, self->content, strlen(self->content));
	}
}

/*
 * Create an empty 200 OK response
 */
HtmxResponse *
htmx_response_empty(void)
{
	return htmx_response_new();
}

/*
 * Create a 286 response to stop polling
 */
HtmxResponse *
htmx_response_stop_polling(void)
{
	HtmxResponse *self;

	self = htmx_response_new();
	htmx_response_set_status(self, 286);

	return self;
}

/*
 * Create a 404 Not Found response
 */
HtmxResponse *
htmx_response_not_found(void)
{
	HtmxResponse *self;

	self = htmx_response_new();
	htmx_response_set_status(self, 404);
	htmx_response_set_content(self, "<h1>404 Not Found</h1>");

	return self;
}

/*
 * Create a 500 error response
 */
HtmxResponse *
htmx_response_error(const gchar *message)
{
	HtmxResponse *self;
	g_autofree gchar *content = NULL;

	self = htmx_response_new();
	htmx_response_set_status(self, 500);

	content = g_strdup_printf("<h1>500 Internal Server Error</h1><p>%s</p>",
	                          message != NULL ? message : "An error occurred");
	htmx_response_set_content(self, content);

	return self;
}
