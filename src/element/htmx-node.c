/*
 * htmx-node.c - Base HTML node class implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-node.h"
#include <string.h>

typedef struct {
	gchar      *tag_name;
	gchar      *id;
	GList      *classes;       /* List of gchar* */
	GHashTable *attributes;    /* name -> value */
	gchar      *text_content;
	gchar      *html_content;
	GList      *children;      /* List of HtmxNode* */
	gboolean    is_void;       /* Void element (no closing tag) */
} HtmxNodePrivate;

static void htmx_element_interface_init(HtmxElementInterface *iface);

G_DEFINE_TYPE_WITH_CODE(HtmxNode, htmx_node, G_TYPE_OBJECT,
                        G_ADD_PRIVATE(HtmxNode)
                        G_IMPLEMENT_INTERFACE(HTMX_TYPE_ELEMENT,
                                              htmx_element_interface_init))

/*
 * Escape HTML text content
 */
static gchar *
escape_html(const gchar *text)
{
	GString *escaped;
	const gchar *p;

	if (text == NULL) {
		return g_strdup("");
	}

	escaped = g_string_sized_new(strlen(text));

	for (p = text; *p != '\0'; p++) {
		switch (*p) {
		case '&':
			g_string_append(escaped, "&amp;");
			break;
		case '<':
			g_string_append(escaped, "&lt;");
			break;
		case '>':
			g_string_append(escaped, "&gt;");
			break;
		case '"':
			g_string_append(escaped, "&quot;");
			break;
		case '\'':
			g_string_append(escaped, "&#39;");
			break;
		default:
			g_string_append_c(escaped, *p);
			break;
		}
	}

	return g_string_free(escaped, FALSE);
}

/*
 * Escape attribute value
 */
static gchar *
escape_attr(const gchar *value)
{
	GString *escaped;
	const gchar *p;

	if (value == NULL) {
		return g_strdup("");
	}

	escaped = g_string_sized_new(strlen(value));

	for (p = value; *p != '\0'; p++) {
		switch (*p) {
		case '"':
			g_string_append(escaped, "&quot;");
			break;
		case '&':
			g_string_append(escaped, "&amp;");
			break;
		default:
			g_string_append_c(escaped, *p);
			break;
		}
	}

	return g_string_free(escaped, FALSE);
}

/*
 * Default render_opening implementation
 */
static gchar *
htmx_node_real_render_opening(HtmxNode *self)
{
	HtmxNodePrivate *priv = htmx_node_get_instance_private(self);
	GString *result;
	GHashTableIter iter;
	gpointer key, value;
	GList *l;

	result = g_string_new("<");
	g_string_append(result, priv->tag_name);

	/* Add ID if set */
	if (priv->id != NULL) {
		g_autofree gchar *escaped = escape_attr(priv->id);
		g_string_append_printf(result, " id=\"%s\"", escaped);
	}

	/* Add classes if set */
	if (priv->classes != NULL) {
		GString *class_str = g_string_new("");

		for (l = priv->classes; l != NULL; l = l->next) {
			if (class_str->len > 0) {
				g_string_append_c(class_str, ' ');
			}
			g_string_append(class_str, (const gchar *)l->data);
		}

		g_autofree gchar *escaped = escape_attr(class_str->str);
		g_string_append_printf(result, " class=\"%s\"", escaped);
		g_string_free(class_str, TRUE);
	}

	/* Add other attributes */
	g_hash_table_iter_init(&iter, priv->attributes);
	while (g_hash_table_iter_next(&iter, &key, &value)) {
		g_autofree gchar *escaped = escape_attr((const gchar *)value);
		g_string_append_printf(result, " %s=\"%s\"", (const gchar *)key, escaped);
	}

	if (priv->is_void) {
		/* DOCTYPE and other declarations (starting with !) don't use self-closing syntax */
		if (priv->tag_name[0] == '!') {
			g_string_append_c(result, '>');
		} else {
			g_string_append(result, " />");
		}
	} else {
		g_string_append_c(result, '>');
	}

	return g_string_free(result, FALSE);
}

/*
 * Default render_closing implementation
 */
