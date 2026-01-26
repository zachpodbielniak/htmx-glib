/*
 * htmx-void.c - HTML void/empty elements implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-void.h"

/*
 * HtmxBr - Line break element (void element)
 */

struct _HtmxBr {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxBr, htmx_br, HTMX_TYPE_NODE)

static void
htmx_br_class_init(HtmxBrClass *klass)
{
}

static void
htmx_br_init(HtmxBr *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "br");
	htmx_node_set_void_element(HTMX_NODE(self), TRUE);
}

HtmxBr *
htmx_br_new(void)
{
	return g_object_new(HTMX_TYPE_BR, NULL);
}

/*
 * HtmxHr - Horizontal rule element (void element)
 */

struct _HtmxHr {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxHr, htmx_hr, HTMX_TYPE_NODE)

static void
htmx_hr_class_init(HtmxHrClass *klass)
{
}

static void
htmx_hr_init(HtmxHr *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "hr");
	htmx_node_set_void_element(HTMX_NODE(self), TRUE);
}

HtmxHr *
htmx_hr_new(void)
{
	return g_object_new(HTMX_TYPE_HR, NULL);
}

/*
 * HtmxWbr - Word break opportunity element (void element)
 */

struct _HtmxWbr {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxWbr, htmx_wbr, HTMX_TYPE_NODE)

static void
htmx_wbr_class_init(HtmxWbrClass *klass)
{
}

static void
htmx_wbr_init(HtmxWbr *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "wbr");
	htmx_node_set_void_element(HTMX_NODE(self), TRUE);
}

HtmxWbr *
htmx_wbr_new(void)
{
	return g_object_new(HTMX_TYPE_WBR, NULL);
}
