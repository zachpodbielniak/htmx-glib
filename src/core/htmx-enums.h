/*
 * htmx-enums.h - HTMX enumerations
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * This file defines all enumerations used throughout the htmx-glib library.
 */

#ifndef HTMX_ENUMS_H
#define HTMX_ENUMS_H

#include <glib-object.h>

G_BEGIN_DECLS

/**
 * HtmxMethod:
 * @HTMX_METHOD_GET: HTTP GET method
 * @HTMX_METHOD_POST: HTTP POST method
 * @HTMX_METHOD_PUT: HTTP PUT method
 * @HTMX_METHOD_PATCH: HTTP PATCH method
 * @HTMX_METHOD_DELETE: HTTP DELETE method
 *
 * HTTP methods supported by HTMX for AJAX requests.
 */
typedef enum {
	HTMX_METHOD_GET = 0,
	HTMX_METHOD_POST,
	HTMX_METHOD_PUT,
	HTMX_METHOD_PATCH,
	HTMX_METHOD_DELETE
} HtmxMethod;

GType htmx_method_get_type(void) G_GNUC_CONST;
#define HTMX_TYPE_METHOD (htmx_method_get_type())

/**
 * HtmxSwapStyle:
 * @HTMX_SWAP_INNER_HTML: Replace inner HTML of target element
 * @HTMX_SWAP_OUTER_HTML: Replace entire target element
 * @HTMX_SWAP_BEFORE_BEGIN: Insert before the target element
 * @HTMX_SWAP_AFTER_BEGIN: Insert as first child of target element
 * @HTMX_SWAP_BEFORE_END: Insert as last child of target element
 * @HTMX_SWAP_AFTER_END: Insert after the target element
 * @HTMX_SWAP_DELETE: Delete the target element
 * @HTMX_SWAP_NONE: Do not swap (useful for triggering events only)
 *
 * Specifies how the response content should be swapped into the DOM.
 */
typedef enum {
	HTMX_SWAP_INNER_HTML = 0,
	HTMX_SWAP_OUTER_HTML,
	HTMX_SWAP_BEFORE_BEGIN,
	HTMX_SWAP_AFTER_BEGIN,
	HTMX_SWAP_BEFORE_END,
	HTMX_SWAP_AFTER_END,
	HTMX_SWAP_DELETE,
	HTMX_SWAP_NONE
} HtmxSwapStyle;

GType htmx_swap_style_get_type(void) G_GNUC_CONST;
#define HTMX_TYPE_SWAP_STYLE (htmx_swap_style_get_type())

/**
 * HtmxTriggerModifier:
 * @HTMX_TRIGGER_NONE: No modifiers
 * @HTMX_TRIGGER_ONCE: Trigger only once
 * @HTMX_TRIGGER_CHANGED: Trigger only if value changed
 * @HTMX_TRIGGER_DELAY: Delay trigger by specified time
 * @HTMX_TRIGGER_THROTTLE: Throttle trigger rate
 * @HTMX_TRIGGER_FROM: Trigger from another element
 * @HTMX_TRIGGER_TARGET: Override the target element
 * @HTMX_TRIGGER_CONSUME: Consume the event
 * @HTMX_TRIGGER_QUEUE: Queue the request
 *
 * Flags that can be combined to modify trigger behavior.
 */
typedef enum {
	HTMX_TRIGGER_NONE     = 0,
	HTMX_TRIGGER_ONCE     = 1 << 0,
	HTMX_TRIGGER_CHANGED  = 1 << 1,
	HTMX_TRIGGER_DELAY    = 1 << 2,
	HTMX_TRIGGER_THROTTLE = 1 << 3,
	HTMX_TRIGGER_FROM     = 1 << 4,
	HTMX_TRIGGER_TARGET   = 1 << 5,
	HTMX_TRIGGER_CONSUME  = 1 << 6,
	HTMX_TRIGGER_QUEUE    = 1 << 7
} HtmxTriggerModifier;

