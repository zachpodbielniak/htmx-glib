/*
 * htmx-router.h - URL router
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef HTMX_ROUTER_H
#define HTMX_ROUTER_H

#include <glib-object.h>
#include "htmx-enums.h"
#include "htmx-request.h"
#include "htmx-response.h"
#include "../htmx-types.h"

G_BEGIN_DECLS

#define HTMX_TYPE_ROUTER (htmx_router_get_type())
G_DECLARE_FINAL_TYPE(HtmxRouter, htmx_router, HTMX, ROUTER, GObject)

/**
 * htmx_router_new:
 *
 * Creates a new #HtmxRouter.
 *
 * Returns: (transfer full): a new #HtmxRouter
 */
HtmxRouter *
htmx_router_new(void);

/**
 * htmx_router_add_route:
 * @self: an #HtmxRouter
 * @method: HTTP method
 * @pattern: URL pattern (supports :param for path parameters)
 * @callback: callback function
 * @user_data: (nullable): user data for callback
 *
 * Adds a route with the given pattern.
 */
void
htmx_router_add_route(
	HtmxRouter       *self,
	HtmxMethod        method,
	const gchar      *pattern,
	HtmxRouteCallback callback,
	gpointer          user_data
);

/**
 * htmx_router_get:
 * @self: an #HtmxRouter
 * @pattern: URL pattern
 * @callback: callback function
 * @user_data: (nullable): user data for callback
 *
 * Adds a GET route.
 */
void
htmx_router_get(
	HtmxRouter       *self,
	const gchar      *pattern,
	HtmxRouteCallback callback,
	gpointer          user_data
);

/**
 * htmx_router_post:
 * @self: an #HtmxRouter
 * @pattern: URL pattern
 * @callback: callback function
 * @user_data: (nullable): user data for callback
 *
 * Adds a POST route.
 */
void
htmx_router_post(
	HtmxRouter       *self,
	const gchar      *pattern,
	HtmxRouteCallback callback,
	gpointer          user_data
);

/**
 * htmx_router_put:
 * @self: an #HtmxRouter
 * @pattern: URL pattern
 * @callback: callback function
 * @user_data: (nullable): user data for callback
 *
 * Adds a PUT route.
 */
void
htmx_router_put(
	HtmxRouter       *self,
	const gchar      *pattern,
	HtmxRouteCallback callback,
	gpointer          user_data
);

/**
 * htmx_router_patch:
 * @self: an #HtmxRouter
 * @pattern: URL pattern
 * @callback: callback function
 * @user_data: (nullable): user data for callback
 *
 * Adds a PATCH route.
 */
void
htmx_router_patch(
	HtmxRouter       *self,
	const gchar      *pattern,
	HtmxRouteCallback callback,
	gpointer          user_data
);

/**
 * htmx_router_delete:
 * @self: an #HtmxRouter
 * @pattern: URL pattern
 * @callback: callback function
 * @user_data: (nullable): user data for callback
 *
 * Adds a DELETE route.
 */
void
htmx_router_delete(
	HtmxRouter       *self,
	const gchar      *pattern,
	HtmxRouteCallback callback,
	gpointer          user_data
);

/**
 * htmx_router_serve_static:
 * @self: an #HtmxRouter
 * @url_prefix: URL prefix (e.g., "/static")
 * @directory: local directory path
 *
 * Serves static files from a directory.
 */
void
htmx_router_serve_static(
	HtmxRouter  *self,
	const gchar *url_prefix,
	const gchar *directory
);

/**
 * htmx_router_match:
 * @self: an #HtmxRouter
 * @request: the incoming request
 * @params: (out) (element-type utf8 utf8) (transfer full): path parameters
 *
 * Matches a request and returns the response.
 *
 * Returns: (transfer full) (nullable): the response, or %NULL if no match
 */
HtmxResponse *
htmx_router_match(
	HtmxRouter  *self,
	HtmxRequest *request,
	GHashTable **params
);

G_END_DECLS

#endif /* HTMX_ROUTER_H */
