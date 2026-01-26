/*
 * htmx-meta.c - HTML metadata and scripting elements implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-meta.h"

/*
 * HtmxScript - Script element
 */

struct _HtmxScript {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxScript, htmx_script, HTMX_TYPE_NODE)

static void
htmx_script_class_init(HtmxScriptClass *klass)
{
}

static void
htmx_script_init(HtmxScript *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "script");
}

HtmxScript *
htmx_script_new(void)
{
	return g_object_new(HTMX_TYPE_SCRIPT, NULL);
}

HtmxScript *
htmx_script_new_with_src(const gchar *src)
{
	HtmxScript *self;

	self = htmx_script_new();
	if (src != NULL) {
		htmx_script_set_src(self, src);
	}

	return self;
}

HtmxScript *
htmx_script_new_from_file(
	const gchar  *filepath,
	GError      **error
){
	HtmxScript *self;
	g_autofree gchar *contents = NULL;

	g_return_val_if_fail(filepath != NULL, NULL);

	if (!g_file_get_contents(filepath, &contents, NULL, error)) {
		return NULL;
	}

	self = htmx_script_new();
	/* Use html_content to avoid escaping - script content is raw JS */
	htmx_node_set_html_content(HTMX_NODE(self), contents);

	return self;
}

void
htmx_script_set_src(HtmxScript *self, const gchar *src)
{
	g_return_if_fail(HTMX_IS_SCRIPT(self));
	htmx_element_set_attribute(HTMX_ELEMENT(self), "src", src);
}

void
htmx_script_set_type(HtmxScript *self, const gchar *type)
{
	g_return_if_fail(HTMX_IS_SCRIPT(self));
	htmx_element_set_attribute(HTMX_ELEMENT(self), "type", type);
}

void
htmx_script_set_async(HtmxScript *self, gboolean async)
{
	g_return_if_fail(HTMX_IS_SCRIPT(self));
	if (async) {
		htmx_element_set_attribute(HTMX_ELEMENT(self), "async", "async");
	}
}

void
htmx_script_set_defer(HtmxScript *self, gboolean defer)
{
	g_return_if_fail(HTMX_IS_SCRIPT(self));
	if (defer) {
		htmx_element_set_attribute(HTMX_ELEMENT(self), "defer", "defer");
	}
}

/*
 * HtmxNoscript - Noscript element
 */

struct _HtmxNoscript {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxNoscript, htmx_noscript, HTMX_TYPE_NODE)

static void
htmx_noscript_class_init(HtmxNoscriptClass *klass)
{
}

static void
htmx_noscript_init(HtmxNoscript *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "noscript");
}

HtmxNoscript *
htmx_noscript_new(void)
{
	return g_object_new(HTMX_TYPE_NOSCRIPT, NULL);
}

/*
 * HtmxLink - Link element (void element)
 */

struct _HtmxLink {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxLink, htmx_link, HTMX_TYPE_NODE)

static void
htmx_link_class_init(HtmxLinkClass *klass)
{
}

static void
htmx_link_init(HtmxLink *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "link");
	htmx_node_set_void_element(HTMX_NODE(self), TRUE);
}

HtmxLink *
htmx_link_new(void)
{
	return g_object_new(HTMX_TYPE_LINK, NULL);
}

HtmxLink *
htmx_link_new_stylesheet(const gchar *href)
{
	HtmxLink *self;

	self = htmx_link_new();
	htmx_link_set_rel(self, "stylesheet");
	if (href != NULL) {
		htmx_link_set_href(self, href);
	}

	return self;
}

void
htmx_link_set_rel(HtmxLink *self, const gchar *rel)
{
	g_return_if_fail(HTMX_IS_LINK(self));
	htmx_element_set_attribute(HTMX_ELEMENT(self), "rel", rel);
}

void
htmx_link_set_href(HtmxLink *self, const gchar *href)
{
	g_return_if_fail(HTMX_IS_LINK(self));
	htmx_element_set_attribute(HTMX_ELEMENT(self), "href", href);
}

void
htmx_link_set_type(HtmxLink *self, const gchar *type)
{
	g_return_if_fail(HTMX_IS_LINK(self));
	htmx_element_set_attribute(HTMX_ELEMENT(self), "type", type);
}

void
htmx_link_set_media(HtmxLink *self, const gchar *media)
{
	g_return_if_fail(HTMX_IS_LINK(self));
	htmx_element_set_attribute(HTMX_ELEMENT(self), "media", media);
}

/*
 * HtmxMeta - Meta element (void element)
 */

struct _HtmxMeta {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxMeta, htmx_meta, HTMX_TYPE_NODE)

static void
htmx_meta_class_init(HtmxMetaClass *klass)
{
}

static void
htmx_meta_init(HtmxMeta *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "meta");
	htmx_node_set_void_element(HTMX_NODE(self), TRUE);
}

