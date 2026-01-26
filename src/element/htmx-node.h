/*
 * htmx-node.h - Base HTML node class
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * This file defines the HtmxNode class, the base class for all HTML elements.
 * It implements the HtmxElement interface and provides common functionality.
 */

#ifndef HTMX_NODE_H
#define HTMX_NODE_H

#include <glib-object.h>
#include "htmx-element.h"

G_BEGIN_DECLS

#define HTMX_TYPE_NODE (htmx_node_get_type())
G_DECLARE_DERIVABLE_TYPE(HtmxNode, htmx_node, HTMX, NODE, GObject)

/**
 * HtmxNodeClass:
 * @parent_class: the parent class
 * @render_opening: render opening tag
 * @render_closing: render closing tag
 * @render_content: render content between tags
 *
 * The class structure for #HtmxNode.
 */
struct _HtmxNodeClass {
	GObjectClass parent_class;

	/* Virtual methods */
	gchar *(*render_opening) (HtmxNode *self);
	gchar *(*render_closing) (HtmxNode *self);
	gchar *(*render_content) (HtmxNode *self);

	/* Padding for future expansion */
	gpointer padding[8];
};

/**
 * htmx_node_new:
 * @tag_name: the HTML tag name
 *
 * Creates a new #HtmxNode with the given tag name.
 *
 * Returns: (transfer full): a new #HtmxNode
 */
HtmxNode *
htmx_node_new(const gchar *tag_name);

/**
 * htmx_node_get_tag_name:
 * @self: an #HtmxNode
 *
 * Gets the tag name.
 *
 * Returns: (transfer none): the tag name
 */
const gchar *
htmx_node_get_tag_name(HtmxNode *self);

/**
 * htmx_node_set_tag_name:
 * @self: an #HtmxNode
 * @tag_name: the HTML tag name
 *
 * Sets the tag name for the node. This is typically called from
 * the init function of derived types to set their tag name.
 */
void
htmx_node_set_tag_name(
	HtmxNode    *self,
	const gchar *tag_name
);

/**
 * htmx_node_set_text_content:
 * @self: an #HtmxNode
 * @text: text content
 *
 * Sets the text content of the node.
 */
void
htmx_node_set_text_content(
	HtmxNode    *self,
	const gchar *text
);

/**
 * htmx_node_get_text_content:
 * @self: an #HtmxNode
 *
 * Gets the text content.
 *
 * Returns: (transfer none) (nullable): the text content, or %NULL
 */
const gchar *
htmx_node_get_text_content(HtmxNode *self);

/**
 * htmx_node_set_html_content:
 * @self: an #HtmxNode
 * @html: raw HTML content
 *
 * Sets raw HTML content (not escaped).
 */
void
htmx_node_set_html_content(
	HtmxNode    *self,
	const gchar *html
);

/**
 * htmx_node_append_html_content:
 * @self: an #HtmxNode
 * @html: (nullable): raw HTML content to append
 *
 * Appends raw HTML content to the node's existing HTML content.
 * If the node has no existing HTML content, this is equivalent to
 * htmx_node_set_html_content(). If @html is %NULL, this function
 * does nothing.
 */
void
htmx_node_append_html_content(
	HtmxNode    *self,
	const gchar *html
);

/**
 * htmx_node_add_child:
 * @self: an #HtmxNode
 * @child: (transfer full): child node
 *
 * Adds a child node. Takes ownership of @child.
 */
void
htmx_node_add_child(
	HtmxNode *self,
	HtmxNode *child
);

/**
 * htmx_node_get_children:
 * @self: an #HtmxNode
 *
 * Gets the child nodes.
 *
 * Returns: (transfer none) (element-type HtmxNode): list of children
 */
GList *
htmx_node_get_children(HtmxNode *self);

/**
 * htmx_node_set_void_element:
 * @self: an #HtmxNode
 * @is_void: whether this is a void element
 *
 * Sets whether this is a void element (no closing tag).
 */
void
htmx_node_set_void_element(
	HtmxNode *self,
	gboolean  is_void
);

/**
 * htmx_node_is_void_element:
 * @self: an #HtmxNode
 *
 * Checks if this is a void element.
 *
 * Returns: %TRUE if this is a void element
 */
gboolean
htmx_node_is_void_element(HtmxNode *self);

/*
 * Convenience attribute setters that return self for chaining
 */

/**
 * htmx_node_id:
 * @self: an #HtmxNode
 * @id: element ID
 *
 * Sets the element ID.
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxNode *
htmx_node_id(
	HtmxNode    *self,
	const gchar *id
);

/**
 * htmx_node_class:
 * @self: an #HtmxNode
 * @class_name: CSS class name
 *
 * Adds a CSS class.
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxNode *
htmx_node_class(
	HtmxNode    *self,
	const gchar *class_name
);

/**
 * htmx_node_attr:
 * @self: an #HtmxNode
 * @name: attribute name
 * @value: attribute value
 *
 * Sets an attribute.
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxNode *
htmx_node_attr(
	HtmxNode    *self,
	const gchar *name,
	const gchar *value
);

/**
 * htmx_node_text:
 * @self: an #HtmxNode
 * @text: text content
 *
 * Sets text content.
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxNode *
htmx_node_text(
	HtmxNode    *self,
	const gchar *text
);

/**
 * htmx_node_html:
 * @self: an #HtmxNode
 * @html: raw HTML
 *
 * Sets raw HTML content.
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxNode *
htmx_node_html(
	HtmxNode    *self,
	const gchar *html
);

/**
 * htmx_node_child:
 * @self: an #HtmxNode
 * @child: (transfer full): child node
 *
 * Adds a child node.
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxNode *
htmx_node_child(
	HtmxNode *self,
	HtmxNode *child
);

/*
 * HTMX attribute setters with chaining
 */

/**
 * htmx_node_hx_get:
 * @self: an #HtmxNode
 * @url: URL for GET request
 *
 * Sets hx-get attribute.
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxNode *
htmx_node_hx_get(
	HtmxNode    *self,
	const gchar *url
);

/**
 * htmx_node_hx_post:
 * @self: an #HtmxNode
 * @url: URL for POST request
 *
 * Sets hx-post attribute.
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxNode *
htmx_node_hx_post(
	HtmxNode    *self,
	const gchar *url
);

/**
 * htmx_node_hx_target:
 * @self: an #HtmxNode
 * @target: CSS selector for target
 *
 * Sets hx-target attribute.
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxNode *
htmx_node_hx_target(
	HtmxNode    *self,
	const gchar *target
);

/**
 * htmx_node_hx_swap:
 * @self: an #HtmxNode
 * @swap_style: swap style
 *
 * Sets hx-swap attribute.
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxNode *
htmx_node_hx_swap(
	HtmxNode     *self,
	HtmxSwapStyle swap_style
);

/**
 * htmx_node_hx_trigger:
 * @self: an #HtmxNode
 * @trigger: trigger string
 *
 * Sets hx-trigger attribute.
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxNode *
htmx_node_hx_trigger(
	HtmxNode    *self,
	const gchar *trigger
);

/**
 * htmx_node_hx_confirm:
 * @self: an #HtmxNode
 * @message: confirmation message
 *
 * Sets hx-confirm attribute.
 *
 * Returns: (transfer none): @self for chaining
 */
HtmxNode *
htmx_node_hx_confirm(
	HtmxNode    *self,
	const gchar *message
);

G_END_DECLS

#endif /* HTMX_NODE_H */
