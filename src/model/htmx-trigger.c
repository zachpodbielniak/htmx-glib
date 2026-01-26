/*
 * htmx-trigger.c - HTMX trigger event boxed type implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-trigger.h"
#include "core/htmx-error.h"
#include <string.h>
#include <stdlib.h>

/*
 * GType registration for HtmxTrigger
 */
G_DEFINE_BOXED_TYPE(HtmxTrigger, htmx_trigger,
                    htmx_trigger_copy, htmx_trigger_free)

/*
 * Create a new trigger with just the event name
 */
HtmxTrigger *
htmx_trigger_new(const gchar *event_name)
{
	HtmxTrigger *self;

	g_return_val_if_fail(event_name != NULL, NULL);

	self = g_slice_new0(HtmxTrigger);
	self->event_name = g_strdup(event_name);
	self->modifiers = HTMX_TRIGGER_NONE;
	self->delay_ms = 0;
	self->throttle_ms = 0;
	self->from_selector = NULL;
	self->target_selector = NULL;
	self->queue = HTMX_QUEUE_LAST;
	self->filter = NULL;

	return self;
}

/*
 * Create a new trigger with full configuration
 */
HtmxTrigger *
htmx_trigger_new_full(
	const gchar        *event_name,
	HtmxTriggerModifier modifiers,
	guint               delay_ms,
	guint               throttle_ms,
	const gchar        *from_selector,
	HtmxQueueOption     queue
){
	HtmxTrigger *self;

	g_return_val_if_fail(event_name != NULL, NULL);

	self = g_slice_new0(HtmxTrigger);
	self->event_name = g_strdup(event_name);
	self->modifiers = modifiers;
	self->delay_ms = delay_ms;
	self->throttle_ms = throttle_ms;
	self->from_selector = g_strdup(from_selector);
	self->target_selector = NULL;
	self->queue = queue;
	self->filter = NULL;

	return self;
}

/*
 * Copy a trigger
 */
HtmxTrigger *
htmx_trigger_copy(const HtmxTrigger *self)
{
	HtmxTrigger *copy;

	if (self == NULL) {
		return NULL;
	}

	copy = g_slice_new0(HtmxTrigger);
	copy->event_name = g_strdup(self->event_name);
	copy->modifiers = self->modifiers;
	copy->delay_ms = self->delay_ms;
	copy->throttle_ms = self->throttle_ms;
	copy->from_selector = g_strdup(self->from_selector);
	copy->target_selector = g_strdup(self->target_selector);
	copy->queue = self->queue;
	copy->filter = g_strdup(self->filter);

	return copy;
}

/*
 * Free a trigger
 */
void
htmx_trigger_free(HtmxTrigger *self)
{
	if (self == NULL) {
		return;
	}

	g_free(self->event_name);
	g_free(self->from_selector);
	g_free(self->target_selector);
	g_free(self->filter);
	g_slice_free(HtmxTrigger, self);
}

/*
 * Get the event name
 */
const gchar *
htmx_trigger_get_event_name(const HtmxTrigger *self)
{
	g_return_val_if_fail(self != NULL, NULL);

	return self->event_name;
}

/*
 * Get the modifier flags
 */
HtmxTriggerModifier
htmx_trigger_get_modifiers(const HtmxTrigger *self)
{
	g_return_val_if_fail(self != NULL, HTMX_TRIGGER_NONE);

	return self->modifiers;
}

/*
 * Set the once modifier
 */
void
htmx_trigger_set_once(HtmxTrigger *self)
{
	g_return_if_fail(self != NULL);

	self->modifiers |= HTMX_TRIGGER_ONCE;
}

/*
 * Set the changed modifier
 */
void
htmx_trigger_set_changed(HtmxTrigger *self)
{
	g_return_if_fail(self != NULL);

	self->modifiers |= HTMX_TRIGGER_CHANGED;
}

/*
 * Set the delay modifier
 */
void
htmx_trigger_set_delay(
	HtmxTrigger *self,
	guint        delay_ms
){
	g_return_if_fail(self != NULL);

	self->modifiers |= HTMX_TRIGGER_DELAY;
	self->delay_ms = delay_ms;
}

/*
 * Set the throttle modifier
 */
