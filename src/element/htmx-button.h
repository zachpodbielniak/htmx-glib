/*
 * htmx-button.h - HTML button element
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * This file defines the HtmxButton class, a typed wrapper for <button> elements.
 */

#ifndef HTMX_BUTTON_H
#define HTMX_BUTTON_H

#include <glib-object.h>
#include "htmx-node.h"
#include "../core/htmx-enums.h"

G_BEGIN_DECLS

#define HTMX_TYPE_BUTTON (htmx_button_get_type())
G_DECLARE_FINAL_TYPE(HtmxButton, htmx_button, HTMX, BUTTON, HtmxNode)

/**
 * htmx_button_new:
 *
 * Creates a new #HtmxButton element with type="button".
 *
 * Returns: (transfer full): a new #HtmxButton
 */
HtmxButton *
htmx_button_new(void);

/**
 * htmx_button_new_with_label:
 * @label: the button label text
 *
 * Creates a new #HtmxButton element with the specified label.
 *
 * Returns: (transfer full): a new #HtmxButton
 */
HtmxButton *
htmx_button_new_with_label(const gchar *label);

/**
 * htmx_button_new_submit:
 *
 * Creates a new submit button.
 *
 * Returns: (transfer full): a new #HtmxButton with type="submit"
 */
HtmxButton *
htmx_button_new_submit(void);

/**
 * htmx_button_new_reset:
 *
 * Creates a new reset button.
 *
 * Returns: (transfer full): a new #HtmxButton with type="reset"
 */
HtmxButton *
htmx_button_new_reset(void);

/**
 * htmx_button_set_button_type:
 * @self: an #HtmxButton
 * @type: the button type
 *
 * Sets the button type attribute.
 */
void
htmx_button_set_button_type(
	HtmxButton    *self,
	HtmxButtonType type
);

/**
 * htmx_button_set_disabled:
 * @self: an #HtmxButton
 * @disabled: whether the button is disabled
 *
 * Sets the disabled attribute.
 */
void
htmx_button_set_disabled(
	HtmxButton *self,
	gboolean    disabled
);

/**
 * htmx_button_set_name:
 * @self: an #HtmxButton
 * @name: the name attribute value
 *
 * Sets the name attribute.
 */
void
htmx_button_set_name(
	HtmxButton  *self,
	const gchar *name
);

/**
 * htmx_button_set_value:
 * @self: an #HtmxButton
 * @value: the value attribute
 *
 * Sets the value attribute.
 */
void
htmx_button_set_value(
	HtmxButton  *self,
	const gchar *value
);

G_END_DECLS

#endif /* HTMX_BUTTON_H */
