/*
 * htmx-text.h - HTML text formatting elements
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * This file defines text formatting elements: strong, em, b, i, u, s,
 * small, mark, sub, sup, code, pre, kbd, samp, var, abbr, cite, q,
 * blockquote, and time.
 */

#ifndef HTMX_TEXT_H
#define HTMX_TEXT_H

#include <glib-object.h>
#include "htmx-node.h"

G_BEGIN_DECLS

/*
 * HtmxStrong - Strong importance element
 */

#define HTMX_TYPE_STRONG (htmx_strong_get_type())
G_DECLARE_FINAL_TYPE(HtmxStrong, htmx_strong, HTMX, STRONG, HtmxNode)

/**
 * htmx_strong_new:
 *
 * Creates a new #HtmxStrong element.
 *
 * Returns: (transfer full): a new #HtmxStrong
 */
HtmxStrong *
htmx_strong_new(void);

/**
 * htmx_strong_new_with_text:
 * @text: the text content
 *
 * Creates a new #HtmxStrong with text content.
 *
 * Returns: (transfer full): a new #HtmxStrong
 */
HtmxStrong *
htmx_strong_new_with_text(const gchar *text);

/*
 * HtmxEm - Emphasis element
 */

#define HTMX_TYPE_EM (htmx_em_get_type())
G_DECLARE_FINAL_TYPE(HtmxEm, htmx_em, HTMX, EM, HtmxNode)

/**
 * htmx_em_new:
 *
 * Creates a new #HtmxEm element.
 *
 * Returns: (transfer full): a new #HtmxEm
 */
HtmxEm *
htmx_em_new(void);

/**
 * htmx_em_new_with_text:
 * @text: the text content
 *
 * Creates a new #HtmxEm with text content.
 *
 * Returns: (transfer full): a new #HtmxEm
 */
HtmxEm *
htmx_em_new_with_text(const gchar *text);

/*
 * HtmxB - Bold (stylistic) element
 */

#define HTMX_TYPE_B (htmx_b_get_type())
G_DECLARE_FINAL_TYPE(HtmxB, htmx_b, HTMX, B, HtmxNode)

/**
 * htmx_b_new:
 *
 * Creates a new #HtmxB element.
 *
 * Returns: (transfer full): a new #HtmxB
 */
HtmxB *
htmx_b_new(void);

/**
 * htmx_b_new_with_text:
 * @text: the text content
 *
 * Creates a new #HtmxB with text content.
 *
 * Returns: (transfer full): a new #HtmxB
 */
HtmxB *
htmx_b_new_with_text(const gchar *text);

/*
 * HtmxI - Italic (stylistic) element
 */

#define HTMX_TYPE_I (htmx_i_get_type())
G_DECLARE_FINAL_TYPE(HtmxI, htmx_i, HTMX, I, HtmxNode)

/**
 * htmx_i_new:
 *
 * Creates a new #HtmxI element.
 *
 * Returns: (transfer full): a new #HtmxI
 */
HtmxI *
htmx_i_new(void);

/**
 * htmx_i_new_with_text:
 * @text: the text content
 *
 * Creates a new #HtmxI with text content.
 *
 * Returns: (transfer full): a new #HtmxI
 */
HtmxI *
htmx_i_new_with_text(const gchar *text);

/*
 * HtmxU - Underline element
 */

#define HTMX_TYPE_U (htmx_u_get_type())
G_DECLARE_FINAL_TYPE(HtmxU, htmx_u, HTMX, U, HtmxNode)

/**
 * htmx_u_new:
 *
 * Creates a new #HtmxU element.
 *
 * Returns: (transfer full): a new #HtmxU
 */
HtmxU *
htmx_u_new(void);

/**
 * htmx_u_new_with_text:
 * @text: the text content
 *
 * Creates a new #HtmxU with text content.
 *
 * Returns: (transfer full): a new #HtmxU
 */
HtmxU *
htmx_u_new_with_text(const gchar *text);

/*
 * HtmxS - Strikethrough element
 */

