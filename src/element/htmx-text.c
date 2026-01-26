/*
 * htmx-text.c - HTML text formatting elements implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-text.h"

/*
 * HtmxStrong - Strong importance element
 */

struct _HtmxStrong {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxStrong, htmx_strong, HTMX_TYPE_NODE)

static void
htmx_strong_class_init(HtmxStrongClass *klass)
{
}

static void
htmx_strong_init(HtmxStrong *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "strong");
}

HtmxStrong *
htmx_strong_new(void)
{
	return g_object_new(HTMX_TYPE_STRONG, NULL);
}

HtmxStrong *
htmx_strong_new_with_text(const gchar *text)
{
	HtmxStrong *self;

	self = htmx_strong_new();
	if (text != NULL) {
		htmx_node_set_text_content(HTMX_NODE(self), text);
	}

	return self;
}

/*
 * HtmxEm - Emphasis element
 */

struct _HtmxEm {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxEm, htmx_em, HTMX_TYPE_NODE)

static void
htmx_em_class_init(HtmxEmClass *klass)
{
}

static void
htmx_em_init(HtmxEm *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "em");
}

HtmxEm *
htmx_em_new(void)
{
	return g_object_new(HTMX_TYPE_EM, NULL);
}

HtmxEm *
htmx_em_new_with_text(const gchar *text)
{
	HtmxEm *self;

	self = htmx_em_new();
	if (text != NULL) {
		htmx_node_set_text_content(HTMX_NODE(self), text);
	}

	return self;
}

/*
 * HtmxB - Bold (stylistic) element
 */

struct _HtmxB {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxB, htmx_b, HTMX_TYPE_NODE)

static void
htmx_b_class_init(HtmxBClass *klass)
{
}

static void
htmx_b_init(HtmxB *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "b");
}

HtmxB *
htmx_b_new(void)
{
	return g_object_new(HTMX_TYPE_B, NULL);
}

HtmxB *
htmx_b_new_with_text(const gchar *text)
{
	HtmxB *self;

	self = htmx_b_new();
	if (text != NULL) {
		htmx_node_set_text_content(HTMX_NODE(self), text);
	}

	return self;
}

/*
 * HtmxI - Italic (stylistic) element
 */

struct _HtmxI {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxI, htmx_i, HTMX_TYPE_NODE)

static void
htmx_i_class_init(HtmxIClass *klass)
{
}

static void
htmx_i_init(HtmxI *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "i");
}

HtmxI *
htmx_i_new(void)
{
	return g_object_new(HTMX_TYPE_I, NULL);
}

HtmxI *
htmx_i_new_with_text(const gchar *text)
{
	HtmxI *self;

	self = htmx_i_new();
	if (text != NULL) {
		htmx_node_set_text_content(HTMX_NODE(self), text);
	}

	return self;
}

/*
 * HtmxU - Underline element
 */

struct _HtmxU {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxU, htmx_u, HTMX_TYPE_NODE)

static void
htmx_u_class_init(HtmxUClass *klass)
{
}

static void
htmx_u_init(HtmxU *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "u");
}

HtmxU *
htmx_u_new(void)
{
	return g_object_new(HTMX_TYPE_U, NULL);
}

HtmxU *
htmx_u_new_with_text(const gchar *text)
{
	HtmxU *self;

	self = htmx_u_new();
	if (text != NULL) {
		htmx_node_set_text_content(HTMX_NODE(self), text);
	}

	return self;
}

/*
 * HtmxS - Strikethrough element
 */

struct _HtmxS {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxS, htmx_s, HTMX_TYPE_NODE)

static void
htmx_s_class_init(HtmxSClass *klass)
{
}

static void
htmx_s_init(HtmxS *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "s");
}

HtmxS *
htmx_s_new(void)
{
	return g_object_new(HTMX_TYPE_S, NULL);
}

HtmxS *
htmx_s_new_with_text(const gchar *text)
{
	HtmxS *self;

	self = htmx_s_new();
	if (text != NULL) {
		htmx_node_set_text_content(HTMX_NODE(self), text);
	}

	return self;
}

/*
 * HtmxSmall - Small text element
 */

struct _HtmxSmall {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxSmall, htmx_small, HTMX_TYPE_NODE)

static void
htmx_small_class_init(HtmxSmallClass *klass)
{
}

static void
htmx_small_init(HtmxSmall *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "small");
}

HtmxSmall *
htmx_small_new(void)
{
	return g_object_new(HTMX_TYPE_SMALL, NULL);
}

