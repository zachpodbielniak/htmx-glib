/*
 * htmx-handler.h - Request handler interface
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef HTMX_HANDLER_H
#define HTMX_HANDLER_H

#include <glib-object.h>
#include "htmx-request.h"
#include "htmx-response.h"

G_BEGIN_DECLS

#define HTMX_TYPE_HANDLER (htmx_handler_get_type())
G_DECLARE_INTERFACE(HtmxHandler, htmx_handler, HTMX, HANDLER, GObject)

/**
 * HtmxHandlerInterface:
 * @handle: Handle an incoming request
 *
 * Interface for request handlers.
 */
struct _HtmxHandlerInterface {
	GTypeInterface parent_iface;

	HtmxResponse *(*handle)(HtmxHandler *self,
	                        HtmxRequest *request,
	                        GHashTable  *params);
};

/**
 * htmx_handler_handle:
 * @self: an #HtmxHandler
 * @request: the incoming #HtmxRequest
 * @params: (element-type utf8 utf8): path parameters
 *
 * Handles a request and returns a response.
 *
 * Returns: (transfer full): an #HtmxResponse
 */
HtmxResponse *
htmx_handler_handle(
	HtmxHandler *self,
	HtmxRequest *request,
	GHashTable  *params
);

G_END_DECLS

#endif /* HTMX_HANDLER_H */
