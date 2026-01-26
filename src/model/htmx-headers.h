/*
 * htmx-headers.h - HTMX header collection boxed types
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * This file defines boxed types for HTMX request and response headers.
 */

#ifndef HTMX_HEADERS_H
#define HTMX_HEADERS_H

#include <glib-object.h>
#include <libsoup/soup.h>

G_BEGIN_DECLS

#define HTMX_TYPE_REQUEST_HEADERS (htmx_request_headers_get_type())
#define HTMX_TYPE_RESPONSE_HEADERS (htmx_response_headers_get_type())

typedef struct _HtmxRequestHeaders HtmxRequestHeaders;
typedef struct _HtmxResponseHeaders HtmxResponseHeaders;

/**
 * HtmxRequestHeaders:
 *
 * A boxed type containing parsed HTMX request headers.
 *
 * HTMX sends several headers with each request that provide context about
 * the request. This structure holds the parsed values of these headers.
 */
struct _HtmxRequestHeaders {
	gboolean  is_htmx_request;     /* HX-Request: true/false */
	gboolean  is_boosted;          /* HX-Boosted: true/false */
	gboolean  is_history_restore;  /* HX-History-Restore-Request: true/false */
	gchar    *current_url;         /* HX-Current-URL */
	gchar    *trigger_id;          /* HX-Trigger (element ID that triggered) */
	gchar    *trigger_name;        /* HX-Trigger-Name */
	gchar    *target_id;           /* HX-Target (target element ID) */
	gchar    *prompt_response;     /* HX-Prompt (user's response to hx-prompt) */
};

GType htmx_request_headers_get_type(void) G_GNUC_CONST;

/**
 * htmx_request_headers_new:
 *
 * Creates a new empty #HtmxRequestHeaders.
 *
 * Returns: (transfer full): a new #HtmxRequestHeaders.
 *          Free with htmx_request_headers_free().
 */
HtmxRequestHeaders *
htmx_request_headers_new(void);

/**
 * htmx_request_headers_copy:
 * @self: (nullable): an #HtmxRequestHeaders
 *
 * Creates a copy of an #HtmxRequestHeaders.
 *
 * Returns: (transfer full) (nullable): a copy of @self, or %NULL if @self is %NULL.
 *          Free with htmx_request_headers_free().
 */
HtmxRequestHeaders *
htmx_request_headers_copy(const HtmxRequestHeaders *self);

/**
 * htmx_request_headers_free:
 * @self: (nullable): an #HtmxRequestHeaders
 *
 * Frees an #HtmxRequestHeaders and its contents.
 */
void
htmx_request_headers_free(HtmxRequestHeaders *self);

/**
 * htmx_request_headers_is_htmx_request:
 * @self: an #HtmxRequestHeaders
 *
 * Checks if this is an HTMX request (HX-Request header is "true").
 *
 * Returns: %TRUE if this is an HTMX request
 */
gboolean
htmx_request_headers_is_htmx_request(const HtmxRequestHeaders *self);

/**
 * htmx_request_headers_is_boosted:
 * @self: an #HtmxRequestHeaders
 *
 * Checks if this request was boosted (HX-Boosted header is "true").
 *
 * Returns: %TRUE if the request is boosted
 */
gboolean
htmx_request_headers_is_boosted(const HtmxRequestHeaders *self);

/**
 * htmx_request_headers_is_history_restore:
 * @self: an #HtmxRequestHeaders
 *
 * Checks if this is a history restore request.
 *
 * Returns: %TRUE if this is a history restore request
 */
gboolean
htmx_request_headers_is_history_restore(const HtmxRequestHeaders *self);

/**
 * htmx_request_headers_get_current_url:
 * @self: an #HtmxRequestHeaders
 *
 * Gets the current URL from the HX-Current-URL header.
 *
 * Returns: (transfer none) (nullable): the current URL, or %NULL
 */
const gchar *
htmx_request_headers_get_current_url(const HtmxRequestHeaders *self);

