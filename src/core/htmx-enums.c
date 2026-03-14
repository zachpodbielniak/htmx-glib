/*
 * htmx-enums.c - HTMX enumerations implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-enums.h"
#include <string.h>

/*
 * Method enum strings
 */
static const gchar *method_strings[] = {
	"GET",
	"POST",
	"PUT",
	"PATCH",
	"DELETE",
	NULL
};

/*
 * Swap style enum strings
 */
static const gchar *swap_style_strings[] = {
	"innerHTML",
	"outerHTML",
	"beforebegin",
	"afterbegin",
	"beforeend",
	"afterend",
	"delete",
	"none",
	NULL
};

/*
 * Button type enum strings
 */
static const gchar *button_type_strings[] = {
	"button",
	"submit",
	"reset",
	NULL
};

/*
 * Input type enum strings
 */
static const gchar *input_type_strings[] = {
	"text",
	"password",
	"email",
	"number",
	"tel",
	"url",
	"search",
	"date",
	"time",
	"datetime-local",
	"month",
	"week",
	"color",
	"range",
	"checkbox",
	"radio",
	"file",
	"hidden",
	"submit",
	"reset",
	"button",
	NULL
};

/*
 * GType registration for HtmxMethod
 */
GType
htmx_method_get_type(void)
{
	static gsize g_type_id = 0;

	if (g_once_init_enter(&g_type_id)) {
		static const GEnumValue values[] = {
			{ HTMX_METHOD_GET, "HTMX_METHOD_GET", "get" },
			{ HTMX_METHOD_POST, "HTMX_METHOD_POST", "post" },
			{ HTMX_METHOD_PUT, "HTMX_METHOD_PUT", "put" },
			{ HTMX_METHOD_PATCH, "HTMX_METHOD_PATCH", "patch" },
			{ HTMX_METHOD_DELETE, "HTMX_METHOD_DELETE", "delete" },
			{ 0, NULL, NULL }
		};
		GType type_id = g_enum_register_static("HtmxMethod", values);
		g_once_init_leave(&g_type_id, type_id);
	}

	return (GType)g_type_id;
}

/*
 * GType registration for HtmxSwapStyle
 */
GType
htmx_swap_style_get_type(void)
{
	static gsize g_type_id = 0;

	if (g_once_init_enter(&g_type_id)) {
		static const GEnumValue values[] = {
			{ HTMX_SWAP_INNER_HTML, "HTMX_SWAP_INNER_HTML", "innerHTML" },
			{ HTMX_SWAP_OUTER_HTML, "HTMX_SWAP_OUTER_HTML", "outerHTML" },
			{ HTMX_SWAP_BEFORE_BEGIN, "HTMX_SWAP_BEFORE_BEGIN", "beforebegin" },
			{ HTMX_SWAP_AFTER_BEGIN, "HTMX_SWAP_AFTER_BEGIN", "afterbegin" },
			{ HTMX_SWAP_BEFORE_END, "HTMX_SWAP_BEFORE_END", "beforeend" },
			{ HTMX_SWAP_AFTER_END, "HTMX_SWAP_AFTER_END", "afterend" },
			{ HTMX_SWAP_DELETE, "HTMX_SWAP_DELETE", "delete" },
			{ HTMX_SWAP_NONE, "HTMX_SWAP_NONE", "none" },
			{ 0, NULL, NULL }
		};
		GType type_id = g_enum_register_static("HtmxSwapStyle", values);
		g_once_init_leave(&g_type_id, type_id);
	}

	return (GType)g_type_id;
}

/*
 * GType registration for HtmxTriggerModifier
 */
