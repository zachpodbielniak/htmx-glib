/*
 * htmx-label.c - HTML label element implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-label.h"

struct _HtmxLabel {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxLabel, htmx_label, HTMX_TYPE_NODE)

static void
htmx_label_class_init(HtmxLabelClass *klass)
{
}

static void
htmx_label_init(HtmxLabel *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "label");
}

HtmxLabel *
htmx_label_new(void)
{
	return g_object_new(HTMX_TYPE_LABEL, NULL);
}

HtmxLabel *
htmx_label_new_for(const gchar *for_id)
{
	HtmxLabel *self;

	self = htmx_label_new();
	if (for_id != NULL) {
		htmx_label_set_for(self, for_id);
	}

	return self;
}

HtmxLabel *
htmx_label_new_with_text(const gchar *text)
{
	HtmxLabel *self;

	self = htmx_label_new();
	if (text != NULL) {
		htmx_node_set_text_content(HTMX_NODE(self), text);
	}

	return self;
}

void
htmx_label_set_for(
	HtmxLabel   *self,
	const gchar *for_id
){
	g_return_if_fail(HTMX_IS_LABEL(self));

	htmx_element_set_attribute(HTMX_ELEMENT(self), "for", for_id);
}