GType htmx_trigger_modifier_get_type(void) G_GNUC_CONST;
#define HTMX_TYPE_TRIGGER_MODIFIER (htmx_trigger_modifier_get_type())

/**
 * HtmxQueueOption:
 * @HTMX_QUEUE_FIRST: Queue first, discard subsequent requests
 * @HTMX_QUEUE_LAST: Queue last, replacing previous queued request
 * @HTMX_QUEUE_ALL: Queue all requests
 * @HTMX_QUEUE_NONE: Do not queue (cancel if request in progress)
 *
 * Options for request queueing behavior.
 */
typedef enum {
	HTMX_QUEUE_FIRST = 0,
	HTMX_QUEUE_LAST,
	HTMX_QUEUE_ALL,
	HTMX_QUEUE_NONE
} HtmxQueueOption;

GType htmx_queue_option_get_type(void) G_GNUC_CONST;
#define HTMX_TYPE_QUEUE_OPTION (htmx_queue_option_get_type())

/**
 * HtmxTargetType:
 * @HTMX_TARGET_THIS: Target the element itself
 * @HTMX_TARGET_CLOSEST: Target the closest ancestor matching selector
 * @HTMX_TARGET_FIND: Target a descendant matching selector
 * @HTMX_TARGET_NEXT: Target the next sibling matching selector
 * @HTMX_TARGET_PREVIOUS: Target the previous sibling matching selector
 * @HTMX_TARGET_SELECTOR: Target element matching CSS selector
 *
 * Types of target selectors for HTMX targeting.
 */
typedef enum {
	HTMX_TARGET_THIS = 0,
	HTMX_TARGET_CLOSEST,
	HTMX_TARGET_FIND,
	HTMX_TARGET_NEXT,
	HTMX_TARGET_PREVIOUS,
	HTMX_TARGET_SELECTOR
} HtmxTargetType;

GType htmx_target_type_get_type(void) G_GNUC_CONST;
#define HTMX_TYPE_TARGET_TYPE (htmx_target_type_get_type())

/**
 * HtmxInputType:
 * @HTMX_INPUT_TEXT: Text input
 * @HTMX_INPUT_PASSWORD: Password input
 * @HTMX_INPUT_EMAIL: Email input
 * @HTMX_INPUT_NUMBER: Number input
 * @HTMX_INPUT_TEL: Telephone input
 * @HTMX_INPUT_URL: URL input
 * @HTMX_INPUT_SEARCH: Search input
 * @HTMX_INPUT_DATE: Date input
 * @HTMX_INPUT_TIME: Time input
 * @HTMX_INPUT_DATETIME_LOCAL: Local datetime input
 * @HTMX_INPUT_MONTH: Month input
 * @HTMX_INPUT_WEEK: Week input
 * @HTMX_INPUT_COLOR: Color picker input
 * @HTMX_INPUT_RANGE: Range slider input
 * @HTMX_INPUT_CHECKBOX: Checkbox input
 * @HTMX_INPUT_RADIO: Radio button input
 * @HTMX_INPUT_FILE: File upload input
 * @HTMX_INPUT_HIDDEN: Hidden input
 * @HTMX_INPUT_SUBMIT: Submit button
 * @HTMX_INPUT_RESET: Reset button
 * @HTMX_INPUT_BUTTON: Generic button
 *
 * HTML input element types.
 */
