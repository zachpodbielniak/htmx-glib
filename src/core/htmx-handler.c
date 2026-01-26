/*
 * htmx-handler.c - Request handler interface implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-handler.h"

G_DEFINE_INTERFACE(HtmxHandler, htmx_handler, G_TYPE_OBJECT)

static void
htmx_handler_default_init(HtmxHandlerInterface *iface)
{
	/* Default implementation is NULL */
}

HtmxResponse *
htmx_handler_handle(
	HtmxHandler *self,
	HtmxRequest *request,
	GHashTable  *params
){
	HtmxHandlerInterface *iface;

	g_return_val_if_fail(HTMX_IS_HANDLER(self), NULL);
	g_return_val_if_fail(HTMX_IS_REQUEST(request), NULL);

	iface = HTMX_HANDLER_GET_IFACE(self);
	g_return_val_if_fail(iface->handle != NULL, NULL);

	return iface->handle(self, request, params);
}
