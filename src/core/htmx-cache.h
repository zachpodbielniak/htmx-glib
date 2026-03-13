/*
 * htmx-cache.h - Cache interface
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * HtmxCache is an interface for key-value caching with optional TTL.
 * Implement this interface to provide custom cache backends (Redis,
 * memcached, file-based, etc.). Use HtmxMemoryCache for an in-process
 * GHashTable-backed implementation.
 */

#ifndef HTMX_CACHE_H
#define HTMX_CACHE_H

#include <glib-object.h>

G_BEGIN_DECLS

#define HTMX_TYPE_CACHE (htmx_cache_get_type())
G_DECLARE_INTERFACE(HtmxCache, htmx_cache, HTMX, CACHE, GObject)

/**
 * HtmxCacheInterface:
 * @parent_iface: parent interface
 * @get: retrieve a cached value by key
 * @set: store a value with optional TTL in seconds (-1 = no expiry)
 * @remove: remove a cached entry
 * @clear: remove all cached entries
 * @contains: check if a key exists and is not expired
 *
 * Interface for cache implementations.
 */
struct _HtmxCacheInterface {
	GTypeInterface parent_iface;

	gchar    *(*get)      (HtmxCache   *self,
	                       const gchar *key);
	void      (*set)      (HtmxCache   *self,
	                       const gchar *key,
	                       const gchar *value,
	                       gint64       ttl_seconds);
	gboolean  (*remove)   (HtmxCache   *self,
	                       const gchar *key);
	void      (*clear)    (HtmxCache   *self);
	gboolean  (*contains) (HtmxCache   *self,
	                       const gchar *key);
};

/**
 * htmx_cache_get:
 * @self: an #HtmxCache
 * @key: the cache key
 *
 * Retrieves a value from the cache.
 *
 * Returns: (transfer full) (nullable): the cached value, or %NULL if
 *   not found or expired
 */
gchar *
htmx_cache_get(HtmxCache *self, const gchar *key);

/**
 * htmx_cache_set:
 * @self: an #HtmxCache
 * @key: the cache key
 * @value: the value to cache
 * @ttl_seconds: time-to-live in seconds, or -1 for no expiry
 *
 * Stores a value in the cache with an optional TTL.
 */
void
htmx_cache_set(HtmxCache   *self,
               const gchar *key,
               const gchar *value,
               gint64       ttl_seconds);

/**
 * htmx_cache_remove:
 * @self: an #HtmxCache
 * @key: the cache key
 *
 * Removes an entry from the cache.
 *
 * Returns: %TRUE if the key was found and removed
 */
gboolean
htmx_cache_remove(HtmxCache *self, const gchar *key);

/**
 * htmx_cache_clear:
 * @self: an #HtmxCache
 *
 * Removes all entries from the cache.
 */
void
htmx_cache_clear(HtmxCache *self);

/**
 * htmx_cache_contains:
 * @self: an #HtmxCache
 * @key: the cache key
 *
 * Checks if a key exists in the cache and has not expired.
 *
 * Returns: %TRUE if the key exists and is valid
 */
gboolean
htmx_cache_contains(HtmxCache *self, const gchar *key);

G_END_DECLS

#endif /* HTMX_CACHE_H */
