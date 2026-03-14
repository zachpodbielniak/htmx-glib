/*
 * htmx-sync.c - hx-sync attribute model implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-sync.h"
#include <string.h>

struct _HtmxSync {
	gchar            *selector;
	HtmxSyncStrategy  strategy;
};

G_DEFINE_BOXED_TYPE(HtmxSync, htmx_sync, htmx_sync_copy, htmx_sync_free)

HtmxSync *
htmx_sync_new(const gchar       *selector,
               HtmxSyncStrategy   strategy)
{
	HtmxSync *self;

	self = g_slice_new0(HtmxSync);
	self->selector = g_strdup(selector);
	self->strategy = strategy;

	return self;
}

HtmxSync *
htmx_sync_copy(const HtmxSync *self)
{
	g_return_val_if_fail(self != NULL, NULL);

	return htmx_sync_new(self->selector, self->strategy);
}

void
htmx_sync_free(HtmxSync *self)
{
	if (self == NULL)
		return;

	g_free(self->selector);
	g_slice_free(HtmxSync, self);
}

const gchar *
htmx_sync_get_selector(const HtmxSync *self)
{
	g_return_val_if_fail(self != NULL, NULL);

	return self->selector;
}

HtmxSyncStrategy
htmx_sync_get_strategy(const HtmxSync *self)
{
	g_return_val_if_fail(self != NULL, HTMX_SYNC_DROP);

	return self->strategy;
}

static const gchar *
sync_strategy_to_string(HtmxSyncStrategy strategy)
{
	switch (strategy) {
	case HTMX_SYNC_DROP:        return "drop";
	case HTMX_SYNC_ABORT:       return "abort";
	case HTMX_SYNC_REPLACE:     return "replace";
	case HTMX_SYNC_QUEUE_FIRST: return "queue first";
	case HTMX_SYNC_QUEUE_LAST:  return "queue last";
	case HTMX_SYNC_QUEUE_ALL:   return "queue all";
	default:                     return "drop";
	}
}

static HtmxSyncStrategy
sync_strategy_from_string(const gchar *str)
{
	if (str == NULL)
		return HTMX_SYNC_DROP;

	if (g_ascii_strcasecmp(str, "drop") == 0)
		return HTMX_SYNC_DROP;
	if (g_ascii_strcasecmp(str, "abort") == 0)
		return HTMX_SYNC_ABORT;
	if (g_ascii_strcasecmp(str, "replace") == 0)
		return HTMX_SYNC_REPLACE;
	if (g_ascii_strcasecmp(str, "queue first") == 0)
		return HTMX_SYNC_QUEUE_FIRST;
	if (g_ascii_strcasecmp(str, "queue last") == 0)
		return HTMX_SYNC_QUEUE_LAST;
	if (g_ascii_strcasecmp(str, "queue all") == 0)
		return HTMX_SYNC_QUEUE_ALL;

	return (HtmxSyncStrategy)-1;
}

gchar *
htmx_sync_render(const HtmxSync *self)
{
	const gchar *strategy_str;

	g_return_val_if_fail(self != NULL, NULL);

	strategy_str = sync_strategy_to_string(self->strategy);

	if (self->selector != NULL && self->selector[0] != '\0')
		return g_strdup_printf("%s:%s", self->selector, strategy_str);
	else
		return g_strdup(strategy_str);
}

HtmxSync *
htmx_sync_parse(const gchar  *str,
                 GError      **error)
{
	const gchar *colon;
	gchar *selector;
	gchar *strategy_str;
	HtmxSyncStrategy strategy;

	g_return_val_if_fail(str != NULL, NULL);

	/* Find the last colon — strategy is always after the last colon */
	colon = strrchr(str, ':');

	if (colon != NULL) {
		selector = g_strndup(str, (gsize)(colon - str));
		strategy_str = g_strdup(colon + 1);
	} else {
		/* No colon — entire string is the strategy */
		selector = NULL;
		strategy_str = g_strdup(str);
	}

	g_strstrip(strategy_str);
	if (selector != NULL)
		g_strstrip(selector);

	strategy = sync_strategy_from_string(strategy_str);

	if ((gint)strategy == -1) {
		g_set_error(error, HTMX_ERROR, HTMX_ERROR_INVALID_REQUEST,
		            "Unknown sync strategy: %s", strategy_str);
		g_free(selector);
		g_free(strategy_str);
		return NULL;
	}

	g_free(strategy_str);

	/* Treat empty selector as NULL */
	if (selector != NULL && selector[0] == '\0') {
		g_free(selector);
		selector = NULL;
	}

	{
		HtmxSync *result = htmx_sync_new(selector, strategy);
		g_free(selector);
		return result;
	}
}