HtmxSmall *
htmx_small_new_with_text(const gchar *text)
{
	HtmxSmall *self;

	self = htmx_small_new();
	if (text != NULL) {
		htmx_node_set_text_content(HTMX_NODE(self), text);
	}

	return self;
}

/*
 * HtmxMark - Marked/highlighted element
 */

struct _HtmxMark {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxMark, htmx_mark, HTMX_TYPE_NODE)

static void
htmx_mark_class_init(HtmxMarkClass *klass)
{
}

static void
htmx_mark_init(HtmxMark *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "mark");
}

HtmxMark *
htmx_mark_new(void)
{
	return g_object_new(HTMX_TYPE_MARK, NULL);
}

HtmxMark *
htmx_mark_new_with_text(const gchar *text)
{
	HtmxMark *self;

	self = htmx_mark_new();
	if (text != NULL) {
		htmx_node_set_text_content(HTMX_NODE(self), text);
	}

	return self;
}

/*
 * HtmxSub - Subscript element
 */

struct _HtmxSub {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxSub, htmx_sub, HTMX_TYPE_NODE)

static void
htmx_sub_class_init(HtmxSubClass *klass)
{
}

static void
htmx_sub_init(HtmxSub *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "sub");
}

HtmxSub *
htmx_sub_new(void)
{
	return g_object_new(HTMX_TYPE_SUB, NULL);
}

HtmxSub *
htmx_sub_new_with_text(const gchar *text)
{
	HtmxSub *self;

	self = htmx_sub_new();
	if (text != NULL) {
		htmx_node_set_text_content(HTMX_NODE(self), text);
	}

	return self;
}

/*
 * HtmxSup - Superscript element
 */

struct _HtmxSup {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxSup, htmx_sup, HTMX_TYPE_NODE)

static void
htmx_sup_class_init(HtmxSupClass *klass)
{
}

static void
htmx_sup_init(HtmxSup *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "sup");
}

HtmxSup *
htmx_sup_new(void)
{
	return g_object_new(HTMX_TYPE_SUP, NULL);
}

HtmxSup *
htmx_sup_new_with_text(const gchar *text)
{
	HtmxSup *self;

	self = htmx_sup_new();
	if (text != NULL) {
		htmx_node_set_text_content(HTMX_NODE(self), text);
	}

	return self;
}

/*
 * HtmxCode - Inline code element
 */

struct _HtmxCode {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxCode, htmx_code, HTMX_TYPE_NODE)

static void
htmx_code_class_init(HtmxCodeClass *klass)
{
}

static void
htmx_code_init(HtmxCode *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "code");
}

HtmxCode *
htmx_code_new(void)
{
	return g_object_new(HTMX_TYPE_CODE, NULL);
}

HtmxCode *
htmx_code_new_with_text(const gchar *text)
{
	HtmxCode *self;

	self = htmx_code_new();
	if (text != NULL) {
		htmx_node_set_text_content(HTMX_NODE(self), text);
	}

	return self;
}

/*
 * HtmxPre - Preformatted text element
 */

struct _HtmxPre {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxPre, htmx_pre, HTMX_TYPE_NODE)

static void
htmx_pre_class_init(HtmxPreClass *klass)
{
}

static void
htmx_pre_init(HtmxPre *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "pre");
}

HtmxPre *
htmx_pre_new(void)
{
	return g_object_new(HTMX_TYPE_PRE, NULL);
}

HtmxPre *
htmx_pre_new_with_text(const gchar *text)
{
	HtmxPre *self;

	self = htmx_pre_new();
	if (text != NULL) {
		htmx_node_set_text_content(HTMX_NODE(self), text);
	}

	return self;
}

/*
 * HtmxKbd - Keyboard input element
 */

struct _HtmxKbd {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxKbd, htmx_kbd, HTMX_TYPE_NODE)

static void
htmx_kbd_class_init(HtmxKbdClass *klass)
{
}

static void
htmx_kbd_init(HtmxKbd *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "kbd");
}

HtmxKbd *
htmx_kbd_new(void)
{
	return g_object_new(HTMX_TYPE_KBD, NULL);
}

HtmxKbd *
htmx_kbd_new_with_text(const gchar *text)
{
	HtmxKbd *self;

	self = htmx_kbd_new();
	if (text != NULL) {
		htmx_node_set_text_content(HTMX_NODE(self), text);
	}

	return self;
}

/*
 * HtmxSamp - Sample output element
 */

struct _HtmxSamp {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxSamp, htmx_samp, HTMX_TYPE_NODE)