GType
htmx_trigger_modifier_get_type(void)
{
	static gsize g_type_id = 0;

	if (g_once_init_enter(&g_type_id)) {
		static const GFlagsValue values[] = {
			{ HTMX_TRIGGER_NONE, "HTMX_TRIGGER_NONE", "none" },
			{ HTMX_TRIGGER_ONCE, "HTMX_TRIGGER_ONCE", "once" },
			{ HTMX_TRIGGER_CHANGED, "HTMX_TRIGGER_CHANGED", "changed" },
			{ HTMX_TRIGGER_DELAY, "HTMX_TRIGGER_DELAY", "delay" },
			{ HTMX_TRIGGER_THROTTLE, "HTMX_TRIGGER_THROTTLE", "throttle" },
			{ HTMX_TRIGGER_FROM, "HTMX_TRIGGER_FROM", "from" },
			{ HTMX_TRIGGER_TARGET, "HTMX_TRIGGER_TARGET", "target" },
			{ HTMX_TRIGGER_CONSUME, "HTMX_TRIGGER_CONSUME", "consume" },
			{ HTMX_TRIGGER_QUEUE, "HTMX_TRIGGER_QUEUE", "queue" },
			{ 0, NULL, NULL }
		};
		GType type_id = g_flags_register_static("HtmxTriggerModifier", values);
		g_once_init_leave(&g_type_id, type_id);
	}

	return (GType)g_type_id;
}

/*
 * GType registration for HtmxQueueOption
 */
GType
htmx_queue_option_get_type(void)
{
	static gsize g_type_id = 0;

	if (g_once_init_enter(&g_type_id)) {
		static const GEnumValue values[] = {
			{ HTMX_QUEUE_FIRST, "HTMX_QUEUE_FIRST", "first" },
			{ HTMX_QUEUE_LAST, "HTMX_QUEUE_LAST", "last" },
			{ HTMX_QUEUE_ALL, "HTMX_QUEUE_ALL", "all" },
			{ HTMX_QUEUE_NONE, "HTMX_QUEUE_NONE", "none" },
			{ 0, NULL, NULL }
		};
		GType type_id = g_enum_register_static("HtmxQueueOption", values);
		g_once_init_leave(&g_type_id, type_id);
	}

	return (GType)g_type_id;
}

/*
 * GType registration for HtmxTargetType
 */
GType
htmx_target_type_get_type(void)
{
	static gsize g_type_id = 0;

	if (g_once_init_enter(&g_type_id)) {
		static const GEnumValue values[] = {
			{ HTMX_TARGET_THIS, "HTMX_TARGET_THIS", "this" },
			{ HTMX_TARGET_CLOSEST, "HTMX_TARGET_CLOSEST", "closest" },
			{ HTMX_TARGET_FIND, "HTMX_TARGET_FIND", "find" },
			{ HTMX_TARGET_NEXT, "HTMX_TARGET_NEXT", "next" },
			{ HTMX_TARGET_PREVIOUS, "HTMX_TARGET_PREVIOUS", "previous" },
			{ HTMX_TARGET_SELECTOR, "HTMX_TARGET_SELECTOR", "selector" },
			{ 0, NULL, NULL }
		};
		GType type_id = g_enum_register_static("HtmxTargetType", values);
		g_once_init_leave(&g_type_id, type_id);
	}

	return (GType)g_type_id;
}

/*
 * GType registration for HtmxButtonType
 */
GType
htmx_button_type_get_type(void)
{
	static gsize g_type_id = 0;

	if (g_once_init_enter(&g_type_id)) {
		static const GEnumValue values[] = {
			{ HTMX_BUTTON_BUTTON, "HTMX_BUTTON_BUTTON", "button" },
			{ HTMX_BUTTON_SUBMIT, "HTMX_BUTTON_SUBMIT", "submit" },
			{ HTMX_BUTTON_RESET, "HTMX_BUTTON_RESET", "reset" },
			{ 0, NULL, NULL }
		};
		GType type_id = g_enum_register_static("HtmxButtonType", values);
		g_once_init_leave(&g_type_id, type_id);
	}

	return (GType)g_type_id;
}

/*
 * GType registration for HtmxInputType
 */
