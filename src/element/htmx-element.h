/*
 * htmx-element.h - HTMX element interface
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * This file defines the HtmxElement interface for HTMX-aware HTML elements.
 */

#ifndef HTMX_ELEMENT_H
#define HTMX_ELEMENT_H

#include <glib-object.h>
#include "core/htmx-enums.h"
#include "model/htmx-attribute.h"
#include "model/htmx-trigger.h"
#include "model/htmx-swap.h"

G_BEGIN_DECLS

#define HTMX_TYPE_ELEMENT (htmx_element_get_type())
G_DECLARE_INTERFACE(HtmxElement, htmx_element, HTMX, ELEMENT, GObject)

/**
 * HtmxElementInterface:
 * @render: Render the element to HTML string
 * @get_attributes: Get all attributes
 * @set_attribute: Set an attribute
 * @get_id: Get element ID
 * @set_id: Set element ID
 * @get_classes: Get CSS classes
 * @add_class: Add a CSS class
 *
 * The interface for HTMX-aware HTML elements.
 */
struct _HtmxElementInterface {
	GTypeInterface parent_iface;

	/* Virtual methods */
	gchar *     (*render)         (HtmxElement *self);
	GList *     (*get_attributes) (HtmxElement *self);
	void        (*set_attribute)  (HtmxElement *self,
	                               const gchar *name,
	                               const gchar *value);
	const gchar *(*get_id)        (HtmxElement *self);
	void        (*set_id)         (HtmxElement *self,
	                               const gchar *id);
	GList *     (*get_classes)    (HtmxElement *self);
	void        (*add_class)      (HtmxElement *self,
	                               const gchar *class_name);
};

/**
 * htmx_element_render:
 * @self: an #HtmxElement
 *
 * Renders the element to an HTML string.
 *
 * Returns: (transfer full): the rendered HTML string. Free with g_free().
 */
gchar *
htmx_element_render(HtmxElement *self);

/**
 * htmx_element_get_attributes:
 * @self: an #HtmxElement
 *
 * Gets all attributes of the element.
 *
 * Returns: (transfer full) (element-type HtmxAttribute): list of attributes.
 *          Free with g_list_free_full() using htmx_attribute_free().
 */
GList *
htmx_element_get_attributes(HtmxElement *self);

/**
 * htmx_element_set_attribute:
 * @self: an #HtmxElement
 * @name: attribute name
 * @value: attribute value
 *
 * Sets an attribute on the element.
 */
void
htmx_element_set_attribute(
	HtmxElement *self,
	const gchar *name,
	const gchar *value
);

/**
 * htmx_element_get_id:
 * @self: an #HtmxElement
 *
 * Gets the element ID.
 *
 * Returns: (transfer none) (nullable): the element ID, or %NULL
 */
const gchar *
htmx_element_get_id(HtmxElement *self);

/**
 * htmx_element_set_id:
 * @self: an #HtmxElement
 * @id: the element ID
 *
 * Sets the element ID.
 */
void
htmx_element_set_id(
	HtmxElement *self,
	const gchar *id
);

/**
 * htmx_element_get_classes:
 * @self: an #HtmxElement
 *
 * Gets the CSS classes.
 *
 * Returns: (transfer full) (element-type utf8): list of class names.
 *          Free with g_list_free_full() using g_free().
 */
GList *
htmx_element_get_classes(HtmxElement *self);

/**
 * htmx_element_add_class:
 * @self: an #HtmxElement
 * @class_name: CSS class name
 *
 * Adds a CSS class to the element.
 */
void
htmx_element_add_class(
	HtmxElement *self,
	const gchar *class_name
);

/*
 * HTMX-specific attribute setters (implemented as default interface methods)
 */

/**
 * htmx_element_set_hx_get:
 * @self: an #HtmxElement
 * @url: the URL for GET request
 *
 * Sets the hx-get attribute.
 */
void
htmx_element_set_hx_get(
	HtmxElement *self,
	const gchar *url
);

/**
 * htmx_element_set_hx_post:
 * @self: an #HtmxElement
 * @url: the URL for POST request
 *
 * Sets the hx-post attribute.
 */
