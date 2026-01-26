/*
 * htmx-trigger.h - HTMX trigger event boxed type
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * This file defines the HtmxTrigger boxed type which represents an HTMX
 * trigger event with modifiers.
 */

#ifndef HTMX_TRIGGER_H
#define HTMX_TRIGGER_H

#include <glib-object.h>
#include "core/htmx-enums.h"

G_BEGIN_DECLS

#define HTMX_TYPE_TRIGGER (htmx_trigger_get_type())

typedef struct _HtmxTrigger HtmxTrigger;

/**
 * HtmxTrigger:
 *
 * A boxed type representing an HTMX trigger event with modifiers.
 *
 * Triggers define when an HTMX request should be sent. They consist of an
 * event name (e.g., "click", "load", "revealed") and optional modifiers
 * (e.g., "once", "delay:500ms", "throttle:1s").
 */
struct _HtmxTrigger {
	gchar              *event_name;    /* Event name (e.g., "click", "load") */
	HtmxTriggerModifier modifiers;     /* Bit flags for modifiers */
	guint               delay_ms;      /* Delay in milliseconds (if HTMX_TRIGGER_DELAY) */
	guint               throttle_ms;   /* Throttle in milliseconds (if HTMX_TRIGGER_THROTTLE) */
	gchar              *from_selector; /* From selector (if HTMX_TRIGGER_FROM) */
	gchar              *target_selector; /* Target selector (if HTMX_TRIGGER_TARGET) */
	HtmxQueueOption     queue;         /* Queue option (if HTMX_TRIGGER_QUEUE) */
	gchar              *filter;        /* JavaScript filter expression */
};

GType htmx_trigger_get_type(void) G_GNUC_CONST;

/**
 * htmx_trigger_new:
 * @event_name: the trigger event name
 *
 * Creates a new #HtmxTrigger with the given event name and no modifiers.
 *
 * Returns: (transfer full): a new #HtmxTrigger. Free with htmx_trigger_free().
 */
HtmxTrigger *
htmx_trigger_new(const gchar *event_name);

/**
 * htmx_trigger_new_full:
 * @event_name: the trigger event name
 * @modifiers: modifier flags
 * @delay_ms: delay in milliseconds (used if HTMX_TRIGGER_DELAY is set)
 * @throttle_ms: throttle in milliseconds (used if HTMX_TRIGGER_THROTTLE is set)
 * @from_selector: (nullable): from selector (used if HTMX_TRIGGER_FROM is set)
 * @queue: queue option (used if HTMX_TRIGGER_QUEUE is set)
 *
 * Creates a new #HtmxTrigger with full configuration.
 *
 * Returns: (transfer full): a new #HtmxTrigger. Free with htmx_trigger_free().
 */
HtmxTrigger *
htmx_trigger_new_full(
	const gchar        *event_name,
	HtmxTriggerModifier modifiers,
	guint               delay_ms,
	guint               throttle_ms,
	const gchar        *from_selector,
	HtmxQueueOption     queue
);

/**
 * htmx_trigger_copy:
 * @self: (nullable): an #HtmxTrigger
 *
 * Creates a copy of an #HtmxTrigger.
 *
 * Returns: (transfer full) (nullable): a copy of @self, or %NULL if @self is %NULL.
 *          Free with htmx_trigger_free().
 */
HtmxTrigger *
htmx_trigger_copy(const HtmxTrigger *self);

/**
 * htmx_trigger_free:
 * @self: (nullable): an #HtmxTrigger
 *
 * Frees an #HtmxTrigger and its contents.
 */
void
htmx_trigger_free(HtmxTrigger *self);

/**
 * htmx_trigger_get_event_name:
 * @self: an #HtmxTrigger
 *
 * Gets the event name.
 *
 * Returns: (transfer none): the event name
 */
const gchar *
htmx_trigger_get_event_name(const HtmxTrigger *self);

/**
 * htmx_trigger_get_modifiers:
 * @self: an #HtmxTrigger
 *
 * Gets the modifier flags.
 *
 * Returns: the modifier flags
 */
HtmxTriggerModifier
htmx_trigger_get_modifiers(const HtmxTrigger *self);

/**
 * htmx_trigger_set_once:
 * @self: an #HtmxTrigger
 *
 * Sets the trigger to fire only once.
 */
void
htmx_trigger_set_once(HtmxTrigger *self);

/**
 * htmx_trigger_set_changed:
 * @self: an #HtmxTrigger
 *
 * Sets the trigger to fire only when the value has changed.
 */
void
htmx_trigger_set_changed(HtmxTrigger *self);

/**
 * htmx_trigger_set_delay:
 * @self: an #HtmxTrigger
 * @delay_ms: delay in milliseconds
 *
 * Sets a delay before the request is sent.
 */
void
htmx_trigger_set_delay(
	HtmxTrigger *self,
	guint        delay_ms
);

/**
 * htmx_trigger_set_throttle:
 * @self: an #HtmxTrigger
 * @throttle_ms: throttle in milliseconds
 *
 * Sets a throttle rate for the trigger.
 */
void
htmx_trigger_set_throttle(
	HtmxTrigger *self,
	guint        throttle_ms
);

/**
 * htmx_trigger_set_from:
 * @self: an #HtmxTrigger
 * @selector: the CSS selector to listen from
 *
 * Sets a different element to listen for events from.
 */
void
htmx_trigger_set_from(
	HtmxTrigger *self,
	const gchar *selector
);

/**
 * htmx_trigger_set_target:
 * @self: an #HtmxTrigger
 * @selector: the CSS selector for the target
 *
 * Sets a different target for the trigger.
 */
void
htmx_trigger_set_target(
	HtmxTrigger *self,
	const gchar *selector
);

/**
 * htmx_trigger_set_consume:
 * @self: an #HtmxTrigger
 *
 * Sets the trigger to consume the event (stopPropagation).
 */
void
htmx_trigger_set_consume(HtmxTrigger *self);

/**
 * htmx_trigger_set_queue:
 * @self: an #HtmxTrigger
 * @queue: the queue option
 *
 * Sets the queue behavior for the trigger.
 */
void
htmx_trigger_set_queue(
	HtmxTrigger    *self,
	HtmxQueueOption queue
);

/**
 * htmx_trigger_set_filter:
 * @self: an #HtmxTrigger
 * @filter: JavaScript filter expression
 *
 * Sets a JavaScript filter expression for the trigger.
 */
void
htmx_trigger_set_filter(
	HtmxTrigger *self,
	const gchar *filter
);

/**
 * htmx_trigger_render:
 * @self: an #HtmxTrigger
 *
 * Renders the trigger as an HTMX trigger string.
 *
 * Returns: (transfer full): the rendered trigger string. Free with g_free().
 */
gchar *
htmx_trigger_render(const HtmxTrigger *self);

/**
 * htmx_trigger_parse:
 * @str: a trigger string to parse
 * @error: (nullable): return location for an error
 *
 * Parses an HTMX trigger string into an #HtmxTrigger.
 *
 * Returns: (transfer full) (nullable): a new #HtmxTrigger, or %NULL on error.
 *          Free with htmx_trigger_free().
 */
HtmxTrigger *
htmx_trigger_parse(
	const gchar *str,
	GError     **error
);

G_DEFINE_AUTOPTR_CLEANUP_FUNC(HtmxTrigger, htmx_trigger_free)

G_END_DECLS

#endif /* HTMX_TRIGGER_H */