GType
htmx_input_type_get_type(void)
{
	static gsize g_type_id = 0;

	if (g_once_init_enter(&g_type_id)) {
		static const GEnumValue values[] = {
			{ HTMX_INPUT_TEXT, "HTMX_INPUT_TEXT", "text" },
			{ HTMX_INPUT_PASSWORD, "HTMX_INPUT_PASSWORD", "password" },
			{ HTMX_INPUT_EMAIL, "HTMX_INPUT_EMAIL", "email" },
			{ HTMX_INPUT_NUMBER, "HTMX_INPUT_NUMBER", "number" },
			{ HTMX_INPUT_TEL, "HTMX_INPUT_TEL", "tel" },
			{ HTMX_INPUT_URL, "HTMX_INPUT_URL", "url" },
			{ HTMX_INPUT_SEARCH, "HTMX_INPUT_SEARCH", "search" },
			{ HTMX_INPUT_DATE, "HTMX_INPUT_DATE", "date" },
			{ HTMX_INPUT_TIME, "HTMX_INPUT_TIME", "time" },
			{ HTMX_INPUT_DATETIME_LOCAL, "HTMX_INPUT_DATETIME_LOCAL", "datetime-local" },
			{ HTMX_INPUT_MONTH, "HTMX_INPUT_MONTH", "month" },
			{ HTMX_INPUT_WEEK, "HTMX_INPUT_WEEK", "week" },
			{ HTMX_INPUT_COLOR, "HTMX_INPUT_COLOR", "color" },
			{ HTMX_INPUT_RANGE, "HTMX_INPUT_RANGE", "range" },
			{ HTMX_INPUT_CHECKBOX, "HTMX_INPUT_CHECKBOX", "checkbox" },
			{ HTMX_INPUT_RADIO, "HTMX_INPUT_RADIO", "radio" },
			{ HTMX_INPUT_FILE, "HTMX_INPUT_FILE", "file" },
			{ HTMX_INPUT_HIDDEN, "HTMX_INPUT_HIDDEN", "hidden" },
			{ HTMX_INPUT_SUBMIT, "HTMX_INPUT_SUBMIT", "submit" },
			{ HTMX_INPUT_RESET, "HTMX_INPUT_RESET", "reset" },
			{ HTMX_INPUT_BUTTON, "HTMX_INPUT_BUTTON", "button" },
			{ 0, NULL, NULL }
		};
		GType type_id = g_enum_register_static("HtmxInputType", values);
		g_once_init_leave(&g_type_id, type_id);
	}

	return (GType)g_type_id;
}

/*
 * GType registration for HtmxScrollBehavior
 */
GType
htmx_scroll_behavior_get_type(void)
{
	static gsize g_type_id = 0;

	if (g_once_init_enter(&g_type_id)) {
		static const GEnumValue values[] = {
			{ HTMX_SCROLL_SMOOTH, "HTMX_SCROLL_SMOOTH", "smooth" },
			{ HTMX_SCROLL_AUTO, "HTMX_SCROLL_AUTO", "auto" },
			{ 0, NULL, NULL }
		};
		GType type_id = g_enum_register_static("HtmxScrollBehavior", values);
		g_once_init_leave(&g_type_id, type_id);
	}

	return (GType)g_type_id;
}

/*
 * GType registration for HtmxScrollDirection
 */
GType
htmx_scroll_direction_get_type(void)
{
	static gsize g_type_id = 0;

	if (g_once_init_enter(&g_type_id)) {
		static const GEnumValue values[] = {
			{ HTMX_SCROLL_TOP, "HTMX_SCROLL_TOP", "top" },
			{ HTMX_SCROLL_BOTTOM, "HTMX_SCROLL_BOTTOM", "bottom" },
			{ 0, NULL, NULL }
		};
		GType type_id = g_enum_register_static("HtmxScrollDirection", values);
		g_once_init_leave(&g_type_id, type_id);
	}

	return (GType)g_type_id;
}

/*
 * GType registration for HtmxSwapModifier
 */