static gchar *
htmx_node_real_render_closing(HtmxNode *self)
{
	HtmxNodePrivate *priv = htmx_node_get_instance_private(self);

	if (priv->is_void) {
		return g_strdup("");
	}

	return g_strdup_printf("</%s>", priv->tag_name);
}

/*
 * Default render_content implementation
 */
static gchar *
htmx_node_real_render_content(HtmxNode *self)
{
	HtmxNodePrivate *priv = htmx_node_get_instance_private(self);
	GString *result;
	GList *l;

	result = g_string_new("");

	/* Add text content (escaped) */
	if (priv->text_content != NULL) {
		g_autofree gchar *escaped = escape_html(priv->text_content);
		g_string_append(result, escaped);
	}

	/* Add HTML content (raw) */
	if (priv->html_content != NULL) {
		g_string_append(result, priv->html_content);
	}

	/* Render children */
	for (l = priv->children; l != NULL; l = l->next) {
		HtmxNode *child = HTMX_NODE(l->data);
		g_autofree gchar *child_html = htmx_element_render(HTMX_ELEMENT(child));
		g_string_append(result, child_html);
	}

	return g_string_free(result, FALSE);
}

/*
 * Interface implementation: render
 */
static gchar *
htmx_node_interface_render(HtmxElement *element)
{
	HtmxNode *self = HTMX_NODE(element);
	HtmxNodeClass *klass = HTMX_NODE_GET_CLASS(self);
	GString *result;

	result = g_string_new("");

	/* Render opening tag */
	if (klass->render_opening != NULL) {
		g_autofree gchar *opening = klass->render_opening(self);
		g_string_append(result, opening);
	}

	/* Render content */
	if (klass->render_content != NULL) {
		g_autofree gchar *content = klass->render_content(self);
		g_string_append(result, content);
	}

	/* Render closing tag */
	if (klass->render_closing != NULL) {
		g_autofree gchar *closing = klass->render_closing(self);
		g_string_append(result, closing);
	}

	return g_string_free(result, FALSE);
}

/*
 * Interface implementation: get_attributes
 */
static GList *
htmx_node_interface_get_attributes(HtmxElement *element)
{
	HtmxNode *self = HTMX_NODE(element);
	HtmxNodePrivate *priv = htmx_node_get_instance_private(self);
	GList *result = NULL;
	GHashTableIter iter;
	gpointer key, value;

	g_hash_table_iter_init(&iter, priv->attributes);
	while (g_hash_table_iter_next(&iter, &key, &value)) {
		HtmxAttribute *attr = htmx_attribute_new((const gchar *)key, (const gchar *)value);
		result = g_list_append(result, attr);
	}

	return result;
}

/*
 * Interface implementation: set_attribute
 */
static void
htmx_node_interface_set_attribute(
	HtmxElement *element,
	const gchar *name,
	const gchar *value
){
	HtmxNode *self = HTMX_NODE(element);
	HtmxNodePrivate *priv = htmx_node_get_instance_private(self);

	g_hash_table_insert(priv->attributes, g_strdup(name), g_strdup(value));
}

/*
 * Interface implementation: get_id
 */
static const gchar *
htmx_node_interface_get_id(HtmxElement *element)
{
	HtmxNode *self = HTMX_NODE(element);
	HtmxNodePrivate *priv = htmx_node_get_instance_private(self);

	return priv->id;
}

/*
 * Interface implementation: set_id
 */
static void
htmx_node_interface_set_id(
	HtmxElement *element,
	const gchar *id
){
	HtmxNode *self = HTMX_NODE(element);
	HtmxNodePrivate *priv = htmx_node_get_instance_private(self);

	g_free(priv->id);
	priv->id = g_strdup(id);
}

/*
 * Interface implementation: get_classes
 */
static GList *
htmx_node_interface_get_classes(HtmxElement *element)
{
	HtmxNode *self = HTMX_NODE(element);
	HtmxNodePrivate *priv = htmx_node_get_instance_private(self);
	GList *result = NULL;
	GList *l;

	for (l = priv->classes; l != NULL; l = l->next) {
		result = g_list_append(result, g_strdup((const gchar *)l->data));
	}

	return result;
}

/*
 * Interface implementation: add_class
 */
static void
htmx_node_interface_add_class(
	HtmxElement *element,
	const gchar *class_name
){
	HtmxNode *self = HTMX_NODE(element);
	HtmxNodePrivate *priv = htmx_node_get_instance_private(self);

	priv->classes = g_list_append(priv->classes, g_strdup(class_name));
}

