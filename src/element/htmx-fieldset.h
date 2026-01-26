/*
 * htmx-fieldset.h - HTML form grouping elements
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * This file defines form grouping elements: fieldset, legend, optgroup,
 * datalist, and output.
 */

#ifndef HTMX_FIELDSET_H
#define HTMX_FIELDSET_H

#include <glib-object.h>
#include "htmx-node.h"

G_BEGIN_DECLS

/*
 * HtmxFieldset - Fieldset element
 */

#define HTMX_TYPE_FIELDSET (htmx_fieldset_get_type())
G_DECLARE_FINAL_TYPE(HtmxFieldset, htmx_fieldset, HTMX, FIELDSET, HtmxNode)

/**
 * htmx_fieldset_new:
 *
 * Creates a new #HtmxFieldset element.
 *
 * Returns: (transfer full): a new #HtmxFieldset
 */
HtmxFieldset *
htmx_fieldset_new(void);

/**
 * htmx_fieldset_set_disabled:
 * @self: an #HtmxFieldset
 * @disabled: whether the fieldset is disabled
 *
 * Sets the disabled attribute.
 */
void
htmx_fieldset_set_disabled(
	HtmxFieldset *self,
	gboolean      disabled
);

/**
 * htmx_fieldset_set_form:
 * @self: an #HtmxFieldset
 * @form_id: the form element id
 *
 * Sets the form attribute.
 */
void
htmx_fieldset_set_form(
	HtmxFieldset *self,
	const gchar  *form_id
);

/**
 * htmx_fieldset_set_name:
 * @self: an #HtmxFieldset
 * @name: the name attribute
 *
 * Sets the name attribute.
 */
void
htmx_fieldset_set_name(
	HtmxFieldset *self,
	const gchar  *name
);

/*
 * HtmxLegend - Legend element
 */

#define HTMX_TYPE_LEGEND (htmx_legend_get_type())
G_DECLARE_FINAL_TYPE(HtmxLegend, htmx_legend, HTMX, LEGEND, HtmxNode)

/**
 * htmx_legend_new:
 *
 * Creates a new #HtmxLegend element.
 *
 * Returns: (transfer full): a new #HtmxLegend
 */
HtmxLegend *
htmx_legend_new(void);

/**
 * htmx_legend_new_with_text:
 * @text: the legend text
 *
 * Creates a new #HtmxLegend with text content.
 *
 * Returns: (transfer full): a new #HtmxLegend
 */
HtmxLegend *
htmx_legend_new_with_text(const gchar *text);

/*
 * HtmxOptgroup - Option group element
 */

#define HTMX_TYPE_OPTGROUP (htmx_optgroup_get_type())
G_DECLARE_FINAL_TYPE(HtmxOptgroup, htmx_optgroup, HTMX, OPTGROUP, HtmxNode)

/**
 * htmx_optgroup_new:
 *
 * Creates a new #HtmxOptgroup element.
 *
 * Returns: (transfer full): a new #HtmxOptgroup
 */
HtmxOptgroup *
htmx_optgroup_new(void);

/**
 * htmx_optgroup_new_with_label:
 * @label: the group label
 *
 * Creates a new #HtmxOptgroup with a label.
 *
 * Returns: (transfer full): a new #HtmxOptgroup
 */
HtmxOptgroup *
htmx_optgroup_new_with_label(const gchar *label);

/**
 * htmx_optgroup_set_label:
 * @self: an #HtmxOptgroup
 * @label: the group label
 *
 * Sets the label attribute.
 */
void
htmx_optgroup_set_label(
	HtmxOptgroup *self,
	const gchar  *label
);

/**
 * htmx_optgroup_set_disabled:
 * @self: an #HtmxOptgroup
 * @disabled: whether the group is disabled
 *
 * Sets the disabled attribute.
 */
void
htmx_optgroup_set_disabled(
	HtmxOptgroup *self,
	gboolean      disabled
);

/*
 * HtmxDatalist - Datalist element
 */

#define HTMX_TYPE_DATALIST (htmx_datalist_get_type())
G_DECLARE_FINAL_TYPE(HtmxDatalist, htmx_datalist, HTMX, DATALIST, HtmxNode)

/**
 * htmx_datalist_new:
 *
 * Creates a new #HtmxDatalist element.
 *
 * Returns: (transfer full): a new #HtmxDatalist
 */
HtmxDatalist *
htmx_datalist_new(void);

/**
 * htmx_datalist_new_with_id:
 * @id: the datalist id
 *
 * Creates a new #HtmxDatalist with an id attribute.
 *
 * Returns: (transfer full): a new #HtmxDatalist
 */
HtmxDatalist *
htmx_datalist_new_with_id(const gchar *id);

/*
 * HtmxOutput - Output element
 */

#define HTMX_TYPE_OUTPUT (htmx_output_get_type())
G_DECLARE_FINAL_TYPE(HtmxOutput, htmx_output, HTMX, OUTPUT, HtmxNode)

/**
 * htmx_output_new:
 *
 * Creates a new #HtmxOutput element.
 *
 * Returns: (transfer full): a new #HtmxOutput
 */
HtmxOutput *
htmx_output_new(void);

/**
 * htmx_output_set_for:
 * @self: an #HtmxOutput
 * @for_ids: space-separated list of element ids
 *
 * Sets the for attribute.
 */
void
htmx_output_set_for(
	HtmxOutput  *self,
	const gchar *for_ids
);

/**
 * htmx_output_set_form:
 * @self: an #HtmxOutput
 * @form_id: the form element id
 *
 * Sets the form attribute.
 */
void
htmx_output_set_form(
	HtmxOutput  *self,
	const gchar *form_id
);

/**
 * htmx_output_set_name:
 * @self: an #HtmxOutput
 * @name: the name attribute
 *
 * Sets the name attribute.
 */
void
htmx_output_set_name(
	HtmxOutput  *self,
	const gchar *name
);

G_END_DECLS

#endif /* HTMX_FIELDSET_H */
