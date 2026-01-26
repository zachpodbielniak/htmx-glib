/*
 * htmx-list.c - HTML list elements implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-list.h"

/*
 * HtmxUl - Unordered list
 */

struct _HtmxUl {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxUl, htmx_ul, HTMX_TYPE_NODE)

static void
htmx_ul_class_init(HtmxUlClass *klass)
{
}

static void
htmx_ul_init(HtmxUl *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "ul");
}

HtmxUl *
htmx_ul_new(void)
{
	return g_object_new(HTMX_TYPE_UL, NULL);
}

/*
 * HtmxOl - Ordered list
 */

struct _HtmxOl {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxOl, htmx_ol, HTMX_TYPE_NODE)

static void
htmx_ol_class_init(HtmxOlClass *klass)
{
}

static void
htmx_ol_init(HtmxOl *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "ol");
}

HtmxOl *
htmx_ol_new(void)
{
	return g_object_new(HTMX_TYPE_OL, NULL);
}

void
htmx_ol_set_start(
	HtmxOl *self,
	gint    start
){
	g_autofree gchar *start_str = NULL;

	g_return_if_fail(HTMX_IS_OL(self));

	start_str = g_strdup_printf("%d", start);
	htmx_element_set_attribute(HTMX_ELEMENT(self), "start", start_str);
}

void
htmx_ol_set_reversed(
	HtmxOl   *self,
	gboolean  reversed
){
	g_return_if_fail(HTMX_IS_OL(self));

	if (reversed) {
		htmx_element_set_attribute(HTMX_ELEMENT(self), "reversed", "reversed");
	}
}

/*
 * HtmxLi - List item
 */

struct _HtmxLi {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxLi, htmx_li, HTMX_TYPE_NODE)

static void
htmx_li_class_init(HtmxLiClass *klass)
{
}

static void
htmx_li_init(HtmxLi *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "li");
}

HtmxLi *
htmx_li_new(void)
{
	return g_object_new(HTMX_TYPE_LI, NULL);
}

HtmxLi *
htmx_li_new_with_text(const gchar *text)
{
	HtmxLi *self;

	self = htmx_li_new();
	if (text != NULL) {
		htmx_node_set_text_content(HTMX_NODE(self), text);
	}

	return self;
}

void
htmx_li_set_value(
	HtmxLi *self,
	gint    value
){
	g_autofree gchar *value_str = NULL;

	g_return_if_fail(HTMX_IS_LI(self));

	value_str = g_strdup_printf("%d", value);
	htmx_element_set_attribute(HTMX_ELEMENT(self), "value", value_str);
}
