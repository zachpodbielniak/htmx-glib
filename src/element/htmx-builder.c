/*
 * htmx-builder.c - Fluent HTML builder implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-builder.h"
#include "htmx-fragment.h"

struct _HtmxBuilder {
	GObject parent_instance;

	HtmxNode *root;         /* Root node of the tree */
	GList    *stack;        /* Stack of open nodes (current is at head) */
};

G_DEFINE_FINAL_TYPE(HtmxBuilder, htmx_builder, G_TYPE_OBJECT)

/*
 * Get the current node (top of stack)
 */
static HtmxNode *
get_current(HtmxBuilder *self)
{
	if (self->stack == NULL) {
		return self->root;
	}
	return HTMX_NODE(self->stack->data);
}

/*
 * Object finalization
 */
static void
htmx_builder_finalize(GObject *object)
{
	HtmxBuilder *self = HTMX_BUILDER(object);

	g_clear_object(&self->root);
	g_list_free(self->stack);

	G_OBJECT_CLASS(htmx_builder_parent_class)->finalize(object);
}

/*
 * Class initialization
 */
static void
htmx_builder_class_init(HtmxBuilderClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);

	object_class->finalize = htmx_builder_finalize;
}

/*
 * Instance initialization
 */
static void
htmx_builder_init(HtmxBuilder *self)
{
	self->root = NULL;
	self->stack = NULL;
}

/*
 * Create a new builder
 */
HtmxBuilder *
htmx_builder_new(void)
{
	return g_object_new(HTMX_TYPE_BUILDER, NULL);
}

/*
 * Begin a new element
 */
HtmxBuilder *
htmx_builder_begin(
	HtmxBuilder *self,
	const gchar *tag_name
){
	HtmxNode *node;
	HtmxNode *current;

	g_return_val_if_fail(HTMX_IS_BUILDER(self), NULL);
	g_return_val_if_fail(tag_name != NULL, NULL);

	node = htmx_node_new(tag_name);

	if (self->root == NULL) {
		self->root = node;
	} else {
		current = get_current(self);
		htmx_node_add_child(current, node);
		g_object_unref(node);  /* Release creation ref; parent now owns it */
	}

	self->stack = g_list_prepend(self->stack, node);

	return self;
}

/*
 * End the current element
 */
HtmxBuilder *
htmx_builder_end(HtmxBuilder *self)
{
	g_return_val_if_fail(HTMX_IS_BUILDER(self), NULL);
	g_return_val_if_fail(self->stack != NULL, NULL);

	self->stack = g_list_delete_link(self->stack, self->stack);

	return self;
}

/*
 * Add a void element
 */
HtmxBuilder *
htmx_builder_void(
	HtmxBuilder *self,
	const gchar *tag_name
){
	HtmxNode *node;
	HtmxNode *current;

	g_return_val_if_fail(HTMX_IS_BUILDER(self), NULL);
	g_return_val_if_fail(tag_name != NULL, NULL);

	node = htmx_node_new(tag_name);
	htmx_node_set_void_element(node, TRUE);

	if (self->root == NULL) {
		self->root = node;
	} else {
		current = get_current(self);
		htmx_node_add_child(current, node);
		g_object_unref(node);  /* Release creation ref; parent now owns it */
	}

	return self;
}

/*
 * Set element ID
 */
HtmxBuilder *
htmx_builder_id(
	HtmxBuilder *self,
	const gchar *id
){
	HtmxNode *current;

	g_return_val_if_fail(HTMX_IS_BUILDER(self), NULL);

	current = get_current(self);
	g_return_val_if_fail(current != NULL, NULL);

	htmx_element_set_id(HTMX_ELEMENT(current), id);

	return self;
}

/*
 * Add CSS class
 */
HtmxBuilder *
htmx_builder_class(
	HtmxBuilder *self,
	const gchar *class_name
){
	HtmxNode *current;

	g_return_val_if_fail(HTMX_IS_BUILDER(self), NULL);

	current = get_current(self);
	g_return_val_if_fail(current != NULL, NULL);

	htmx_element_add_class(HTMX_ELEMENT(current), class_name);

	return self;
}

/*
 * Set attribute
 */
HtmxBuilder *
htmx_builder_attr(
	HtmxBuilder *self,
	const gchar *name,
	const gchar *value
){
	HtmxNode *current;

	g_return_val_if_fail(HTMX_IS_BUILDER(self), NULL);

	current = get_current(self);
	g_return_val_if_fail(current != NULL, NULL);

	htmx_element_set_attribute(HTMX_ELEMENT(current), name, value);

	return self;
}

/*
 * Add text content
 */
HtmxBuilder *
htmx_builder_text(
	HtmxBuilder *self,
	const gchar *text
){
	HtmxNode *current;

	g_return_val_if_fail(HTMX_IS_BUILDER(self), NULL);

	current = get_current(self);
	g_return_val_if_fail(current != NULL, NULL);

	htmx_node_set_text_content(current, text);

	return self;
}

/*
 * Add raw HTML content
 */
HtmxBuilder *
htmx_builder_html(
	HtmxBuilder *self,
	const gchar *html
){
	HtmxNode *current;

	g_return_val_if_fail(HTMX_IS_BUILDER(self), NULL);

	current = get_current(self);
	g_return_val_if_fail(current != NULL, NULL);

	htmx_node_set_html_content(current, html);

	return self;
}

/*
 * HTMX attribute setters
 */

HtmxBuilder *
htmx_builder_hx_get(
	HtmxBuilder *self,
	const gchar *url
){
	return htmx_builder_attr(self, "hx-get", url);
}

