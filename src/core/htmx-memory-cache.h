/*
 * htmx-memory-cache.h - In-memory cache with TTL support
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * HtmxMemoryCache is an in-process GHashTable-backed implementation
 * of the HtmxCache interface. Entries are lazily evicted on access
 * when their TTL expires.
 */

#ifndef HTMX_MEMORY_CACHE_H
#define HTMX_MEMORY_CACHE_H

#include <glib-object.h>

G_BEGIN_DECLS

#define HTMX_TYPE_MEMORY_CACHE (htmx_memory_cache_get_type())
G_DECLARE_FINAL_TYPE(HtmxMemoryCache, htmx_memory_cache, HTMX, MEMORY_CACHE, GObject)

/**
 * htmx_memory_cache_new:
 *
 * Creates a new in-memory cache.
 *
 * Returns: (transfer full): a new #HtmxMemoryCache
 */
HtmxMemoryCache *
htmx_memory_cache_new(void);

/**
 * htmx_memory_cache_get_size:
 * @self: an #HtmxMemoryCache
 *
 * Returns the number of entries currently in the cache,
 * including expired entries that have not yet been evicted.
 *
 * Returns: the number of entries
 */
guint
htmx_memory_cache_get_size(HtmxMemoryCache *self);

G_END_DECLS

#endif /* HTMX_MEMORY_CACHE_H */
