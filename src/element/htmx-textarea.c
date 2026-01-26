/*
 * htmx-textarea.c - HTML textarea element implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-textarea.h"

struct _HtmxTextarea {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxTextarea, htmx_textarea, HTMX_TYPE_NODE)

static void
htmx_textarea_class_init(HtmxTextareaClass *klass)
{
}

static void
htmx_textarea_init(HtmxTextarea *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "textarea");
}

HtmxTextarea *
htmx_textarea_new(void)
{
	return g_object_new(HTMX_TYPE_TEXTAREA, NULL);
}

HtmxTextarea *
htmx_textarea_new_with_name(const gchar *name)
{
	HtmxTextarea *self;

	self = htmx_textarea_new();
	if (name != NULL) {
		htmx_textarea_set_name(self, name);
	}

	return self;
}

void
htmx_textarea_set_name(
	HtmxTextarea *self,
	const gchar  *name
){
	g_return_if_fail(HTMX_IS_TEXTAREA(self));

	htmx_element_set_attribute(HTMX_ELEMENT(self), "name", name);
}

void
htmx_textarea_set_placeholder(
	HtmxTextarea *self,
	const gchar  *placeholder
){
	g_return_if_fail(HTMX_IS_TEXTAREA(self));

	htmx_element_set_attribute(HTMX_ELEMENT(self), "placeholder", placeholder);
}

void
htmx_textarea_set_rows(
	HtmxTextarea *self,
	guint         rows
){
	g_autofree gchar *rows_str = NULL;

	g_return_if_fail(HTMX_IS_TEXTAREA(self));

	rows_str = g_strdup_printf("%u", rows);
	htmx_element_set_attribute(HTMX_ELEMENT(self), "rows", rows_str);
}

void
htmx_textarea_set_cols(
	HtmxTextarea *self,
	guint         cols
){
	g_autofree gchar *cols_str = NULL;

	g_return_if_fail(HTMX_IS_TEXTAREA(self));

	cols_str = g_strdup_printf("%u", cols);
	htmx_element_set_attribute(HTMX_ELEMENT(self), "cols", cols_str);
}

void
htmx_textarea_set_required(
	HtmxTextarea *self,
	gboolean      required
){
	g_return_if_fail(HTMX_IS_TEXTAREA(self));

	if (required) {
		htmx_element_set_attribute(HTMX_ELEMENT(self), "required", "required");
	}
}

void
htmx_textarea_set_readonly(
	HtmxTextarea *self,
	gboolean      readonly
){
	g_return_if_fail(HTMX_IS_TEXTAREA(self));

	if (readonly) {
		htmx_element_set_attribute(HTMX_ELEMENT(self), "readonly", "readonly");
	}
}

void
htmx_textarea_set_disabled(
	HtmxTextarea *self,
	gboolean      disabled
){
	g_return_if_fail(HTMX_IS_TEXTAREA(self));

	if (disabled) {
		htmx_element_set_attribute(HTMX_ELEMENT(self), "disabled", "disabled");
	}
}
