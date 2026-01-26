/*
 * htmx-fieldset.c - HTML form grouping elements implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-fieldset.h"

/*
 * HtmxFieldset - Fieldset element
 */

struct _HtmxFieldset {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxFieldset, htmx_fieldset, HTMX_TYPE_NODE)

static void
htmx_fieldset_class_init(HtmxFieldsetClass *klass)
{
}

static void
htmx_fieldset_init(HtmxFieldset *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "fieldset");
}

HtmxFieldset *
htmx_fieldset_new(void)
{
	return g_object_new(HTMX_TYPE_FIELDSET, NULL);
}

void
htmx_fieldset_set_disabled(
	HtmxFieldset *self,
	gboolean      disabled
){
	g_return_if_fail(HTMX_IS_FIELDSET(self));

	if (disabled) {
		htmx_element_set_attribute(HTMX_ELEMENT(self), "disabled", "disabled");
	}
}

void
htmx_fieldset_set_form(
	HtmxFieldset *self,
	const gchar  *form_id
){
	g_return_if_fail(HTMX_IS_FIELDSET(self));

	htmx_element_set_attribute(HTMX_ELEMENT(self), "form", form_id);
}

void
htmx_fieldset_set_name(
	HtmxFieldset *self,
	const gchar  *name
){
	g_return_if_fail(HTMX_IS_FIELDSET(self));

	htmx_element_set_attribute(HTMX_ELEMENT(self), "name", name);
}

/*
 * HtmxLegend - Legend element
 */

struct _HtmxLegend {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxLegend, htmx_legend, HTMX_TYPE_NODE)

static void
htmx_legend_class_init(HtmxLegendClass *klass)
{
}

static void
htmx_legend_init(HtmxLegend *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "legend");
}

HtmxLegend *
htmx_legend_new(void)
{
	return g_object_new(HTMX_TYPE_LEGEND, NULL);
}

HtmxLegend *
htmx_legend_new_with_text(const gchar *text)
{
	HtmxLegend *self;

	self = htmx_legend_new();
	if (text != NULL) {
		htmx_node_set_text_content(HTMX_NODE(self), text);
	}

	return self;
}

/*
 * HtmxOptgroup - Option group element
 */

struct _HtmxOptgroup {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxOptgroup, htmx_optgroup, HTMX_TYPE_NODE)

static void
htmx_optgroup_class_init(HtmxOptgroupClass *klass)
{
}

static void
htmx_optgroup_init(HtmxOptgroup *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "optgroup");
}

HtmxOptgroup *
htmx_optgroup_new(void)
{
	return g_object_new(HTMX_TYPE_OPTGROUP, NULL);
}

HtmxOptgroup *
htmx_optgroup_new_with_label(const gchar *label)
{
	HtmxOptgroup *self;

	self = htmx_optgroup_new();
	if (label != NULL) {
		htmx_optgroup_set_label(self, label);
	}

	return self;
}

void
htmx_optgroup_set_label(
	HtmxOptgroup *self,
	const gchar  *label
){
	g_return_if_fail(HTMX_IS_OPTGROUP(self));

	htmx_element_set_attribute(HTMX_ELEMENT(self), "label", label);
}

void
htmx_optgroup_set_disabled(
	HtmxOptgroup *self,
	gboolean      disabled
){
	g_return_if_fail(HTMX_IS_OPTGROUP(self));

	if (disabled) {
		htmx_element_set_attribute(HTMX_ELEMENT(self), "disabled", "disabled");
	}
}

/*
 * HtmxDatalist - Datalist element
 */

struct _HtmxDatalist {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxDatalist, htmx_datalist, HTMX_TYPE_NODE)

static void
htmx_datalist_class_init(HtmxDatalistClass *klass)
{
}

static void
htmx_datalist_init(HtmxDatalist *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "datalist");
}

HtmxDatalist *
htmx_datalist_new(void)
{
	return g_object_new(HTMX_TYPE_DATALIST, NULL);
}

HtmxDatalist *
htmx_datalist_new_with_id(const gchar *id)
{
	HtmxDatalist *self;

	self = htmx_datalist_new();
	if (id != NULL) {
		htmx_element_set_id(HTMX_ELEMENT(self), id);
	}

	return self;
}

/*
 * HtmxOutput - Output element
 */

struct _HtmxOutput {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxOutput, htmx_output, HTMX_TYPE_NODE)

static void
htmx_output_class_init(HtmxOutputClass *klass)
{
}

static void
htmx_output_init(HtmxOutput *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "output");
}

HtmxOutput *
htmx_output_new(void)
{
	return g_object_new(HTMX_TYPE_OUTPUT, NULL);
}

void
htmx_output_set_for(
	HtmxOutput  *self,
	const gchar *for_ids
){
	g_return_if_fail(HTMX_IS_OUTPUT(self));

	htmx_element_set_attribute(HTMX_ELEMENT(self), "for", for_ids);
}

void
htmx_output_set_form(
	HtmxOutput  *self,
	const gchar *form_id
){
	g_return_if_fail(HTMX_IS_OUTPUT(self));

	htmx_element_set_attribute(HTMX_ELEMENT(self), "form", form_id);
}

void
htmx_output_set_name(
	HtmxOutput  *self,
	const gchar *name
){
	g_return_if_fail(HTMX_IS_OUTPUT(self));

	htmx_element_set_attribute(HTMX_ELEMENT(self), "name", name);
}
