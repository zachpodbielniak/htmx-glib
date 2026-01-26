/*
 * htmx-document.c - HTML document structure elements implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-document.h"

/*
 * HtmxHtml - Root HTML element
 */

struct _HtmxHtml {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxHtml, htmx_html, HTMX_TYPE_NODE)

static void
htmx_html_class_init(HtmxHtmlClass *klass)
{
}

static void
htmx_html_init(HtmxHtml *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "html");
}

HtmxHtml *
htmx_html_new(void)
{
	return g_object_new(HTMX_TYPE_HTML, NULL);
}

void
htmx_html_set_lang(
	HtmxHtml    *self,
	const gchar *lang
){
	g_return_if_fail(HTMX_IS_HTML(self));

	htmx_element_set_attribute(HTMX_ELEMENT(self), "lang", lang);
}

/*
 * HtmxHead - Document head element
 */

struct _HtmxHead {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxHead, htmx_head, HTMX_TYPE_NODE)

static void
htmx_head_class_init(HtmxHeadClass *klass)
{
}

static void
htmx_head_init(HtmxHead *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "head");
}

HtmxHead *
htmx_head_new(void)
{
	return g_object_new(HTMX_TYPE_HEAD, NULL);
}

/*
 * HtmxBody - Document body element
 */

struct _HtmxBody {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxBody, htmx_body, HTMX_TYPE_NODE)

static void
htmx_body_class_init(HtmxBodyClass *klass)
{
}

static void
htmx_body_init(HtmxBody *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "body");
}

HtmxBody *
htmx_body_new(void)
{
	return g_object_new(HTMX_TYPE_BODY, NULL);
}

/*
 * HtmxTitle - Document title element
 */

struct _HtmxTitle {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxTitle, htmx_title, HTMX_TYPE_NODE)

static void
htmx_title_class_init(HtmxTitleClass *klass)
{
}

static void
htmx_title_init(HtmxTitle *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "title");
}

HtmxTitle *
htmx_title_new(void)
{
	return g_object_new(HTMX_TYPE_TITLE, NULL);
}

HtmxTitle *
htmx_title_new_with_text(const gchar *text)
{
	HtmxTitle *self;

	self = htmx_title_new();
	if (text != NULL) {
		htmx_node_set_text_content(HTMX_NODE(self), text);
	}

	return self;
}

/*
 * HtmxBase - Document base URL element (void element)
 */

struct _HtmxBase {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxBase, htmx_base, HTMX_TYPE_NODE)

static void
htmx_base_class_init(HtmxBaseClass *klass)
{
}

static void
htmx_base_init(HtmxBase *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "base");
	htmx_node_set_void_element(HTMX_NODE(self), TRUE);
}

HtmxBase *
htmx_base_new(void)
{
	return g_object_new(HTMX_TYPE_BASE, NULL);
}

HtmxBase *
htmx_base_new_with_href(const gchar *href)
{
	HtmxBase *self;

	self = htmx_base_new();
	if (href != NULL) {
		htmx_base_set_href(self, href);
	}

	return self;
}

void
htmx_base_set_href(
	HtmxBase    *self,
	const gchar *href
){
	g_return_if_fail(HTMX_IS_BASE(self));

	htmx_element_set_attribute(HTMX_ELEMENT(self), "href", href);
}

void
htmx_base_set_target(
	HtmxBase    *self,
	const gchar *target
){
	g_return_if_fail(HTMX_IS_BASE(self));

	htmx_element_set_attribute(HTMX_ELEMENT(self), "target", target);
}
