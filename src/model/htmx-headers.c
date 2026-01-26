/*
 * htmx-headers.c - HTMX header collection boxed types implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-headers.h"
#include <libsoup/soup.h>

/*
 * GType registration for HtmxRequestHeaders
 */
G_DEFINE_BOXED_TYPE(HtmxRequestHeaders, htmx_request_headers,
                    htmx_request_headers_copy, htmx_request_headers_free)

/*
 * GType registration for HtmxResponseHeaders
 */
G_DEFINE_BOXED_TYPE(HtmxResponseHeaders, htmx_response_headers,
                    htmx_response_headers_copy, htmx_response_headers_free)

/*
 * Create a new empty request headers structure
 */
HtmxRequestHeaders *
htmx_request_headers_new(void)
{
	HtmxRequestHeaders *self;

	self = g_slice_new0(HtmxRequestHeaders);
	self->is_htmx_request = FALSE;
	self->is_boosted = FALSE;
	self->is_history_restore = FALSE;
	self->current_url = NULL;
	self->trigger_id = NULL;
	self->trigger_name = NULL;
	self->target_id = NULL;
	self->prompt_response = NULL;

	return self;
}

/*
 * Copy request headers
 */
HtmxRequestHeaders *
htmx_request_headers_copy(const HtmxRequestHeaders *self)
{
	HtmxRequestHeaders *copy;

	if (self == NULL) {
		return NULL;
	}

	copy = g_slice_new0(HtmxRequestHeaders);
	copy->is_htmx_request = self->is_htmx_request;
	copy->is_boosted = self->is_boosted;
	copy->is_history_restore = self->is_history_restore;
	copy->current_url = g_strdup(self->current_url);
	copy->trigger_id = g_strdup(self->trigger_id);
	copy->trigger_name = g_strdup(self->trigger_name);
	copy->target_id = g_strdup(self->target_id);
	copy->prompt_response = g_strdup(self->prompt_response);

	return copy;
}

/*
 * Free request headers
 */
void
htmx_request_headers_free(HtmxRequestHeaders *self)
{
	if (self == NULL) {
		return;
	}

	g_free(self->current_url);
	g_free(self->trigger_id);
	g_free(self->trigger_name);
	g_free(self->target_id);
	g_free(self->prompt_response);
	g_slice_free(HtmxRequestHeaders, self);
}

/*
 * Check if this is an HTMX request
 */
gboolean
htmx_request_headers_is_htmx_request(const HtmxRequestHeaders *self)
{
	g_return_val_if_fail(self != NULL, FALSE);

	return self->is_htmx_request;
}

/*
 * Check if this request is boosted
 */
gboolean
htmx_request_headers_is_boosted(const HtmxRequestHeaders *self)
{
	g_return_val_if_fail(self != NULL, FALSE);

	return self->is_boosted;
}

/*
 * Check if this is a history restore request
 */
gboolean
htmx_request_headers_is_history_restore(const HtmxRequestHeaders *self)
{
	g_return_val_if_fail(self != NULL, FALSE);

	return self->is_history_restore;
}

/*
 * Get the current URL
 */
const gchar *
htmx_request_headers_get_current_url(const HtmxRequestHeaders *self)
{
	g_return_val_if_fail(self != NULL, NULL);

	return self->current_url;
}

/*
 * Get the trigger element ID
 */
const gchar *
htmx_request_headers_get_trigger_id(const HtmxRequestHeaders *self)
{
	g_return_val_if_fail(self != NULL, NULL);

	return self->trigger_id;
}

/*
 * Get the trigger element name
 */
const gchar *
htmx_request_headers_get_trigger_name(const HtmxRequestHeaders *self)
{
	g_return_val_if_fail(self != NULL, NULL);

	return self->trigger_name;
}

/*
 * Get the target element ID
 */
const gchar *
htmx_request_headers_get_target_id(const HtmxRequestHeaders *self)
{
	g_return_val_if_fail(self != NULL, NULL);

	return self->target_id;
}

/*
 * Get the prompt response
 */
const gchar *
htmx_request_headers_get_prompt_response(const HtmxRequestHeaders *self)
{
	g_return_val_if_fail(self != NULL, NULL);

	return self->prompt_response;
}


/*
 * Create a new empty response headers structure
 */
HtmxResponseHeaders *
htmx_response_headers_new(void)
{
	HtmxResponseHeaders *self;

	self = g_slice_new0(HtmxResponseHeaders);
	self->location = NULL;
	self->push_url = NULL;
	self->redirect = NULL;
	self->refresh = FALSE;
	self->replace_url = NULL;
	self->reswap = NULL;
	self->retarget = NULL;
	self->reselect = NULL;
	self->trigger = NULL;
	self->trigger_after_settle = NULL;
	self->trigger_after_swap = NULL;

	return self;
}

/*
 * Copy response headers
 */
HtmxResponseHeaders *
htmx_response_headers_copy(const HtmxResponseHeaders *self)
{
	HtmxResponseHeaders *copy;

	if (self == NULL) {
		return NULL;
	}

	copy = g_slice_new0(HtmxResponseHeaders);
	copy->location = g_strdup(self->location);
	copy->push_url = g_strdup(self->push_url);
	copy->redirect = g_strdup(self->redirect);
	copy->refresh = self->refresh;
	copy->replace_url = g_strdup(self->replace_url);
	copy->reswap = g_strdup(self->reswap);
	copy->retarget = g_strdup(self->retarget);
	copy->reselect = g_strdup(self->reselect);
	copy->trigger = g_strdup(self->trigger);
	copy->trigger_after_settle = g_strdup(self->trigger_after_settle);
	copy->trigger_after_swap = g_strdup(self->trigger_after_swap);

	return copy;
}