void
htmx_trigger_set_throttle(
	HtmxTrigger *self,
	guint        throttle_ms
){
	g_return_if_fail(self != NULL);

	self->modifiers |= HTMX_TRIGGER_THROTTLE;
	self->throttle_ms = throttle_ms;
}

/*
 * Set the from modifier
 */
void
htmx_trigger_set_from(
	HtmxTrigger *self,
	const gchar *selector
){
	g_return_if_fail(self != NULL);
	g_return_if_fail(selector != NULL);

	self->modifiers |= HTMX_TRIGGER_FROM;
	g_free(self->from_selector);
	self->from_selector = g_strdup(selector);
}

/*
 * Set the target modifier
 */
void
htmx_trigger_set_target(
	HtmxTrigger *self,
	const gchar *selector
){
	g_return_if_fail(self != NULL);
	g_return_if_fail(selector != NULL);

	self->modifiers |= HTMX_TRIGGER_TARGET;
	g_free(self->target_selector);
	self->target_selector = g_strdup(selector);
}

/*
 * Set the consume modifier
 */
void
htmx_trigger_set_consume(HtmxTrigger *self)
{
	g_return_if_fail(self != NULL);

	self->modifiers |= HTMX_TRIGGER_CONSUME;
}

/*
 * Set the queue modifier
 */
void
htmx_trigger_set_queue(
	HtmxTrigger    *self,
	HtmxQueueOption queue
){
	g_return_if_fail(self != NULL);

	self->modifiers |= HTMX_TRIGGER_QUEUE;
	self->queue = queue;
}

/*
 * Set the filter expression
 */
void
htmx_trigger_set_filter(
	HtmxTrigger *self,
	const gchar *filter
){
	g_return_if_fail(self != NULL);

	g_free(self->filter);
	self->filter = g_strdup(filter);
}

/*
 * Helper to format milliseconds as HTMX duration string
 */
static gchar *
format_duration(guint ms)
{
	if (ms >= 1000 && ms % 1000 == 0) {
		return g_strdup_printf("%us", ms / 1000);
	}
	return g_strdup_printf("%ums", ms);
}

/*
 * Render the trigger as an HTMX trigger string
 */
gchar *
htmx_trigger_render(const HtmxTrigger *self)
{
	GString *result;

	g_return_val_if_fail(self != NULL, NULL);

	result = g_string_new(self->event_name);

	/* Add filter if present */
	if (self->filter != NULL) {
		g_string_append_printf(result, "[%s]", self->filter);
	}

	/* Add modifiers */
	if (self->modifiers & HTMX_TRIGGER_ONCE) {
		g_string_append(result, " once");
	}

	if (self->modifiers & HTMX_TRIGGER_CHANGED) {
		g_string_append(result, " changed");
	}

	if (self->modifiers & HTMX_TRIGGER_DELAY) {
		g_autofree gchar *duration = format_duration(self->delay_ms);
		g_string_append_printf(result, " delay:%s", duration);
	}

	if (self->modifiers & HTMX_TRIGGER_THROTTLE) {
		g_autofree gchar *duration = format_duration(self->throttle_ms);
		g_string_append_printf(result, " throttle:%s", duration);
	}

	if ((self->modifiers & HTMX_TRIGGER_FROM) && self->from_selector != NULL) {
		g_string_append_printf(result, " from:%s", self->from_selector);
	}

	if ((self->modifiers & HTMX_TRIGGER_TARGET) && self->target_selector != NULL) {
		g_string_append_printf(result, " target:%s", self->target_selector);
	}

	if (self->modifiers & HTMX_TRIGGER_CONSUME) {
		g_string_append(result, " consume");
	}

	if (self->modifiers & HTMX_TRIGGER_QUEUE) {
		const gchar *queue_str;
		switch (self->queue) {
		case HTMX_QUEUE_FIRST:
			queue_str = "first";
			break;
		case HTMX_QUEUE_LAST:
			queue_str = "last";
			break;
		case HTMX_QUEUE_ALL:
			queue_str = "all";
			break;
		case HTMX_QUEUE_NONE:
			queue_str = "none";
			break;
		default:
			queue_str = "last";
			break;
		}
		g_string_append_printf(result, " queue:%s", queue_str);
	}

	return g_string_free(result, FALSE);
}

/*
 * Parse a duration string (e.g., "500ms", "1s") to milliseconds
 */
