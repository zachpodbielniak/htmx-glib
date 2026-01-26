/*
 * htmx-heading.c - HTML heading elements implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-heading.h"

struct _HtmxHeading {
	HtmxNode parent_instance;
	guint    level;
};

G_DEFINE_FINAL_TYPE(HtmxHeading, htmx_heading, HTMX_TYPE_NODE)

static void
htmx_heading_class_init(HtmxHeadingClass *klass)
{
}

static void
htmx_heading_init(HtmxHeading *self)
{
	/* Default to h1, will be overridden by htmx_heading_new */
	self->level = 1;
	htmx_node_set_tag_name(HTMX_NODE(self), "h1");
}

HtmxHeading *
htmx_heading_new(guint level)
{
	HtmxHeading *self;
	g_autofree gchar *tag_name = NULL;

	g_return_val_if_fail(level >= 1 && level <= 6, NULL);

	self = g_object_new(HTMX_TYPE_HEADING, NULL);
	self->level = level;

	tag_name = g_strdup_printf("h%u", level);
	htmx_node_set_tag_name(HTMX_NODE(self), tag_name);

	return self;
}

guint
htmx_heading_get_level(HtmxHeading *self)
{
	g_return_val_if_fail(HTMX_IS_HEADING(self), 0);

	return self->level;
}

HtmxHeading *
htmx_h1_new(void)
{
	return htmx_heading_new(1);
}

HtmxHeading *
htmx_h2_new(void)
{
	return htmx_heading_new(2);
}

HtmxHeading *
htmx_h3_new(void)
{
	return htmx_heading_new(3);
}

HtmxHeading *
htmx_h4_new(void)
{
	return htmx_heading_new(4);
}

HtmxHeading *
htmx_h5_new(void)
{
	return htmx_heading_new(5);
}

HtmxHeading *
htmx_h6_new(void)
{
	return htmx_heading_new(6);
}
