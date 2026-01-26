/*
 * htmx-meta.h - HTML metadata and scripting elements
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * This file defines metadata and scripting elements: script, noscript,
 * link, meta, style, template, and slot.
 *
 * Note: HtmxHtmlTemplate is used for the <template> element to avoid
 * conflicting with the existing HtmxTemplate class (which handles
 * server-side template rendering).
 */

#ifndef HTMX_META_H
#define HTMX_META_H

#include <glib-object.h>
#include "htmx-node.h"

G_BEGIN_DECLS

/*
 * HtmxScript - Script element
 */

#define HTMX_TYPE_SCRIPT (htmx_script_get_type())
G_DECLARE_FINAL_TYPE(HtmxScript, htmx_script, HTMX, SCRIPT, HtmxNode)

/**
 * htmx_script_new:
 *
 * Creates a new #HtmxScript element.
 *
 * Returns: (transfer full): a new #HtmxScript
 */
HtmxScript *
htmx_script_new(void);

/**
 * htmx_script_new_with_src:
 * @src: the script source URL
 *
 * Creates a new #HtmxScript with a source.
 *
 * Returns: (transfer full): a new #HtmxScript
 */
HtmxScript *
htmx_script_new_with_src(const gchar *src);

/**
 * htmx_script_new_from_file:
 * @filepath: path to the JavaScript file
 * @error: (nullable): return location for a #GError, or %NULL
 *
 * Creates a new #HtmxScript with inline content loaded from a file.
 *
 * Returns: (transfer full) (nullable): a new #HtmxScript, or %NULL on error
 */
HtmxScript *
htmx_script_new_from_file(
	const gchar  *filepath,
	GError      **error
);

void htmx_script_set_src(HtmxScript *self, const gchar *src);
void htmx_script_set_type(HtmxScript *self, const gchar *type);
void htmx_script_set_async(HtmxScript *self, gboolean async);
void htmx_script_set_defer(HtmxScript *self, gboolean defer);

/*
 * HtmxNoscript - Noscript element
 */

#define HTMX_TYPE_NOSCRIPT (htmx_noscript_get_type())
G_DECLARE_FINAL_TYPE(HtmxNoscript, htmx_noscript, HTMX, NOSCRIPT, HtmxNode)

/**
 * htmx_noscript_new:
 *
 * Creates a new #HtmxNoscript element.
 *
 * Returns: (transfer full): a new #HtmxNoscript
 */
HtmxNoscript *
htmx_noscript_new(void);

/*
 * HtmxLink - Link element (void element)
 */

#define HTMX_TYPE_LINK (htmx_link_get_type())
G_DECLARE_FINAL_TYPE(HtmxLink, htmx_link, HTMX, LINK, HtmxNode)

/**
 * htmx_link_new:
 *
 * Creates a new #HtmxLink element.
 *
 * Returns: (transfer full): a new #HtmxLink
 */
HtmxLink *
htmx_link_new(void);

/**
 * htmx_link_new_stylesheet:
 * @href: the stylesheet URL
 *
 * Creates a new #HtmxLink for a stylesheet.
 *
 * Returns: (transfer full): a new #HtmxLink
 */
HtmxLink *
htmx_link_new_stylesheet(const gchar *href);

void htmx_link_set_rel(HtmxLink *self, const gchar *rel);
void htmx_link_set_href(HtmxLink *self, const gchar *href);
void htmx_link_set_type(HtmxLink *self, const gchar *type);
void htmx_link_set_media(HtmxLink *self, const gchar *media);

/*
 * HtmxMeta - Meta element (void element)
 */

#define HTMX_TYPE_META (htmx_meta_get_type())
G_DECLARE_FINAL_TYPE(HtmxMeta, htmx_meta, HTMX, META, HtmxNode)

/**
 * htmx_meta_new:
 *
 * Creates a new #HtmxMeta element.
 *
 * Returns: (transfer full): a new #HtmxMeta
 */