HtmxBuilder *
htmx_builder_hx_post(
	HtmxBuilder *self,
	const gchar *url
){
	return htmx_builder_attr(self, "hx-post", url);
}

HtmxBuilder *
htmx_builder_hx_put(
	HtmxBuilder *self,
	const gchar *url
){
	return htmx_builder_attr(self, "hx-put", url);
}

HtmxBuilder *
htmx_builder_hx_patch(
	HtmxBuilder *self,
	const gchar *url
){
	return htmx_builder_attr(self, "hx-patch", url);
}

HtmxBuilder *
htmx_builder_hx_delete(
	HtmxBuilder *self,
	const gchar *url
){
	return htmx_builder_attr(self, "hx-delete", url);
}

HtmxBuilder *
htmx_builder_hx_target(
	HtmxBuilder *self,
	const gchar *target
){
	return htmx_builder_attr(self, "hx-target", target);
}

HtmxBuilder *
htmx_builder_hx_swap(
	HtmxBuilder  *self,
	HtmxSwapStyle swap_style
){
	const gchar *swap_str;

	swap_str = htmx_swap_style_to_string(swap_style);
	return htmx_builder_attr(self, "hx-swap", swap_str);
}

HtmxBuilder *
htmx_builder_hx_trigger(
	HtmxBuilder *self,
	const gchar *trigger
){
	return htmx_builder_attr(self, "hx-trigger", trigger);
}

HtmxBuilder *
htmx_builder_hx_confirm(
	HtmxBuilder *self,
	const gchar *message
){
	return htmx_builder_attr(self, "hx-confirm", message);
}

HtmxBuilder *
htmx_builder_hx_boost(HtmxBuilder *self)
{
	return htmx_builder_attr(self, "hx-boost", "true");
}

HtmxBuilder *
htmx_builder_hx_push_url(
	HtmxBuilder *self,
	const gchar *url
){
	return htmx_builder_attr(self, "hx-push-url", url);
}

HtmxBuilder *
htmx_builder_hx_vals(
	HtmxBuilder *self,
	const gchar *json
){
	return htmx_builder_attr(self, "hx-vals", json);
}

HtmxBuilder *
htmx_builder_hx_indicator(
	HtmxBuilder *self,
	const gchar *selector
){
	return htmx_builder_attr(self, "hx-indicator", selector);
}

/*
 * Render to string
 */
gchar *
htmx_builder_render(HtmxBuilder *self)
{
	g_return_val_if_fail(HTMX_IS_BUILDER(self), NULL);

	if (self->root == NULL) {
		return g_strdup("");
	}

	return htmx_element_render(HTMX_ELEMENT(self->root));
}

/*
 * Get root node
 */
HtmxNode *
htmx_builder_get_root(HtmxBuilder *self)
{
	g_return_val_if_fail(HTMX_IS_BUILDER(self), NULL);

	return self->root;
}

/*
 * Reset the builder
 */
void
htmx_builder_reset(HtmxBuilder *self)
{
	g_return_if_fail(HTMX_IS_BUILDER(self));

	g_clear_object(&self->root);
	g_list_free(self->stack);
	self->stack = NULL;
}

/*
 * Add HTML5 doctype declaration
 */
void
htmx_builder_doctype(HtmxBuilder *self)
{
	HtmxNode *node;

	g_return_if_fail(HTMX_IS_BUILDER(self));

	/* Create a special node for doctype */
	node = htmx_node_new("!DOCTYPE html");
	htmx_node_set_void_element(node, TRUE);

	if (self->root == NULL) {
		self->root = node;
	} else {
		/* Doctype should be first, prepend to root's children */
		HtmxNode *current = get_current(self);
		htmx_node_add_child(current, node);
		g_object_unref(node);  /* Release creation ref; parent now owns it */
	}
}

/*
 * Begin a void element (like input, br, hr, img, meta)
 */
void
htmx_builder_begin_void(
	HtmxBuilder *self,
	const gchar *tag_name
){
	HtmxNode *node;
	HtmxNode *current;

	g_return_if_fail(HTMX_IS_BUILDER(self));
	g_return_if_fail(tag_name != NULL);

	node = htmx_node_new(tag_name);
	htmx_node_set_void_element(node, TRUE);

	if (self->root == NULL) {
		self->root = node;
	} else {
		current = get_current(self);
		htmx_node_add_child(current, node);
		g_object_unref(node);  /* Release creation ref; parent now owns it */
	}

	/* Push onto stack so attributes can be added */
	self->stack = g_list_prepend(self->stack, node);
}

/*
 * End a void element
 */
void
htmx_builder_end_void(HtmxBuilder *self)
{
	g_return_if_fail(HTMX_IS_BUILDER(self));
	g_return_if_fail(self->stack != NULL);

	self->stack = g_list_delete_link(self->stack, self->stack);
}

/*
 * Insert raw HTML without escaping.
 * Creates a fragment child to preserve ordering with other children.
 */
void
htmx_builder_raw_html(
	HtmxBuilder *self,
	const gchar *html
){
	HtmxNode *current;
	g_autoptr(HtmxFragment) fragment = NULL;

	g_return_if_fail(HTMX_IS_BUILDER(self));

	if (html == NULL || html[0] == '\0') {
		return;
	}

	current = get_current(self);
	if (current != NULL) {
		/* Create a fragment to hold the raw HTML as a child node */
		/* This ensures raw HTML is rendered in order with other children */
		fragment = htmx_fragment_new();
		htmx_node_set_html_content(HTMX_NODE(fragment), html);
		htmx_node_add_child(current, HTMX_NODE(fragment));
	}
}