/**
 * htmx_request_headers_get_trigger_id:
 * @self: an #HtmxRequestHeaders
 *
 * Gets the ID of the element that triggered the request.
 *
 * Returns: (transfer none) (nullable): the trigger element ID, or %NULL
 */
const gchar *
htmx_request_headers_get_trigger_id(const HtmxRequestHeaders *self);

/**
 * htmx_request_headers_get_trigger_name:
 * @self: an #HtmxRequestHeaders
 *
 * Gets the name of the element that triggered the request.
 *
 * Returns: (transfer none) (nullable): the trigger element name, or %NULL
 */
const gchar *
htmx_request_headers_get_trigger_name(const HtmxRequestHeaders *self);

/**
 * htmx_request_headers_get_target_id:
 * @self: an #HtmxRequestHeaders
 *
 * Gets the ID of the target element.
 *
 * Returns: (transfer none) (nullable): the target element ID, or %NULL
 */
const gchar *
htmx_request_headers_get_target_id(const HtmxRequestHeaders *self);

/**
 * htmx_request_headers_get_prompt_response:
 * @self: an #HtmxRequestHeaders
 *
 * Gets the user's response to an hx-prompt.
 *
 * Returns: (transfer none) (nullable): the prompt response, or %NULL
 */
const gchar *
htmx_request_headers_get_prompt_response(const HtmxRequestHeaders *self);

G_DEFINE_AUTOPTR_CLEANUP_FUNC(HtmxRequestHeaders, htmx_request_headers_free)


/**
 * HtmxResponseHeaders:
 *
 * A boxed type for building HTMX response headers.
 *
 * HTMX can be controlled from the server by sending specific response headers.
 * This structure allows building these headers before sending a response.
 */
struct _HtmxResponseHeaders {
	gchar    *location;              /* HX-Location */
	gchar    *push_url;              /* HX-Push-Url */
	gchar    *redirect;              /* HX-Redirect */
	gboolean  refresh;               /* HX-Refresh */
	gchar    *replace_url;           /* HX-Replace-Url */
	gchar    *reswap;                /* HX-Reswap */
	gchar    *retarget;              /* HX-Retarget */
	gchar    *reselect;              /* HX-Reselect */
	gchar    *trigger;               /* HX-Trigger */
	gchar    *trigger_after_settle;  /* HX-Trigger-After-Settle */
	gchar    *trigger_after_swap;    /* HX-Trigger-After-Swap */
};

GType htmx_response_headers_get_type(void) G_GNUC_CONST;

/**
 * htmx_response_headers_new:
 *
 * Creates a new empty #HtmxResponseHeaders.
 *
 * Returns: (transfer full): a new #HtmxResponseHeaders.
 *          Free with htmx_response_headers_free().
 */
HtmxResponseHeaders *
htmx_response_headers_new(void);

/**
 * htmx_response_headers_copy:
 * @self: (nullable): an #HtmxResponseHeaders
 *
 * Creates a copy of an #HtmxResponseHeaders.
 *
 * Returns: (transfer full) (nullable): a copy of @self, or %NULL if @self is %NULL.
 *          Free with htmx_response_headers_free().
 */
HtmxResponseHeaders *
htmx_response_headers_copy(const HtmxResponseHeaders *self);

/**
 * htmx_response_headers_free:
 * @self: (nullable): an #HtmxResponseHeaders
 *
 * Frees an #HtmxResponseHeaders and its contents.
 */
void
htmx_response_headers_free(HtmxResponseHeaders *self);

/**
 * htmx_response_headers_set_location:
 * @self: an #HtmxResponseHeaders
 * @location: the location value (can be URL or JSON object)
 *
 * Sets the HX-Location header. Allows you to do a client-side redirect
 * that does not do a full page reload.
 */
void
htmx_response_headers_set_location(
	HtmxResponseHeaders *self,
	const gchar         *location
);

/**
 * htmx_response_headers_set_push_url:
 * @self: an #HtmxResponseHeaders
 * @url: the URL to push to browser history
 *
 * Sets the HX-Push-Url header. Pushes a new URL into the browser location bar.
 */
