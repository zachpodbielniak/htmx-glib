/*
 * htmx-form.c - HTML form element implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-form.h"

struct _HtmxForm {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxForm, htmx_form, HTMX_TYPE_NODE)

static void
htmx_form_class_init(HtmxFormClass *klass)
{
}

static void
htmx_form_init(HtmxForm *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "form");
}

HtmxForm *
htmx_form_new(void)
{
	HtmxForm *self;
	HtmxNode *node;

	self = g_object_new(HTMX_TYPE_FORM, NULL);
	node = HTMX_NODE(self);

	/* Set tag name via node functions */
	htmx_element_set_attribute(HTMX_ELEMENT(node), "method", "post");

	return self;
}

HtmxForm *
htmx_form_new_with_action(const gchar *action, HtmxMethod method)
{
	HtmxForm *self;
	const gchar *method_str;

	self = htmx_form_new();

	if (action != NULL) {
		htmx_element_set_attribute(HTMX_ELEMENT(self), "action", action);
	}

	method_str = htmx_method_to_string(method);
	htmx_element_set_attribute(HTMX_ELEMENT(self), "method",
	                           g_ascii_strdown(method_str, -1));

	return self;
}

/*
 * HtmxProgress - Progress indicator element
 */

struct _HtmxProgress {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxProgress, htmx_progress, HTMX_TYPE_NODE)

static void
htmx_progress_class_init(HtmxProgressClass *klass)
{
}

static void
htmx_progress_init(HtmxProgress *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "progress");
}

HtmxProgress *
htmx_progress_new(void)
{
	return g_object_new(HTMX_TYPE_PROGRESS, NULL);
}

HtmxProgress *
htmx_progress_new_with_value(
	gdouble value,
	gdouble max
){
	HtmxProgress *self;

	self = htmx_progress_new();
	htmx_progress_set_value(self, value);
	htmx_progress_set_max(self, max);

	return self;
}

void
htmx_progress_set_value(
	HtmxProgress *self,
	gdouble       value
){
	g_autofree gchar *value_str = NULL;

	g_return_if_fail(HTMX_IS_PROGRESS(self));

	value_str = g_strdup_printf("%g", value);
	htmx_element_set_attribute(HTMX_ELEMENT(self), "value", value_str);
}

void
htmx_progress_set_max(
	HtmxProgress *self,
	gdouble       max
){
	g_autofree gchar *max_str = NULL;

	g_return_if_fail(HTMX_IS_PROGRESS(self));

	max_str = g_strdup_printf("%g", max);
	htmx_element_set_attribute(HTMX_ELEMENT(self), "max", max_str);
}

/*
 * HtmxMeter - Meter/gauge element
 */

struct _HtmxMeter {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxMeter, htmx_meter, HTMX_TYPE_NODE)

static void
htmx_meter_class_init(HtmxMeterClass *klass)
{
}

static void
htmx_meter_init(HtmxMeter *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "meter");
}

HtmxMeter *
htmx_meter_new(void)
{
	return g_object_new(HTMX_TYPE_METER, NULL);
}

void
htmx_meter_set_value(
	HtmxMeter *self,
	gdouble    value
){
	g_autofree gchar *value_str = NULL;

	g_return_if_fail(HTMX_IS_METER(self));

	value_str = g_strdup_printf("%g", value);
	htmx_element_set_attribute(HTMX_ELEMENT(self), "value", value_str);
}

void
htmx_meter_set_min(
	HtmxMeter *self,
	gdouble    min
){
	g_autofree gchar *min_str = NULL;

	g_return_if_fail(HTMX_IS_METER(self));

	min_str = g_strdup_printf("%g", min);
	htmx_element_set_attribute(HTMX_ELEMENT(self), "min", min_str);
}

void
htmx_meter_set_max(
	HtmxMeter *self,
	gdouble    max
){
	g_autofree gchar *max_str = NULL;

	g_return_if_fail(HTMX_IS_METER(self));

	max_str = g_strdup_printf("%g", max);
	htmx_element_set_attribute(HTMX_ELEMENT(self), "max", max_str);
}

void
htmx_meter_set_low(
	HtmxMeter *self,
	gdouble    low
){
	g_autofree gchar *low_str = NULL;

	g_return_if_fail(HTMX_IS_METER(self));

	low_str = g_strdup_printf("%g", low);
	htmx_element_set_attribute(HTMX_ELEMENT(self), "low", low_str);
}

void
htmx_meter_set_high(
	HtmxMeter *self,
	gdouble    high
){
	g_autofree gchar *high_str = NULL;

	g_return_if_fail(HTMX_IS_METER(self));

	high_str = g_strdup_printf("%g", high);
	htmx_element_set_attribute(HTMX_ELEMENT(self), "high", high_str);
}

void
htmx_meter_set_optimum(
	HtmxMeter *self,
	gdouble    optimum
){
	g_autofree gchar *optimum_str = NULL;

	g_return_if_fail(HTMX_IS_METER(self));

	optimum_str = g_strdup_printf("%g", optimum);
	htmx_element_set_attribute(HTMX_ELEMENT(self), "optimum", optimum_str);
}
