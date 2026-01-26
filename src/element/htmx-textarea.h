/*
 * htmx-textarea.h - HTML textarea element
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * This file defines the HtmxTextarea class, a typed wrapper for <textarea> elements.
 */

#ifndef HTMX_TEXTAREA_H
#define HTMX_TEXTAREA_H

#include <glib-object.h>
#include "htmx-node.h"

G_BEGIN_DECLS

#define HTMX_TYPE_TEXTAREA (htmx_textarea_get_type())
G_DECLARE_FINAL_TYPE(HtmxTextarea, htmx_textarea, HTMX, TEXTAREA, HtmxNode)

/**
 * htmx_textarea_new:
 *
 * Creates a new #HtmxTextarea element.
 *
 * Returns: (transfer full): a new #HtmxTextarea
 */
HtmxTextarea *
htmx_textarea_new(void);

/**
 * htmx_textarea_new_with_name:
 * @name: the name attribute value
 *
 * Creates a new #HtmxTextarea element with the specified name.
 *
 * Returns: (transfer full): a new #HtmxTextarea
 */
HtmxTextarea *
htmx_textarea_new_with_name(const gchar *name);

/**
 * htmx_textarea_set_name:
 * @self: an #HtmxTextarea
 * @name: the name attribute value
 *
 * Sets the name attribute.
 */
void
htmx_textarea_set_name(
	HtmxTextarea *self,
	const gchar  *name
);

/**
 * htmx_textarea_set_placeholder:
 * @self: an #HtmxTextarea
 * @placeholder: the placeholder text
 *
 * Sets the placeholder attribute.
 */
void
htmx_textarea_set_placeholder(
	HtmxTextarea *self,
	const gchar  *placeholder
);

/**
 * htmx_textarea_set_rows:
 * @self: an #HtmxTextarea
 * @rows: the number of visible rows
 *
 * Sets the rows attribute.
 */
void
htmx_textarea_set_rows(
	HtmxTextarea *self,
	guint         rows
);

/**
 * htmx_textarea_set_cols:
 * @self: an #HtmxTextarea
 * @cols: the number of visible columns
 *
 * Sets the cols attribute.
 */
void
htmx_textarea_set_cols(
	HtmxTextarea *self,
	guint         cols
);

/**
 * htmx_textarea_set_required:
 * @self: an #HtmxTextarea
 * @required: whether the textarea is required
 *
 * Sets the required attribute.
 */
void
htmx_textarea_set_required(
	HtmxTextarea *self,
	gboolean      required
);

/**
 * htmx_textarea_set_readonly:
 * @self: an #HtmxTextarea
 * @readonly: whether the textarea is readonly
 *
 * Sets the readonly attribute.
 */
void
htmx_textarea_set_readonly(
	HtmxTextarea *self,
	gboolean      readonly
);

/**
 * htmx_textarea_set_disabled:
 * @self: an #HtmxTextarea
 * @disabled: whether the textarea is disabled
 *
 * Sets the disabled attribute.
 */
void
htmx_textarea_set_disabled(
	HtmxTextarea *self,
	gboolean      disabled
);

G_END_DECLS

#endif /* HTMX_TEXTAREA_H */