GType
htmx_swap_modifier_get_type(void)
{
	static gsize g_type_id = 0;

	if (g_once_init_enter(&g_type_id)) {
		static const GFlagsValue values[] = {
			{ HTMX_SWAP_MOD_NONE, "HTMX_SWAP_MOD_NONE", "none" },
			{ HTMX_SWAP_MOD_SCROLL, "HTMX_SWAP_MOD_SCROLL", "scroll" },
			{ HTMX_SWAP_MOD_SHOW, "HTMX_SWAP_MOD_SHOW", "show" },
			{ HTMX_SWAP_MOD_FOCUS_SCROLL, "HTMX_SWAP_MOD_FOCUS_SCROLL", "focus-scroll" },
			{ HTMX_SWAP_MOD_SETTLE, "HTMX_SWAP_MOD_SETTLE", "settle" },
			{ HTMX_SWAP_MOD_SWAP, "HTMX_SWAP_MOD_SWAP", "swap" },
			{ HTMX_SWAP_MOD_TRANSITION, "HTMX_SWAP_MOD_TRANSITION", "transition" },
			{ HTMX_SWAP_MOD_IGNORE_TITLE, "HTMX_SWAP_MOD_IGNORE_TITLE", "ignoreTitle" },
			{ 0, NULL, NULL }
		};
		GType type_id = g_flags_register_static("HtmxSwapModifier", values);
		g_once_init_leave(&g_type_id, type_id);
	}

	return (GType)g_type_id;
}

/*
 * Convert HtmxMethod to string
 */
const gchar *
htmx_method_to_string(HtmxMethod method)
{
	g_return_val_if_fail(method >= HTMX_METHOD_GET && method <= HTMX_METHOD_DELETE, NULL);

	return method_strings[method];
}

/*
 * Convert string to HtmxMethod
 */
HtmxMethod
htmx_method_from_string(const gchar *str)
{
	gint i;

	g_return_val_if_fail(str != NULL, -1);

	for (i = 0; method_strings[i] != NULL; i++) {
		if (g_ascii_strcasecmp(str, method_strings[i]) == 0) {
			return (HtmxMethod)i;
		}
	}

	return (HtmxMethod)-1;
}

/*
 * Convert HtmxSwapStyle to string
 */
const gchar *
htmx_swap_style_to_string(HtmxSwapStyle style)
{
	g_return_val_if_fail(style >= HTMX_SWAP_INNER_HTML && style <= HTMX_SWAP_NONE, NULL);

	return swap_style_strings[style];
}

/*
 * Convert string to HtmxSwapStyle
 */
HtmxSwapStyle
htmx_swap_style_from_string(const gchar *str)
{
	gint i;

	g_return_val_if_fail(str != NULL, -1);

	for (i = 0; swap_style_strings[i] != NULL; i++) {
		if (g_ascii_strcasecmp(str, swap_style_strings[i]) == 0) {
			return (HtmxSwapStyle)i;
		}
	}

	return (HtmxSwapStyle)-1;
}

/*
 * Convert HtmxInputType to string
 */
const gchar *
htmx_input_type_to_string(HtmxInputType type)
{
	g_return_val_if_fail(type >= HTMX_INPUT_TEXT && type <= HTMX_INPUT_BUTTON, NULL);

	return input_type_strings[type];
}

/*
 * Convert string to HtmxInputType
 */
HtmxInputType
htmx_input_type_from_string(const gchar *str)
{
	gint i;

	g_return_val_if_fail(str != NULL, -1);

	for (i = 0; input_type_strings[i] != NULL; i++) {
		if (g_ascii_strcasecmp(str, input_type_strings[i]) == 0) {
			return (HtmxInputType)i;
		}
	}

	return (HtmxInputType)-1;
}

/*
 * Sync strategy enum strings
 */
static const gchar *sync_strategy_strings[] = {
	"drop",
	"abort",
	"replace",
	"queue first",
	"queue last",
	"queue all",
	NULL
};

/*
 * GType registration for HtmxSyncStrategy
 */
