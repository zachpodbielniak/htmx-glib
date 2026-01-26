/*
 * htmx-table.h - HTML table elements
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * This file defines table-related elements: table, thead, tbody, tfoot,
 * tr, th, td, caption, colgroup, and col.
 */

#ifndef HTMX_TABLE_H
#define HTMX_TABLE_H

#include <glib-object.h>
#include "htmx-node.h"

G_BEGIN_DECLS

/*
 * HtmxTable - Table element
 */

#define HTMX_TYPE_TABLE (htmx_table_get_type())
G_DECLARE_FINAL_TYPE(HtmxTable, htmx_table, HTMX, TABLE, HtmxNode)

/**
 * htmx_table_new:
 *
 * Creates a new #HtmxTable element.
 *
 * Returns: (transfer full): a new #HtmxTable
 */
HtmxTable *
htmx_table_new(void);

/*
 * HtmxThead - Table head element
 */

#define HTMX_TYPE_THEAD (htmx_thead_get_type())
G_DECLARE_FINAL_TYPE(HtmxThead, htmx_thead, HTMX, THEAD, HtmxNode)

/**
 * htmx_thead_new:
 *
 * Creates a new #HtmxThead element.
 *
 * Returns: (transfer full): a new #HtmxThead
 */
HtmxThead *
htmx_thead_new(void);

/*
 * HtmxTbody - Table body element
 */

#define HTMX_TYPE_TBODY (htmx_tbody_get_type())
G_DECLARE_FINAL_TYPE(HtmxTbody, htmx_tbody, HTMX, TBODY, HtmxNode)

/**
 * htmx_tbody_new:
 *
 * Creates a new #HtmxTbody element.
 *
 * Returns: (transfer full): a new #HtmxTbody
 */
HtmxTbody *
htmx_tbody_new(void);

/*
 * HtmxTfoot - Table foot element
 */

#define HTMX_TYPE_TFOOT (htmx_tfoot_get_type())
G_DECLARE_FINAL_TYPE(HtmxTfoot, htmx_tfoot, HTMX, TFOOT, HtmxNode)

/**
 * htmx_tfoot_new:
 *
 * Creates a new #HtmxTfoot element.
 *
 * Returns: (transfer full): a new #HtmxTfoot
 */
HtmxTfoot *
htmx_tfoot_new(void);

/*
 * HtmxTr - Table row element
 */

#define HTMX_TYPE_TR (htmx_tr_get_type())
G_DECLARE_FINAL_TYPE(HtmxTr, htmx_tr, HTMX, TR, HtmxNode)

/**
 * htmx_tr_new:
 *
 * Creates a new #HtmxTr table row element.
 *
 * Returns: (transfer full): a new #HtmxTr
 */
HtmxTr *
htmx_tr_new(void);

/*
 * HtmxTh - Table header cell element
 */

#define HTMX_TYPE_TH (htmx_th_get_type())
G_DECLARE_FINAL_TYPE(HtmxTh, htmx_th, HTMX, TH, HtmxNode)

/**
 * htmx_th_new:
 *
 * Creates a new #HtmxTh table header cell element.
 *
 * Returns: (transfer full): a new #HtmxTh
 */
HtmxTh *
htmx_th_new(void);

/**
 * htmx_th_new_with_text:
 * @text: the header text
 *
 * Creates a new #HtmxTh with text content.
 *
 * Returns: (transfer full): a new #HtmxTh
 */
HtmxTh *
htmx_th_new_with_text(const gchar *text);

/**
 * htmx_th_set_colspan:
 * @self: an #HtmxTh
 * @colspan: number of columns to span
 *
 * Sets the colspan attribute.
 */
void
htmx_th_set_colspan(
	HtmxTh *self,
	guint   colspan
);

/**
 * htmx_th_set_rowspan:
 * @self: an #HtmxTh
 * @rowspan: number of rows to span
 *
 * Sets the rowspan attribute.
 */
