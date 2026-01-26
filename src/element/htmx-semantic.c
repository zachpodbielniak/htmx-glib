/*
 * htmx-semantic.c - HTML semantic container elements implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-semantic.h"

/*
 * HtmxArticle - Article element
 */

struct _HtmxArticle {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxArticle, htmx_article, HTMX_TYPE_NODE)

static void
htmx_article_class_init(HtmxArticleClass *klass)
{
}

static void
htmx_article_init(HtmxArticle *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "article");
}

HtmxArticle *
htmx_article_new(void)
{
	return g_object_new(HTMX_TYPE_ARTICLE, NULL);
}

/*
 * HtmxSection - Section element
 */

struct _HtmxSection {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxSection, htmx_section, HTMX_TYPE_NODE)

static void
htmx_section_class_init(HtmxSectionClass *klass)
{
}

static void
htmx_section_init(HtmxSection *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "section");
}

HtmxSection *
htmx_section_new(void)
{
	return g_object_new(HTMX_TYPE_SECTION, NULL);
}

/*
 * HtmxNav - Navigation element
 */

struct _HtmxNav {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxNav, htmx_nav, HTMX_TYPE_NODE)

static void
htmx_nav_class_init(HtmxNavClass *klass)
{
}

static void
htmx_nav_init(HtmxNav *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "nav");
}

HtmxNav *
htmx_nav_new(void)
{
	return g_object_new(HTMX_TYPE_NAV, NULL);
}

/*
 * HtmxHeader - Header element
 */

struct _HtmxHeader {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxHeader, htmx_header, HTMX_TYPE_NODE)

static void
htmx_header_class_init(HtmxHeaderClass *klass)
{
}

static void
htmx_header_init(HtmxHeader *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "header");
}

HtmxHeader *
htmx_header_new(void)
{
	return g_object_new(HTMX_TYPE_HEADER, NULL);
}

/*
 * HtmxFooter - Footer element
 */

struct _HtmxFooter {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxFooter, htmx_footer, HTMX_TYPE_NODE)

static void
htmx_footer_class_init(HtmxFooterClass *klass)
{
}

static void
htmx_footer_init(HtmxFooter *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "footer");
}

HtmxFooter *
htmx_footer_new(void)
{
	return g_object_new(HTMX_TYPE_FOOTER, NULL);
}

/*
 * HtmxAside - Aside element
 */

struct _HtmxAside {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxAside, htmx_aside, HTMX_TYPE_NODE)

static void
htmx_aside_class_init(HtmxAsideClass *klass)
{
}

static void
htmx_aside_init(HtmxAside *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "aside");
}

HtmxAside *
htmx_aside_new(void)
{
	return g_object_new(HTMX_TYPE_ASIDE, NULL);
}

/*
 * HtmxMain - Main element
 */

struct _HtmxMain {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxMain, htmx_main, HTMX_TYPE_NODE)

static void
htmx_main_class_init(HtmxMainClass *klass)
{
}

static void
htmx_main_init(HtmxMain *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "main");
}

HtmxMain *
htmx_main_new(void)
{
	return g_object_new(HTMX_TYPE_MAIN, NULL);
}

/*
 * HtmxFigure - Figure element
 */

struct _HtmxFigure {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxFigure, htmx_figure, HTMX_TYPE_NODE)

static void
htmx_figure_class_init(HtmxFigureClass *klass)
{
}

static void
htmx_figure_init(HtmxFigure *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "figure");
}

HtmxFigure *
htmx_figure_new(void)
{
	return g_object_new(HTMX_TYPE_FIGURE, NULL);
}

/*
 * HtmxFigcaption - Figure caption element
 */

struct _HtmxFigcaption {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxFigcaption, htmx_figcaption, HTMX_TYPE_NODE)

static void
htmx_figcaption_class_init(HtmxFigcaptionClass *klass)
{
}

static void
htmx_figcaption_init(HtmxFigcaption *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "figcaption");
}

HtmxFigcaption *
htmx_figcaption_new(void)
{
	return g_object_new(HTMX_TYPE_FIGCAPTION, NULL);
}

HtmxFigcaption *
htmx_figcaption_new_with_text(const gchar *text)
{
	HtmxFigcaption *self;

	self = htmx_figcaption_new();
	if (text != NULL) {
		htmx_node_set_text_content(HTMX_NODE(self), text);
	}

	return self;
}

/*
 * HtmxAddress - Address element
 */

struct _HtmxAddress {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxAddress, htmx_address, HTMX_TYPE_NODE)

static void
htmx_address_class_init(HtmxAddressClass *klass)
{
}

static void
htmx_address_init(HtmxAddress *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "address");
}

HtmxAddress *
htmx_address_new(void)
{
	return g_object_new(HTMX_TYPE_ADDRESS, NULL);
}

/*
 * HtmxHgroup - Heading group element
 */

struct _HtmxHgroup {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxHgroup, htmx_hgroup, HTMX_TYPE_NODE)

static void
htmx_hgroup_class_init(HtmxHgroupClass *klass)
{
}

static void
htmx_hgroup_init(HtmxHgroup *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "hgroup");
}

HtmxHgroup *
htmx_hgroup_new(void)
{
	return g_object_new(HTMX_TYPE_HGROUP, NULL);
}

/*
 * HtmxSearch - Search section element
 */

struct _HtmxSearch {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxSearch, htmx_search, HTMX_TYPE_NODE)

static void
htmx_search_class_init(HtmxSearchClass *klass)
{
}

static void
htmx_search_init(HtmxSearch *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "search");
}

HtmxSearch *
htmx_search_new(void)
{
	return g_object_new(HTMX_TYPE_SEARCH, NULL);
}
