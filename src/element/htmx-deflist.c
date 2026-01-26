/*
 * htmx-deflist.c - HTML definition list elements implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-deflist.h"

/*
 * HtmxDl - Definition list element
 */

struct _HtmxDl {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxDl, htmx_dl, HTMX_TYPE_NODE)

static void
htmx_dl_class_init(HtmxDlClass *klass)
{
}

static void
htmx_dl_init(HtmxDl *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "dl");
}

HtmxDl *
htmx_dl_new(void)
{
	return g_object_new(HTMX_TYPE_DL, NULL);
}

/*
 * HtmxDt - Definition term element
 */

struct _HtmxDt {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxDt, htmx_dt, HTMX_TYPE_NODE)

static void
htmx_dt_class_init(HtmxDtClass *klass)
{
}

static void
htmx_dt_init(HtmxDt *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "dt");
}

HtmxDt *
htmx_dt_new(void)
{
	return g_object_new(HTMX_TYPE_DT, NULL);
}

HtmxDt *
htmx_dt_new_with_text(const gchar *text)
{
	HtmxDt *self;

	self = htmx_dt_new();
	if (text != NULL) {
		htmx_node_set_text_content(HTMX_NODE(self), text);
	}

	return self;
}

/*
 * HtmxDd - Definition description element
 */

struct _HtmxDd {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxDd, htmx_dd, HTMX_TYPE_NODE)

static void
htmx_dd_class_init(HtmxDdClass *klass)
{
}

static void
htmx_dd_init(HtmxDd *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "dd");
}

HtmxDd *
htmx_dd_new(void)
{
	return g_object_new(HTMX_TYPE_DD, NULL);
}

HtmxDd *
htmx_dd_new_with_text(const gchar *text)
{
	HtmxDd *self;

	self = htmx_dd_new();
	if (text != NULL) {
		htmx_node_set_text_content(HTMX_NODE(self), text);
	}

	return self;
}
