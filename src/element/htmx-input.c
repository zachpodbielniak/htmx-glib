/*
 * htmx-input.c - HTML input element implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-input.h"

struct _HtmxInput {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxInput, htmx_input, HTMX_TYPE_NODE)

static void
htmx_input_class_init(HtmxInputClass *klass)
{
}

static void
htmx_input_init(HtmxInput *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "input");
	htmx_node_set_void_element(HTMX_NODE(self), TRUE);
}

HtmxInput *
htmx_input_new(HtmxInputType type)
{
	HtmxInput *self;
	const gchar *type_str;

	self = g_object_new(HTMX_TYPE_INPUT, NULL);
	type_str = htmx_input_type_to_string(type);
	htmx_element_set_attribute(HTMX_ELEMENT(self), "type", type_str);

	return self;
}

HtmxInput *
htmx_input_new_text(const gchar *name)
{
	HtmxInput *self;

	self = htmx_input_new(HTMX_INPUT_TEXT);
	if (name != NULL) {
		htmx_input_set_name(self, name);
	}

	return self;
}

HtmxInput *
htmx_input_new_hidden(const gchar *name, const gchar *value)
{
	HtmxInput *self;

	self = htmx_input_new(HTMX_INPUT_HIDDEN);
	if (name != NULL) {
		htmx_input_set_name(self, name);
	}
	if (value != NULL) {
		htmx_input_set_value(self, value);
	}

	return self;
}

void
htmx_input_set_name(HtmxInput *self, const gchar *name)
{
	g_return_if_fail(HTMX_IS_INPUT(self));

	htmx_element_set_attribute(HTMX_ELEMENT(self), "name", name);
}

void
htmx_input_set_value(HtmxInput *self, const gchar *value)
{
	g_return_if_fail(HTMX_IS_INPUT(self));

	htmx_element_set_attribute(HTMX_ELEMENT(self), "value", value);
}

void
htmx_input_set_placeholder(HtmxInput *self, const gchar *placeholder)
{
	g_return_if_fail(HTMX_IS_INPUT(self));

	htmx_element_set_attribute(HTMX_ELEMENT(self), "placeholder", placeholder);
}

void
htmx_input_set_required(HtmxInput *self, gboolean required)
{
	g_return_if_fail(HTMX_IS_INPUT(self));

	if (required) {
		htmx_element_set_attribute(HTMX_ELEMENT(self), "required", "required");
	}
}
