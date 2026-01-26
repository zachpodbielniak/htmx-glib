/*
 * htmx-heading.h - HTML heading elements (h1-h6)
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * This file defines the HtmxHeading class, a typed wrapper for <h1>-<h6> elements.
 */

#ifndef HTMX_HEADING_H
#define HTMX_HEADING_H

#include <glib-object.h>
#include "htmx-node.h"

G_BEGIN_DECLS

#define HTMX_TYPE_HEADING (htmx_heading_get_type())
G_DECLARE_FINAL_TYPE(HtmxHeading, htmx_heading, HTMX, HEADING, HtmxNode)

/**
 * htmx_heading_new:
 * @level: heading level (1-6)
 *
 * Creates a new #HtmxHeading element with the specified level.
 * Level must be between 1 and 6 (inclusive).
 *
 * Returns: (transfer full): a new #HtmxHeading
 */
HtmxHeading *
htmx_heading_new(guint level);

/**
 * htmx_heading_get_level:
 * @self: an #HtmxHeading
 *
 * Gets the heading level.
 *
 * Returns: the heading level (1-6)
 */
guint
htmx_heading_get_level(HtmxHeading *self);

/**
 * htmx_h1_new:
 *
 * Convenience function to create an h1 element.
 *
 * Returns: (transfer full): a new #HtmxHeading with level 1
 */
HtmxHeading *
htmx_h1_new(void);

/**
 * htmx_h2_new:
 *
 * Convenience function to create an h2 element.
 *
 * Returns: (transfer full): a new #HtmxHeading with level 2
 */
HtmxHeading *
htmx_h2_new(void);

/**
 * htmx_h3_new:
 *
 * Convenience function to create an h3 element.
 *
 * Returns: (transfer full): a new #HtmxHeading with level 3
 */
HtmxHeading *
htmx_h3_new(void);

/**
 * htmx_h4_new:
 *
 * Convenience function to create an h4 element.
 *
 * Returns: (transfer full): a new #HtmxHeading with level 4
 */
HtmxHeading *
htmx_h4_new(void);

/**
 * htmx_h5_new:
 *
 * Convenience function to create an h5 element.
 *
 * Returns: (transfer full): a new #HtmxHeading with level 5
 */
HtmxHeading *
htmx_h5_new(void);

/**
 * htmx_h6_new:
 *
 * Convenience function to create an h6 element.
 *
 * Returns: (transfer full): a new #HtmxHeading with level 6
 */
HtmxHeading *
htmx_h6_new(void);

G_END_DECLS

#endif /* HTMX_HEADING_H */
