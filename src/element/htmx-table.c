/*
 * htmx-table.c - HTML table elements implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-table.h"

/*
 * HtmxTable - Table element
 */

struct _HtmxTable {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxTable, htmx_table, HTMX_TYPE_NODE)

static void
htmx_table_class_init(HtmxTableClass *klass)
{
}

static void
htmx_table_init(HtmxTable *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "table");
}

HtmxTable *
htmx_table_new(void)
{
	return g_object_new(HTMX_TYPE_TABLE, NULL);
}

/*
 * HtmxThead - Table head element
 */

struct _HtmxThead {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxThead, htmx_thead, HTMX_TYPE_NODE)

static void
htmx_thead_class_init(HtmxTheadClass *klass)
{
}

static void
htmx_thead_init(HtmxThead *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "thead");
}

HtmxThead *
htmx_thead_new(void)
{
	return g_object_new(HTMX_TYPE_THEAD, NULL);
}

/*
 * HtmxTbody - Table body element
 */

struct _HtmxTbody {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxTbody, htmx_tbody, HTMX_TYPE_NODE)

static void
htmx_tbody_class_init(HtmxTbodyClass *klass)
{
}

static void
htmx_tbody_init(HtmxTbody *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "tbody");
}

HtmxTbody *
htmx_tbody_new(void)
{
	return g_object_new(HTMX_TYPE_TBODY, NULL);
}

/*
 * HtmxTfoot - Table foot element
 */

struct _HtmxTfoot {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxTfoot, htmx_tfoot, HTMX_TYPE_NODE)

static void
htmx_tfoot_class_init(HtmxTfootClass *klass)
{
}

static void
htmx_tfoot_init(HtmxTfoot *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "tfoot");
}

HtmxTfoot *
htmx_tfoot_new(void)
{
	return g_object_new(HTMX_TYPE_TFOOT, NULL);
}

/*
 * HtmxTr - Table row element
 */

struct _HtmxTr {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxTr, htmx_tr, HTMX_TYPE_NODE)

static void
htmx_tr_class_init(HtmxTrClass *klass)
{
}

static void
htmx_tr_init(HtmxTr *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "tr");
}

HtmxTr *
htmx_tr_new(void)
{
	return g_object_new(HTMX_TYPE_TR, NULL);
}

/*
 * HtmxTh - Table header cell element
 */

struct _HtmxTh {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxTh, htmx_th, HTMX_TYPE_NODE)

static void
htmx_th_class_init(HtmxThClass *klass)
{
}

static void
htmx_th_init(HtmxTh *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "th");
}

HtmxTh *
htmx_th_new(void)
{
	return g_object_new(HTMX_TYPE_TH, NULL);
}

HtmxTh *
htmx_th_new_with_text(const gchar *text)
{
	HtmxTh *self;

	self = htmx_th_new();
	if (text != NULL) {
		htmx_node_set_text_content(HTMX_NODE(self), text);
	}

	return self;
}

void
htmx_th_set_colspan(
	HtmxTh *self,
	guint   colspan
){
	g_autofree gchar *colspan_str = NULL;

	g_return_if_fail(HTMX_IS_TH(self));

	colspan_str = g_strdup_printf("%u", colspan);
	htmx_element_set_attribute(HTMX_ELEMENT(self), "colspan", colspan_str);
}

void
htmx_th_set_rowspan(
	HtmxTh *self,
	guint   rowspan
){
	g_autofree gchar *rowspan_str = NULL;

	g_return_if_fail(HTMX_IS_TH(self));

	rowspan_str = g_strdup_printf("%u", rowspan);
	htmx_element_set_attribute(HTMX_ELEMENT(self), "rowspan", rowspan_str);
}

void
htmx_th_set_scope(
	HtmxTh      *self,
	const gchar *scope
){
	g_return_if_fail(HTMX_IS_TH(self));

	htmx_element_set_attribute(HTMX_ELEMENT(self), "scope", scope);
}

/*
 * HtmxTd - Table data cell element
 */

struct _HtmxTd {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxTd, htmx_td, HTMX_TYPE_NODE)

static void
htmx_td_class_init(HtmxTdClass *klass)
{
}

static void
htmx_td_init(HtmxTd *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "td");
}

HtmxTd *
htmx_td_new(void)
{
	return g_object_new(HTMX_TYPE_TD, NULL);
}

HtmxTd *
htmx_td_new_with_text(const gchar *text)
{
	HtmxTd *self;

	self = htmx_td_new();
	if (text != NULL) {
		htmx_node_set_text_content(HTMX_NODE(self), text);
	}

	return self;
}

void
htmx_td_set_colspan(
	HtmxTd *self,
	guint   colspan
){
	g_autofree gchar *colspan_str = NULL;

	g_return_if_fail(HTMX_IS_TD(self));

	colspan_str = g_strdup_printf("%u", colspan);
	htmx_element_set_attribute(HTMX_ELEMENT(self), "colspan", colspan_str);
}

void
htmx_td_set_rowspan(
	HtmxTd *self,
	guint   rowspan
){
	g_autofree gchar *rowspan_str = NULL;

	g_return_if_fail(HTMX_IS_TD(self));

	rowspan_str = g_strdup_printf("%u", rowspan);
	htmx_element_set_attribute(HTMX_ELEMENT(self), "rowspan", rowspan_str);
}

/*
 * HtmxCaption - Table caption element
 */

struct _HtmxCaption {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxCaption, htmx_caption, HTMX_TYPE_NODE)

static void
htmx_caption_class_init(HtmxCaptionClass *klass)
{
}

static void
htmx_caption_init(HtmxCaption *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "caption");
}

HtmxCaption *
htmx_caption_new(void)
{
	return g_object_new(HTMX_TYPE_CAPTION, NULL);
}

HtmxCaption *
htmx_caption_new_with_text(const gchar *text)
{
	HtmxCaption *self;

	self = htmx_caption_new();
	if (text != NULL) {
		htmx_node_set_text_content(HTMX_NODE(self), text);
	}

	return self;
}

/*
 * HtmxColgroup - Column group element
 */

struct _HtmxColgroup {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxColgroup, htmx_colgroup, HTMX_TYPE_NODE)

static void
htmx_colgroup_class_init(HtmxColgroupClass *klass)
{
}

static void
htmx_colgroup_init(HtmxColgroup *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "colgroup");
}

HtmxColgroup *
htmx_colgroup_new(void)
{
	return g_object_new(HTMX_TYPE_COLGROUP, NULL);
}

void
htmx_colgroup_set_span(
	HtmxColgroup *self,
	guint         span
){
	g_autofree gchar *span_str = NULL;

	g_return_if_fail(HTMX_IS_COLGROUP(self));

	span_str = g_strdup_printf("%u", span);
	htmx_element_set_attribute(HTMX_ELEMENT(self), "span", span_str);
}

/*
 * HtmxCol - Column element (void element)
 */

struct _HtmxCol {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxCol, htmx_col, HTMX_TYPE_NODE)

static void
htmx_col_class_init(HtmxColClass *klass)
{
}

static void
htmx_col_init(HtmxCol *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "col");
	htmx_node_set_void_element(HTMX_NODE(self), TRUE);
}

HtmxCol *
htmx_col_new(void)
{
	return g_object_new(HTMX_TYPE_COL, NULL);
}

void
htmx_col_set_span(
	HtmxCol *self,
	guint    span
){
	g_autofree gchar *span_str = NULL;

	g_return_if_fail(HTMX_IS_COL(self));

	span_str = g_strdup_printf("%u", span);
	htmx_element_set_attribute(HTMX_ELEMENT(self), "span", span_str);
}
