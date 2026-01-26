/*
 * htmx-select.h - HTML select and option elements
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * This file defines the HtmxSelect and HtmxOption classes for HTML select dropdowns.
 */

#ifndef HTMX_SELECT_H
#define HTMX_SELECT_H

#include <glib-object.h>
#include "htmx-node.h"

G_BEGIN_DECLS

/*
 * HtmxSelect - Select dropdown
 */

#define HTMX_TYPE_SELECT (htmx_select_get_type())
G_DECLARE_FINAL_TYPE(HtmxSelect, htmx_select, HTMX, SELECT, HtmxNode)

/**
 * htmx_select_new:
 *
 * Creates a new #HtmxSelect element.
 *
 * Returns: (transfer full): a new #HtmxSelect
 */
HtmxSelect *
htmx_select_new(void);

/**
 * htmx_select_new_with_name:
 * @name: the name attribute value
 *
 * Creates a new #HtmxSelect element with the specified name.
 *
 * Returns: (transfer full): a new #HtmxSelect
 */
HtmxSelect *
htmx_select_new_with_name(const gchar *name);

/**
 * htmx_select_set_name:
 * @self: an #HtmxSelect
 * @name: the name attribute value
 *
 * Sets the name attribute.
 */
void
htmx_select_set_name(
	HtmxSelect  *self,
	const gchar *name
);

/**
 * htmx_select_set_multiple:
 * @self: an #HtmxSelect
 * @multiple: whether multiple selection is allowed
 *
 * Sets the multiple attribute.
 */
void
htmx_select_set_multiple(
	HtmxSelect *self,
	gboolean    multiple
);

/**
 * htmx_select_set_required:
 * @self: an #HtmxSelect
 * @required: whether selection is required
 *
 * Sets the required attribute.
 */
void
htmx_select_set_required(
	HtmxSelect *self,
	gboolean    required
);

/**
 * htmx_select_set_disabled:
 * @self: an #HtmxSelect
 * @disabled: whether the select is disabled
 *
 * Sets the disabled attribute.
 */
void
htmx_select_set_disabled(
	HtmxSelect *self,
	gboolean    disabled
);

/**
 * htmx_select_set_size:
 * @self: an #HtmxSelect
 * @size: the number of visible options
 *
 * Sets the size attribute (number of visible options).
 */
void
htmx_select_set_size(
	HtmxSelect *self,
	guint       size
);

/*
 * HtmxOption - Option element
 */

#define HTMX_TYPE_OPTION (htmx_option_get_type())
G_DECLARE_FINAL_TYPE(HtmxOption, htmx_option, HTMX, OPTION, HtmxNode)

/**
 * htmx_option_new:
 *
 * Creates a new #HtmxOption element.
 *
 * Returns: (transfer full): a new #HtmxOption
 */
HtmxOption *
htmx_option_new(void);

/**
 * htmx_option_new_with_value:
 * @value: the option value
 * @label: the option display text
 *
 * Creates a new #HtmxOption element with value and label.
 *
 * Returns: (transfer full): a new #HtmxOption
 */
HtmxOption *
htmx_option_new_with_value(
	const gchar *value,
	const gchar *label
);

/**
 * htmx_option_set_value:
 * @self: an #HtmxOption
 * @value: the option value
 *
 * Sets the value attribute.
 */
void
htmx_option_set_value(
	HtmxOption  *self,
	const gchar *value
);

/**
 * htmx_option_set_selected:
 * @self: an #HtmxOption
 * @selected: whether this option is selected
 *
 * Sets the selected attribute.
 */
void
htmx_option_set_selected(
	HtmxOption *self,
	gboolean    selected
);

/**
 * htmx_option_set_disabled:
 * @self: an #HtmxOption
 * @disabled: whether this option is disabled
 *
 * Sets the disabled attribute.
 */
void
htmx_option_set_disabled(
	HtmxOption *self,
	gboolean    disabled
);

G_END_DECLS

#endif /* HTMX_SELECT_H */
