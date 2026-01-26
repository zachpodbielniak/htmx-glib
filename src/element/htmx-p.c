/*
 * htmx-p.c - HTML paragraph element implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-p.h"

struct _HtmxP {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxP, htmx_p, HTMX_TYPE_NODE)

static void
htmx_p_class_init(HtmxPClass *klass)
{
}

static void
htmx_p_init(HtmxP *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "p");
}

HtmxP *
htmx_p_new(void)
{
	return g_object_new(HTMX_TYPE_P, NULL);
}
