/*
 * htmx-form.h - HTML form element
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef HTMX_FORM_H
#define HTMX_FORM_H

#include <glib-object.h>
#include "htmx-node.h"
#include "core/htmx-enums.h"

G_BEGIN_DECLS

#define HTMX_TYPE_FORM (htmx_form_get_type())
G_DECLARE_FINAL_TYPE(HtmxForm, htmx_form, HTMX, FORM, HtmxNode)

HtmxForm *htmx_form_new(void);
HtmxForm *htmx_form_new_with_action(const gchar *action, HtmxMethod method);

/*
 * HtmxProgress - Progress indicator element
 */

#define HTMX_TYPE_PROGRESS (htmx_progress_get_type())
G_DECLARE_FINAL_TYPE(HtmxProgress, htmx_progress, HTMX, PROGRESS, HtmxNode)

/**
 * htmx_progress_new:
 *
 * Creates a new #HtmxProgress element.
 *
 * Returns: (transfer full): a new #HtmxProgress
 */
HtmxProgress *
htmx_progress_new(void);

/**
 * htmx_progress_new_with_value:
 * @value: the current progress value
 * @max: the maximum value
 *
 * Creates a new #HtmxProgress with value and max.
 *
 * Returns: (transfer full): a new #HtmxProgress
 */
HtmxProgress *
htmx_progress_new_with_value(
	gdouble value,
	gdouble max
);

/**
 * htmx_progress_set_value:
 * @self: an #HtmxProgress
 * @value: the current progress value
 *
 * Sets the value attribute indicating current progress.
 */
void
htmx_progress_set_value(
	HtmxProgress *self,
	gdouble       value
);

/**
 * htmx_progress_set_max:
 * @self: an #HtmxProgress
 * @max: the maximum value
 *
 * Sets the max attribute.
 */
void
htmx_progress_set_max(
	HtmxProgress *self,
	gdouble       max
);

/*
 * HtmxMeter - Meter/gauge element
 */

#define HTMX_TYPE_METER (htmx_meter_get_type())
G_DECLARE_FINAL_TYPE(HtmxMeter, htmx_meter, HTMX, METER, HtmxNode)

/**
 * htmx_meter_new:
 *
 * Creates a new #HtmxMeter element.
 *
 * Returns: (transfer full): a new #HtmxMeter
 */
HtmxMeter *
htmx_meter_new(void);

/**
 * htmx_meter_set_value:
 * @self: an #HtmxMeter
 * @value: the current value
 *
 * Sets the value attribute.
 */
void
htmx_meter_set_value(
	HtmxMeter *self,
	gdouble    value
);

/**
 * htmx_meter_set_min:
 * @self: an #HtmxMeter
 * @min: the minimum value
 *
 * Sets the min attribute.
 */
void
htmx_meter_set_min(
	HtmxMeter *self,
	gdouble    min
);

/**
 * htmx_meter_set_max:
 * @self: an #HtmxMeter
 * @max: the maximum value
 *
 * Sets the max attribute.
 */
void
htmx_meter_set_max(
	HtmxMeter *self,
	gdouble    max
);

/**
 * htmx_meter_set_low:
 * @self: an #HtmxMeter
 * @low: the low threshold
 *
 * Sets the low attribute.
 */
void
htmx_meter_set_low(
	HtmxMeter *self,
	gdouble    low
);

/**
 * htmx_meter_set_high:
 * @self: an #HtmxMeter
 * @high: the high threshold
 *
 * Sets the high attribute.
 */
void
htmx_meter_set_high(
	HtmxMeter *self,
	gdouble    high
);

/**
 * htmx_meter_set_optimum:
 * @self: an #HtmxMeter
 * @optimum: the optimum value
 *
 * Sets the optimum attribute.
 */
void
htmx_meter_set_optimum(
	HtmxMeter *self,
	gdouble    optimum
);

G_END_DECLS

#endif /* HTMX_FORM_H */
