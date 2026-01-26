/*
 * htmx-select.c - HTML select and option elements implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-select.h"

/*
 * HtmxSelect - Select dropdown
 */

struct _HtmxSelect {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxSelect, htmx_select, HTMX_TYPE_NODE)

static void
htmx_select_class_init(HtmxSelectClass *klass)
{
}

static void
htmx_select_init(HtmxSelect *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "select");
}

HtmxSelect *
htmx_select_new(void)
{
	return g_object_new(HTMX_TYPE_SELECT, NULL);
}

HtmxSelect *
htmx_select_new_with_name(const gchar *name)
{
	HtmxSelect *self;

	self = htmx_select_new();
	if (name != NULL) {
		htmx_select_set_name(self, name);
	}

	return self;
}

void
htmx_select_set_name(
	HtmxSelect  *self,
	const gchar *name
){
	g_return_if_fail(HTMX_IS_SELECT(self));

	htmx_element_set_attribute(HTMX_ELEMENT(self), "name", name);
}

void
htmx_select_set_multiple(
	HtmxSelect *self,
	gboolean    multiple
){
	g_return_if_fail(HTMX_IS_SELECT(self));

	if (multiple) {
		htmx_element_set_attribute(HTMX_ELEMENT(self), "multiple", "multiple");
	}
}

void
htmx_select_set_required(
	HtmxSelect *self,
	gboolean    required
){
	g_return_if_fail(HTMX_IS_SELECT(self));

	if (required) {
		htmx_element_set_attribute(HTMX_ELEMENT(self), "required", "required");
	}
}

void
htmx_select_set_disabled(
	HtmxSelect *self,
	gboolean    disabled
){
	g_return_if_fail(HTMX_IS_SELECT(self));

	if (disabled) {
		htmx_element_set_attribute(HTMX_ELEMENT(self), "disabled", "disabled");
	}
}

void
htmx_select_set_size(
	HtmxSelect *self,
	guint       size
){
	g_autofree gchar *size_str = NULL;

	g_return_if_fail(HTMX_IS_SELECT(self));

	size_str = g_strdup_printf("%u", size);
	htmx_element_set_attribute(HTMX_ELEMENT(self), "size", size_str);
}

/*
 * HtmxOption - Option element
 */

struct _HtmxOption {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxOption, htmx_option, HTMX_TYPE_NODE)

static void
htmx_option_class_init(HtmxOptionClass *klass)
{
}

static void
htmx_option_init(HtmxOption *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "option");
}

HtmxOption *
htmx_option_new(void)
{
	return g_object_new(HTMX_TYPE_OPTION, NULL);
}

HtmxOption *
htmx_option_new_with_value(
	const gchar *value,
	const gchar *label
){
	HtmxOption *self;

	self = htmx_option_new();
	if (value != NULL) {
		htmx_option_set_value(self, value);
	}
	if (label != NULL) {
		htmx_node_set_text_content(HTMX_NODE(self), label);
	}

	return self;
}

void
htmx_option_set_value(
	HtmxOption  *self,
	const gchar *value
){
	g_return_if_fail(HTMX_IS_OPTION(self));

	htmx_element_set_attribute(HTMX_ELEMENT(self), "value", value);
}

void
htmx_option_set_selected(
	HtmxOption *self,
	gboolean    selected
){
	g_return_if_fail(HTMX_IS_OPTION(self));

	if (selected) {
		htmx_element_set_attribute(HTMX_ELEMENT(self), "selected", "selected");
	}
}

void
htmx_option_set_disabled(
	HtmxOption *self,
	gboolean    disabled
){
	g_return_if_fail(HTMX_IS_OPTION(self));

	if (disabled) {
		htmx_element_set_attribute(HTMX_ELEMENT(self), "disabled", "disabled");
	}
}
