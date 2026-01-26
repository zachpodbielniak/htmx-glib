/*
 * htmx-semantic.h - HTML semantic container elements
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * This file defines semantic container elements: article, section, nav,
 * header, footer, aside, main, figure, figcaption, and address.
 */

#ifndef HTMX_SEMANTIC_H
#define HTMX_SEMANTIC_H

#include <glib-object.h>
#include "htmx-node.h"

G_BEGIN_DECLS

/*
 * HtmxArticle - Article element
 */

#define HTMX_TYPE_ARTICLE (htmx_article_get_type())
G_DECLARE_FINAL_TYPE(HtmxArticle, htmx_article, HTMX, ARTICLE, HtmxNode)

/**
 * htmx_article_new:
 *
 * Creates a new #HtmxArticle element.
 *
 * Returns: (transfer full): a new #HtmxArticle
 */
HtmxArticle *
htmx_article_new(void);

/*
 * HtmxSection - Section element
 */

#define HTMX_TYPE_SECTION (htmx_section_get_type())
G_DECLARE_FINAL_TYPE(HtmxSection, htmx_section, HTMX, SECTION, HtmxNode)

/**
 * htmx_section_new:
 *
 * Creates a new #HtmxSection element.
 *
 * Returns: (transfer full): a new #HtmxSection
 */
HtmxSection *
htmx_section_new(void);

/*
 * HtmxNav - Navigation element
 */

#define HTMX_TYPE_NAV (htmx_nav_get_type())
G_DECLARE_FINAL_TYPE(HtmxNav, htmx_nav, HTMX, NAV, HtmxNode)

/**
 * htmx_nav_new:
 *
 * Creates a new #HtmxNav element.
 *
 * Returns: (transfer full): a new #HtmxNav
 */
HtmxNav *
htmx_nav_new(void);

/*
 * HtmxHeader - Header element (not the same as <head>)
 */

#define HTMX_TYPE_HEADER (htmx_header_get_type())
G_DECLARE_FINAL_TYPE(HtmxHeader, htmx_header, HTMX, HEADER, HtmxNode)

/**
 * htmx_header_new:
 *
 * Creates a new #HtmxHeader element.
 *
 * Returns: (transfer full): a new #HtmxHeader
 */
HtmxHeader *
htmx_header_new(void);

/*
 * HtmxFooter - Footer element
 */

#define HTMX_TYPE_FOOTER (htmx_footer_get_type())
G_DECLARE_FINAL_TYPE(HtmxFooter, htmx_footer, HTMX, FOOTER, HtmxNode)

/**
 * htmx_footer_new:
 *
 * Creates a new #HtmxFooter element.
 *
 * Returns: (transfer full): a new #HtmxFooter
 */
HtmxFooter *
htmx_footer_new(void);

/*
 * HtmxAside - Aside element
 */

#define HTMX_TYPE_ASIDE (htmx_aside_get_type())
G_DECLARE_FINAL_TYPE(HtmxAside, htmx_aside, HTMX, ASIDE, HtmxNode)

/**
 * htmx_aside_new:
 *
 * Creates a new #HtmxAside element.
 *
 * Returns: (transfer full): a new #HtmxAside
 */
HtmxAside *
htmx_aside_new(void);

/*
 * HtmxMain - Main element
 */

#define HTMX_TYPE_MAIN (htmx_main_get_type())
G_DECLARE_FINAL_TYPE(HtmxMain, htmx_main, HTMX, MAIN, HtmxNode)

/**
 * htmx_main_new:
 *
 * Creates a new #HtmxMain element.
 *
 * Returns: (transfer full): a new #HtmxMain
 */
HtmxMain *
htmx_main_new(void);

/*
 * HtmxFigure - Figure element
 */

#define HTMX_TYPE_FIGURE (htmx_figure_get_type())
G_DECLARE_FINAL_TYPE(HtmxFigure, htmx_figure, HTMX, FIGURE, HtmxNode)

/**
 * htmx_figure_new:
 *
 * Creates a new #HtmxFigure element.
 *
 * Returns: (transfer full): a new #HtmxFigure
 */
HtmxFigure *
htmx_figure_new(void);

/*
 * HtmxFigcaption - Figure caption element
 */

#define HTMX_TYPE_FIGCAPTION (htmx_figcaption_get_type())
G_DECLARE_FINAL_TYPE(HtmxFigcaption, htmx_figcaption, HTMX, FIGCAPTION, HtmxNode)

/**
 * htmx_figcaption_new:
 *
 * Creates a new #HtmxFigcaption element.
 *
 * Returns: (transfer full): a new #HtmxFigcaption
 */
HtmxFigcaption *
htmx_figcaption_new(void);

/**
 * htmx_figcaption_new_with_text:
 * @text: the caption text
 *
 * Creates a new #HtmxFigcaption with text content.
 *
 * Returns: (transfer full): a new #HtmxFigcaption
 */
HtmxFigcaption *
htmx_figcaption_new_with_text(const gchar *text);

/*
 * HtmxAddress - Address element
 */

#define HTMX_TYPE_ADDRESS (htmx_address_get_type())
G_DECLARE_FINAL_TYPE(HtmxAddress, htmx_address, HTMX, ADDRESS, HtmxNode)

/**
 * htmx_address_new:
 *
 * Creates a new #HtmxAddress element.
 *
 * Returns: (transfer full): a new #HtmxAddress
 */
HtmxAddress *
htmx_address_new(void);

/*
 * HtmxHgroup - Heading group element
 */

#define HTMX_TYPE_HGROUP (htmx_hgroup_get_type())
G_DECLARE_FINAL_TYPE(HtmxHgroup, htmx_hgroup, HTMX, HGROUP, HtmxNode)

/**
 * htmx_hgroup_new:
 *
 * Creates a new #HtmxHgroup element.
 *
 * Returns: (transfer full): a new #HtmxHgroup
 */
HtmxHgroup *
htmx_hgroup_new(void);

/*
 * HtmxSearch - Search section element
 */

#define HTMX_TYPE_SEARCH (htmx_search_get_type())
G_DECLARE_FINAL_TYPE(HtmxSearch, htmx_search, HTMX, SEARCH, HtmxNode)

/**
 * htmx_search_new:
 *
 * Creates a new #HtmxSearch element.
 *
 * Returns: (transfer full): a new #HtmxSearch
 */
HtmxSearch *
htmx_search_new(void);

G_END_DECLS

#endif /* HTMX_SEMANTIC_H */
