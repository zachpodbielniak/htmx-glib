/*
 * htmx-interactive.h - HTML interactive elements
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * This file defines interactive elements: details, summary, dialog, and menu.
 */

#ifndef HTMX_INTERACTIVE_H
#define HTMX_INTERACTIVE_H

#include <glib-object.h>
#include "htmx-node.h"

G_BEGIN_DECLS

/*
 * HtmxDetails - Disclosure widget element
 */

#define HTMX_TYPE_DETAILS (htmx_details_get_type())
G_DECLARE_FINAL_TYPE(HtmxDetails, htmx_details, HTMX, DETAILS, HtmxNode)

/**
 * htmx_details_new:
 *
 * Creates a new #HtmxDetails element.
 *
 * Returns: (transfer full): a new #HtmxDetails
 */
HtmxDetails *
htmx_details_new(void);

/**
 * htmx_details_set_open:
 * @self: an #HtmxDetails
 * @open: whether the details are visible
 *
 * Sets the open attribute.
 */
void
htmx_details_set_open(
	HtmxDetails *self,
	gboolean     open
);

/*
 * HtmxSummary - Summary/caption for details element
 */

#define HTMX_TYPE_SUMMARY (htmx_summary_get_type())
G_DECLARE_FINAL_TYPE(HtmxSummary, htmx_summary, HTMX, SUMMARY, HtmxNode)

/**
 * htmx_summary_new:
 *
 * Creates a new #HtmxSummary element.
 *
 * Returns: (transfer full): a new #HtmxSummary
 */
HtmxSummary *
htmx_summary_new(void);

/**
 * htmx_summary_new_with_text:
 * @text: the summary text
 *
 * Creates a new #HtmxSummary with text content.
 *
 * Returns: (transfer full): a new #HtmxSummary
 */
HtmxSummary *
htmx_summary_new_with_text(const gchar *text);

/*
 * HtmxDialog - Dialog box element
 */

#define HTMX_TYPE_DIALOG (htmx_dialog_get_type())
G_DECLARE_FINAL_TYPE(HtmxDialog, htmx_dialog, HTMX, DIALOG, HtmxNode)

/**
 * htmx_dialog_new:
 *
 * Creates a new #HtmxDialog element.
 *
 * Returns: (transfer full): a new #HtmxDialog
 */
HtmxDialog *
htmx_dialog_new(void);

/**
 * htmx_dialog_set_open:
 * @self: an #HtmxDialog
 * @open: whether the dialog is visible
 *
 * Sets the open attribute.
 */
void
htmx_dialog_set_open(
	HtmxDialog *self,
	gboolean    open
);

/*
 * HtmxMenu - Menu element
 */

#define HTMX_TYPE_MENU (htmx_menu_get_type())
G_DECLARE_FINAL_TYPE(HtmxMenu, htmx_menu, HTMX, MENU, HtmxNode)

/**
 * htmx_menu_new:
 *
 * Creates a new #HtmxMenu element.
 *
 * Returns: (transfer full): a new #HtmxMenu
 */
HtmxMenu *
htmx_menu_new(void);

G_END_DECLS

#endif /* HTMX_INTERACTIVE_H */
