/*
 * htmx-middleware.h - Middleware pipeline
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * HtmxMiddleware provides a pipeline of functions that process
 * each request in an "onion" pattern. Each middleware receives the
 * request context and a continuation function. It can:
 *
 *  - Inspect or modify the request (before calling next)
 *  - Call next() to invoke the remaining pipeline
 *  - Inspect or modify the response (after next returns)
 *  - Short-circuit by setting a response without calling next
 */

#ifndef HTMX_MIDDLEWARE_H
#define HTMX_MIDDLEWARE_H

#include <glib-object.h>
#include "htmx-context.h"
#include "../htmx-types.h"

G_BEGIN_DECLS

#define HTMX_TYPE_MIDDLEWARE (htmx_middleware_get_type())
G_DECLARE_FINAL_TYPE(HtmxMiddleware, htmx_middleware, HTMX, MIDDLEWARE, GObject)

/**
 * htmx_middleware_new:
 *
 * Creates a new empty middleware pipeline.
 *
 * Returns: (transfer full): a new #HtmxMiddleware
 */
HtmxMiddleware *
htmx_middleware_new(void);

/**
 * htmx_middleware_use:
 * @self: an #HtmxMiddleware
 * @func: the middleware function
 * @user_data: (nullable): user data for @func
 * @destroy: (nullable): destroy function for @user_data
 *
 * Appends a middleware function to the end of the pipeline.
 * Middleware added later runs later (closer to the handler).
 */
void
htmx_middleware_use(
	HtmxMiddleware     *self,
	HtmxMiddlewareFunc  func,
	gpointer            user_data,
	GDestroyNotify      destroy
);

/**
 * htmx_middleware_use_before:
 * @self: an #HtmxMiddleware
 * @func: the middleware function
 * @user_data: (nullable): user data for @func
 * @destroy: (nullable): destroy function for @user_data
 *
 * Prepends a middleware function to the beginning of the pipeline.
 * Middleware added this way runs first (outermost layer).
 */
void
htmx_middleware_use_before(
	HtmxMiddleware     *self,
	HtmxMiddlewareFunc  func,
	gpointer            user_data,
	GDestroyNotify      destroy
);

/**
 * htmx_middleware_run:
 * @self: an #HtmxMiddleware
 * @context: the request context
 * @handler: (nullable): terminal handler called after all middleware
 * @handler_data: (nullable): user data for @handler
 *
 * Runs the middleware pipeline on the given context.
 *
 * The pipeline executes in an "onion" pattern: each middleware
 * wraps the next. After all middleware have run, @handler is
 * called as the terminal step (typically the router dispatch).
 *
 * If @handler is %NULL, the pipeline simply ends after the last
 * middleware.
 */
void
htmx_middleware_run(
	HtmxMiddleware     *self,
	HtmxContext        *context,
	HtmxMiddlewareFunc  handler,
	gpointer            handler_data
);

/**
 * htmx_middleware_get_length:
 * @self: an #HtmxMiddleware
 *
 * Returns the number of middleware functions in the pipeline.
 *
 * Returns: the number of middleware
 */
guint
htmx_middleware_get_length(HtmxMiddleware *self);

G_END_DECLS

#endif /* HTMX_MIDDLEWARE_H */
