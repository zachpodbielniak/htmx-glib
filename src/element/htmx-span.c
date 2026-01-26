/*
 * htmx-span.c - HTML span element implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-span.h"

struct _HtmxSpan {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxSpan, htmx_span, HTMX_TYPE_NODE)

static void
htmx_span_class_init(HtmxSpanClass *klass)
{
}

static void
htmx_span_init(HtmxSpan *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "span");
}

HtmxSpan *
htmx_span_new(void)
{
	return g_object_new(HTMX_TYPE_SPAN, NULL);
}