static gboolean
parse_duration(const gchar *str, guint *ms_out)
{
	gchar *endptr;
	guint value;

	value = (guint)strtoul(str, &endptr, 10);

	if (endptr == str) {
		return FALSE;
	}

	if (g_str_equal(endptr, "s")) {
		*ms_out = value * 1000;
	} else if (g_str_equal(endptr, "ms") || *endptr == '\0') {
		*ms_out = value;
	} else {
		return FALSE;
	}

	return TRUE;
}

/*
 * Parse an HTMX trigger string
 */
HtmxTrigger *
htmx_trigger_parse(
	const gchar *str,
	GError     **error
){
	g_autoptr(HtmxTrigger) self = NULL;
	g_auto(GStrv) parts = NULL;
	const gchar *event_name;
	gint i;

	g_return_val_if_fail(str != NULL, NULL);

	/* Split by whitespace */
	parts = g_strsplit(str, " ", -1);
	if (parts == NULL || parts[0] == NULL || parts[0][0] == '\0') {
		g_set_error(error, HTMX_ERROR, HTMX_ERROR_INVALID_TRIGGER,
		            "Empty trigger string");
		return NULL;
	}

	/* First part is the event name (possibly with filter) */
	event_name = parts[0];

	/* Check for filter expression */
	if (strchr(event_name, '[') != NULL) {
		g_autofree gchar *name = NULL;
		g_autofree gchar *filter = NULL;
		gchar *bracket;

		name = g_strdup(event_name);
		bracket = strchr(name, '[');
		*bracket = '\0';
		filter = g_strdup(bracket + 1);

		/* Remove trailing ] */
		if (filter[strlen(filter) - 1] == ']') {
			filter[strlen(filter) - 1] = '\0';
		}

		self = htmx_trigger_new(name);
		htmx_trigger_set_filter(self, filter);
	} else {
		self = htmx_trigger_new(event_name);
	}

	/* Parse modifiers */
	for (i = 1; parts[i] != NULL; i++) {
		const gchar *part = parts[i];

		if (part[0] == '\0') {
			continue;
		}

		if (g_str_equal(part, "once")) {
			htmx_trigger_set_once(self);
		} else if (g_str_equal(part, "changed")) {
			htmx_trigger_set_changed(self);
		} else if (g_str_equal(part, "consume")) {
			htmx_trigger_set_consume(self);
		} else if (g_str_has_prefix(part, "delay:")) {
			guint ms;
			if (!parse_duration(part + 6, &ms)) {
				g_set_error(error, HTMX_ERROR, HTMX_ERROR_INVALID_TRIGGER,
				            "Invalid delay value: %s", part + 6);
				return NULL;
			}
			htmx_trigger_set_delay(self, ms);
		} else if (g_str_has_prefix(part, "throttle:")) {
			guint ms;
			if (!parse_duration(part + 9, &ms)) {
				g_set_error(error, HTMX_ERROR, HTMX_ERROR_INVALID_TRIGGER,
				            "Invalid throttle value: %s", part + 9);
				return NULL;
			}
			htmx_trigger_set_throttle(self, ms);
		} else if (g_str_has_prefix(part, "from:")) {
			htmx_trigger_set_from(self, part + 5);
		} else if (g_str_has_prefix(part, "target:")) {
			htmx_trigger_set_target(self, part + 7);
		} else if (g_str_has_prefix(part, "queue:")) {
			const gchar *queue_str = part + 6;
			HtmxQueueOption queue;

			if (g_str_equal(queue_str, "first")) {
				queue = HTMX_QUEUE_FIRST;
			} else if (g_str_equal(queue_str, "last")) {
				queue = HTMX_QUEUE_LAST;
			} else if (g_str_equal(queue_str, "all")) {
				queue = HTMX_QUEUE_ALL;
			} else if (g_str_equal(queue_str, "none")) {
				queue = HTMX_QUEUE_NONE;
			} else {
				g_set_error(error, HTMX_ERROR, HTMX_ERROR_INVALID_TRIGGER,
				            "Invalid queue value: %s", queue_str);
				return NULL;
			}
			htmx_trigger_set_queue(self, queue);
		}
	}

	return (HtmxTrigger *)g_steal_pointer(&self);
}
