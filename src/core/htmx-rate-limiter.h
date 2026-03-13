/*
 * htmx-rate-limiter.h - Token bucket rate limiter
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * HtmxRateLimiter implements a per-client token bucket rate limiter.
 * Each client (identified by IP address) gets a bucket that refills
 * at a fixed rate. When the bucket is empty, requests are rejected
 * with HTTP 429 Too Many Requests.
 *
 * Use htmx_rate_limiter_middleware() with htmx_server_use() to
 * apply rate limiting to all requests.
 */

#ifndef HTMX_RATE_LIMITER_H
#define HTMX_RATE_LIMITER_H

#include <glib-object.h>
#include "htmx-context.h"
#include "../htmx-types.h"

G_BEGIN_DECLS

#define HTMX_TYPE_RATE_LIMITER (htmx_rate_limiter_get_type())
G_DECLARE_FINAL_TYPE(HtmxRateLimiter, htmx_rate_limiter, HTMX, RATE_LIMITER, GObject)

/**
 * htmx_rate_limiter_new:
 * @max_tokens: maximum tokens per bucket (burst capacity)
 * @refill_rate: tokens added per second
 *
 * Creates a new rate limiter. Each client starts with a full
 * bucket of @max_tokens. Tokens are consumed on each request
 * and replenished at @refill_rate tokens per second.
 *
 * Returns: (transfer full): a new #HtmxRateLimiter
 */
HtmxRateLimiter *
htmx_rate_limiter_new(guint max_tokens, gdouble refill_rate);

/**
 * htmx_rate_limiter_allow:
 * @self: an #HtmxRateLimiter
 * @client_id: identifier for the client (typically IP address)
 *
 * Checks if a request from @client_id is allowed and consumes
 * one token if so.
 *
 * Returns: %TRUE if the request is allowed
 */
gboolean
htmx_rate_limiter_allow(HtmxRateLimiter *self, const gchar *client_id);

/**
 * htmx_rate_limiter_middleware:
 * @context: the request context
 * @next: continuation function
 * @next_data: continuation data
 * @user_data: (type HtmxRateLimiter): the rate limiter
 *
 * Middleware function for rate limiting. Checks the client's
 * IP address against the rate limiter and returns 429 Too Many
 * Requests if the limit is exceeded.
 *
 * Register with: htmx_server_use(server, htmx_rate_limiter_middleware,
 *                                g_object_ref(limiter), g_object_unref)
 */
void
htmx_rate_limiter_middleware(
	HtmxContext        *context,
	HtmxMiddlewareNext  next,
	gpointer            next_data,
	gpointer            user_data
);

G_END_DECLS

#endif /* HTMX_RATE_LIMITER_H */