/*
 * Interface initialization
 */
static void
htmx_element_interface_init(HtmxElementInterface *iface)
{
	iface->render = htmx_node_interface_render;
	iface->get_attributes = htmx_node_interface_get_attributes;
	iface->set_attribute = htmx_node_interface_set_attribute;
	iface->get_id = htmx_node_interface_get_id;
	iface->set_id = htmx_node_interface_set_id;
	iface->get_classes = htmx_node_interface_get_classes;
	iface->add_class = htmx_node_interface_add_class;
}

/*
 * Object finalization
 */
static void
htmx_node_finalize(GObject *object)
{
	HtmxNode *self = HTMX_NODE(object);
	HtmxNodePrivate *priv = htmx_node_get_instance_private(self);

	g_free(priv->tag_name);
	g_free(priv->id);
	g_list_free_full(priv->classes, g_free);
	g_hash_table_unref(priv->attributes);
	g_free(priv->text_content);
	g_free(priv->html_content);
	g_list_free_full(priv->children, g_object_unref);

	G_OBJECT_CLASS(htmx_node_parent_class)->finalize(object);
}

/*
 * Class initialization
 */
static void
htmx_node_class_init(HtmxNodeClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);

	object_class->finalize = htmx_node_finalize;

	klass->render_opening = htmx_node_real_render_opening;
	klass->render_closing = htmx_node_real_render_closing;
	klass->render_content = htmx_node_real_render_content;
}

/*
 * Instance initialization
 */
static void
htmx_node_init(HtmxNode *self)
{
	HtmxNodePrivate *priv = htmx_node_get_instance_private(self);

	priv->tag_name = NULL;
	priv->id = NULL;
	priv->classes = NULL;
	priv->attributes = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
	priv->text_content = NULL;
	priv->html_content = NULL;
	priv->children = NULL;
	priv->is_void = FALSE;
}

/*
 * Create a new node
 */
HtmxNode *
htmx_node_new(const gchar *tag_name)
{
	HtmxNode *self;
	HtmxNodePrivate *priv;

	g_return_val_if_fail(tag_name != NULL, NULL);

	self = g_object_new(HTMX_TYPE_NODE, NULL);
	priv = htmx_node_get_instance_private(self);
	priv->tag_name = g_strdup(tag_name);

	return self;
}

/*
 * Get tag name
 */
const gchar *
htmx_node_get_tag_name(HtmxNode *self)
{
	HtmxNodePrivate *priv;

	g_return_val_if_fail(HTMX_IS_NODE(self), NULL);

	priv = htmx_node_get_instance_private(self);
	return priv->tag_name;
}

/*
 * Set tag name - used by derived types in their init functions
 */
void
htmx_node_set_tag_name(
	HtmxNode    *self,
	const gchar *tag_name
){
	HtmxNodePrivate *priv;

	g_return_if_fail(HTMX_IS_NODE(self));
	g_return_if_fail(tag_name != NULL);

	priv = htmx_node_get_instance_private(self);
	g_free(priv->tag_name);
	priv->tag_name = g_strdup(tag_name);
}

/*
 * Set text content
 */
void
htmx_node_set_text_content(
	HtmxNode    *self,
	const gchar *text
){
	HtmxNodePrivate *priv;

	g_return_if_fail(HTMX_IS_NODE(self));

	priv = htmx_node_get_instance_private(self);
	g_free(priv->text_content);
	priv->text_content = g_strdup(text);
}

/*
 * Get text content
 */
const gchar *
htmx_node_get_text_content(HtmxNode *self)
{
	HtmxNodePrivate *priv;

	g_return_val_if_fail(HTMX_IS_NODE(self), NULL);

	priv = htmx_node_get_instance_private(self);
	return priv->text_content;
}

/*
 * Set HTML content
 */
