/*
 * htmx-div.c - HTML div element implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-div.h"

struct _HtmxDiv {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxDiv, htmx_div, HTMX_TYPE_NODE)

static void
htmx_div_class_init(HtmxDivClass *klass)
{
}

static void
htmx_div_init(HtmxDiv *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "div");
}

HtmxDiv *
htmx_div_new(void)
{
	return g_object_new(HTMX_TYPE_DIV, NULL);
}
