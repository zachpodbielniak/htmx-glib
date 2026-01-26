/*
 * htmx-a.h - HTML anchor element
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * This file defines the HtmxA class, a typed wrapper for <a> elements.
 */

#ifndef HTMX_A_H
#define HTMX_A_H

#include <glib-object.h>
#include "htmx-node.h"

G_BEGIN_DECLS

#define HTMX_TYPE_A (htmx_a_get_type())
G_DECLARE_FINAL_TYPE(HtmxA, htmx_a, HTMX, A, HtmxNode)

/**
 * htmx_a_new:
 *
 * Creates a new #HtmxA anchor element.
 *
 * Returns: (transfer full): a new #HtmxA
 */
HtmxA *
htmx_a_new(void);

/**
 * htmx_a_new_with_href:
 * @href: the href attribute value
 *
 * Creates a new #HtmxA anchor element with the specified href.
 *
 * Returns: (transfer full): a new #HtmxA
 */
HtmxA *
htmx_a_new_with_href(const gchar *href);

/**
 * htmx_a_set_href:
 * @self: an #HtmxA
 * @href: the href attribute value
 *
 * Sets the href attribute.
 */
void
htmx_a_set_href(
	HtmxA       *self,
	const gchar *href
);

/**
 * htmx_a_set_target:
 * @self: an #HtmxA
 * @target: the target attribute value (e.g., "_blank", "_self")
 *
 * Sets the target attribute.
 */
void
htmx_a_set_target(
	HtmxA       *self,
	const gchar *target
);

/**
 * htmx_a_set_rel:
 * @self: an #HtmxA
 * @rel: the rel attribute value (e.g., "noopener", "noreferrer")
 *
 * Sets the rel attribute.
 */
void
htmx_a_set_rel(
	HtmxA       *self,
	const gchar *rel
);

G_END_DECLS

#endif /* HTMX_A_H */
