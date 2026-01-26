/*
 * htmx-swap.h - HTMX swap configuration boxed type
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * This file defines the HtmxSwap boxed type which represents an HTMX
 * swap configuration with style and modifiers.
 */

#ifndef HTMX_SWAP_H
#define HTMX_SWAP_H

#include <glib-object.h>
#include "core/htmx-enums.h"

G_BEGIN_DECLS

#define HTMX_TYPE_SWAP (htmx_swap_get_type())

typedef struct _HtmxSwap HtmxSwap;

/**
 * HtmxSwap:
 *
 * A boxed type representing an HTMX swap configuration.
 *
 * A swap configuration specifies how the response content should be inserted
 * into the DOM and can include modifiers for timing, scrolling, and transitions.
 */
struct _HtmxSwap {
	HtmxSwapStyle      style;           /* The swap style (innerHTML, outerHTML, etc.) */
	HtmxSwapModifier   modifiers;       /* Bit flags for modifiers */
	guint              swap_ms;         /* Swap timing in milliseconds */
	guint              settle_ms;       /* Settle timing in milliseconds */
	HtmxScrollBehavior scroll_behavior; /* Scroll behavior */
	HtmxScrollDirection scroll_direction; /* Scroll direction */
	gchar             *scroll_target;   /* Target for scroll (selector) */
	gchar             *show_target;     /* Target for show (selector) */
	HtmxScrollDirection show_direction; /* Show direction */
	gboolean           focus_scroll;    /* Whether to focus scroll */
	gboolean           transition;      /* Whether to use view transitions */
	gboolean           ignore_title;    /* Whether to ignore title updates */
};

GType htmx_swap_get_type(void) G_GNUC_CONST;

/**
 * htmx_swap_new:
 * @style: the swap style
 *
 * Creates a new #HtmxSwap with the given style and no modifiers.
 *
 * Returns: (transfer full): a new #HtmxSwap. Free with htmx_swap_free().
 */
HtmxSwap *
htmx_swap_new(HtmxSwapStyle style);

/**
 * htmx_swap_copy:
 * @self: (nullable): an #HtmxSwap
 *
 * Creates a copy of an #HtmxSwap.
 *
 * Returns: (transfer full) (nullable): a copy of @self, or %NULL if @self is %NULL.
 *          Free with htmx_swap_free().
 */
HtmxSwap *
htmx_swap_copy(const HtmxSwap *self);

/**
 * htmx_swap_free:
 * @self: (nullable): an #HtmxSwap
 *
 * Frees an #HtmxSwap and its contents.
 */
void
htmx_swap_free(HtmxSwap *self);

/**
 * htmx_swap_get_style:
 * @self: an #HtmxSwap
 *
 * Gets the swap style.
 *
 * Returns: the swap style
 */
HtmxSwapStyle
htmx_swap_get_style(const HtmxSwap *self);

/**
 * htmx_swap_set_style:
 * @self: an #HtmxSwap
 * @style: the swap style
 *
 * Sets the swap style.
 */
void
htmx_swap_set_style(
	HtmxSwap     *self,
	HtmxSwapStyle style
);

/**
 * htmx_swap_set_swap_time:
 * @self: an #HtmxSwap
 * @ms: time in milliseconds
 *
 * Sets the swap timing (how long to wait between request and swap).
 */
void
htmx_swap_set_swap_time(
	HtmxSwap *self,
	guint     ms
);

/**
 * htmx_swap_set_settle_time:
 * @self: an #HtmxSwap
 * @ms: time in milliseconds
 *
 * Sets the settle timing (how long to wait between swap and settle).
 */
void
htmx_swap_set_settle_time(
	HtmxSwap *self,
	guint     ms
);

/**
 * htmx_swap_set_scroll:
 * @self: an #HtmxSwap
 * @direction: scroll direction
 * @target: (nullable): CSS selector for scroll target
 * @behavior: scroll behavior
 *
 * Sets the scroll behavior after swap.
 */
void
htmx_swap_set_scroll(
	HtmxSwap           *self,
	HtmxScrollDirection direction,
	const gchar        *target,
	HtmxScrollBehavior  behavior
);

/**
 * htmx_swap_set_show:
 * @self: an #HtmxSwap
 * @direction: show direction (top or bottom)
 * @target: (nullable): CSS selector for show target
 *
 * Sets the show behavior after swap.
 */
void
htmx_swap_set_show(
	HtmxSwap           *self,
	HtmxScrollDirection direction,
	const gchar        *target
);

/**
 * htmx_swap_set_focus_scroll:
 * @self: an #HtmxSwap
 * @focus_scroll: whether to enable focus scroll
 *
 * Sets whether to scroll focused element into view.
 */
void
htmx_swap_set_focus_scroll(
	HtmxSwap *self,
	gboolean  focus_scroll
);

/**
 * htmx_swap_set_transition:
 * @self: an #HtmxSwap
 * @transition: whether to enable view transitions
 *
 * Sets whether to use View Transitions API.
 */
void
htmx_swap_set_transition(
	HtmxSwap *self,
	gboolean  transition
);

/**
 * htmx_swap_set_ignore_title:
 * @self: an #HtmxSwap
 * @ignore_title: whether to ignore title updates
 *
 * Sets whether to ignore title tag updates in response.
 */
void
htmx_swap_set_ignore_title(
	HtmxSwap *self,
	gboolean  ignore_title
);

/**
 * htmx_swap_render:
 * @self: an #HtmxSwap
 *
 * Renders the swap configuration as an HTMX swap string.
 *
 * Returns: (transfer full): the rendered swap string. Free with g_free().
 */
gchar *
htmx_swap_render(const HtmxSwap *self);

/**
 * htmx_swap_parse:
 * @str: a swap string to parse
 * @error: (nullable): return location for an error
 *
 * Parses an HTMX swap string into an #HtmxSwap.
 *
 * Returns: (transfer full) (nullable): a new #HtmxSwap, or %NULL on error.
 *          Free with htmx_swap_free().
 */
HtmxSwap *
htmx_swap_parse(
	const gchar *str,
	GError     **error
);

G_DEFINE_AUTOPTR_CLEANUP_FUNC(HtmxSwap, htmx_swap_free)

G_END_DECLS

#endif /* HTMX_SWAP_H */
