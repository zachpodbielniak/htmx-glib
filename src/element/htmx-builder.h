/*
 * htmx-builder.h - Fluent HTML builder
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * This file defines the HtmxBuilder class for fluent HTML construction
 * with HTMX attributes.
 */

#ifndef HTMX_BUILDER_H
#define HTMX_BUILDER_H

#include <glib-object.h>
#include "core/htmx-enums.h"
#include "model/htmx-swap.h"
#include "model/htmx-trigger.h"
#include "htmx-node.h"

G_BEGIN_DECLS

#define HTMX_TYPE_BUILDER (htmx_builder_get_type())
G_DECLARE_FINAL_TYPE(HtmxBuilder, htmx_builder, HTMX, BUILDER, GObject)

/**
 * htmx_builder_new:
 *
 * Creates a new #HtmxBuilder.
 *
 * Returns: (transfer full): a new #HtmxBuilder
 */
HtmxBuilder *
htmx_builder_new(void);

/**
 * htmx_builder_begin:
 * @self: an #HtmxBuilder
 * @tag_name: HTML tag name
 *
 * Begins a new element. Must be paired with htmx_builder_end().
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxBuilder *
htmx_builder_begin(
	HtmxBuilder *self,
	const gchar *tag_name
);

/**
 * htmx_builder_end:
 * @self: an #HtmxBuilder
 *
 * Ends the current element.
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxBuilder *
htmx_builder_end(HtmxBuilder *self);

/**
 * htmx_builder_void:
 * @self: an #HtmxBuilder
 * @tag_name: HTML tag name
 *
 * Adds a void element (no closing tag, like <br>, <img>, <input>).
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxBuilder *
htmx_builder_void(
	HtmxBuilder *self,
	const gchar *tag_name
);

/**
 * htmx_builder_id:
 * @self: an #HtmxBuilder
 * @id: element ID
 *
 * Sets the ID of the current element.
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxBuilder *
htmx_builder_id(
	HtmxBuilder *self,
	const gchar *id
);

/**
 * htmx_builder_class:
 * @self: an #HtmxBuilder
 * @class_name: CSS class name
 *
 * Adds a CSS class to the current element.
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxBuilder *
htmx_builder_class(
	HtmxBuilder *self,
	const gchar *class_name
);

/**
 * htmx_builder_attr:
 * @self: an #HtmxBuilder
 * @name: attribute name
 * @value: attribute value
 *
 * Sets an attribute on the current element.
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxBuilder *
htmx_builder_attr(
	HtmxBuilder *self,
	const gchar *name,
	const gchar *value
);

/**
 * htmx_builder_text:
 * @self: an #HtmxBuilder
 * @text: text content
 *
 * Adds text content (HTML escaped).
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxBuilder *
htmx_builder_text(
	HtmxBuilder *self,
	const gchar *text
);

/**
 * htmx_builder_html:
 * @self: an #HtmxBuilder
 * @html: raw HTML content
 *
 * Adds raw HTML content (not escaped).
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxBuilder *
htmx_builder_html(
	HtmxBuilder *self,
	const gchar *html
);

/*
 * HTMX attribute setters
 */