static void
htmx_samp_class_init(HtmxSampClass *klass)
{
}

static void
htmx_samp_init(HtmxSamp *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "samp");
}

HtmxSamp *
htmx_samp_new(void)
{
	return g_object_new(HTMX_TYPE_SAMP, NULL);
}

HtmxSamp *
htmx_samp_new_with_text(const gchar *text)
{
	HtmxSamp *self;

	self = htmx_samp_new();
	if (text != NULL) {
		htmx_node_set_text_content(HTMX_NODE(self), text);
	}

	return self;
}

/*
 * HtmxVar - Variable element
 */

struct _HtmxVar {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxVar, htmx_var, HTMX_TYPE_NODE)

static void
htmx_var_class_init(HtmxVarClass *klass)
{
}

static void
htmx_var_init(HtmxVar *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "var");
}

HtmxVar *
htmx_var_new(void)
{
	return g_object_new(HTMX_TYPE_VAR, NULL);
}

HtmxVar *
htmx_var_new_with_text(const gchar *text)
{
	HtmxVar *self;

	self = htmx_var_new();
	if (text != NULL) {
		htmx_node_set_text_content(HTMX_NODE(self), text);
	}

	return self;
}

/*
 * HtmxAbbr - Abbreviation element
 */

struct _HtmxAbbr {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxAbbr, htmx_abbr, HTMX_TYPE_NODE)

static void
htmx_abbr_class_init(HtmxAbbrClass *klass)
{
}

static void
htmx_abbr_init(HtmxAbbr *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "abbr");
}

HtmxAbbr *
htmx_abbr_new(void)
{
	return g_object_new(HTMX_TYPE_ABBR, NULL);
}

HtmxAbbr *
htmx_abbr_new_with_title(
	const gchar *abbr,
	const gchar *title
){
	HtmxAbbr *self;

	self = htmx_abbr_new();
	if (abbr != NULL) {
		htmx_node_set_text_content(HTMX_NODE(self), abbr);
	}
	if (title != NULL) {
		htmx_abbr_set_title(self, title);
	}

	return self;
}

void
htmx_abbr_set_title(
	HtmxAbbr    *self,
	const gchar *title
){
	g_return_if_fail(HTMX_IS_ABBR(self));

	htmx_element_set_attribute(HTMX_ELEMENT(self), "title", title);
}

/*
 * HtmxCite - Citation element
 */

struct _HtmxCite {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxCite, htmx_cite, HTMX_TYPE_NODE)

static void
htmx_cite_class_init(HtmxCiteClass *klass)
{
}

static void
htmx_cite_init(HtmxCite *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "cite");
}

HtmxCite *
htmx_cite_new(void)
{
	return g_object_new(HTMX_TYPE_CITE, NULL);
}

HtmxCite *
htmx_cite_new_with_text(const gchar *text)
{
	HtmxCite *self;

	self = htmx_cite_new();
	if (text != NULL) {
		htmx_node_set_text_content(HTMX_NODE(self), text);
	}

	return self;
}

/*
 * HtmxQ - Inline quotation element
 */

struct _HtmxQ {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxQ, htmx_q, HTMX_TYPE_NODE)

static void
htmx_q_class_init(HtmxQClass *klass)
{
}

static void
htmx_q_init(HtmxQ *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "q");
}

HtmxQ *
htmx_q_new(void)
{
	return g_object_new(HTMX_TYPE_Q, NULL);
}

HtmxQ *
htmx_q_new_with_text(const gchar *text)
{
	HtmxQ *self;

	self = htmx_q_new();
	if (text != NULL) {
		htmx_node_set_text_content(HTMX_NODE(self), text);
	}

	return self;
}

void
htmx_q_set_cite(
	HtmxQ       *self,
	const gchar *cite
){
	g_return_if_fail(HTMX_IS_Q(self));

	htmx_element_set_attribute(HTMX_ELEMENT(self), "cite", cite);
}

/*
 * HtmxBlockquote - Block quotation element
 */

struct _HtmxBlockquote {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxBlockquote, htmx_blockquote, HTMX_TYPE_NODE)

static void
htmx_blockquote_class_init(HtmxBlockquoteClass *klass)
{
}

static void
htmx_blockquote_init(HtmxBlockquote *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "blockquote");
}

HtmxBlockquote *
htmx_blockquote_new(void)
{
	return g_object_new(HTMX_TYPE_BLOCKQUOTE, NULL);
}