GType
htmx_sync_strategy_get_type(void)
{
	static gsize g_type_id = 0;

	if (g_once_init_enter(&g_type_id)) {
		static const GEnumValue values[] = {
			{ HTMX_SYNC_DROP, "HTMX_SYNC_DROP", "drop" },
			{ HTMX_SYNC_ABORT, "HTMX_SYNC_ABORT", "abort" },
			{ HTMX_SYNC_REPLACE, "HTMX_SYNC_REPLACE", "replace" },
			{ HTMX_SYNC_QUEUE_FIRST, "HTMX_SYNC_QUEUE_FIRST", "queue-first" },
			{ HTMX_SYNC_QUEUE_LAST, "HTMX_SYNC_QUEUE_LAST", "queue-last" },
			{ HTMX_SYNC_QUEUE_ALL, "HTMX_SYNC_QUEUE_ALL", "queue-all" },
			{ 0, NULL, NULL }
		};
		GType type_id = g_enum_register_static("HtmxSyncStrategy", values);
		g_once_init_leave(&g_type_id, type_id);
	}

	return (GType)g_type_id;
}

const gchar *
htmx_sync_strategy_to_string(HtmxSyncStrategy strategy)
{
	g_return_val_if_fail(strategy >= HTMX_SYNC_DROP && strategy <= HTMX_SYNC_QUEUE_ALL, NULL);

	return sync_strategy_strings[strategy];
}

HtmxSyncStrategy
htmx_sync_strategy_from_string(const gchar *str)
{
	gint i;

	g_return_val_if_fail(str != NULL, -1);

	for (i = 0; sync_strategy_strings[i] != NULL; i++) {
		if (g_ascii_strcasecmp(str, sync_strategy_strings[i]) == 0) {
			return (HtmxSyncStrategy)i;
		}
	}

	return (HtmxSyncStrategy)-1;
}

/*
 * GType registration for HtmxCookieSameSite
 */
GType
htmx_cookie_same_site_get_type(void)
{
	static gsize g_type_id = 0;

	if (g_once_init_enter(&g_type_id)) {
		static const GEnumValue values[] = {
			{ HTMX_COOKIE_SAME_SITE_STRICT, "HTMX_COOKIE_SAME_SITE_STRICT", "Strict" },
			{ HTMX_COOKIE_SAME_SITE_LAX, "HTMX_COOKIE_SAME_SITE_LAX", "Lax" },
			{ HTMX_COOKIE_SAME_SITE_NONE, "HTMX_COOKIE_SAME_SITE_NONE", "None" },
			{ 0, NULL, NULL }
		};
		GType type_id = g_enum_register_static("HtmxCookieSameSite", values);
		g_once_init_leave(&g_type_id, type_id);
	}

	return (GType)g_type_id;
}

/*
 * SameSite enum strings
 */
static const gchar *same_site_strings[] = {
	"Strict",
	"Lax",
	"None",
	NULL
};

/*
 * Convert HtmxCookieSameSite to string
 */
const gchar *
htmx_cookie_same_site_to_string(HtmxCookieSameSite same_site)
{
	g_return_val_if_fail(same_site >= HTMX_COOKIE_SAME_SITE_STRICT &&
	                     same_site <= HTMX_COOKIE_SAME_SITE_NONE, "Lax");

	return same_site_strings[same_site];
}

/*
 * Convert HtmxButtonType to string
 */
const gchar *
htmx_button_type_to_string(HtmxButtonType type)
{
	g_return_val_if_fail(type >= HTMX_BUTTON_BUTTON && type <= HTMX_BUTTON_RESET, NULL);

	return button_type_strings[type];
}

/*
 * Convert string to HtmxButtonType
 */
HtmxButtonType
htmx_button_type_from_string(const gchar *str)
{
	gint i;

	g_return_val_if_fail(str != NULL, -1);

	for (i = 0; button_type_strings[i] != NULL; i++) {
		if (g_ascii_strcasecmp(str, button_type_strings[i]) == 0) {
			return (HtmxButtonType)i;
		}
	}

	return (HtmxButtonType)-1;
}