/**
 * htmx_builder_hx_get:
 * @self: an #HtmxBuilder
 * @url: URL for GET request
 *
 * Sets hx-get attribute.
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxBuilder *
htmx_builder_hx_get(
	HtmxBuilder *self,
	const gchar *url
);

/**
 * htmx_builder_hx_post:
 * @self: an #HtmxBuilder
 * @url: URL for POST request
 *
 * Sets hx-post attribute.
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxBuilder *
htmx_builder_hx_post(
	HtmxBuilder *self,
	const gchar *url
);

/**
 * htmx_builder_hx_put:
 * @self: an #HtmxBuilder
 * @url: URL for PUT request
 *
 * Sets hx-put attribute.
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxBuilder *
htmx_builder_hx_put(
	HtmxBuilder *self,
	const gchar *url
);

/**
 * htmx_builder_hx_patch:
 * @self: an #HtmxBuilder
 * @url: URL for PATCH request
 *
 * Sets hx-patch attribute.
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxBuilder *
htmx_builder_hx_patch(
	HtmxBuilder *self,
	const gchar *url
);

/**
 * htmx_builder_hx_delete:
 * @self: an #HtmxBuilder
 * @url: URL for DELETE request
 *
 * Sets hx-delete attribute.
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxBuilder *
htmx_builder_hx_delete(
	HtmxBuilder *self,
	const gchar *url
);

/**
 * htmx_builder_hx_target:
 * @self: an #HtmxBuilder
 * @target: CSS selector for target
 *
 * Sets hx-target attribute.
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxBuilder *
htmx_builder_hx_target(
	HtmxBuilder *self,
	const gchar *target
);

/**
 * htmx_builder_hx_swap:
 * @self: an #HtmxBuilder
 * @swap_style: swap style
 *
 * Sets hx-swap attribute.
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxBuilder *
htmx_builder_hx_swap(
	HtmxBuilder  *self,
	HtmxSwapStyle swap_style
);

/**
 * htmx_builder_hx_trigger:
 * @self: an #HtmxBuilder
 * @trigger: trigger string
 *
 * Sets hx-trigger attribute.
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxBuilder *
htmx_builder_hx_trigger(
	HtmxBuilder *self,
	const gchar *trigger
);

/**
 * htmx_builder_hx_confirm:
 * @self: an #HtmxBuilder
 * @message: confirmation message
 *
 * Sets hx-confirm attribute.
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxBuilder *
htmx_builder_hx_confirm(
	HtmxBuilder *self,
	const gchar *message
);

/**
 * htmx_builder_hx_boost:
 * @self: an #HtmxBuilder
 *
 * Sets hx-boost="true".
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxBuilder *
htmx_builder_hx_boost(HtmxBuilder *self);

/**
 * htmx_builder_hx_push_url:
 * @self: an #HtmxBuilder
 * @url: URL or "true"
 *
 * Sets hx-push-url attribute.
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxBuilder *
htmx_builder_hx_push_url(
	HtmxBuilder *self,
	const gchar *url
);

/**
 * htmx_builder_hx_vals:
 * @self: an #HtmxBuilder
 * @json: JSON values
 *
 * Sets hx-vals attribute.
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxBuilder *
htmx_builder_hx_vals(
	HtmxBuilder *self,
	const gchar *json
);

/**
 * htmx_builder_hx_indicator:
 * @self: an #HtmxBuilder
 * @selector: CSS selector for indicator
 *
 * Sets hx-indicator attribute.
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxBuilder *
htmx_builder_hx_indicator(
	HtmxBuilder *self,
	const gchar *selector
);

/**
 * htmx_builder_render:
 * @self: an #HtmxBuilder
 *
 * Renders the built HTML to a string.
 *
 * Returns: (transfer full): the rendered HTML. Free with g_free().
 */
gchar *
htmx_builder_render(HtmxBuilder *self);

/**
 * htmx_builder_get_root:
 * @self: an #HtmxBuilder
 *
 * Gets the root node of the built tree.
 *
 * Returns: (transfer none) (nullable): the root #HtmxNode, or %NULL if empty
 */
HtmxNode *
htmx_builder_get_root(HtmxBuilder *self);

/**
 * htmx_builder_reset:
 * @self: an #HtmxBuilder
 *
 * Resets the builder for reuse.
 */
void
htmx_builder_reset(HtmxBuilder *self);

/**
 * htmx_builder_doctype:
 * @self: an #HtmxBuilder
 *
 * Adds an HTML5 doctype declaration.
 */
void
htmx_builder_doctype(HtmxBuilder *self);

/**
 * htmx_builder_begin_void:
 * @self: an #HtmxBuilder
 * @tag: the void element tag name
 *
 * Begins a void element (like input, br, hr, img, meta).
 * Must be paired with htmx_builder_end_void().
 */
void
htmx_builder_begin_void(
	HtmxBuilder *self,
	const gchar *tag
);

/**
 * htmx_builder_end_void:
 * @self: an #HtmxBuilder
 *
 * Ends a void element started with htmx_builder_begin_void().
 */
void
htmx_builder_end_void(HtmxBuilder *self);

/**
 * htmx_builder_raw_html:
 * @self: an #HtmxBuilder
 * @html: raw HTML string to insert
 *
 * Inserts raw HTML without escaping.
 */
void
htmx_builder_raw_html(
	HtmxBuilder *self,
	const gchar *html
);

G_END_DECLS

#endif /* HTMX_BUILDER_H */