#define HTMX_TYPE_S (htmx_s_get_type())
G_DECLARE_FINAL_TYPE(HtmxS, htmx_s, HTMX, S, HtmxNode)

/**
 * htmx_s_new:
 *
 * Creates a new #HtmxS element.
 *
 * Returns: (transfer full): a new #HtmxS
 */
HtmxS *
htmx_s_new(void);

/**
 * htmx_s_new_with_text:
 * @text: the text content
 *
 * Creates a new #HtmxS with text content.
 *
 * Returns: (transfer full): a new #HtmxS
 */
HtmxS *
htmx_s_new_with_text(const gchar *text);

/*
 * HtmxSmall - Small text element
 */

#define HTMX_TYPE_SMALL (htmx_small_get_type())
G_DECLARE_FINAL_TYPE(HtmxSmall, htmx_small, HTMX, SMALL, HtmxNode)

/**
 * htmx_small_new:
 *
 * Creates a new #HtmxSmall element.
 *
 * Returns: (transfer full): a new #HtmxSmall
 */
HtmxSmall *
htmx_small_new(void);

/**
 * htmx_small_new_with_text:
 * @text: the text content
 *
 * Creates a new #HtmxSmall with text content.
 *
 * Returns: (transfer full): a new #HtmxSmall
 */
HtmxSmall *
htmx_small_new_with_text(const gchar *text);

/*
 * HtmxMark - Marked/highlighted element
 */

#define HTMX_TYPE_MARK (htmx_mark_get_type())
G_DECLARE_FINAL_TYPE(HtmxMark, htmx_mark, HTMX, MARK, HtmxNode)

/**
 * htmx_mark_new:
 *
 * Creates a new #HtmxMark element.
 *
 * Returns: (transfer full): a new #HtmxMark
 */
HtmxMark *
htmx_mark_new(void);

/**
 * htmx_mark_new_with_text:
 * @text: the text content
 *
 * Creates a new #HtmxMark with text content.
 *
 * Returns: (transfer full): a new #HtmxMark
 */
HtmxMark *
htmx_mark_new_with_text(const gchar *text);

/*
 * HtmxSub - Subscript element
 */

#define HTMX_TYPE_SUB (htmx_sub_get_type())
G_DECLARE_FINAL_TYPE(HtmxSub, htmx_sub, HTMX, SUB, HtmxNode)

/**
 * htmx_sub_new:
 *
 * Creates a new #HtmxSub element.
 *
 * Returns: (transfer full): a new #HtmxSub
 */
HtmxSub *
htmx_sub_new(void);

/**
 * htmx_sub_new_with_text:
 * @text: the text content
 *
 * Creates a new #HtmxSub with text content.
 *
 * Returns: (transfer full): a new #HtmxSub
 */
HtmxSub *
htmx_sub_new_with_text(const gchar *text);

/*
 * HtmxSup - Superscript element
 */

#define HTMX_TYPE_SUP (htmx_sup_get_type())
G_DECLARE_FINAL_TYPE(HtmxSup, htmx_sup, HTMX, SUP, HtmxNode)

/**
 * htmx_sup_new:
 *
 * Creates a new #HtmxSup element.
 *
 * Returns: (transfer full): a new #HtmxSup
 */
HtmxSup *
htmx_sup_new(void);

/**
 * htmx_sup_new_with_text:
 * @text: the text content
 *
 * Creates a new #HtmxSup with text content.
 *
 * Returns: (transfer full): a new #HtmxSup
 */
HtmxSup *
htmx_sup_new_with_text(const gchar *text);

/*
 * HtmxCode - Inline code element
 */

#define HTMX_TYPE_CODE (htmx_code_get_type())
G_DECLARE_FINAL_TYPE(HtmxCode, htmx_code, HTMX, CODE, HtmxNode)

/**
 * htmx_code_new:
 *
 * Creates a new #HtmxCode element.
 *
 * Returns: (transfer full): a new #HtmxCode
 */
HtmxCode *
htmx_code_new(void);

