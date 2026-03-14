/*
 * htmx-error.c - HTMX error domain implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-error.h"

/*
 * Error messages for each error code
 */
static const gchar *error_messages[] = {
	"Invalid request",
	"Invalid response",
	"Invalid HTMX header",
	"Invalid trigger specification",
	"Invalid swap specification",
	"Invalid target selector",
	"Invalid HTML element",
	"Route not found",
	"Method not allowed",
	"Internal server error",
	"Connection failed",
	"SSE error",
	"WebSocket error",
	"Template error",
	"Configuration error",
	"Upload error",
	"Compression error",
	"Validation error"
};

/*
 * Get the error quark for htmx-glib
 */
GQuark
htmx_error_quark(void)
{
	return g_quark_from_static_string("htmx-error-quark");
}

/*
 * GType registration for HtmxError
 */
GType
htmx_error_get_type(void)
{
	static gsize g_type_id = 0;

	if (g_once_init_enter(&g_type_id)) {
		static const GEnumValue values[] = {
			{ HTMX_ERROR_INVALID_REQUEST, "HTMX_ERROR_INVALID_REQUEST", "invalid-request" },
			{ HTMX_ERROR_INVALID_RESPONSE, "HTMX_ERROR_INVALID_RESPONSE", "invalid-response" },
			{ HTMX_ERROR_INVALID_HEADER, "HTMX_ERROR_INVALID_HEADER", "invalid-header" },
			{ HTMX_ERROR_INVALID_TRIGGER, "HTMX_ERROR_INVALID_TRIGGER", "invalid-trigger" },
			{ HTMX_ERROR_INVALID_SWAP, "HTMX_ERROR_INVALID_SWAP", "invalid-swap" },
			{ HTMX_ERROR_INVALID_TARGET, "HTMX_ERROR_INVALID_TARGET", "invalid-target" },
			{ HTMX_ERROR_INVALID_ELEMENT, "HTMX_ERROR_INVALID_ELEMENT", "invalid-element" },
			{ HTMX_ERROR_ROUTE_NOT_FOUND, "HTMX_ERROR_ROUTE_NOT_FOUND", "route-not-found" },
			{ HTMX_ERROR_METHOD_NOT_ALLOWED, "HTMX_ERROR_METHOD_NOT_ALLOWED", "method-not-allowed" },
			{ HTMX_ERROR_SERVER_ERROR, "HTMX_ERROR_SERVER_ERROR", "server-error" },
			{ HTMX_ERROR_CONNECTION_FAILED, "HTMX_ERROR_CONNECTION_FAILED", "connection-failed" },
			{ HTMX_ERROR_SSE_ERROR, "HTMX_ERROR_SSE_ERROR", "sse-error" },
			{ HTMX_ERROR_WEBSOCKET_ERROR, "HTMX_ERROR_WEBSOCKET_ERROR", "websocket-error" },
			{ HTMX_ERROR_TEMPLATE_ERROR, "HTMX_ERROR_TEMPLATE_ERROR", "template-error" },
			{ HTMX_ERROR_CONFIG_ERROR, "HTMX_ERROR_CONFIG_ERROR", "config-error" },
			{ HTMX_ERROR_UPLOAD_ERROR, "HTMX_ERROR_UPLOAD_ERROR", "upload-error" },
			{ HTMX_ERROR_COMPRESSION_ERROR, "HTMX_ERROR_COMPRESSION_ERROR", "compression-error" },
			{ HTMX_ERROR_VALIDATION_ERROR, "HTMX_ERROR_VALIDATION_ERROR", "validation-error" },
			{ 0, NULL, NULL }
		};
		GType type_id = g_enum_register_static("HtmxError", values);
		g_once_init_leave(&g_type_id, type_id);
	}

	return (GType)g_type_id;
}

/*
 * Convert error code to string description
 */
const gchar *
htmx_error_to_string(HtmxError error)
{
	g_return_val_if_fail(error >= HTMX_ERROR_INVALID_REQUEST && error <= HTMX_ERROR_VALIDATION_ERROR, NULL);

	return error_messages[error];
}
