/*
 * htmx-swap.c - HTMX swap configuration boxed type implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-swap.h"
#include "core/htmx-error.h"
#include <string.h>
#include <stdlib.h>

/*
 * GType registration for HtmxSwap
 */
G_DEFINE_BOXED_TYPE(HtmxSwap, htmx_swap,
                    htmx_swap_copy, htmx_swap_free)

/*
 * Create a new swap configuration with just the style
 */
HtmxSwap *
htmx_swap_new(HtmxSwapStyle style)
{
	HtmxSwap *self;

	self = g_slice_new0(HtmxSwap);
	self->style = style;
	self->modifiers = HTMX_SWAP_MOD_NONE;
	self->swap_ms = 0;
	self->settle_ms = 0;
	self->scroll_behavior = HTMX_SCROLL_SMOOTH;
	self->scroll_direction = HTMX_SCROLL_TOP;
	self->scroll_target = NULL;
	self->show_target = NULL;
	self->show_direction = HTMX_SCROLL_TOP;
	self->focus_scroll = FALSE;
	self->transition = FALSE;
	self->ignore_title = FALSE;

	return self;
}

/*
 * Copy a swap configuration
 */
HtmxSwap *
htmx_swap_copy(const HtmxSwap *self)
{
	HtmxSwap *copy;

	if (self == NULL) {
		return NULL;
	}

	copy = g_slice_new0(HtmxSwap);
	copy->style = self->style;
	copy->modifiers = self->modifiers;
	copy->swap_ms = self->swap_ms;
	copy->settle_ms = self->settle_ms;
	copy->scroll_behavior = self->scroll_behavior;
	copy->scroll_direction = self->scroll_direction;
	copy->scroll_target = g_strdup(self->scroll_target);
	copy->show_target = g_strdup(self->show_target);
	copy->show_direction = self->show_direction;
	copy->focus_scroll = self->focus_scroll;
	copy->transition = self->transition;
	copy->ignore_title = self->ignore_title;

	return copy;
}

/*
 * Free a swap configuration
 */
void
htmx_swap_free(HtmxSwap *self)
{
	if (self == NULL) {
		return;
	}

	g_free(self->scroll_target);
	g_free(self->show_target);
	g_slice_free(HtmxSwap, self);
}

/*
 * Get the swap style
 */
HtmxSwapStyle
htmx_swap_get_style(const HtmxSwap *self)
{
	g_return_val_if_fail(self != NULL, HTMX_SWAP_INNER_HTML);

	return self->style;
}

/*
 * Set the swap style
 */
void
htmx_swap_set_style(
	HtmxSwap     *self,
	HtmxSwapStyle style
){
	g_return_if_fail(self != NULL);

	self->style = style;
}

/*
 * Set swap timing
 */
void
htmx_swap_set_swap_time(
	HtmxSwap *self,
	guint     ms
){
	g_return_if_fail(self != NULL);

	self->modifiers |= HTMX_SWAP_MOD_SWAP;
	self->swap_ms = ms;
}

/*
 * Set settle timing
 */
void
htmx_swap_set_settle_time(
	HtmxSwap *self,
	guint     ms
){
	g_return_if_fail(self != NULL);

	self->modifiers |= HTMX_SWAP_MOD_SETTLE;
	self->settle_ms = ms;
}

/*
 * Set scroll behavior
 */
void
htmx_swap_set_scroll(
	HtmxSwap           *self,
	HtmxScrollDirection direction,
	const gchar        *target,
	HtmxScrollBehavior  behavior
){
	g_return_if_fail(self != NULL);

	self->modifiers |= HTMX_SWAP_MOD_SCROLL;
	self->scroll_direction = direction;
	self->scroll_behavior = behavior;

	g_free(self->scroll_target);
	self->scroll_target = g_strdup(target);
}

/*
 * Set show behavior
 */
void
htmx_swap_set_show(
	HtmxSwap           *self,
	HtmxScrollDirection direction,
	const gchar        *target
){
	g_return_if_fail(self != NULL);

	self->modifiers |= HTMX_SWAP_MOD_SHOW;
	self->show_direction = direction;

	g_free(self->show_target);
	self->show_target = g_strdup(target);
}

/*
 * Set focus scroll
 */
void
htmx_swap_set_focus_scroll(
	HtmxSwap *self,
	gboolean  focus_scroll
){
	g_return_if_fail(self != NULL);

	if (focus_scroll) {
		self->modifiers |= HTMX_SWAP_MOD_FOCUS_SCROLL;
	} else {
		self->modifiers &= ~HTMX_SWAP_MOD_FOCUS_SCROLL;
	}
	self->focus_scroll = focus_scroll;
}

/*
 * Set view transitions
 */
void
htmx_swap_set_transition(
	HtmxSwap *self,
	gboolean  transition
){
	g_return_if_fail(self != NULL);

	if (transition) {
		self->modifiers |= HTMX_SWAP_MOD_TRANSITION;
	} else {
		self->modifiers &= ~HTMX_SWAP_MOD_TRANSITION;
	}
	self->transition = transition;
}

/*
 * Set ignore title
 */
