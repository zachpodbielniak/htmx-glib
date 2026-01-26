/*
 * htmx-a.c - HTML anchor element implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-a.h"

struct _HtmxA {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxA, htmx_a, HTMX_TYPE_NODE)

static void
htmx_a_class_init(HtmxAClass *klass)
{
}

static void
htmx_a_init(HtmxA *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "a");
}

HtmxA *
htmx_a_new(void)
{
	return g_object_new(HTMX_TYPE_A, NULL);
}

HtmxA *
htmx_a_new_with_href(const gchar *href)
{
	HtmxA *self;

	self = htmx_a_new();
	if (href != NULL) {
		htmx_a_set_href(self, href);
	}

	return self;
}

void
htmx_a_set_href(
	HtmxA       *self,
	const gchar *href
){
	g_return_if_fail(HTMX_IS_A(self));

	htmx_element_set_attribute(HTMX_ELEMENT(self), "href", href);
}

void
htmx_a_set_target(
	HtmxA       *self,
	const gchar *target
){
	g_return_if_fail(HTMX_IS_A(self));

	htmx_element_set_attribute(HTMX_ELEMENT(self), "target", target);
}

void
htmx_a_set_rel(
	HtmxA       *self,
	const gchar *rel
){
	g_return_if_fail(HTMX_IS_A(self));

	htmx_element_set_attribute(HTMX_ELEMENT(self), "rel", rel);
}