void
htmx_response_headers_set_push_url(
	HtmxResponseHeaders *self,
	const gchar         *url
);

/**
 * htmx_response_headers_set_redirect:
 * @self: an #HtmxResponseHeaders
 * @url: the URL to redirect to
 *
 * Sets the HX-Redirect header. Can be used to do a client-side redirect
 * to a new location.
 */
void
htmx_response_headers_set_redirect(
	HtmxResponseHeaders *self,
	const gchar         *url
);

/**
 * htmx_response_headers_set_refresh:
 * @self: an #HtmxResponseHeaders
 * @refresh: whether to trigger a full refresh
 *
 * Sets the HX-Refresh header. If set to true, a full page refresh will
 * be done on the client.
 */
void
htmx_response_headers_set_refresh(
	HtmxResponseHeaders *self,
	gboolean             refresh
);

/**
 * htmx_response_headers_set_replace_url:
 * @self: an #HtmxResponseHeaders
 * @url: the URL to replace in browser history
 *
 * Sets the HX-Replace-Url header. Replaces the current URL in the
 * browser location bar.
 */
void
htmx_response_headers_set_replace_url(
	HtmxResponseHeaders *self,
	const gchar         *url
);

/**
 * htmx_response_headers_set_reswap:
 * @self: an #HtmxResponseHeaders
 * @swap: the swap specification
 *
 * Sets the HX-Reswap header. Allows you to override the swap behavior
 * for the response.
 */
void
htmx_response_headers_set_reswap(
	HtmxResponseHeaders *self,
	const gchar         *swap
);

/**
 * htmx_response_headers_set_retarget:
 * @self: an #HtmxResponseHeaders
 * @target: CSS selector for the new target
 *
 * Sets the HX-Retarget header. Allows you to override the target
 * element for the response.
 */
void
htmx_response_headers_set_retarget(
	HtmxResponseHeaders *self,
	const gchar         *target
);

/**
 * htmx_response_headers_set_reselect:
 * @self: an #HtmxResponseHeaders
 * @selector: CSS selector to select content from response
 *
 * Sets the HX-Reselect header. Allows you to select a specific part
 * of the response to swap in.
 */
void
htmx_response_headers_set_reselect(
	HtmxResponseHeaders *self,
	const gchar         *selector
);

/**
 * htmx_response_headers_set_trigger:
 * @self: an #HtmxResponseHeaders
 * @trigger: event name or JSON object
 *
 * Sets the HX-Trigger header. Allows you to trigger client-side events.
 */
void
htmx_response_headers_set_trigger(
	HtmxResponseHeaders *self,
	const gchar         *trigger
);

/**
 * htmx_response_headers_set_trigger_after_settle:
 * @self: an #HtmxResponseHeaders
 * @trigger: event name or JSON object
 *
 * Sets the HX-Trigger-After-Settle header. Triggers events after
 * the settle phase.
 */
void
htmx_response_headers_set_trigger_after_settle(
	HtmxResponseHeaders *self,
	const gchar         *trigger
);

/**
 * htmx_response_headers_set_trigger_after_swap:
 * @self: an #HtmxResponseHeaders
 * @trigger: event name or JSON object
 *
 * Sets the HX-Trigger-After-Swap header. Triggers events after
 * the swap phase.
 */
void
htmx_response_headers_set_trigger_after_swap(
	HtmxResponseHeaders *self,
	const gchar         *trigger
);

/**
 * htmx_response_headers_apply:
 * @self: an #HtmxResponseHeaders
 * @message: a #SoupServerMessage to apply headers to
 *
 * Applies the configured response headers to a SoupServerMessage.
 */
void
htmx_response_headers_apply(
	HtmxResponseHeaders *self,
	SoupServerMessage   *message
);

G_DEFINE_AUTOPTR_CLEANUP_FUNC(HtmxResponseHeaders, htmx_response_headers_free)

G_END_DECLS

#endif /* HTMX_HEADERS_H */