typedef enum {
	HTMX_INPUT_TEXT = 0,
	HTMX_INPUT_PASSWORD,
	HTMX_INPUT_EMAIL,
	HTMX_INPUT_NUMBER,
	HTMX_INPUT_TEL,
	HTMX_INPUT_URL,
	HTMX_INPUT_SEARCH,
	HTMX_INPUT_DATE,
	HTMX_INPUT_TIME,
	HTMX_INPUT_DATETIME_LOCAL,
	HTMX_INPUT_MONTH,
	HTMX_INPUT_WEEK,
	HTMX_INPUT_COLOR,
	HTMX_INPUT_RANGE,
	HTMX_INPUT_CHECKBOX,
	HTMX_INPUT_RADIO,
	HTMX_INPUT_FILE,
	HTMX_INPUT_HIDDEN,
	HTMX_INPUT_SUBMIT,
	HTMX_INPUT_RESET,
	HTMX_INPUT_BUTTON
} HtmxInputType;

GType htmx_input_type_get_type(void) G_GNUC_CONST;
#define HTMX_TYPE_INPUT_TYPE (htmx_input_type_get_type())

/**
 * HtmxButtonType:
 * @HTMX_BUTTON_BUTTON: Generic button (type="button")
 * @HTMX_BUTTON_SUBMIT: Submit button (type="submit")
 * @HTMX_BUTTON_RESET: Reset button (type="reset")
 *
 * HTML button element types.
 */
typedef enum {
	HTMX_BUTTON_BUTTON = 0,
	HTMX_BUTTON_SUBMIT,
	HTMX_BUTTON_RESET
} HtmxButtonType;

GType htmx_button_type_get_type(void) G_GNUC_CONST;
#define HTMX_TYPE_BUTTON_TYPE (htmx_button_type_get_type())

/**
 * htmx_button_type_to_string:
 * @type: an #HtmxButtonType
 *
 * Converts a button type enum to its HTML string representation.
 *
 * Returns: (transfer none): the button type string (e.g., "button", "submit")
 */
const gchar *
htmx_button_type_to_string(HtmxButtonType type);

/**
 * htmx_button_type_from_string:
 * @str: a button type string
 *
 * Converts a button type string to its enum value.
 *
 * Returns: the corresponding #HtmxButtonType, or -1 on error
 */
HtmxButtonType
htmx_button_type_from_string(const gchar *str);

/**
 * HtmxScrollBehavior:
 * @HTMX_SCROLL_SMOOTH: Smooth scrolling animation
 * @HTMX_SCROLL_AUTO: Browser default scrolling
 *
 * Scroll behavior for swap operations.
 */
typedef enum {
	HTMX_SCROLL_SMOOTH = 0,
	HTMX_SCROLL_AUTO
} HtmxScrollBehavior;

GType htmx_scroll_behavior_get_type(void) G_GNUC_CONST;
#define HTMX_TYPE_SCROLL_BEHAVIOR (htmx_scroll_behavior_get_type())

/**
 * HtmxScrollDirection:
 * @HTMX_SCROLL_TOP: Scroll to top
 * @HTMX_SCROLL_BOTTOM: Scroll to bottom
 *
 * Direction for scroll operations.
 */
typedef enum {
	HTMX_SCROLL_TOP = 0,
	HTMX_SCROLL_BOTTOM
} HtmxScrollDirection;

GType htmx_scroll_direction_get_type(void) G_GNUC_CONST;
#define HTMX_TYPE_SCROLL_DIRECTION (htmx_scroll_direction_get_type())

/**
 * HtmxSwapModifier:
 * @HTMX_SWAP_MOD_NONE: No modifiers
 * @HTMX_SWAP_MOD_SCROLL: Scroll after swap
 * @HTMX_SWAP_MOD_SHOW: Show element after swap
 * @HTMX_SWAP_MOD_FOCUS_SCROLL: Focus scroll after swap
 * @HTMX_SWAP_MOD_SETTLE: Settle time modifier
 * @HTMX_SWAP_MOD_SWAP: Swap time modifier
 * @HTMX_SWAP_MOD_TRANSITION: Transition behavior
 * @HTMX_SWAP_MOD_IGNORE_TITLE: Ignore title updates
 *
 * Flags that can be combined to modify swap behavior.
 */
