/*
 * htmx-label.h - HTML label element
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * This file defines the HtmxLabel class, a typed wrapper for <label> elements.
 */

#ifndef HTMX_LABEL_H
#define HTMX_LABEL_H

#include <glib-object.h>
#include "htmx-node.h"

G_BEGIN_DECLS

#define HTMX_TYPE_LABEL (htmx_label_get_type())
G_DECLARE_FINAL_TYPE(HtmxLabel, htmx_label, HTMX, LABEL, HtmxNode)

/**
 * htmx_label_new:
 *
 * Creates a new #HtmxLabel element.
 *
 * Returns: (transfer full): a new #HtmxLabel
 */
HtmxLabel *
htmx_label_new(void);

/**
 * htmx_label_new_for:
 * @for_id: the ID of the form element this label is for
 *
 * Creates a new #HtmxLabel element with the for attribute set.
 *
 * Returns: (transfer full): a new #HtmxLabel
 */
HtmxLabel *
htmx_label_new_for(const gchar *for_id);

/**
 * htmx_label_new_with_text:
 * @text: the label text
 *
 * Creates a new #HtmxLabel element with text content.
 *
 * Returns: (transfer full): a new #HtmxLabel
 */
HtmxLabel *
htmx_label_new_with_text(const gchar *text);

/**
 * htmx_label_set_for:
 * @self: an #HtmxLabel
 * @for_id: the ID of the form element this label is for
 *
 * Sets the for attribute.
 */
void
htmx_label_set_for(
	HtmxLabel   *self,
	const gchar *for_id
);

G_END_DECLS

#endif /* HTMX_LABEL_H */
