/*
 * htmx-p.h - HTML paragraph element
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * This file defines the HtmxP class, a typed wrapper for <p> elements.
 */

#ifndef HTMX_P_H
#define HTMX_P_H

#include <glib-object.h>
#include "htmx-node.h"

G_BEGIN_DECLS

#define HTMX_TYPE_P (htmx_p_get_type())
G_DECLARE_FINAL_TYPE(HtmxP, htmx_p, HTMX, P, HtmxNode)

/**
 * htmx_p_new:
 *
 * Creates a new #HtmxP paragraph element.
 *
 * Returns: (transfer full): a new #HtmxP
 */
HtmxP *
htmx_p_new(void);

G_END_DECLS

#endif /* HTMX_P_H */
