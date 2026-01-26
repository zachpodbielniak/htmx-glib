/*
 * htmx-document.h - HTML document structure elements
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * This file defines document structure elements: html, head, body, title,
 * and base.
 */

#ifndef HTMX_DOCUMENT_H
#define HTMX_DOCUMENT_H

#include <glib-object.h>
#include "htmx-node.h"

G_BEGIN_DECLS

/*
 * HtmxHtml - Root HTML element
 */

#define HTMX_TYPE_HTML (htmx_html_get_type())
G_DECLARE_FINAL_TYPE(HtmxHtml, htmx_html, HTMX, HTML, HtmxNode)

/**
 * htmx_html_new:
 *
 * Creates a new #HtmxHtml root element.
 *
 * Returns: (transfer full): a new #HtmxHtml
 */
HtmxHtml *
htmx_html_new(void);

/**
 * htmx_html_set_lang:
 * @self: an #HtmxHtml
 * @lang: the language code (e.g., "en", "en-US")
 *
 * Sets the lang attribute for the document.
 */
void
htmx_html_set_lang(
	HtmxHtml    *self,
	const gchar *lang
);

/*
 * HtmxHead - Document head element
 */

#define HTMX_TYPE_HEAD (htmx_head_get_type())
G_DECLARE_FINAL_TYPE(HtmxHead, htmx_head, HTMX, HEAD, HtmxNode)

/**
 * htmx_head_new:
 *
 * Creates a new #HtmxHead element.
 *
 * Returns: (transfer full): a new #HtmxHead
 */
HtmxHead *
htmx_head_new(void);

/*
 * HtmxBody - Document body element
 */

#define HTMX_TYPE_BODY (htmx_body_get_type())
G_DECLARE_FINAL_TYPE(HtmxBody, htmx_body, HTMX, BODY, HtmxNode)

/**
 * htmx_body_new:
 *
 * Creates a new #HtmxBody element.
 *
 * Returns: (transfer full): a new #HtmxBody
 */
HtmxBody *
htmx_body_new(void);

/*
 * HtmxTitle - Document title element
 */

#define HTMX_TYPE_TITLE (htmx_title_get_type())
G_DECLARE_FINAL_TYPE(HtmxTitle, htmx_title, HTMX, TITLE, HtmxNode)

/**
 * htmx_title_new:
 *
 * Creates a new #HtmxTitle element.
 *
 * Returns: (transfer full): a new #HtmxTitle
 */
HtmxTitle *
htmx_title_new(void);

/**
 * htmx_title_new_with_text:
 * @text: the title text
 *
 * Creates a new #HtmxTitle with text content.
 *
 * Returns: (transfer full): a new #HtmxTitle
 */
HtmxTitle *
htmx_title_new_with_text(const gchar *text);

/*
 * HtmxBase - Document base URL element (void element)
 */

#define HTMX_TYPE_BASE (htmx_base_get_type())
G_DECLARE_FINAL_TYPE(HtmxBase, htmx_base, HTMX, BASE, HtmxNode)

/**
 * htmx_base_new:
 *
 * Creates a new #HtmxBase element.
 *
 * Returns: (transfer full): a new #HtmxBase
 */
HtmxBase *
htmx_base_new(void);

/**
 * htmx_base_new_with_href:
 * @href: the base URL
 *
 * Creates a new #HtmxBase with href.
 *
 * Returns: (transfer full): a new #HtmxBase
 */
HtmxBase *
htmx_base_new_with_href(const gchar *href);

/**
 * htmx_base_set_href:
 * @self: an #HtmxBase
 * @href: the base URL
 *
 * Sets the href attribute.
 */
void
htmx_base_set_href(
	HtmxBase    *self,
	const gchar *href
);

/**
 * htmx_base_set_target:
 * @self: an #HtmxBase
 * @target: the default target (e.g., "_blank", "_self")
 *
 * Sets the target attribute.
 */
void
htmx_base_set_target(
	HtmxBase    *self,
	const gchar *target
);

G_END_DECLS

#endif /* HTMX_DOCUMENT_H */
