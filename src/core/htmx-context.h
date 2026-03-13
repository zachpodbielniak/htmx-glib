/*
 * htmx-context.h - Per-request context
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * HtmxContext carries request state through the middleware pipeline.
 * It provides a typed bag for request, response, route parameters,
 * and arbitrary user data (session, auth info, etc.).
 */

#ifndef HTMX_CONTEXT_H
#define HTMX_CONTEXT_H

#include <glib-object.h>
#include "htmx-request.h"
#include "htmx-response.h"

G_BEGIN_DECLS

#define HTMX_TYPE_CONTEXT (htmx_context_get_type())
G_DECLARE_FINAL_TYPE(HtmxContext, htmx_context, HTMX, CONTEXT, GObject)

/**
 * htmx_context_new:
 * @request: the incoming #HtmxRequest
 *
 * Creates a new #HtmxContext for the given request.
 *
 * Returns: (transfer full): a new #HtmxContext
 */
HtmxContext *
htmx_context_new(HtmxRequest *request);

/**
 * htmx_context_get_request:
 * @self: an #HtmxContext
 *
 * Gets the request associated with this context.
 *
 * Returns: (transfer none): the #HtmxRequest
 */
HtmxRequest *
htmx_context_get_request(HtmxContext *self);

/**
 * htmx_context_get_response:
 * @self: an #HtmxContext
 *
 * Gets the response, if one has been set by middleware or a handler.
 *
 * Returns: (transfer none) (nullable): the #HtmxResponse, or %NULL
 */
HtmxResponse *
htmx_context_get_response(HtmxContext *self);

/**
 * htmx_context_set_response:
 * @self: an #HtmxContext
 * @response: (transfer full): the #HtmxResponse to set
 *
 * Sets the response for this context. Takes ownership of @response.
 * Replaces any previously set response.
 */
void
htmx_context_set_response(
	HtmxContext  *self,
	HtmxResponse *response
);

/**
 * htmx_context_get_params:
 * @self: an #HtmxContext
 *
 * Gets the route parameters extracted by the router.
 * May be %NULL if routing has not yet occurred.
 *
 * Returns: (transfer none) (nullable) (element-type utf8 utf8): route parameters
 */
GHashTable *
htmx_context_get_params(HtmxContext *self);

/**
 * htmx_context_set_params:
 * @self: an #HtmxContext
 * @params: (transfer full) (element-type utf8 utf8): route parameters
 *
 * Sets the route parameters. Takes ownership of @params.
 */
void
htmx_context_set_params(
	HtmxContext *self,
	GHashTable  *params
);

/**
 * htmx_context_get_param:
 * @self: an #HtmxContext
 * @name: the parameter name
 *
 * Gets a specific route parameter by name.
 *
 * Returns: (transfer none) (nullable): the parameter value, or %NULL
 */
const gchar *
htmx_context_get_param(
	HtmxContext *self,
	const gchar *name
);

/**
 * htmx_context_set_data:
 * @self: an #HtmxContext
 * @key: a string key
 * @data: (nullable): the data to store
 * @destroy: (nullable): destroy function for @data
 *
 * Stores arbitrary data on the context, keyed by @key.
 * If data already exists for @key, the old data's destroy function
 * is called before replacement.
 */
void
htmx_context_set_data(
	HtmxContext    *self,
	const gchar    *key,
	gpointer        data,
	GDestroyNotify  destroy
);

/**
 * htmx_context_get_data:
 * @self: an #HtmxContext
 * @key: a string key
 *
 * Retrieves data previously stored with htmx_context_set_data().
 *
 * Returns: (transfer none) (nullable): the stored data, or %NULL
 */
gpointer
htmx_context_get_data(
	HtmxContext *self,
	const gchar *key
);

G_END_DECLS

#endif /* HTMX_CONTEXT_H */
