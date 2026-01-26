/*
 * htmx-interactive.c - HTML interactive elements implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-interactive.h"

/*
 * HtmxDetails - Disclosure widget element
 */

struct _HtmxDetails {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxDetails, htmx_details, HTMX_TYPE_NODE)

static void
htmx_details_class_init(HtmxDetailsClass *klass)
{
}

static void
htmx_details_init(HtmxDetails *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "details");
}

HtmxDetails *
htmx_details_new(void)
{
	return g_object_new(HTMX_TYPE_DETAILS, NULL);
}

void
htmx_details_set_open(
	HtmxDetails *self,
	gboolean     open
){
	g_return_if_fail(HTMX_IS_DETAILS(self));

	if (open) {
		htmx_element_set_attribute(HTMX_ELEMENT(self), "open", "open");
	}
}

/*
 * HtmxSummary - Summary/caption for details element
 */

struct _HtmxSummary {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxSummary, htmx_summary, HTMX_TYPE_NODE)

static void
htmx_summary_class_init(HtmxSummaryClass *klass)
{
}

static void
htmx_summary_init(HtmxSummary *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "summary");
}

HtmxSummary *
htmx_summary_new(void)
{
	return g_object_new(HTMX_TYPE_SUMMARY, NULL);
}

HtmxSummary *
htmx_summary_new_with_text(const gchar *text)
{
	HtmxSummary *self;

	self = htmx_summary_new();
	if (text != NULL) {
		htmx_node_set_text_content(HTMX_NODE(self), text);
	}

	return self;
}

/*
 * HtmxDialog - Dialog box element
 */

struct _HtmxDialog {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxDialog, htmx_dialog, HTMX_TYPE_NODE)

static void
htmx_dialog_class_init(HtmxDialogClass *klass)
{
}

static void
htmx_dialog_init(HtmxDialog *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "dialog");
}

HtmxDialog *
htmx_dialog_new(void)
{
	return g_object_new(HTMX_TYPE_DIALOG, NULL);
}

void
htmx_dialog_set_open(
	HtmxDialog *self,
	gboolean    open
){
	g_return_if_fail(HTMX_IS_DIALOG(self));

	if (open) {
		htmx_element_set_attribute(HTMX_ELEMENT(self), "open", "open");
	}
}

/*
 * HtmxMenu - Menu element
 */

struct _HtmxMenu {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxMenu, htmx_menu, HTMX_TYPE_NODE)

static void
htmx_menu_class_init(HtmxMenuClass *klass)
{
}

static void
htmx_menu_init(HtmxMenu *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "menu");
}

HtmxMenu *
htmx_menu_new(void)
{
	return g_object_new(HTMX_TYPE_MENU, NULL);
}