HtmxMeta *
htmx_meta_new(void);

/**
 * htmx_meta_new_name_content:
 * @name: the meta name
 * @content: the meta content
 *
 * Creates a new #HtmxMeta with name and content.
 *
 * Returns: (transfer full): a new #HtmxMeta
 */
HtmxMeta *
htmx_meta_new_name_content(
	const gchar *name,
	const gchar *content
);

/**
 * htmx_meta_new_charset:
 * @charset: the character encoding
 *
 * Creates a new #HtmxMeta with charset.
 *
 * Returns: (transfer full): a new #HtmxMeta
 */
HtmxMeta *
htmx_meta_new_charset(const gchar *charset);

void htmx_meta_set_name(HtmxMeta *self, const gchar *name);
void htmx_meta_set_content(HtmxMeta *self, const gchar *content);
void htmx_meta_set_charset(HtmxMeta *self, const gchar *charset);
void htmx_meta_set_http_equiv(HtmxMeta *self, const gchar *http_equiv);

/*
 * HtmxStyle - Style element
 */

#define HTMX_TYPE_STYLE (htmx_style_get_type())
G_DECLARE_FINAL_TYPE(HtmxStyle, htmx_style, HTMX, STYLE, HtmxNode)

/**
 * htmx_style_new:
 *
 * Creates a new #HtmxStyle element.
 *
 * Returns: (transfer full): a new #HtmxStyle
 */
HtmxStyle *
htmx_style_new(void);

/**
 * htmx_style_new_with_css:
 * @css: the CSS content
 *
 * Creates a new #HtmxStyle with CSS content.
 *
 * Returns: (transfer full): a new #HtmxStyle
 */
HtmxStyle *
htmx_style_new_with_css(const gchar *css);

/**
 * htmx_style_new_from_file:
 * @filepath: path to the CSS file
 * @error: (nullable): return location for a #GError, or %NULL
 *
 * Creates a new #HtmxStyle with inline content loaded from a file.
 *
 * Returns: (transfer full) (nullable): a new #HtmxStyle, or %NULL on error
 */
HtmxStyle *
htmx_style_new_from_file(
	const gchar  *filepath,
	GError      **error
);

void htmx_style_set_type(HtmxStyle *self, const gchar *type);
void htmx_style_set_media(HtmxStyle *self, const gchar *media);

/*
 * HtmxHtmlTemplate - HTML template element
 *
 * Note: Named HtmxHtmlTemplate to avoid conflict with HtmxTemplate
 * (server-side template rendering class).
 */

#define HTMX_TYPE_HTML_TEMPLATE (htmx_html_template_get_type())
G_DECLARE_FINAL_TYPE(HtmxHtmlTemplate, htmx_html_template, HTMX, HTML_TEMPLATE, HtmxNode)

/**
 * htmx_html_template_new:
 *
 * Creates a new #HtmxHtmlTemplate element.
 *
 * Returns: (transfer full): a new #HtmxHtmlTemplate
 */
HtmxHtmlTemplate *
htmx_html_template_new(void);

/*
 * HtmxSlot - Slot element
 */

#define HTMX_TYPE_SLOT (htmx_slot_get_type())
G_DECLARE_FINAL_TYPE(HtmxSlot, htmx_slot, HTMX, SLOT, HtmxNode)

/**
 * htmx_slot_new:
 *
 * Creates a new #HtmxSlot element.
 *
 * Returns: (transfer full): a new #HtmxSlot
 */
HtmxSlot *
htmx_slot_new(void);

/**
 * htmx_slot_new_named:
 * @name: the slot name
 *
 * Creates a new named #HtmxSlot.
 *
 * Returns: (transfer full): a new #HtmxSlot
 */
HtmxSlot *
htmx_slot_new_named(const gchar *name);

void htmx_slot_set_name(HtmxSlot *self, const gchar *name);

G_END_DECLS

#endif /* HTMX_META_H */
