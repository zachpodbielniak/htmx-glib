/*
 * htmx-deflist.h - HTML definition list elements
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * This file defines definition list elements: dl, dt, and dd.
 */

#ifndef HTMX_DEFLIST_H
#define HTMX_DEFLIST_H

#include <glib-object.h>
#include "htmx-node.h"

G_BEGIN_DECLS

/*
 * HtmxDl - Definition list element
 */

#define HTMX_TYPE_DL (htmx_dl_get_type())
G_DECLARE_FINAL_TYPE(HtmxDl, htmx_dl, HTMX, DL, HtmxNode)

/**
 * htmx_dl_new:
 *
 * Creates a new #HtmxDl definition list element.
 *
 * Returns: (transfer full): a new #HtmxDl
 */
HtmxDl *
htmx_dl_new(void);

/*
 * HtmxDt - Definition term element
 */

#define HTMX_TYPE_DT (htmx_dt_get_type())
G_DECLARE_FINAL_TYPE(HtmxDt, htmx_dt, HTMX, DT, HtmxNode)

/**
 * htmx_dt_new:
 *
 * Creates a new #HtmxDt definition term element.
 *
 * Returns: (transfer full): a new #HtmxDt
 */
HtmxDt *
htmx_dt_new(void);

/**
 * htmx_dt_new_with_text:
 * @text: the term text
 *
 * Creates a new #HtmxDt with text content.
 *
 * Returns: (transfer full): a new #HtmxDt
 */
HtmxDt *
htmx_dt_new_with_text(const gchar *text);

/*
 * HtmxDd - Definition description element
 */

#define HTMX_TYPE_DD (htmx_dd_get_type())
G_DECLARE_FINAL_TYPE(HtmxDd, htmx_dd, HTMX, DD, HtmxNode)

/**
 * htmx_dd_new:
 *
 * Creates a new #HtmxDd definition description element.
 *
 * Returns: (transfer full): a new #HtmxDd
 */
HtmxDd *
htmx_dd_new(void);

/**
 * htmx_dd_new_with_text:
 * @text: the description text
 *
 * Creates a new #HtmxDd with text content.
 *
 * Returns: (transfer full): a new #HtmxDd
 */
HtmxDd *
htmx_dd_new_with_text(const gchar *text);

G_END_DECLS

#endif /* HTMX_DEFLIST_H */
