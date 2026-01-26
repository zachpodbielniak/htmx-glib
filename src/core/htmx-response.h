/*
 * htmx-response.h - HTMX response builder
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * This file defines the HtmxResponse GObject which provides a fluent
 * interface for building HTTP responses with HTMX headers.
 */

#ifndef HTMX_RESPONSE_H
#define HTMX_RESPONSE_H

#include <glib-object.h>
#include <libsoup/soup.h>
#include "htmx-enums.h"
#include "model/htmx-headers.h"
#include "model/htmx-swap.h"

G_BEGIN_DECLS

#define HTMX_TYPE_RESPONSE (htmx_response_get_type())
G_DECLARE_FINAL_TYPE(HtmxResponse, htmx_response, HTMX, RESPONSE, GObject)

/**
 * htmx_response_new:
 *
 * Creates a new empty #HtmxResponse.
 *
 * Returns: (transfer full): a new #HtmxResponse
 */
HtmxResponse *
htmx_response_new(void);

/**
 * htmx_response_new_with_content:
 * @content: HTML content
 *
 * Creates a new #HtmxResponse with the given HTML content.
 *
 * Returns: (transfer full): a new #HtmxResponse
 */
HtmxResponse *
htmx_response_new_with_content(const gchar *content);

/**
 * htmx_response_set_content:
 * @self: an #HtmxResponse
 * @content: HTML content
 *
 * Sets the response content.
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxResponse *
htmx_response_set_content(
	HtmxResponse *self,
	const gchar  *content
);

/**
 * htmx_response_get_content:
 * @self: an #HtmxResponse
 *
 * Gets the response content.
 *
 * Returns: (transfer none) (nullable): the response content
 */
const gchar *
htmx_response_get_content(HtmxResponse *self);

/**
 * htmx_response_set_status:
 * @self: an #HtmxResponse
 * @status_code: HTTP status code
 *
 * Sets the HTTP status code.
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxResponse *
htmx_response_set_status(
	HtmxResponse *self,
	guint         status_code
);

/**
 * htmx_response_get_status:
 * @self: an #HtmxResponse
 *
 * Gets the HTTP status code.
 *
 * Returns: the status code
 */
guint
htmx_response_get_status(HtmxResponse *self);

/**
 * htmx_response_set_content_type:
 * @self: an #HtmxResponse
 * @content_type: MIME type
 *
 * Sets the Content-Type header.
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxResponse *
htmx_response_set_content_type(
	HtmxResponse *self,
	const gchar  *content_type
);

/**
 * htmx_response_add_header:
 * @self: an #HtmxResponse
 * @name: header name
 * @value: header value
 *
 * Adds a custom header to the response.
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxResponse *
htmx_response_add_header(
	HtmxResponse *self,
	const gchar  *name,
	const gchar  *value
);

/**
 * htmx_response_get_headers:
 * @self: an #HtmxResponse
 *
 * Gets the HTMX response headers builder.
 *
 * Returns: (transfer none): the #HtmxResponseHeaders
 */
HtmxResponseHeaders *
htmx_response_get_headers(HtmxResponse *self);

/*
 * HTMX response header setters (fluent interface)
 */

/**
 * htmx_response_location:
 * @self: an #HtmxResponse
 * @location: location value
 *
 * Sets the HX-Location header.
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxResponse *
htmx_response_location(
	HtmxResponse *self,
	const gchar  *location
);

/**
 * htmx_response_push_url:
 * @self: an #HtmxResponse
 * @url: URL to push
 *
 * Sets the HX-Push-Url header.
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxResponse *
htmx_response_push_url(
	HtmxResponse *self,
	const gchar  *url
);

/**
 * htmx_response_redirect:
 * @self: an #HtmxResponse
 * @url: URL to redirect to
 *
 * Sets the HX-Redirect header.
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxResponse *
htmx_response_redirect(
	HtmxResponse *self,
	const gchar  *url
);

/**
 * htmx_response_refresh:
 * @self: an #HtmxResponse
 *
 * Sets the HX-Refresh header to trigger a full page refresh.
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxResponse *
htmx_response_refresh(HtmxResponse *self);

/**
 * htmx_response_replace_url:
 * @self: an #HtmxResponse
 * @url: URL to replace
 *
 * Sets the HX-Replace-Url header.
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxResponse *
htmx_response_replace_url(
	HtmxResponse *self,
	const gchar  *url
);

/**
 * htmx_response_reswap:
 * @self: an #HtmxResponse
 * @swap_style: the swap style
 *
 * Sets the HX-Reswap header to override the swap behavior.
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxResponse *
htmx_response_reswap(
	HtmxResponse *self,
	HtmxSwapStyle swap_style
);

/**
 * htmx_response_reswap_with_config:
 * @self: an #HtmxResponse
 * @swap: the swap configuration
 *
 * Sets the HX-Reswap header with full swap configuration.
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxResponse *
htmx_response_reswap_with_config(
	HtmxResponse   *self,
	const HtmxSwap *swap
);

/**
 * htmx_response_retarget:
 * @self: an #HtmxResponse
 * @target: CSS selector for new target
 *
 * Sets the HX-Retarget header.
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxResponse *
htmx_response_retarget(
	HtmxResponse *self,
	const gchar  *target
);

/**
 * htmx_response_reselect:
 * @self: an #HtmxResponse
 * @selector: CSS selector for content selection
 *
 * Sets the HX-Reselect header.
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxResponse *
htmx_response_reselect(
	HtmxResponse *self,
	const gchar  *selector
);

/**
 * htmx_response_trigger:
 * @self: an #HtmxResponse
 * @event: event name or JSON
 *
 * Sets the HX-Trigger header.
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxResponse *
htmx_response_trigger(
	HtmxResponse *self,
	const gchar  *event
);

/**
 * htmx_response_trigger_after_settle:
 * @self: an #HtmxResponse
 * @event: event name or JSON
 *
 * Sets the HX-Trigger-After-Settle header.
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxResponse *
htmx_response_trigger_after_settle(
	HtmxResponse *self,
	const gchar  *event
);

/**
 * htmx_response_trigger_after_swap:
 * @self: an #HtmxResponse
 * @event: event name or JSON
 *
 * Sets the HX-Trigger-After-Swap header.
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxResponse *
htmx_response_trigger_after_swap(
	HtmxResponse *self,
	const gchar  *event
);

/**
 * htmx_response_apply:
 * @self: an #HtmxResponse
 * @message: a #SoupServerMessage
 *
 * Applies the response to a SoupServerMessage.
 */
void
htmx_response_apply(
	HtmxResponse      *self,
	SoupServerMessage *message
);

/*
 * Convenience constructors for common responses
 */

/**
 * htmx_response_empty:
 *
 * Creates an empty 200 OK response with no content.
 *
 * Returns: (transfer full): a new #HtmxResponse
 */
HtmxResponse *
htmx_response_empty(void);

/**
 * htmx_response_stop_polling:
 *
 * Creates a 286 response to stop HTMX polling.
 *
 * Returns: (transfer full): a new #HtmxResponse
 */
HtmxResponse *
htmx_response_stop_polling(void);

/**
 * htmx_response_not_found:
 *
 * Creates a 404 Not Found response.
 *
 * Returns: (transfer full): a new #HtmxResponse
 */
HtmxResponse *
htmx_response_not_found(void);

/**
 * htmx_response_error:
 * @message: error message
 *
 * Creates a 500 Internal Server Error response.
 *
 * Returns: (transfer full): a new #HtmxResponse
 */
HtmxResponse *
htmx_response_error(const gchar *message);

G_END_DECLS

#endif /* HTMX_RESPONSE_H */
