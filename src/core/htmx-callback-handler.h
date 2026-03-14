/*
 * htmx-callback-handler.h - Closure-based handler implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * HtmxCallbackHandler implements #HtmxHandler by wrapping a plain
 * #HtmxRouteCallback function pointer. This bridges the functional
 * route callback API with the GObject handler interface.
 */

#ifndef HTMX_CALLBACK_HANDLER_H
#define HTMX_CALLBACK_HANDLER_H

#include <glib-object.h>
#include "htmx-handler.h"
#include "htmx-request.h"
#include "htmx-response.h"
#include "../htmx-types.h"

G_BEGIN_DECLS

#define HTMX_TYPE_CALLBACK_HANDLER (htmx_callback_handler_get_type())
G_DECLARE_FINAL_TYPE(HtmxCallbackHandler, htmx_callback_handler, HTMX, CALLBACK_HANDLER, GObject)

/**
 * htmx_callback_handler_new:
 * @callback: the route callback function
 * @user_data: (nullable) (closure callback): user data for @callback
 * @destroy: (nullable): destroy function for @user_data
 *
 * Creates a new #HtmxCallbackHandler that wraps a function pointer.
 * The handler implements #HtmxHandler by delegating to @callback.
 *
 * Returns: (transfer full): a new #HtmxCallbackHandler
 */
HtmxCallbackHandler *
htmx_callback_handler_new(HtmxRouteCallback  callback,
                           gpointer           user_data,
                           GDestroyNotify     destroy);

G_END_DECLS

#endif /* HTMX_CALLBACK_HANDLER_H */