void
htmx_element_set_hx_post(
	HtmxElement *self,
	const gchar *url
);

/**
 * htmx_element_set_hx_put:
 * @self: an #HtmxElement
 * @url: the URL for PUT request
 *
 * Sets the hx-put attribute.
 */
void
htmx_element_set_hx_put(
	HtmxElement *self,
	const gchar *url
);

/**
 * htmx_element_set_hx_patch:
 * @self: an #HtmxElement
 * @url: the URL for PATCH request
 *
 * Sets the hx-patch attribute.
 */
void
htmx_element_set_hx_patch(
	HtmxElement *self,
	const gchar *url
);

/**
 * htmx_element_set_hx_delete:
 * @self: an #HtmxElement
 * @url: the URL for DELETE request
 *
 * Sets the hx-delete attribute.
 */
void
htmx_element_set_hx_delete(
	HtmxElement *self,
	const gchar *url
);

/**
 * htmx_element_set_hx_target:
 * @self: an #HtmxElement
 * @target: CSS selector for target
 *
 * Sets the hx-target attribute.
 */
void
htmx_element_set_hx_target(
	HtmxElement *self,
	const gchar *target
);

/**
 * htmx_element_set_hx_swap:
 * @self: an #HtmxElement
 * @swap_style: the swap style
 *
 * Sets the hx-swap attribute.
 */
void
htmx_element_set_hx_swap(
	HtmxElement  *self,
	HtmxSwapStyle swap_style
);

/**
 * htmx_element_set_hx_swap_with_config:
 * @self: an #HtmxElement
 * @swap: the swap configuration
 *
 * Sets the hx-swap attribute with full configuration.
 */
void
htmx_element_set_hx_swap_with_config(
	HtmxElement    *self,
	const HtmxSwap *swap
);

/**
 * htmx_element_set_hx_trigger:
 * @self: an #HtmxElement
 * @trigger: the trigger configuration
 *
 * Sets the hx-trigger attribute.
 */
void
htmx_element_set_hx_trigger(
	HtmxElement       *self,
	const HtmxTrigger *trigger
);

/**
 * htmx_element_set_hx_trigger_string:
 * @self: an #HtmxElement
 * @trigger: trigger string
 *
 * Sets the hx-trigger attribute from a string.
 */
void
htmx_element_set_hx_trigger_string(
	HtmxElement *self,
	const gchar *trigger
);

/**
 * htmx_element_set_hx_confirm:
 * @self: an #HtmxElement
 * @message: confirmation message
 *
 * Sets the hx-confirm attribute.
 */
void
htmx_element_set_hx_confirm(
	HtmxElement *self,
	const gchar *message
);

/**
 * htmx_element_set_hx_boost:
 * @self: an #HtmxElement
 * @boost: whether to enable boost
 *
 * Sets the hx-boost attribute.
 */
void
htmx_element_set_hx_boost(
	HtmxElement *self,
	gboolean     boost
);

/**
 * htmx_element_set_hx_push_url:
 * @self: an #HtmxElement
 * @url: URL to push (or "true"/"false")
 *
 * Sets the hx-push-url attribute.
 */
void
htmx_element_set_hx_push_url(
	HtmxElement *self,
	const gchar *url
);

/**
 * htmx_element_set_hx_vals:
 * @self: an #HtmxElement
 * @json: JSON values
 *
 * Sets the hx-vals attribute.
 */
void
htmx_element_set_hx_vals(
	HtmxElement *self,
	const gchar *json
);

/**
 * htmx_element_set_hx_include:
 * @self: an #HtmxElement
 * @selector: CSS selector for elements to include
 *
 * Sets the hx-include attribute.
 */
void
htmx_element_set_hx_include(
	HtmxElement *self,
	const gchar *selector
);

/**
 * htmx_element_set_hx_indicator:
 * @self: an #HtmxElement
 * @selector: CSS selector for indicator element
 *
 * Sets the hx-indicator attribute.
 */
void
htmx_element_set_hx_indicator(
	HtmxElement *self,
	const gchar *selector
);

G_END_DECLS

#endif /* HTMX_ELEMENT_H */
