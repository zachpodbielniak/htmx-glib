/*
 * htmx-request.h - Parsed HTMX request
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * This file defines the HtmxRequest GObject which represents a parsed
 * incoming HTMX request with all its headers and data.
 */

#ifndef HTMX_REQUEST_H
#define HTMX_REQUEST_H

#include <glib-object.h>
#include <libsoup/soup.h>
#include <json-glib/json-glib.h>
#include "htmx-enums.h"
#include "model/htmx-headers.h"

G_BEGIN_DECLS

#define HTMX_TYPE_REQUEST (htmx_request_get_type())
G_DECLARE_FINAL_TYPE(HtmxRequest, htmx_request, HTMX, REQUEST, GObject)

/**
 * htmx_request_new_from_message:
 * @message: a #SoupServerMessage
 *
 * Creates a new #HtmxRequest by parsing the headers from a SoupServerMessage.
 *
 * Returns: (transfer full): a new #HtmxRequest
 */
HtmxRequest *
htmx_request_new_from_message(SoupServerMessage *message);

/**
 * htmx_request_get_message:
 * @self: an #HtmxRequest
 *
 * Gets the underlying SoupServerMessage.
 *
 * Returns: (transfer none): the #SoupServerMessage
 */
SoupServerMessage *
htmx_request_get_message(HtmxRequest *self);

/**
 * htmx_request_get_method:
 * @self: an #HtmxRequest
 *
 * Gets the HTTP method of the request.
 *
 * Returns: the #HtmxMethod
 */
HtmxMethod
htmx_request_get_method(HtmxRequest *self);

/**
 * htmx_request_get_path:
 * @self: an #HtmxRequest
 *
 * Gets the request path.
 *
 * Returns: (transfer none): the request path
 */
const gchar *
htmx_request_get_path(HtmxRequest *self);

/**
 * htmx_request_get_query:
 * @self: an #HtmxRequest
 *
 * Gets the query string (without the leading '?').
 *
 * Returns: (transfer none) (nullable): the query string, or %NULL if none
 */
const gchar *
htmx_request_get_query(HtmxRequest *self);

/**
 * htmx_request_get_query_params:
 * @self: an #HtmxRequest
 *
 * Gets the parsed query parameters as a hash table.
 *
 * Returns: (transfer none) (element-type utf8 utf8): the query parameters
 */
GHashTable *
htmx_request_get_query_params(HtmxRequest *self);

/**
 * htmx_request_get_query_param:
 * @self: an #HtmxRequest
 * @name: the parameter name
 *
 * Gets a specific query parameter value.
 *
 * Returns: (transfer none) (nullable): the parameter value, or %NULL if not found
 */
const gchar *
htmx_request_get_query_param(
	HtmxRequest *self,
	const gchar *name
);

/**
 * htmx_request_get_headers:
 * @self: an #HtmxRequest
 *
 * Gets the parsed HTMX request headers.
 *
 * Returns: (transfer none): the #HtmxRequestHeaders
 */
HtmxRequestHeaders *
htmx_request_get_headers(HtmxRequest *self);

/**
 * htmx_request_is_htmx:
 * @self: an #HtmxRequest
 *
 * Checks if this is an HTMX request (has HX-Request header).
 *
 * Returns: %TRUE if this is an HTMX request
 */
gboolean
htmx_request_is_htmx(HtmxRequest *self);

/**
 * htmx_request_is_boosted:
 * @self: an #HtmxRequest
 *
 * Checks if this is a boosted request (has HX-Boosted header).
 *
 * Returns: %TRUE if this is a boosted request
 */
gboolean
htmx_request_is_boosted(HtmxRequest *self);

/**
 * htmx_request_get_trigger_id:
 * @self: an #HtmxRequest
 *
 * Gets the ID of the element that triggered the request.
 *
 * Returns: (transfer none) (nullable): the trigger element ID, or %NULL
 */
const gchar *
htmx_request_get_trigger_id(HtmxRequest *self);

/**
 * htmx_request_get_target_id:
 * @self: an #HtmxRequest
 *
 * Gets the ID of the target element.
 *
 * Returns: (transfer none) (nullable): the target element ID, or %NULL
 */
const gchar *
htmx_request_get_target_id(HtmxRequest *self);

/**
 * htmx_request_get_body:
 * @self: an #HtmxRequest
 *
 * Gets the request body as a string.
 *
 * Returns: (transfer none) (nullable): the request body, or %NULL if empty
 */
const gchar *
htmx_request_get_body(HtmxRequest *self);

/**
 * htmx_request_get_body_bytes:
 * @self: an #HtmxRequest
 *
 * Gets the request body as bytes.
 *
 * Returns: (transfer none) (nullable): the request body as #GBytes, or %NULL if empty
 */
GBytes *
htmx_request_get_body_bytes(HtmxRequest *self);

/**
 * htmx_request_get_form_data:
 * @self: an #HtmxRequest
 *
 * Gets form data from a POST/PUT request.
 *
 * Returns: (transfer none) (element-type utf8 utf8): the form data
 */
GHashTable *
htmx_request_get_form_data(HtmxRequest *self);

/**
 * htmx_request_get_form_value:
 * @self: an #HtmxRequest
 * @name: the form field name
 *
 * Gets a specific form field value.
 *
 * Returns: (transfer none) (nullable): the field value, or %NULL if not found
 */
const gchar *
htmx_request_get_form_value(
	HtmxRequest *self,
	const gchar *name
);

/**
 * htmx_request_get_json:
 * @self: an #HtmxRequest
 * @error: (nullable): return location for an error
 *
 * Parses the request body as JSON and returns a JsonNode.
 *
 * Returns: (transfer full) (nullable): a #JsonNode, or %NULL on error.
 *          Free with json_node_unref().
 */
JsonNode *
htmx_request_get_json(
	HtmxRequest *self,
	GError     **error
);

/**
 * htmx_request_get_content_type:
 * @self: an #HtmxRequest
 *
 * Gets the Content-Type header value.
 *
 * Returns: (transfer none) (nullable): the content type, or %NULL if not set
 */
const gchar *
htmx_request_get_content_type(HtmxRequest *self);

G_END_DECLS

#endif /* HTMX_REQUEST_H */
