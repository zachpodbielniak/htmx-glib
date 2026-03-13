/*
 * htmx-cache.c - Cache interface implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-cache.h"

G_DEFINE_INTERFACE(HtmxCache, htmx_cache, G_TYPE_OBJECT)

static void
htmx_cache_default_init(HtmxCacheInterface *iface)
{
	(void)iface;
}

gchar *
htmx_cache_get(HtmxCache *self, const gchar *key)
{
	HtmxCacheInterface *iface;

	g_return_val_if_fail(HTMX_IS_CACHE(self), NULL);
	g_return_val_if_fail(key != NULL, NULL);

	iface = HTMX_CACHE_GET_IFACE(self);
	g_return_val_if_fail(iface->get != NULL, NULL);

	return iface->get(self, key);
}

void
htmx_cache_set(HtmxCache   *self,
               const gchar *key,
               const gchar *value,
               gint64       ttl_seconds)
{
	HtmxCacheInterface *iface;

	g_return_if_fail(HTMX_IS_CACHE(self));
	g_return_if_fail(key != NULL);
	g_return_if_fail(value != NULL);

	iface = HTMX_CACHE_GET_IFACE(self);
	g_return_if_fail(iface->set != NULL);

	iface->set(self, key, value, ttl_seconds);
}

gboolean
htmx_cache_remove(HtmxCache *self, const gchar *key)
{
	HtmxCacheInterface *iface;

	g_return_val_if_fail(HTMX_IS_CACHE(self), FALSE);
	g_return_val_if_fail(key != NULL, FALSE);

	iface = HTMX_CACHE_GET_IFACE(self);
	g_return_val_if_fail(iface->remove != NULL, FALSE);

	return iface->remove(self, key);
}

void
htmx_cache_clear(HtmxCache *self)
{
	HtmxCacheInterface *iface;

	g_return_if_fail(HTMX_IS_CACHE(self));

	iface = HTMX_CACHE_GET_IFACE(self);
	g_return_if_fail(iface->clear != NULL);

	iface->clear(self);
}

gboolean
htmx_cache_contains(HtmxCache *self, const gchar *key)
{
	HtmxCacheInterface *iface;

	g_return_val_if_fail(HTMX_IS_CACHE(self), FALSE);
	g_return_val_if_fail(key != NULL, FALSE);

	iface = HTMX_CACHE_GET_IFACE(self);
	g_return_val_if_fail(iface->contains != NULL, FALSE);

	return iface->contains(self, key);
}