/**
 * htmx_code_new_with_text:
 * @text: the code text
 *
 * Creates a new #HtmxCode with text content.
 *
 * Returns: (transfer full): a new #HtmxCode
 */
HtmxCode *
htmx_code_new_with_text(const gchar *text);

/*
 * HtmxPre - Preformatted text element
 */

#define HTMX_TYPE_PRE (htmx_pre_get_type())
G_DECLARE_FINAL_TYPE(HtmxPre, htmx_pre, HTMX, PRE, HtmxNode)

/**
 * htmx_pre_new:
 *
 * Creates a new #HtmxPre element.
 *
 * Returns: (transfer full): a new #HtmxPre
 */
HtmxPre *
htmx_pre_new(void);

/**
 * htmx_pre_new_with_text:
 * @text: the preformatted text
 *
 * Creates a new #HtmxPre with text content.
 *
 * Returns: (transfer full): a new #HtmxPre
 */
HtmxPre *
htmx_pre_new_with_text(const gchar *text);

/*
 * HtmxKbd - Keyboard input element
 */

#define HTMX_TYPE_KBD (htmx_kbd_get_type())
G_DECLARE_FINAL_TYPE(HtmxKbd, htmx_kbd, HTMX, KBD, HtmxNode)

/**
 * htmx_kbd_new:
 *
 * Creates a new #HtmxKbd element.
 *
 * Returns: (transfer full): a new #HtmxKbd
 */
HtmxKbd *
htmx_kbd_new(void);

/**
 * htmx_kbd_new_with_text:
 * @text: the keyboard input text
 *
 * Creates a new #HtmxKbd with text content.
 *
 * Returns: (transfer full): a new #HtmxKbd
 */
HtmxKbd *
htmx_kbd_new_with_text(const gchar *text);

/*
 * HtmxSamp - Sample output element
 */

#define HTMX_TYPE_SAMP (htmx_samp_get_type())
G_DECLARE_FINAL_TYPE(HtmxSamp, htmx_samp, HTMX, SAMP, HtmxNode)

/**
 * htmx_samp_new:
 *
 * Creates a new #HtmxSamp element.
 *
 * Returns: (transfer full): a new #HtmxSamp
 */
HtmxSamp *
htmx_samp_new(void);

/**
 * htmx_samp_new_with_text:
 * @text: the sample output text
 *
 * Creates a new #HtmxSamp with text content.
 *
 * Returns: (transfer full): a new #HtmxSamp
 */
HtmxSamp *
htmx_samp_new_with_text(const gchar *text);

/*
 * HtmxVar - Variable element
 */

#define HTMX_TYPE_VAR (htmx_var_get_type())
G_DECLARE_FINAL_TYPE(HtmxVar, htmx_var, HTMX, VAR, HtmxNode)

/**
 * htmx_var_new:
 *
 * Creates a new #HtmxVar element.
 *
 * Returns: (transfer full): a new #HtmxVar
 */
HtmxVar *
htmx_var_new(void);

/**
 * htmx_var_new_with_text:
 * @text: the variable name
 *
 * Creates a new #HtmxVar with text content.
 *
 * Returns: (transfer full): a new #HtmxVar
 */
HtmxVar *
htmx_var_new_with_text(const gchar *text);

/*
 * HtmxAbbr - Abbreviation element
 */

#define HTMX_TYPE_ABBR (htmx_abbr_get_type())
G_DECLARE_FINAL_TYPE(HtmxAbbr, htmx_abbr, HTMX, ABBR, HtmxNode)

/**
 * htmx_abbr_new:
 *
 * Creates a new #HtmxAbbr element.
 *
 * Returns: (transfer full): a new #HtmxAbbr
 */
HtmxAbbr *
htmx_abbr_new(void);

/**
 * htmx_abbr_new_with_title:
 * @abbr: the abbreviation text
 * @title: the full expansion
 *
 * Creates a new #HtmxAbbr with text and title.
 *
 * Returns: (transfer full): a new #HtmxAbbr
 */
HtmxAbbr *
htmx_abbr_new_with_title(
	const gchar *abbr,
	const gchar *title
);

