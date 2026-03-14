/*
 * htmx-metrics.h - Request metrics and observability
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * HtmxMetrics collects request metrics (count, latency, status codes)
 * and exposes them via accessors and signals. Use as middleware for
 * automatic instrumentation.
 *
 * Signals:
 *   "request-completed" — emitted after each request with path, method,
 *                          status code, and latency in microseconds.
 */

#ifndef HTMX_METRICS_H
#define HTMX_METRICS_H

#include <glib-object.h>
#include "htmx-context.h"
#include "htmx-middleware.h"

G_BEGIN_DECLS

#define HTMX_TYPE_METRICS (htmx_metrics_get_type())
G_DECLARE_FINAL_TYPE(HtmxMetrics, htmx_metrics, HTMX, METRICS, GObject)

/**
 * htmx_metrics_new:
 *
 * Creates a new metrics collector.
 *
 * Returns: (transfer full): a new #HtmxMetrics
 */
HtmxMetrics *
htmx_metrics_new(void);

/**
 * htmx_metrics_get_total_requests:
 * @self: an #HtmxMetrics
 *
 * Returns: the total number of requests processed
 */
guint64
htmx_metrics_get_total_requests(HtmxMetrics *self);

/**
 * htmx_metrics_get_average_latency_us:
 * @self: an #HtmxMetrics
 *
 * Returns: the average request latency in microseconds, or 0 if no requests
 */
guint64
htmx_metrics_get_average_latency_us(HtmxMetrics *self);

/**
 * htmx_metrics_get_status_count:
 * @self: an #HtmxMetrics
 * @status_code: the HTTP status code
 *
 * Returns: the number of responses with this status code
 */
guint64
htmx_metrics_get_status_count(HtmxMetrics *self,
                               guint        status_code);

/**
 * htmx_metrics_get_path_count:
 * @self: an #HtmxMetrics
 * @path: the request path
 *
 * Returns: the number of requests to this path
 */
guint64
htmx_metrics_get_path_count(HtmxMetrics *self,
                             const gchar *path);

/**
 * htmx_metrics_reset:
 * @self: an #HtmxMetrics
 *
 * Resets all counters to zero.
 */
void
htmx_metrics_reset(HtmxMetrics *self);

/**
 * htmx_metrics_middleware:
 * @context: the request context
 * @next: continuation
 * @next_data: (closure next): continuation data
 * @user_data: (closure): the #HtmxMetrics instance
 *
 * Middleware function. Records timing, status codes, and path hits.
 * Emits "request-completed" after each request.
 */
void
htmx_metrics_middleware(HtmxContext        *context,
                         HtmxMiddlewareNext  next,
                         gpointer            next_data,
                         gpointer            user_data);

G_END_DECLS

#endif /* HTMX_METRICS_H */