void
htmx_blockquote_set_cite(
	HtmxBlockquote *self,
	const gchar    *cite
){
	g_return_if_fail(HTMX_IS_BLOCKQUOTE(self));

	htmx_element_set_attribute(HTMX_ELEMENT(self), "cite", cite);
}

/*
 * HtmxTime - Time element
 */

struct _HtmxTime {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxTime, htmx_time, HTMX_TYPE_NODE)

static void
htmx_time_class_init(HtmxTimeClass *klass)
{
}

static void
htmx_time_init(HtmxTime *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "time");
}

HtmxTime *
htmx_time_new(void)
{
	return g_object_new(HTMX_TYPE_TIME, NULL);
}

HtmxTime *
htmx_time_new_with_datetime(
	const gchar *display,
	const gchar *datetime
){
	HtmxTime *self;

	self = htmx_time_new();
	if (display != NULL) {
		htmx_node_set_text_content(HTMX_NODE(self), display);
	}
	if (datetime != NULL) {
		htmx_time_set_datetime(self, datetime);
	}

	return self;
}

void
htmx_time_set_datetime(
	HtmxTime    *self,
	const gchar *datetime
){
	g_return_if_fail(HTMX_IS_TIME(self));

	htmx_element_set_attribute(HTMX_ELEMENT(self), "datetime", datetime);
}

/*
 * HtmxDel - Deleted text element
 */

struct _HtmxDel {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxDel, htmx_del, HTMX_TYPE_NODE)

static void
htmx_del_class_init(HtmxDelClass *klass)
{
}

static void
htmx_del_init(HtmxDel *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "del");
}

HtmxDel *
htmx_del_new(void)
{
	return g_object_new(HTMX_TYPE_DEL, NULL);
}

HtmxDel *
htmx_del_new_with_text(const gchar *text)
{
	HtmxDel *self;

	self = htmx_del_new();
	if (text != NULL) {
		htmx_node_set_text_content(HTMX_NODE(self), text);
	}

	return self;
}

void
htmx_del_set_cite(
	HtmxDel     *self,
	const gchar *cite
){
	g_return_if_fail(HTMX_IS_DEL(self));

	htmx_element_set_attribute(HTMX_ELEMENT(self), "cite", cite);
}

void
htmx_del_set_datetime(
	HtmxDel     *self,
	const gchar *datetime
){
	g_return_if_fail(HTMX_IS_DEL(self));

	htmx_element_set_attribute(HTMX_ELEMENT(self), "datetime", datetime);
}

/*
 * HtmxIns - Inserted text element
 */

struct _HtmxIns {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxIns, htmx_ins, HTMX_TYPE_NODE)

static void
htmx_ins_class_init(HtmxInsClass *klass)
{
}

static void
htmx_ins_init(HtmxIns *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "ins");
}

HtmxIns *
htmx_ins_new(void)
{
	return g_object_new(HTMX_TYPE_INS, NULL);
}

HtmxIns *
htmx_ins_new_with_text(const gchar *text)
{
	HtmxIns *self;

	self = htmx_ins_new();
	if (text != NULL) {
		htmx_node_set_text_content(HTMX_NODE(self), text);
	}

	return self;
}

void
htmx_ins_set_cite(
	HtmxIns     *self,
	const gchar *cite
){
	g_return_if_fail(HTMX_IS_INS(self));

	htmx_element_set_attribute(HTMX_ELEMENT(self), "cite", cite);
}

void
htmx_ins_set_datetime(
	HtmxIns     *self,
	const gchar *datetime
){
	g_return_if_fail(HTMX_IS_INS(self));

	htmx_element_set_attribute(HTMX_ELEMENT(self), "datetime", datetime);
}

/*
 * HtmxRuby - Ruby annotation container element
 */

struct _HtmxRuby {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxRuby, htmx_ruby, HTMX_TYPE_NODE)

static void
htmx_ruby_class_init(HtmxRubyClass *klass)
{
}

static void
htmx_ruby_init(HtmxRuby *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "ruby");
}

HtmxRuby *
htmx_ruby_new(void)
{
	return g_object_new(HTMX_TYPE_RUBY, NULL);
}

/*
 * HtmxRt - Ruby text element
 */

struct _HtmxRt {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxRt, htmx_rt, HTMX_TYPE_NODE)

static void
htmx_rt_class_init(HtmxRtClass *klass)
{
}

static void
htmx_rt_init(HtmxRt *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "rt");
}

HtmxRt *
htmx_rt_new(void)
{
	return g_object_new(HTMX_TYPE_RT, NULL);
}

