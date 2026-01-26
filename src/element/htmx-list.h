/*
 * htmx-list.h - HTML list elements (ul, ol, li)
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * This file defines the HtmxUl, HtmxOl, and HtmxLi classes for HTML lists.
 */

#ifndef HTMX_LIST_H
#define HTMX_LIST_H

#include <glib-object.h>
#include "htmx-node.h"

G_BEGIN_DECLS

/*
 * HtmxUl - Unordered list
 */

#define HTMX_TYPE_UL (htmx_ul_get_type())
G_DECLARE_FINAL_TYPE(HtmxUl, htmx_ul, HTMX, UL, HtmxNode)

/**
 * htmx_ul_new:
 *
 * Creates a new #HtmxUl unordered list element.
 *
 * Returns: (transfer full): a new #HtmxUl
 */
HtmxUl *
htmx_ul_new(void);

/*
 * HtmxOl - Ordered list
 */

#define HTMX_TYPE_OL (htmx_ol_get_type())
G_DECLARE_FINAL_TYPE(HtmxOl, htmx_ol, HTMX, OL, HtmxNode)

/**
 * htmx_ol_new:
 *
 * Creates a new #HtmxOl ordered list element.
 *
 * Returns: (transfer full): a new #HtmxOl
 */
HtmxOl *
htmx_ol_new(void);

/**
 * htmx_ol_set_start:
 * @self: an #HtmxOl
 * @start: the starting number
 *
 * Sets the start attribute for the ordered list.
 */
void
htmx_ol_set_start(
	HtmxOl *self,
	gint    start
);

/**
 * htmx_ol_set_reversed:
 * @self: an #HtmxOl
 * @reversed: whether the list is reversed
 *
 * Sets the reversed attribute.
 */
void
htmx_ol_set_reversed(
	HtmxOl   *self,
	gboolean  reversed
);

/*
 * HtmxLi - List item
 */

#define HTMX_TYPE_LI (htmx_li_get_type())
G_DECLARE_FINAL_TYPE(HtmxLi, htmx_li, HTMX, LI, HtmxNode)

/**
 * htmx_li_new:
 *
 * Creates a new #HtmxLi list item element.
 *
 * Returns: (transfer full): a new #HtmxLi
 */
HtmxLi *
htmx_li_new(void);

/**
 * htmx_li_new_with_text:
 * @text: the list item text
 *
 * Creates a new #HtmxLi with text content.
 *
 * Returns: (transfer full): a new #HtmxLi
 */
HtmxLi *
htmx_li_new_with_text(const gchar *text);

/**
 * htmx_li_set_value:
 * @self: an #HtmxLi
 * @value: the value attribute (for ordered lists)
 *
 * Sets the value attribute for list items in ordered lists.
 */
void
htmx_li_set_value(
	HtmxLi *self,
	gint    value
);

G_END_DECLS

#endif /* HTMX_LIST_H */
