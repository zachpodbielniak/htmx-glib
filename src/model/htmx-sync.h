/*
 * htmx-sync.h - hx-sync attribute model
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * HtmxSync is a boxed type representing the hx-sync attribute which
 * controls how requests are synchronized when multiple are triggered
 * from the same element or a related element.
 *
 * Format: "selector:strategy" e.g. "closest form:abort"
 */

#ifndef HTMX_SYNC_H
#define HTMX_SYNC_H

#include <glib-object.h>
#include "core/htmx-enums.h"
#include "core/htmx-error.h"

G_BEGIN_DECLS

#define HTMX_TYPE_SYNC (htmx_sync_get_type())

typedef struct _HtmxSync HtmxSync;

GType htmx_sync_get_type(void) G_GNUC_CONST;

/**
 * htmx_sync_new:
 * @selector: (nullable): CSS selector for sync target, or %NULL for "this"
 * @strategy: the synchronization strategy
 *
 * Creates a new #HtmxSync.
 *
 * Returns: (transfer full): a new #HtmxSync
 */
HtmxSync *
htmx_sync_new(const gchar       *selector,
               HtmxSyncStrategy   strategy);

/**
 * htmx_sync_copy:
 * @self: an #HtmxSync
 *
 * Creates a deep copy.
 *
 * Returns: (transfer full): a copy of @self
 */
HtmxSync *
htmx_sync_copy(const HtmxSync *self);

/**
 * htmx_sync_free:
 * @self: an #HtmxSync
 *
 * Frees the sync specification.
 */
void
htmx_sync_free(HtmxSync *self);

G_DEFINE_AUTOPTR_CLEANUP_FUNC(HtmxSync, htmx_sync_free)

/**
 * htmx_sync_get_selector:
 * @self: an #HtmxSync
 *
 * Gets the CSS selector for the sync target.
 *
 * Returns: (transfer none) (nullable): the selector, or %NULL for "this"
 */
const gchar *
htmx_sync_get_selector(const HtmxSync *self);

/**
 * htmx_sync_get_strategy:
 * @self: an #HtmxSync
 *
 * Gets the synchronization strategy.
 *
 * Returns: the #HtmxSyncStrategy
 */
HtmxSyncStrategy
htmx_sync_get_strategy(const HtmxSync *self);

/**
 * htmx_sync_render:
 * @self: an #HtmxSync
 *
 * Renders the sync specification as an hx-sync attribute value.
 * e.g. "closest form:abort", "this:drop", ":replace"
 *
 * Returns: (transfer full): the rendered string
 */
gchar *
htmx_sync_render(const HtmxSync *self);

/**
 * htmx_sync_parse:
 * @str: the hx-sync attribute value to parse
 * @error: (nullable): return location for an error
 *
 * Parses an hx-sync attribute value.
 *
 * Returns: (transfer full) (nullable): a new #HtmxSync, or %NULL on error
 */
HtmxSync *
htmx_sync_parse(const gchar  *str,
                 GError      **error);

G_END_DECLS

#endif /* HTMX_SYNC_H */
