/*
 * htmx-div.h - HTML div element
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * This file defines the HtmxDiv class, a typed wrapper for <div> elements.
 */

#ifndef HTMX_DIV_H
#define HTMX_DIV_H

#include <glib-object.h>
#include "htmx-node.h"

G_BEGIN_DECLS

#define HTMX_TYPE_DIV (htmx_div_get_type())
G_DECLARE_FINAL_TYPE(HtmxDiv, htmx_div, HTMX, DIV, HtmxNode)

/**
 * htmx_div_new:
 *
 * Creates a new #HtmxDiv element.
 *
 * Returns: (transfer full): a new #HtmxDiv
 */
HtmxDiv *
htmx_div_new(void);

G_END_DECLS

#endif /* HTMX_DIV_H */