void
htmx_node_set_html_content(
	HtmxNode    *self,
	const gchar *html
){
	HtmxNodePrivate *priv;

	g_return_if_fail(HTMX_IS_NODE(self));

	priv = htmx_node_get_instance_private(self);
	g_free(priv->html_content);
	priv->html_content = g_strdup(html);
}

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
){
	HtmxNodePrivate *priv;

	g_return_if_fail(HTMX_IS_NODE(self));

	if (html == NULL)
		return;

	priv = htmx_node_get_instance_private(self);

	if (priv->html_content == NULL) {
		priv->html_content = g_strdup(html);
	} else {
		gchar *combined;

		combined = g_strconcat(priv->html_content, html, NULL);
		g_free(priv->html_content);
		priv->html_content = combined;
	}
}

/*
 * Add a child node
 */
void
htmx_node_add_child(
	HtmxNode *self,
	HtmxNode *child
){
	HtmxNodePrivate *priv;

	g_return_if_fail(HTMX_IS_NODE(self));
	g_return_if_fail(HTMX_IS_NODE(child));

	priv = htmx_node_get_instance_private(self);
	priv->children = g_list_append(priv->children, g_object_ref(child));
}

/*
 * Get children
 */
GList *
htmx_node_get_children(HtmxNode *self)
{
	HtmxNodePrivate *priv;

	g_return_val_if_fail(HTMX_IS_NODE(self), NULL);

	priv = htmx_node_get_instance_private(self);
	return priv->children;
}

/*
 * Set void element
 */
void
htmx_node_set_void_element(
	HtmxNode *self,
	gboolean  is_void
){
	HtmxNodePrivate *priv;

	g_return_if_fail(HTMX_IS_NODE(self));

	priv = htmx_node_get_instance_private(self);
	priv->is_void = is_void;
}

/*
 * Check if void element
 */
gboolean
htmx_node_is_void_element(HtmxNode *self)
{
	HtmxNodePrivate *priv;

	g_return_val_if_fail(HTMX_IS_NODE(self), FALSE);

	priv = htmx_node_get_instance_private(self);
	return priv->is_void;
}

/*
 * Chaining setters
 */

HtmxNode *
htmx_node_id(
	HtmxNode    *self,
	const gchar *id
){
	htmx_element_set_id(HTMX_ELEMENT(self), id);
	return self;
}

HtmxNode *
htmx_node_class(
	HtmxNode    *self,
	const gchar *class_name
){
	htmx_element_add_class(HTMX_ELEMENT(self), class_name);
	return self;
}

HtmxNode *
htmx_node_attr(
	HtmxNode    *self,
	const gchar *name,
	const gchar *value
){
	htmx_element_set_attribute(HTMX_ELEMENT(self), name, value);
	return self;
}

HtmxNode *
htmx_node_text(
	HtmxNode    *self,
	const gchar *text
){
	htmx_node_set_text_content(self, text);
	return self;
}

HtmxNode *
htmx_node_html(
	HtmxNode    *self,
	const gchar *html
){
	htmx_node_set_html_content(self, html);
	return self;
}

HtmxNode *
htmx_node_child(
	HtmxNode *self,
	HtmxNode *child
){
	htmx_node_add_child(self, child);
	return self;
}

/*
 * HTMX chaining setters
 */

HtmxNode *
htmx_node_hx_get(
	HtmxNode    *self,
	const gchar *url
){
	htmx_element_set_hx_get(HTMX_ELEMENT(self), url);
	return self;
}

HtmxNode *
htmx_node_hx_post(
	HtmxNode    *self,
	const gchar *url
){
	htmx_element_set_hx_post(HTMX_ELEMENT(self), url);
	return self;
}

HtmxNode *
htmx_node_hx_target(
	HtmxNode    *self,
	const gchar *target
){
	htmx_element_set_hx_target(HTMX_ELEMENT(self), target);
	return self;
}

HtmxNode *
htmx_node_hx_swap(
	HtmxNode     *self,
	HtmxSwapStyle swap_style
){
	htmx_element_set_hx_swap(HTMX_ELEMENT(self), swap_style);
	return self;
}

HtmxNode *
htmx_node_hx_trigger(
	HtmxNode    *self,
	const gchar *trigger
){
	htmx_element_set_hx_trigger_string(HTMX_ELEMENT(self), trigger);
	return self;
}

HtmxNode *
htmx_node_hx_confirm(
	HtmxNode    *self,
	const gchar *message
){
	htmx_element_set_hx_confirm(HTMX_ELEMENT(self), message);
	return self;
}
