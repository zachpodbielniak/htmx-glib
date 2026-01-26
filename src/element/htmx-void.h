/*
 * htmx-void.h - HTML void/empty elements
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * This file defines void elements: br, hr, and wbr.
 */

#ifndef HTMX_VOID_H
#define HTMX_VOID_H

#include <glib-object.h>
#include "htmx-node.h"

G_BEGIN_DECLS

/*
 * HtmxBr - Line break element (void element)
 */

#define HTMX_TYPE_BR (htmx_br_get_type())
G_DECLARE_FINAL_TYPE(HtmxBr, htmx_br, HTMX, BR, HtmxNode)

/**
 * htmx_br_new:
 *
 * Creates a new #HtmxBr line break element.
 *
 * Returns: (transfer full): a new #HtmxBr
 */
HtmxBr *
htmx_br_new(void);

/*
 * HtmxHr - Horizontal rule element (void element)
 */

#define HTMX_TYPE_HR (htmx_hr_get_type())
G_DECLARE_FINAL_TYPE(HtmxHr, htmx_hr, HTMX, HR, HtmxNode)

/**
 * htmx_hr_new:
 *
 * Creates a new #HtmxHr horizontal rule element.
 *
 * Returns: (transfer full): a new #HtmxHr
 */
HtmxHr *
htmx_hr_new(void);

/*
 * HtmxWbr - Word break opportunity element (void element)
 */

#define HTMX_TYPE_WBR (htmx_wbr_get_type())
G_DECLARE_FINAL_TYPE(HtmxWbr, htmx_wbr, HTMX, WBR, HtmxNode)

/**
 * htmx_wbr_new:
 *
 * Creates a new #HtmxWbr word break opportunity element.
 *
 * Returns: (transfer full): a new #HtmxWbr
 */
HtmxWbr *
htmx_wbr_new(void);

G_END_DECLS

#endif /* HTMX_VOID_H */
