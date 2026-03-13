/*
 * htmx-memory-cache.c - In-memory cache implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-memory-cache.h"
#include "htmx-cache.h"

typedef struct {
	gchar  *value;
	gint64  expires_at;  /* monotonic microseconds, or -1 for no expiry */
} CacheEntry;

static void
cache_entry_free(gpointer data)
{
	CacheEntry *entry = (CacheEntry *)data;

	g_free(entry->value);
	g_slice_free(CacheEntry, entry);
}

static gboolean
cache_entry_is_expired(const CacheEntry *entry)
{
	if (entry->expires_at < 0) {
		return FALSE;
	}
	return g_get_monotonic_time() >= entry->expires_at;
}

struct _HtmxMemoryCache {
	GObject     parent_instance;
	GHashTable *entries;  /* gchar* -> CacheEntry* */
};

static void htmx_memory_cache_iface_init(HtmxCacheInterface *iface);

G_DEFINE_FINAL_TYPE_WITH_CODE(HtmxMemoryCache, htmx_memory_cache, G_TYPE_OBJECT,
	G_IMPLEMENT_INTERFACE(HTMX_TYPE_CACHE, htmx_memory_cache_iface_init))

static gchar *
memory_cache_get(HtmxCache *cache, const gchar *key)
{
	HtmxMemoryCache *self = HTMX_MEMORY_CACHE(cache);
	CacheEntry *entry;

	entry = (CacheEntry *)g_hash_table_lookup(self->entries, key);
	if (entry == NULL) {
		return NULL;
	}

	if (cache_entry_is_expired(entry)) {
		g_hash_table_remove(self->entries, key);
		return NULL;
	}

	return g_strdup(entry->value);
}

static void
memory_cache_set(HtmxCache   *cache,
                 const gchar *key,
                 const gchar *value,
                 gint64       ttl_seconds)
{
	HtmxMemoryCache *self = HTMX_MEMORY_CACHE(cache);
	CacheEntry *entry;

	entry = g_slice_new(CacheEntry);
	entry->value = g_strdup(value);

	if (ttl_seconds < 0) {
		entry->expires_at = -1;
	} else {
		entry->expires_at = g_get_monotonic_time() +
		                    (ttl_seconds * G_USEC_PER_SEC);
	}

	g_hash_table_insert(self->entries, g_strdup(key), entry);
}

static gboolean
memory_cache_remove(HtmxCache *cache, const gchar *key)
{
	HtmxMemoryCache *self = HTMX_MEMORY_CACHE(cache);

	return g_hash_table_remove(self->entries, key);
}

static void
memory_cache_clear(HtmxCache *cache)
{
	HtmxMemoryCache *self = HTMX_MEMORY_CACHE(cache);

	g_hash_table_remove_all(self->entries);
}

static gboolean
memory_cache_contains(HtmxCache *cache, const gchar *key)
{
	HtmxMemoryCache *self = HTMX_MEMORY_CACHE(cache);
	CacheEntry *entry;

	entry = (CacheEntry *)g_hash_table_lookup(self->entries, key);
	if (entry == NULL) {
		return FALSE;
	}

	if (cache_entry_is_expired(entry)) {
		g_hash_table_remove(self->entries, key);
		return FALSE;
	}

	return TRUE;
}

static void
htmx_memory_cache_iface_init(HtmxCacheInterface *iface)
{
	iface->get      = memory_cache_get;
	iface->set      = memory_cache_set;
	iface->remove   = memory_cache_remove;
	iface->clear    = memory_cache_clear;
	iface->contains = memory_cache_contains;
}

static void
htmx_memory_cache_finalize(GObject *object)
{
	HtmxMemoryCache *self = HTMX_MEMORY_CACHE(object);

	g_hash_table_unref(self->entries);

	G_OBJECT_CLASS(htmx_memory_cache_parent_class)->finalize(object);
}

static void
htmx_memory_cache_class_init(HtmxMemoryCacheClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);

	object_class->finalize = htmx_memory_cache_finalize;
}

static void
htmx_memory_cache_init(HtmxMemoryCache *self)
{
	self->entries = g_hash_table_new_full(g_str_hash, g_str_equal,
	                                     g_free, cache_entry_free);
}

HtmxMemoryCache *
htmx_memory_cache_new(void)
{
	return g_object_new(HTMX_TYPE_MEMORY_CACHE, NULL);
}

guint
htmx_memory_cache_get_size(HtmxMemoryCache *self)
{
	g_return_val_if_fail(HTMX_IS_MEMORY_CACHE(self), 0);

	return g_hash_table_size(self->entries);
}
