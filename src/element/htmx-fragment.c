/*
 * htmx-fragment.c - HTML fragment container implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-fragment.h"

struct _HtmxFragment {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxFragment, htmx_fragment, HTMX_TYPE_NODE)

static gchar *
htmx_fragment_render_opening(HtmxNode *node)
{
	return g_strdup("");
}

static gchar *
htmx_fragment_render_closing(HtmxNode *node)
{
	return g_strdup("");
}

static void
htmx_fragment_class_init(HtmxFragmentClass *klass)
{
	HtmxNodeClass *node_class = HTMX_NODE_CLASS(klass);

	node_class->render_opening = htmx_fragment_render_opening;
	node_class->render_closing = htmx_fragment_render_closing;
}

static void
htmx_fragment_init(HtmxFragment *self)
{
}

HtmxFragment *
htmx_fragment_new(void)
{
	return g_object_new(HTMX_TYPE_FRAGMENT, NULL);
}