typedef enum {
	HTMX_SWAP_MOD_NONE         = 0,
	HTMX_SWAP_MOD_SCROLL       = 1 << 0,
	HTMX_SWAP_MOD_SHOW         = 1 << 1,
	HTMX_SWAP_MOD_FOCUS_SCROLL = 1 << 2,
	HTMX_SWAP_MOD_SETTLE       = 1 << 3,
	HTMX_SWAP_MOD_SWAP         = 1 << 4,
	HTMX_SWAP_MOD_TRANSITION   = 1 << 5,
	HTMX_SWAP_MOD_IGNORE_TITLE = 1 << 6
} HtmxSwapModifier;

GType htmx_swap_modifier_get_type(void) G_GNUC_CONST;
#define HTMX_TYPE_SWAP_MODIFIER (htmx_swap_modifier_get_type())

/*
 * Utility functions for enum conversion
 */

/**
 * htmx_method_to_string:
 * @method: an #HtmxMethod
 *
 * Converts an HTTP method enum to its string representation.
 *
 * Returns: (transfer none): the method string (e.g., "GET", "POST")
 */
const gchar *
htmx_method_to_string(HtmxMethod method);

/**
 * htmx_method_from_string:
 * @str: a method string
 *
 * Converts a method string to its enum value.
 *
 * Returns: the corresponding #HtmxMethod, or -1 on error
 */
HtmxMethod
htmx_method_from_string(const gchar *str);

/**
 * htmx_swap_style_to_string:
 * @style: an #HtmxSwapStyle
 *
 * Converts a swap style enum to its HTMX string representation.
 *
 * Returns: (transfer none): the swap style string (e.g., "innerHTML", "outerHTML")
 */
const gchar *
htmx_swap_style_to_string(HtmxSwapStyle style);

/**
 * htmx_swap_style_from_string:
 * @str: a swap style string
 *
 * Converts a swap style string to its enum value.
 *
 * Returns: the corresponding #HtmxSwapStyle, or -1 on error
 */
HtmxSwapStyle
htmx_swap_style_from_string(const gchar *str);

/**
 * htmx_input_type_to_string:
 * @type: an #HtmxInputType
 *
 * Converts an input type enum to its HTML string representation.
 *
 * Returns: (transfer none): the input type string (e.g., "text", "password")
 */
const gchar *
htmx_input_type_to_string(HtmxInputType type);

/**
 * htmx_input_type_from_string:
 * @str: an input type string
 *
 * Converts an input type string to its enum value.
 *
 * Returns: the corresponding #HtmxInputType, or -1 on error
 */
HtmxInputType
htmx_input_type_from_string(const gchar *str);

/**
 * HtmxCookieSameSite:
 * @HTMX_COOKIE_SAME_SITE_STRICT: Only send cookie for same-site requests
 * @HTMX_COOKIE_SAME_SITE_LAX: Send for same-site and top-level navigations
 * @HTMX_COOKIE_SAME_SITE_NONE: Send for all requests (requires Secure)
 *
 * SameSite attribute values for HTTP cookies.
 */
typedef enum {
	HTMX_COOKIE_SAME_SITE_STRICT = 0,
	HTMX_COOKIE_SAME_SITE_LAX,
	HTMX_COOKIE_SAME_SITE_NONE
} HtmxCookieSameSite;

GType htmx_cookie_same_site_get_type(void) G_GNUC_CONST;
#define HTMX_TYPE_COOKIE_SAME_SITE (htmx_cookie_same_site_get_type())

/**
 * htmx_cookie_same_site_to_string:
 * @same_site: an #HtmxCookieSameSite
 *
 * Converts a SameSite enum to its string representation.
 *
 * Returns: (transfer none): the SameSite string (e.g., "Strict", "Lax")
 */
const gchar *
htmx_cookie_same_site_to_string(HtmxCookieSameSite same_site);

G_END_DECLS

#endif /* HTMX_ENUMS_H */
