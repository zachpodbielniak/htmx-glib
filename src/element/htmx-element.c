/*
 * htmx-element.c - HTMX element interface implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-element.h"

G_DEFINE_INTERFACE(HtmxElement, htmx_element, G_TYPE_OBJECT)

/*
 * Default interface initialization
 */
static void
htmx_element_default_init(HtmxElementInterface *iface)
{
	/* Default implementations are NULL */
}

/*
 * Render the element to HTML
 */
gchar *
htmx_element_render(HtmxElement *self)
{
	HtmxElementInterface *iface;

	g_return_val_if_fail(HTMX_IS_ELEMENT(self), NULL);

	iface = HTMX_ELEMENT_GET_IFACE(self);
	g_return_val_if_fail(iface->render != NULL, NULL);

	return iface->render(self);
}

/*
 * Get all attributes
 */
GList *
htmx_element_get_attributes(HtmxElement *self)
{
	HtmxElementInterface *iface;

	g_return_val_if_fail(HTMX_IS_ELEMENT(self), NULL);

	iface = HTMX_ELEMENT_GET_IFACE(self);
	if (iface->get_attributes == NULL) {
		return NULL;
	}

	return iface->get_attributes(self);
}

/*
 * Set an attribute
 */
void
htmx_element_set_attribute(
	HtmxElement *self,
	const gchar *name,
	const gchar *value
){
	HtmxElementInterface *iface;

	g_return_if_fail(HTMX_IS_ELEMENT(self));
	g_return_if_fail(name != NULL);

	iface = HTMX_ELEMENT_GET_IFACE(self);
	g_return_if_fail(iface->set_attribute != NULL);

	iface->set_attribute(self, name, value);
}

/*
 * Get element ID
 */
const gchar *
htmx_element_get_id(HtmxElement *self)
{
	HtmxElementInterface *iface;

	g_return_val_if_fail(HTMX_IS_ELEMENT(self), NULL);

	iface = HTMX_ELEMENT_GET_IFACE(self);
	if (iface->get_id == NULL) {
		return NULL;
	}

	return iface->get_id(self);
}

/*
 * Set element ID
 */
void
htmx_element_set_id(
	HtmxElement *self,
	const gchar *id
){
	HtmxElementInterface *iface;

	g_return_if_fail(HTMX_IS_ELEMENT(self));

	iface = HTMX_ELEMENT_GET_IFACE(self);
	g_return_if_fail(iface->set_id != NULL);

	iface->set_id(self, id);
}

/*
 * Get CSS classes
 */
GList *
htmx_element_get_classes(HtmxElement *self)
{
	HtmxElementInterface *iface;

	g_return_val_if_fail(HTMX_IS_ELEMENT(self), NULL);

	iface = HTMX_ELEMENT_GET_IFACE(self);
	if (iface->get_classes == NULL) {
		return NULL;
	}

	return iface->get_classes(self);
}

/*
 * Add a CSS class
 */
void
htmx_element_add_class(
	HtmxElement *self,
	const gchar *class_name
){
	HtmxElementInterface *iface;

	g_return_if_fail(HTMX_IS_ELEMENT(self));
	g_return_if_fail(class_name != NULL);

	iface = HTMX_ELEMENT_GET_IFACE(self);
	g_return_if_fail(iface->add_class != NULL);

	iface->add_class(self, class_name);
}

/*
 * HTMX-specific attribute setters
 */

void
htmx_element_set_hx_get(
	HtmxElement *self,
	const gchar *url
){
	htmx_element_set_attribute(self, "hx-get", url);
}

void
htmx_element_set_hx_post(
	HtmxElement *self,
	const gchar *url
){
	htmx_element_set_attribute(self, "hx-post", url);
}

void
htmx_element_set_hx_put(
	HtmxElement *self,
	const gchar *url
){
	htmx_element_set_attribute(self, "hx-put", url);
}

void
htmx_element_set_hx_patch(
	HtmxElement *self,
	const gchar *url
){
	htmx_element_set_attribute(self, "hx-patch", url);
}

void
htmx_element_set_hx_delete(
	HtmxElement *self,
	const gchar *url
){
	htmx_element_set_attribute(self, "hx-delete", url);
}

void
htmx_element_set_hx_target(
	HtmxElement *self,
	const gchar *target
){
	htmx_element_set_attribute(self, "hx-target", target);
}

void
htmx_element_set_hx_swap(
	HtmxElement  *self,
	HtmxSwapStyle swap_style
){
	const gchar *swap_str;

	swap_str = htmx_swap_style_to_string(swap_style);
	htmx_element_set_attribute(self, "hx-swap", swap_str);
}

void
htmx_element_set_hx_swap_with_config(
	HtmxElement    *self,
	const HtmxSwap *swap
){
	g_autofree gchar *swap_str = NULL;

	g_return_if_fail(swap != NULL);

	swap_str = htmx_swap_render(swap);
	htmx_element_set_attribute(self, "hx-swap", swap_str);
}

void
htmx_element_set_hx_trigger(
	HtmxElement       *self,
	const HtmxTrigger *trigger
){
	g_autofree gchar *trigger_str = NULL;

	g_return_if_fail(trigger != NULL);

	trigger_str = htmx_trigger_render(trigger);
	htmx_element_set_attribute(self, "hx-trigger", trigger_str);
}

void
htmx_element_set_hx_trigger_string(
	HtmxElement *self,
	const gchar *trigger
){
	htmx_element_set_attribute(self, "hx-trigger", trigger);
}

void
htmx_element_set_hx_confirm(
	HtmxElement *self,
	const gchar *message
){
	htmx_element_set_attribute(self, "hx-confirm", message);
}

void
htmx_element_set_hx_boost(
	HtmxElement *self,
	gboolean     boost
){
	htmx_element_set_attribute(self, "hx-boost", boost ? "true" : "false");
}

void
htmx_element_set_hx_push_url(
	HtmxElement *self,
	const gchar *url
){
	htmx_element_set_attribute(self, "hx-push-url", url);
}

void
htmx_element_set_hx_vals(
	HtmxElement *self,
	const gchar *json
){
	htmx_element_set_attribute(self, "hx-vals", json);
}

void
htmx_element_set_hx_include(
	HtmxElement *self,
	const gchar *selector
){
	htmx_element_set_attribute(self, "hx-include", selector);
}

void
htmx_element_set_hx_indicator(
	HtmxElement *self,
	const gchar *selector
){
	htmx_element_set_attribute(self, "hx-indicator", selector);
}
