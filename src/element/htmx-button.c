/*
 * htmx-button.c - HTML button element implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-button.h"

struct _HtmxButton {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxButton, htmx_button, HTMX_TYPE_NODE)

static void
htmx_button_class_init(HtmxButtonClass *klass)
{
}

static void
htmx_button_init(HtmxButton *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "button");
	/* Default to type="button" to avoid accidental form submission */
	htmx_element_set_attribute(HTMX_ELEMENT(self), "type", "button");
}

HtmxButton *
htmx_button_new(void)
{
	return g_object_new(HTMX_TYPE_BUTTON, NULL);
}

HtmxButton *
htmx_button_new_with_label(const gchar *label)
{
	HtmxButton *self;

	self = htmx_button_new();
	if (label != NULL) {
		htmx_node_set_text_content(HTMX_NODE(self), label);
	}

	return self;
}

HtmxButton *
htmx_button_new_submit(void)
{
	HtmxButton *self;

	self = htmx_button_new();
	htmx_button_set_button_type(self, HTMX_BUTTON_SUBMIT);

	return self;
}

HtmxButton *
htmx_button_new_reset(void)
{
	HtmxButton *self;

	self = htmx_button_new();
	htmx_button_set_button_type(self, HTMX_BUTTON_RESET);

	return self;
}

void
htmx_button_set_button_type(
	HtmxButton    *self,
	HtmxButtonType type
){
	const gchar *type_str;

	g_return_if_fail(HTMX_IS_BUTTON(self));

	type_str = htmx_button_type_to_string(type);
	htmx_element_set_attribute(HTMX_ELEMENT(self), "type", type_str);
}

void
htmx_button_set_disabled(
	HtmxButton *self,
	gboolean    disabled
){
	g_return_if_fail(HTMX_IS_BUTTON(self));

	if (disabled) {
		htmx_element_set_attribute(HTMX_ELEMENT(self), "disabled", "disabled");
	}
	/* Note: to un-disable, the attribute would need to be removed */
}

void
htmx_button_set_name(
	HtmxButton  *self,
	const gchar *name
){
	g_return_if_fail(HTMX_IS_BUTTON(self));

	htmx_element_set_attribute(HTMX_ELEMENT(self), "name", name);
}

void
htmx_button_set_value(
	HtmxButton  *self,
	const gchar *value
){
	g_return_if_fail(HTMX_IS_BUTTON(self));

	htmx_element_set_attribute(HTMX_ELEMENT(self), "value", value);
}