/**
 * htmx_abbr_set_title:
 * @self: an #HtmxAbbr
 * @title: the full expansion
 *
 * Sets the title attribute.
 */
void
htmx_abbr_set_title(
	HtmxAbbr    *self,
	const gchar *title
);

/*
 * HtmxCite - Citation element
 */

#define HTMX_TYPE_CITE (htmx_cite_get_type())
G_DECLARE_FINAL_TYPE(HtmxCite, htmx_cite, HTMX, CITE, HtmxNode)

/**
 * htmx_cite_new:
 *
 * Creates a new #HtmxCite element.
 *
 * Returns: (transfer full): a new #HtmxCite
 */
HtmxCite *
htmx_cite_new(void);

/**
 * htmx_cite_new_with_text:
 * @text: the citation text
 *
 * Creates a new #HtmxCite with text content.
 *
 * Returns: (transfer full): a new #HtmxCite
 */
HtmxCite *
htmx_cite_new_with_text(const gchar *text);

/*
 * HtmxQ - Inline quotation element
 */

#define HTMX_TYPE_Q (htmx_q_get_type())
G_DECLARE_FINAL_TYPE(HtmxQ, htmx_q, HTMX, Q, HtmxNode)

/**
 * htmx_q_new:
 *
 * Creates a new #HtmxQ element.
 *
 * Returns: (transfer full): a new #HtmxQ
 */
HtmxQ *
htmx_q_new(void);

/**
 * htmx_q_new_with_text:
 * @text: the quoted text
 *
 * Creates a new #HtmxQ with text content.
 *
 * Returns: (transfer full): a new #HtmxQ
 */
HtmxQ *
htmx_q_new_with_text(const gchar *text);

/**
 * htmx_q_set_cite:
 * @self: an #HtmxQ
 * @cite: URL of the quote source
 *
 * Sets the cite attribute.
 */
void
htmx_q_set_cite(
	HtmxQ       *self,
	const gchar *cite
);

/*
 * HtmxBlockquote - Block quotation element
 */

#define HTMX_TYPE_BLOCKQUOTE (htmx_blockquote_get_type())
G_DECLARE_FINAL_TYPE(HtmxBlockquote, htmx_blockquote, HTMX, BLOCKQUOTE, HtmxNode)

/**
 * htmx_blockquote_new:
 *
 * Creates a new #HtmxBlockquote element.
 *
 * Returns: (transfer full): a new #HtmxBlockquote
 */
HtmxBlockquote *
htmx_blockquote_new(void);

/**
 * htmx_blockquote_set_cite:
 * @self: an #HtmxBlockquote
 * @cite: URL of the quote source
 *
 * Sets the cite attribute.
 */
void
htmx_blockquote_set_cite(
	HtmxBlockquote *self,
	const gchar    *cite
);

/*
 * HtmxTime - Time element
 */

#define HTMX_TYPE_TIME (htmx_time_get_type())
G_DECLARE_FINAL_TYPE(HtmxTime, htmx_time, HTMX, TIME, HtmxNode)

/**
 * htmx_time_new:
 *
 * Creates a new #HtmxTime element.
 *
 * Returns: (transfer full): a new #HtmxTime
 */
HtmxTime *
htmx_time_new(void);

/**
 * htmx_time_new_with_datetime:
 * @display: the human-readable text
 * @datetime: the machine-readable datetime
 *
 * Creates a new #HtmxTime with display text and datetime.
 *
 * Returns: (transfer full): a new #HtmxTime
 */
HtmxTime *
htmx_time_new_with_datetime(
	const gchar *display,
	const gchar *datetime
);

/**
 * htmx_time_set_datetime:
 * @self: an #HtmxTime
 * @datetime: the machine-readable datetime
 *
 * Sets the datetime attribute.
 */
void
htmx_time_set_datetime(
	HtmxTime    *self,
	const gchar *datetime
);

/*
 * HtmxDel - Deleted text element
 */

#define HTMX_TYPE_DEL (htmx_del_get_type())
G_DECLARE_FINAL_TYPE(HtmxDel, htmx_del, HTMX, DEL, HtmxNode)