/*
 * Free response headers
 */
void
htmx_response_headers_free(HtmxResponseHeaders *self)
{
	if (self == NULL) {
		return;
	}

	g_free(self->location);
	g_free(self->push_url);
	g_free(self->redirect);
	g_free(self->replace_url);
	g_free(self->reswap);
	g_free(self->retarget);
	g_free(self->reselect);
	g_free(self->trigger);
	g_free(self->trigger_after_settle);
	g_free(self->trigger_after_swap);
	g_slice_free(HtmxResponseHeaders, self);
}

/*
 * Set HX-Location header
 */
void
htmx_response_headers_set_location(
	HtmxResponseHeaders *self,
	const gchar         *location
){
	g_return_if_fail(self != NULL);

	g_free(self->location);
	self->location = g_strdup(location);
}

/*
 * Set HX-Push-Url header
 */
void
htmx_response_headers_set_push_url(
	HtmxResponseHeaders *self,
	const gchar         *url
){
	g_return_if_fail(self != NULL);

	g_free(self->push_url);
	self->push_url = g_strdup(url);
}

/*
 * Set HX-Redirect header
 */
void
htmx_response_headers_set_redirect(
	HtmxResponseHeaders *self,
	const gchar         *url
){
	g_return_if_fail(self != NULL);

	g_free(self->redirect);
	self->redirect = g_strdup(url);
}

/*
 * Set HX-Refresh header
 */
void
htmx_response_headers_set_refresh(
	HtmxResponseHeaders *self,
	gboolean             refresh
){
	g_return_if_fail(self != NULL);

	self->refresh = refresh;
}

/*
 * Set HX-Replace-Url header
 */
void
htmx_response_headers_set_replace_url(
	HtmxResponseHeaders *self,
	const gchar         *url
){
	g_return_if_fail(self != NULL);

	g_free(self->replace_url);
	self->replace_url = g_strdup(url);
}

/*
 * Set HX-Reswap header
 */
void
htmx_response_headers_set_reswap(
	HtmxResponseHeaders *self,
	const gchar         *swap
){
	g_return_if_fail(self != NULL);

	g_free(self->reswap);
	self->reswap = g_strdup(swap);
}

/*
 * Set HX-Retarget header
 */
void
htmx_response_headers_set_retarget(
	HtmxResponseHeaders *self,
	const gchar         *target
){
	g_return_if_fail(self != NULL);

	g_free(self->retarget);
	self->retarget = g_strdup(target);
}

/*
 * Set HX-Reselect header
 */
void
htmx_response_headers_set_reselect(
	HtmxResponseHeaders *self,
	const gchar         *selector
){
	g_return_if_fail(self != NULL);

	g_free(self->reselect);
	self->reselect = g_strdup(selector);
}

/*
 * Set HX-Trigger header
 */
void
htmx_response_headers_set_trigger(
	HtmxResponseHeaders *self,
	const gchar         *trigger
){
	g_return_if_fail(self != NULL);

	g_free(self->trigger);
	self->trigger = g_strdup(trigger);
}

/*
 * Set HX-Trigger-After-Settle header
 */
void
htmx_response_headers_set_trigger_after_settle(
	HtmxResponseHeaders *self,
	const gchar         *trigger
){
	g_return_if_fail(self != NULL);

	g_free(self->trigger_after_settle);
	self->trigger_after_settle = g_strdup(trigger);
}

/*
 * Set HX-Trigger-After-Swap header
 */
void
htmx_response_headers_set_trigger_after_swap(
	HtmxResponseHeaders *self,
	const gchar         *trigger
){
	g_return_if_fail(self != NULL);

	g_free(self->trigger_after_swap);
	self->trigger_after_swap = g_strdup(trigger);
}

/*
 * Apply response headers to a SoupServerMessage
 */
void
htmx_response_headers_apply(
	HtmxResponseHeaders *self,
	SoupServerMessage   *message
){
	SoupMessageHeaders *headers;

	g_return_if_fail(self != NULL);
	g_return_if_fail(SOUP_IS_SERVER_MESSAGE(message));

	headers = soup_server_message_get_response_headers(message);

	if (self->location != NULL) {
		soup_message_headers_replace(headers, "HX-Location", self->location);
	}

	if (self->push_url != NULL) {
		soup_message_headers_replace(headers, "HX-Push-Url", self->push_url);
	}

	if (self->redirect != NULL) {
		soup_message_headers_replace(headers, "HX-Redirect", self->redirect);
	}

	if (self->refresh) {
		soup_message_headers_replace(headers, "HX-Refresh", "true");
	}

	if (self->replace_url != NULL) {
		soup_message_headers_replace(headers, "HX-Replace-Url", self->replace_url);
	}

	if (self->reswap != NULL) {
		soup_message_headers_replace(headers, "HX-Reswap", self->reswap);
	}

	if (self->retarget != NULL) {
		soup_message_headers_replace(headers, "HX-Retarget", self->retarget);
	}

	if (self->reselect != NULL) {
		soup_message_headers_replace(headers, "HX-Reselect", self->reselect);
	}

	if (self->trigger != NULL) {
		soup_message_headers_replace(headers, "HX-Trigger", self->trigger);
	}

	if (self->trigger_after_settle != NULL) {
		soup_message_headers_replace(headers, "HX-Trigger-After-Settle",
		                             self->trigger_after_settle);
	}

	if (self->trigger_after_swap != NULL) {
		soup_message_headers_replace(headers, "HX-Trigger-After-Swap",
		                             self->trigger_after_swap);
	}
}