HtmxMeta *
htmx_meta_new(void)
{
	return g_object_new(HTMX_TYPE_META, NULL);
}

HtmxMeta *
htmx_meta_new_name_content(
	const gchar *name,
	const gchar *content
){
	HtmxMeta *self;

	self = htmx_meta_new();
	if (name != NULL) {
		htmx_meta_set_name(self, name);
	}
	if (content != NULL) {
		htmx_meta_set_content(self, content);
	}

	return self;
}

HtmxMeta *
htmx_meta_new_charset(const gchar *charset)
{
	HtmxMeta *self;

	self = htmx_meta_new();
	if (charset != NULL) {
		htmx_meta_set_charset(self, charset);
	}

	return self;
}

void
htmx_meta_set_name(HtmxMeta *self, const gchar *name)
{
	g_return_if_fail(HTMX_IS_META(self));
	htmx_element_set_attribute(HTMX_ELEMENT(self), "name", name);
}

void
htmx_meta_set_content(HtmxMeta *self, const gchar *content)
{
	g_return_if_fail(HTMX_IS_META(self));
	htmx_element_set_attribute(HTMX_ELEMENT(self), "content", content);
}

void
htmx_meta_set_charset(HtmxMeta *self, const gchar *charset)
{
	g_return_if_fail(HTMX_IS_META(self));
	htmx_element_set_attribute(HTMX_ELEMENT(self), "charset", charset);
}

void
htmx_meta_set_http_equiv(HtmxMeta *self, const gchar *http_equiv)
{
	g_return_if_fail(HTMX_IS_META(self));
	htmx_element_set_attribute(HTMX_ELEMENT(self), "http-equiv", http_equiv);
}

/*
 * HtmxStyle - Style element
 */

struct _HtmxStyle {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxStyle, htmx_style, HTMX_TYPE_NODE)

static void
htmx_style_class_init(HtmxStyleClass *klass)
{
}

static void
htmx_style_init(HtmxStyle *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "style");
}

HtmxStyle *
htmx_style_new(void)
{
	return g_object_new(HTMX_TYPE_STYLE, NULL);
}

HtmxStyle *
htmx_style_new_with_css(const gchar *css)
{
	HtmxStyle *self;

	self = htmx_style_new();
	if (css != NULL) {
		/* Use html_content to avoid escaping - style content is raw CSS */
		htmx_node_set_html_content(HTMX_NODE(self), css);
	}

	return self;
}

HtmxStyle *
htmx_style_new_from_file(
	const gchar  *filepath,
	GError      **error
){
	HtmxStyle *self;
	g_autofree gchar *contents = NULL;

	g_return_val_if_fail(filepath != NULL, NULL);

	if (!g_file_get_contents(filepath, &contents, NULL, error)) {
		return NULL;
	}

	self = htmx_style_new();
	/* Use html_content to avoid escaping - style content is raw CSS */
	htmx_node_set_html_content(HTMX_NODE(self), contents);

	return self;
}

void
htmx_style_set_type(HtmxStyle *self, const gchar *type)
{
	g_return_if_fail(HTMX_IS_STYLE(self));
	htmx_element_set_attribute(HTMX_ELEMENT(self), "type", type);
}

void
htmx_style_set_media(HtmxStyle *self, const gchar *media)
{
	g_return_if_fail(HTMX_IS_STYLE(self));
	htmx_element_set_attribute(HTMX_ELEMENT(self), "media", media);
}

/*
 * HtmxHtmlTemplate - HTML template element
 */

struct _HtmxHtmlTemplate {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxHtmlTemplate, htmx_html_template, HTMX_TYPE_NODE)

static void
htmx_html_template_class_init(HtmxHtmlTemplateClass *klass)
{
}

static void
htmx_html_template_init(HtmxHtmlTemplate *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "template");
}

HtmxHtmlTemplate *
htmx_html_template_new(void)
{
	return g_object_new(HTMX_TYPE_HTML_TEMPLATE, NULL);
}

/*
 * HtmxSlot - Slot element
 */

struct _HtmxSlot {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxSlot, htmx_slot, HTMX_TYPE_NODE)

static void
htmx_slot_class_init(HtmxSlotClass *klass)
{
}

static void
htmx_slot_init(HtmxSlot *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "slot");
}

HtmxSlot *
htmx_slot_new(void)
{
	return g_object_new(HTMX_TYPE_SLOT, NULL);
}

HtmxSlot *
htmx_slot_new_named(const gchar *name)
{
	HtmxSlot *self;

	self = htmx_slot_new();
	if (name != NULL) {
		htmx_slot_set_name(self, name);
	}

	return self;
}

void
htmx_slot_set_name(HtmxSlot *self, const gchar *name)
{
	g_return_if_fail(HTMX_IS_SLOT(self));
	htmx_element_set_attribute(HTMX_ELEMENT(self), "name", name);
}