/**
 * htmx_del_new:
 *
 * Creates a new #HtmxDel element.
 *
 * Returns: (transfer full): a new #HtmxDel
 */
HtmxDel *
htmx_del_new(void);

/**
 * htmx_del_new_with_text:
 * @text: the deleted text
 *
 * Creates a new #HtmxDel with text content.
 *
 * Returns: (transfer full): a new #HtmxDel
 */
HtmxDel *
htmx_del_new_with_text(const gchar *text);

/**
 * htmx_del_set_cite:
 * @self: an #HtmxDel
 * @cite: URL explaining the change
 *
 * Sets the cite attribute.
 */
void
htmx_del_set_cite(
	HtmxDel     *self,
	const gchar *cite
);

/**
 * htmx_del_set_datetime:
 * @self: an #HtmxDel
 * @datetime: the date/time of the change
 *
 * Sets the datetime attribute.
 */
void
htmx_del_set_datetime(
	HtmxDel     *self,
	const gchar *datetime
);

/*
 * HtmxIns - Inserted text element
 */

#define HTMX_TYPE_INS (htmx_ins_get_type())
G_DECLARE_FINAL_TYPE(HtmxIns, htmx_ins, HTMX, INS, HtmxNode)

/**
 * htmx_ins_new:
 *
 * Creates a new #HtmxIns element.
 *
 * Returns: (transfer full): a new #HtmxIns
 */
HtmxIns *
htmx_ins_new(void);

/**
 * htmx_ins_new_with_text:
 * @text: the inserted text
 *
 * Creates a new #HtmxIns with text content.
 *
 * Returns: (transfer full): a new #HtmxIns
 */
HtmxIns *
htmx_ins_new_with_text(const gchar *text);

/**
 * htmx_ins_set_cite:
 * @self: an #HtmxIns
 * @cite: URL explaining the change
 *
 * Sets the cite attribute.
 */
void
htmx_ins_set_cite(
	HtmxIns     *self,
	const gchar *cite
);

/**
 * htmx_ins_set_datetime:
 * @self: an #HtmxIns
 * @datetime: the date/time of the change
 *
 * Sets the datetime attribute.
 */
void
htmx_ins_set_datetime(
	HtmxIns     *self,
	const gchar *datetime
);

/*
 * HtmxRuby - Ruby annotation container element
 */

#define HTMX_TYPE_RUBY (htmx_ruby_get_type())
G_DECLARE_FINAL_TYPE(HtmxRuby, htmx_ruby, HTMX, RUBY, HtmxNode)

/**
 * htmx_ruby_new:
 *
 * Creates a new #HtmxRuby element.
 *
 * Returns: (transfer full): a new #HtmxRuby
 */
HtmxRuby *
htmx_ruby_new(void);

/*
 * HtmxRt - Ruby text element
 */

#define HTMX_TYPE_RT (htmx_rt_get_type())
G_DECLARE_FINAL_TYPE(HtmxRt, htmx_rt, HTMX, RT, HtmxNode)

/**
 * htmx_rt_new:
 *
 * Creates a new #HtmxRt element.
 *
 * Returns: (transfer full): a new #HtmxRt
 */
HtmxRt *
htmx_rt_new(void);

/**
 * htmx_rt_new_with_text:
 * @text: the ruby text
 *
 * Creates a new #HtmxRt with text content.
 *
 * Returns: (transfer full): a new #HtmxRt
 */
HtmxRt *
htmx_rt_new_with_text(const gchar *text);

/*
 * HtmxRp - Ruby fallback parenthesis element
 */

#define HTMX_TYPE_RP (htmx_rp_get_type())
G_DECLARE_FINAL_TYPE(HtmxRp, htmx_rp, HTMX, RP, HtmxNode)

/**
 * htmx_rp_new:
 *
 * Creates a new #HtmxRp element.
 *
 * Returns: (transfer full): a new #HtmxRp
 */
HtmxRp *
htmx_rp_new(void);