void
htmx_th_set_rowspan(
	HtmxTh *self,
	guint   rowspan
);

/**
 * htmx_th_set_scope:
 * @self: an #HtmxTh
 * @scope: the scope (row, col, rowgroup, colgroup)
 *
 * Sets the scope attribute.
 */
void
htmx_th_set_scope(
	HtmxTh      *self,
	const gchar *scope
);

/*
 * HtmxTd - Table data cell element
 */

#define HTMX_TYPE_TD (htmx_td_get_type())
G_DECLARE_FINAL_TYPE(HtmxTd, htmx_td, HTMX, TD, HtmxNode)

/**
 * htmx_td_new:
 *
 * Creates a new #HtmxTd table data cell element.
 *
 * Returns: (transfer full): a new #HtmxTd
 */
HtmxTd *
htmx_td_new(void);

/**
 * htmx_td_new_with_text:
 * @text: the cell text
 *
 * Creates a new #HtmxTd with text content.
 *
 * Returns: (transfer full): a new #HtmxTd
 */
HtmxTd *
htmx_td_new_with_text(const gchar *text);

/**
 * htmx_td_set_colspan:
 * @self: an #HtmxTd
 * @colspan: number of columns to span
 *
 * Sets the colspan attribute.
 */
void
htmx_td_set_colspan(
	HtmxTd *self,
	guint   colspan
);

/**
 * htmx_td_set_rowspan:
 * @self: an #HtmxTd
 * @rowspan: number of rows to span
 *
 * Sets the rowspan attribute.
 */
void
htmx_td_set_rowspan(
	HtmxTd *self,
	guint   rowspan
);

/*
 * HtmxCaption - Table caption element
 */

#define HTMX_TYPE_CAPTION (htmx_caption_get_type())
G_DECLARE_FINAL_TYPE(HtmxCaption, htmx_caption, HTMX, CAPTION, HtmxNode)

/**
 * htmx_caption_new:
 *
 * Creates a new #HtmxCaption element.
 *
 * Returns: (transfer full): a new #HtmxCaption
 */
HtmxCaption *
htmx_caption_new(void);

/**
 * htmx_caption_new_with_text:
 * @text: the caption text
 *
 * Creates a new #HtmxCaption with text content.
 *
 * Returns: (transfer full): a new #HtmxCaption
 */
HtmxCaption *
htmx_caption_new_with_text(const gchar *text);

/*
 * HtmxColgroup - Column group element
 */

#define HTMX_TYPE_COLGROUP (htmx_colgroup_get_type())
G_DECLARE_FINAL_TYPE(HtmxColgroup, htmx_colgroup, HTMX, COLGROUP, HtmxNode)

/**
 * htmx_colgroup_new:
 *
 * Creates a new #HtmxColgroup element.
 *
 * Returns: (transfer full): a new #HtmxColgroup
 */
HtmxColgroup *
htmx_colgroup_new(void);

/**
 * htmx_colgroup_set_span:
 * @self: an #HtmxColgroup
 * @span: number of columns to span
 *
 * Sets the span attribute.
 */
void
htmx_colgroup_set_span(
	HtmxColgroup *self,
	guint         span
);

/*
 * HtmxCol - Column element (void element)
 */

#define HTMX_TYPE_COL (htmx_col_get_type())
G_DECLARE_FINAL_TYPE(HtmxCol, htmx_col, HTMX, COL, HtmxNode)

/**
 * htmx_col_new:
 *
 * Creates a new #HtmxCol element.
 *
 * Returns: (transfer full): a new #HtmxCol
 */
HtmxCol *
htmx_col_new(void);

/**
 * htmx_col_set_span:
 * @self: an #HtmxCol
 * @span: number of columns to span
 *
 * Sets the span attribute.
 */
void
htmx_col_set_span(
	HtmxCol *self,
	guint    span
);

G_END_DECLS

#endif /* HTMX_TABLE_H */
