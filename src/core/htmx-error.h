/*
 * htmx-error.h - HTMX error domain and codes
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * This file defines the error domain and error codes for htmx-glib.
 */

#ifndef HTMX_ERROR_H
#define HTMX_ERROR_H

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

/**
 * HTMX_ERROR:
 *
 * Error domain for htmx-glib errors. Errors in this domain will be from
 * the #HtmxError enumeration. See #GError for information on error domains.
 */
#define HTMX_ERROR (htmx_error_quark())

/**
 * HtmxError:
 * @HTMX_ERROR_INVALID_REQUEST: The request is malformed or invalid
 * @HTMX_ERROR_INVALID_RESPONSE: The response could not be generated
 * @HTMX_ERROR_INVALID_HEADER: An HTMX header has an invalid value
 * @HTMX_ERROR_INVALID_TRIGGER: A trigger specification is invalid
 * @HTMX_ERROR_INVALID_SWAP: A swap specification is invalid
 * @HTMX_ERROR_INVALID_TARGET: A target selector is invalid
 * @HTMX_ERROR_INVALID_ELEMENT: An HTML element is malformed
 * @HTMX_ERROR_ROUTE_NOT_FOUND: No route matches the request path
 * @HTMX_ERROR_METHOD_NOT_ALLOWED: The HTTP method is not allowed for the route
 * @HTMX_ERROR_SERVER_ERROR: An internal server error occurred
 * @HTMX_ERROR_CONNECTION_FAILED: A connection could not be established
 * @HTMX_ERROR_SSE_ERROR: An SSE-related error occurred
 * @HTMX_ERROR_WEBSOCKET_ERROR: A WebSocket-related error occurred
 * @HTMX_ERROR_TEMPLATE_ERROR: A template parsing or rendering error occurred
 * @HTMX_ERROR_CONFIG_ERROR: A configuration error occurred
 *
 * Error codes returned by htmx-glib functions.
 */
typedef enum {
	HTMX_ERROR_INVALID_REQUEST = 0,
	HTMX_ERROR_INVALID_RESPONSE,
	HTMX_ERROR_INVALID_HEADER,
	HTMX_ERROR_INVALID_TRIGGER,
	HTMX_ERROR_INVALID_SWAP,
	HTMX_ERROR_INVALID_TARGET,
	HTMX_ERROR_INVALID_ELEMENT,
	HTMX_ERROR_ROUTE_NOT_FOUND,
	HTMX_ERROR_METHOD_NOT_ALLOWED,
	HTMX_ERROR_SERVER_ERROR,
	HTMX_ERROR_CONNECTION_FAILED,
	HTMX_ERROR_SSE_ERROR,
	HTMX_ERROR_WEBSOCKET_ERROR,
	HTMX_ERROR_TEMPLATE_ERROR,
	HTMX_ERROR_CONFIG_ERROR
} HtmxError;

GType htmx_error_get_type(void) G_GNUC_CONST;
#define HTMX_TYPE_ERROR (htmx_error_get_type())

/**
 * htmx_error_quark:
 *
 * Gets the #GQuark representing the htmx-glib error domain.
 *
 * Returns: a #GQuark representing the htmx-glib error domain
 */
GQuark
htmx_error_quark(void);

/**
 * htmx_error_to_string:
 * @error: an #HtmxError code
 *
 * Gets a human-readable description of an error code.
 *
 * Returns: (transfer none): a description of the error
 */
const gchar *
htmx_error_to_string(HtmxError error);

G_END_DECLS

#endif /* HTMX_ERROR_H */
