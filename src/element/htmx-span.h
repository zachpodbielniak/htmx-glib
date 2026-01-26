/*
 * htmx-span.h - HTML span element
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * This file defines the HtmxSpan class, a typed wrapper for <span> elements.
 */

#ifndef HTMX_SPAN_H
#define HTMX_SPAN_H

#include <glib-object.h>
#include "htmx-node.h"

G_BEGIN_DECLS

#define HTMX_TYPE_SPAN (htmx_span_get_type())
G_DECLARE_FINAL_TYPE(HtmxSpan, htmx_span, HTMX, SPAN, HtmxNode)

/**
 * htmx_span_new:
 *
 * Creates a new #HtmxSpan element.
 *
 * Returns: (transfer full): a new #HtmxSpan
 */
HtmxSpan *
htmx_span_new(void);

G_END_DECLS

#endif /* HTMX_SPAN_H */
