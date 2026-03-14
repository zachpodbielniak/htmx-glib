/*
 * htmx-compression.h - Response compression middleware
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * HtmxCompression provides middleware that transparently compresses
 * response bodies using gzip or deflate based on the client's
 * Accept-Encoding header.
 */

#ifndef HTMX_COMPRESSION_H
#define HTMX_COMPRESSION_H

#include <glib-object.h>
#include "htmx-enums.h"
#include "htmx-context.h"
#include "htmx-middleware.h"

G_BEGIN_DECLS

#define HTMX_TYPE_COMPRESSION (htmx_compression_get_type())
G_DECLARE_FINAL_TYPE(HtmxCompression, htmx_compression, HTMX, COMPRESSION, GObject)

/**
 * htmx_compression_new:
 *
 * Creates a new compression middleware with default settings
 * (min size 256 bytes, prefer gzip).
 *
 * Returns: (transfer full): a new #HtmxCompression
 */
HtmxCompression *
htmx_compression_new(void);

/**
 * htmx_compression_set_min_size:
 * @self: an #HtmxCompression
 * @min_size: minimum response body size to compress (bytes)
 *
 * Sets the minimum response size threshold for compression.
 * Responses smaller than this won't be compressed.
 */
void
htmx_compression_set_min_size(HtmxCompression *self,
                               gsize            min_size);

/**
 * htmx_compression_get_min_size:
 * @self: an #HtmxCompression
 *
 * Returns: the minimum size threshold
 */
gsize
htmx_compression_get_min_size(HtmxCompression *self);

/**
 * htmx_compression_middleware:
 * @context: the request context
 * @next: continuation
 * @next_data: (closure next): continuation data
 * @user_data: (closure): the #HtmxCompression instance
 *
 * Middleware function. Register with htmx_server_use() passing
 * the #HtmxCompression instance as user_data.
 */
void
htmx_compression_middleware(HtmxContext        *context,
                             HtmxMiddlewareNext  next,
                             gpointer            next_data,
                             gpointer            user_data);

G_END_DECLS

#endif /* HTMX_COMPRESSION_H */