HtmxRt *
htmx_rt_new_with_text(const gchar *text)
{
	HtmxRt *self;

	self = htmx_rt_new();
	if (text != NULL) {
		htmx_node_set_text_content(HTMX_NODE(self), text);
	}

	return self;
}

/*
 * HtmxRp - Ruby fallback parenthesis element
 */

struct _HtmxRp {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxRp, htmx_rp, HTMX_TYPE_NODE)

static void
htmx_rp_class_init(HtmxRpClass *klass)
{
}

static void
htmx_rp_init(HtmxRp *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "rp");
}

HtmxRp *
htmx_rp_new(void)
{
	return g_object_new(HTMX_TYPE_RP, NULL);
}

HtmxRp *
htmx_rp_new_with_text(const gchar *text)
{
	HtmxRp *self;

	self = htmx_rp_new();
	if (text != NULL) {
		htmx_node_set_text_content(HTMX_NODE(self), text);
	}

	return self;
}

/*
 * HtmxDfn - Definition term element
 */

struct _HtmxDfn {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxDfn, htmx_dfn, HTMX_TYPE_NODE)

static void
htmx_dfn_class_init(HtmxDfnClass *klass)
{
}

static void
htmx_dfn_init(HtmxDfn *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "dfn");
}

HtmxDfn *
htmx_dfn_new(void)
{
	return g_object_new(HTMX_TYPE_DFN, NULL);
}

HtmxDfn *
htmx_dfn_new_with_text(const gchar *text)
{
	HtmxDfn *self;

	self = htmx_dfn_new();
	if (text != NULL) {
		htmx_node_set_text_content(HTMX_NODE(self), text);
	}

	return self;
}

/*
 * HtmxData - Machine-readable data element
 */

struct _HtmxData {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxData, htmx_data, HTMX_TYPE_NODE)

static void
htmx_data_class_init(HtmxDataClass *klass)
{
}

static void
htmx_data_init(HtmxData *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "data");
}

HtmxData *
htmx_data_new(void)
{
	return g_object_new(HTMX_TYPE_DATA, NULL);
}

HtmxData *
htmx_data_new_with_value(
	const gchar *display,
	const gchar *value
){
	HtmxData *self;

	self = htmx_data_new();
	if (display != NULL) {
		htmx_node_set_text_content(HTMX_NODE(self), display);
	}
	if (value != NULL) {
		htmx_data_set_value(self, value);
	}

	return self;
}

void
htmx_data_set_value(
	HtmxData    *self,
	const gchar *value
){
	g_return_if_fail(HTMX_IS_DATA(self));

	htmx_element_set_attribute(HTMX_ELEMENT(self), "value", value);
}

/*
 * HtmxBdi - Bidirectional isolation element
 */

struct _HtmxBdi {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxBdi, htmx_bdi, HTMX_TYPE_NODE)

static void
htmx_bdi_class_init(HtmxBdiClass *klass)
{
}

static void
htmx_bdi_init(HtmxBdi *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "bdi");
}

HtmxBdi *
htmx_bdi_new(void)
{
	return g_object_new(HTMX_TYPE_BDI, NULL);
}

HtmxBdi *
htmx_bdi_new_with_text(const gchar *text)
{
	HtmxBdi *self;

	self = htmx_bdi_new();
	if (text != NULL) {
		htmx_node_set_text_content(HTMX_NODE(self), text);
	}

	return self;
}

/*
 * HtmxBdo - Bidirectional override element
 */

struct _HtmxBdo {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxBdo, htmx_bdo, HTMX_TYPE_NODE)

static void
htmx_bdo_class_init(HtmxBdoClass *klass)
{
}

static void
htmx_bdo_init(HtmxBdo *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "bdo");
}

HtmxBdo *
htmx_bdo_new(void)
{
	return g_object_new(HTMX_TYPE_BDO, NULL);
}

HtmxBdo *
htmx_bdo_new_with_dir(
	const gchar *text,
	const gchar *dir
){
	HtmxBdo *self;

	self = htmx_bdo_new();
	if (text != NULL) {
		htmx_node_set_text_content(HTMX_NODE(self), text);
	}
	if (dir != NULL) {
		htmx_bdo_set_dir(self, dir);
	}

	return self;
}

void
htmx_bdo_set_dir(
	HtmxBdo     *self,
	const gchar *dir
){
	g_return_if_fail(HTMX_IS_BDO(self));

	htmx_element_set_attribute(HTMX_ELEMENT(self), "dir", dir);
}