/**
 * htmx_rp_new_with_text:
 * @text: the fallback text (typically "(" or ")")
 *
 * Creates a new #HtmxRp with text content.
 *
 * Returns: (transfer full): a new #HtmxRp
 */
HtmxRp *
htmx_rp_new_with_text(const gchar *text);

/*
 * HtmxDfn - Definition term element
 */

#define HTMX_TYPE_DFN (htmx_dfn_get_type())
G_DECLARE_FINAL_TYPE(HtmxDfn, htmx_dfn, HTMX, DFN, HtmxNode)

/**
 * htmx_dfn_new:
 *
 * Creates a new #HtmxDfn element.
 *
 * Returns: (transfer full): a new #HtmxDfn
 */
HtmxDfn *
htmx_dfn_new(void);

/**
 * htmx_dfn_new_with_text:
 * @text: the term being defined
 *
 * Creates a new #HtmxDfn with text content.
 *
 * Returns: (transfer full): a new #HtmxDfn
 */
HtmxDfn *
htmx_dfn_new_with_text(const gchar *text);

/*
 * HtmxData - Machine-readable data element
 */

#define HTMX_TYPE_DATA (htmx_data_get_type())
G_DECLARE_FINAL_TYPE(HtmxData, htmx_data, HTMX, DATA, HtmxNode)

/**
 * htmx_data_new:
 *
 * Creates a new #HtmxData element.
 *
 * Returns: (transfer full): a new #HtmxData
 */
HtmxData *
htmx_data_new(void);

/**
 * htmx_data_new_with_value:
 * @display: the human-readable text
 * @value: the machine-readable value
 *
 * Creates a new #HtmxData with display text and value.
 *
 * Returns: (transfer full): a new #HtmxData
 */
HtmxData *
htmx_data_new_with_value(
	const gchar *display,
	const gchar *value
);

/**
 * htmx_data_set_value:
 * @self: an #HtmxData
 * @value: the machine-readable value
 *
 * Sets the value attribute.
 */
void
htmx_data_set_value(
	HtmxData    *self,
	const gchar *value
);

/*
 * HtmxBdi - Bidirectional isolation element
 */

#define HTMX_TYPE_BDI (htmx_bdi_get_type())
G_DECLARE_FINAL_TYPE(HtmxBdi, htmx_bdi, HTMX, BDI, HtmxNode)

/**
 * htmx_bdi_new:
 *
 * Creates a new #HtmxBdi element.
 *
 * Returns: (transfer full): a new #HtmxBdi
 */
HtmxBdi *
htmx_bdi_new(void);

/**
 * htmx_bdi_new_with_text:
 * @text: the isolated text
 *
 * Creates a new #HtmxBdi with text content.
 *
 * Returns: (transfer full): a new #HtmxBdi
 */
HtmxBdi *
htmx_bdi_new_with_text(const gchar *text);

/*
 * HtmxBdo - Bidirectional override element
 */

#define HTMX_TYPE_BDO (htmx_bdo_get_type())
G_DECLARE_FINAL_TYPE(HtmxBdo, htmx_bdo, HTMX, BDO, HtmxNode)

/**
 * htmx_bdo_new:
 *
 * Creates a new #HtmxBdo element.
 *
 * Returns: (transfer full): a new #HtmxBdo
 */
HtmxBdo *
htmx_bdo_new(void);

/**
 * htmx_bdo_new_with_dir:
 * @text: the text content
 * @dir: the text direction ("ltr" or "rtl")
 *
 * Creates a new #HtmxBdo with text and direction.
 *
 * Returns: (transfer full): a new #HtmxBdo
 */
HtmxBdo *
htmx_bdo_new_with_dir(
	const gchar *text,
	const gchar *dir
);

/**
 * htmx_bdo_set_dir:
 * @self: an #HtmxBdo
 * @dir: the text direction ("ltr" or "rtl")
 *
 * Sets the dir attribute.
 */
void
htmx_bdo_set_dir(
	HtmxBdo     *self,
	const gchar *dir
);

G_END_DECLS

#endif /* HTMX_TEXT_H */