void
htmx_swap_set_ignore_title(
	HtmxSwap *self,
	gboolean  ignore_title
){
	g_return_if_fail(self != NULL);

	if (ignore_title) {
		self->modifiers |= HTMX_SWAP_MOD_IGNORE_TITLE;
	} else {
		self->modifiers &= ~HTMX_SWAP_MOD_IGNORE_TITLE;
	}
	self->ignore_title = ignore_title;
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
 * Render the swap configuration as an HTMX swap string
 */
gchar *
htmx_swap_render(const HtmxSwap *self)
{
	GString *result;
	const gchar *style_str;

	g_return_val_if_fail(self != NULL, NULL);

	style_str = htmx_swap_style_to_string(self->style);
	result = g_string_new(style_str);

	/* Add swap timing */
	if (self->modifiers & HTMX_SWAP_MOD_SWAP) {
		g_autofree gchar *duration = format_duration(self->swap_ms);
		g_string_append_printf(result, " swap:%s", duration);
	}

	/* Add settle timing */
	if (self->modifiers & HTMX_SWAP_MOD_SETTLE) {
		g_autofree gchar *duration = format_duration(self->settle_ms);
		g_string_append_printf(result, " settle:%s", duration);
	}

	/* Add scroll behavior */
	if (self->modifiers & HTMX_SWAP_MOD_SCROLL) {
		const gchar *direction = (self->scroll_direction == HTMX_SCROLL_TOP) ? "top" : "bottom";

		if (self->scroll_target != NULL) {
			g_string_append_printf(result, " scroll:%s:%s", self->scroll_target, direction);
		} else {
			g_string_append_printf(result, " scroll:%s", direction);
		}
	}

	/* Add show behavior */
	if (self->modifiers & HTMX_SWAP_MOD_SHOW) {
		const gchar *direction = (self->show_direction == HTMX_SCROLL_TOP) ? "top" : "bottom";

		if (self->show_target != NULL) {
			g_string_append_printf(result, " show:%s:%s", self->show_target, direction);
		} else {
			g_string_append_printf(result, " show:%s", direction);
		}
	}

	/* Add focus scroll */
	if (self->modifiers & HTMX_SWAP_MOD_FOCUS_SCROLL) {
		g_string_append_printf(result, " focus-scroll:%s",
		                       self->focus_scroll ? "true" : "false");
	}

	/* Add transition */
	if (self->modifiers & HTMX_SWAP_MOD_TRANSITION) {
		g_string_append_printf(result, " transition:%s",
		                       self->transition ? "true" : "false");
	}

	/* Add ignore title */
	if (self->modifiers & HTMX_SWAP_MOD_IGNORE_TITLE) {
		g_string_append(result, " ignoreTitle:true");
	}

	return g_string_free(result, FALSE);
}

/*
 * Parse a duration string to milliseconds
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
 * Parse an HTMX swap string
 */
HtmxSwap *
htmx_swap_parse(
	const gchar *str,
	GError     **error
){
	g_autoptr(HtmxSwap) self = NULL;
	g_auto(GStrv) parts = NULL;
	HtmxSwapStyle style;
	gint i;

	g_return_val_if_fail(str != NULL, NULL);

	/* Split by whitespace */
	parts = g_strsplit(str, " ", -1);
	if (parts == NULL || parts[0] == NULL || parts[0][0] == '\0') {
		g_set_error(error, HTMX_ERROR, HTMX_ERROR_INVALID_SWAP,
		            "Empty swap string");
		return NULL;
	}

	/* Parse the swap style */
	style = htmx_swap_style_from_string(parts[0]);
	if ((gint)style == -1) {
		g_set_error(error, HTMX_ERROR, HTMX_ERROR_INVALID_SWAP,
		            "Invalid swap style: %s", parts[0]);
		return NULL;
	}

	self = htmx_swap_new(style);

	/* Parse modifiers */
	for (i = 1; parts[i] != NULL; i++) {
		const gchar *part = parts[i];

		if (part[0] == '\0') {
			continue;
		}

		if (g_str_has_prefix(part, "swap:")) {
			guint ms;
			if (!parse_duration(part + 5, &ms)) {
				g_set_error(error, HTMX_ERROR, HTMX_ERROR_INVALID_SWAP,
				            "Invalid swap time: %s", part + 5);
				return NULL;
			}
			htmx_swap_set_swap_time(self, ms);
		} else if (g_str_has_prefix(part, "settle:")) {
			guint ms;
			if (!parse_duration(part + 7, &ms)) {
				g_set_error(error, HTMX_ERROR, HTMX_ERROR_INVALID_SWAP,
				            "Invalid settle time: %s", part + 7);
				return NULL;
			}
			htmx_swap_set_settle_time(self, ms);
		} else if (g_str_has_prefix(part, "scroll:")) {
			const gchar *value = part + 7;
			HtmxScrollDirection direction = HTMX_SCROLL_TOP;

			if (g_str_equal(value, "top")) {
				direction = HTMX_SCROLL_TOP;
			} else if (g_str_equal(value, "bottom")) {
				direction = HTMX_SCROLL_BOTTOM;
			}

			htmx_swap_set_scroll(self, direction, NULL, HTMX_SCROLL_SMOOTH);
		} else if (g_str_has_prefix(part, "show:")) {
			const gchar *value = part + 5;
			HtmxScrollDirection direction = HTMX_SCROLL_TOP;

			if (g_str_equal(value, "top")) {
				direction = HTMX_SCROLL_TOP;
			} else if (g_str_equal(value, "bottom")) {
				direction = HTMX_SCROLL_BOTTOM;
			}

			htmx_swap_set_show(self, direction, NULL);
		} else if (g_str_has_prefix(part, "focus-scroll:")) {
			const gchar *value = part + 13;
			htmx_swap_set_focus_scroll(self, g_str_equal(value, "true"));
		} else if (g_str_has_prefix(part, "transition:")) {
			const gchar *value = part + 11;
			htmx_swap_set_transition(self, g_str_equal(value, "true"));
		} else if (g_str_equal(part, "ignoreTitle:true") ||
		           g_str_equal(part, "ignoreTitle")) {
			htmx_swap_set_ignore_title(self, TRUE);
		}
	}

	return (HtmxSwap *)g_steal_pointer(&self);
}
