/*
 * htmx-extension.h - Extension interface with lifecycle hooks
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * HtmxExtension is an interface that extensions implement to hook
 * into htmx-glib's request/response lifecycle. This mirrors the
 * htmx JavaScript extension API with server-side equivalents.
 *
 * Extensions can intercept and modify behavior at 13+ hook points
 * covering the full request lifecycle: initialization, request
 * building, response handling, swapping, settling, and more.
 *
 * All hook methods are optional — implement only what you need.
 */

#ifndef HTMX_EXTENSION_H
#define HTMX_EXTENSION_H

#include <glib-object.h>

G_BEGIN_DECLS

#define HTMX_TYPE_EXTENSION (htmx_extension_get_type())
G_DECLARE_INTERFACE(HtmxExtension, htmx_extension, HTMX, EXTENSION, GObject)

/**
 * HtmxExtensionInterface:
 * @parent_iface: parent interface
 * @get_name: returns the extension name (used in hx-ext attribute)
 * @init: called when the extension is registered
 * @on_event: generic event hook — return %FALSE to cancel the event
 * @transform_response: modify response content before processing
 * @is_inline_swap: check if a swap mode is handled by this extension
 * @handle_swap: perform a custom swap operation
 * @encode_parameters: modify request parameters before sending
 * @get_selectors: return additional CSS selectors to process
 * @header_filter: filter or modify response headers
 * @on_request: called before a request is sent
 * @on_response: called after a response is received
 * @on_error: called when an error occurs
 * @on_before_swap: called before content is swapped
 * @on_after_swap: called after content is swapped
 * @on_before_settle: called before settling (CSS transitions, etc.)
 * @on_after_settle: called after settling completes
 *
 * Interface for htmx extensions with lifecycle hooks.
 * All methods are optional.
 */
struct _HtmxExtensionInterface {
	GTypeInterface parent_iface;

	/* Identity */
	const gchar *(*get_name)         (HtmxExtension *self);

	/* Lifecycle */
	void         (*init)             (HtmxExtension *self);

	/* Event handling */
	gboolean     (*on_event)         (HtmxExtension *self,
	                                  const gchar   *event_name,
	                                  GHashTable    *detail);

	/* Response processing */
	gchar       *(*transform_response)(HtmxExtension *self,
	                                   const gchar   *content,
	                                   const gchar   *content_type);

	/* Swap handling */
	gboolean     (*is_inline_swap)   (HtmxExtension *self,
	                                  const gchar   *swap_style);
	gboolean     (*handle_swap)      (HtmxExtension *self,
	                                  const gchar   *swap_style,
	                                  gpointer       target,
	                                  const gchar   *fragment);

	/* Request building */
	void         (*encode_parameters)(HtmxExtension *self,
	                                  gpointer       element,
	                                  GHashTable    *parameters);
	GPtrArray   *(*get_selectors)    (HtmxExtension *self);

	/* Header processing */
	void         (*header_filter)    (HtmxExtension *self,
	                                  GHashTable    *headers,
	                                  gboolean       is_request);

	/* Request/Response hooks */
	void         (*on_request)       (HtmxExtension *self,
	                                  gpointer       request);
	void         (*on_response)      (HtmxExtension *self,
	                                  gpointer       response);
	void         (*on_error)         (HtmxExtension *self,
	                                  const gchar   *message,
	                                  gpointer       request);

	/* Swap lifecycle */
	gboolean     (*on_before_swap)   (HtmxExtension *self,
	                                  gpointer       response);
	void         (*on_after_swap)    (HtmxExtension *self,
	                                  gpointer       target);
	gboolean     (*on_before_settle) (HtmxExtension *self,
	                                  gpointer       target);
	void         (*on_after_settle)  (HtmxExtension *self,
	                                  gpointer       target);
};

/* Identity */
const gchar *
htmx_extension_get_name(HtmxExtension *self);

/* Lifecycle */
void
htmx_extension_init_ext(HtmxExtension *self);

/* Event handling */
gboolean
htmx_extension_on_event(HtmxExtension *self,
                        const gchar   *event_name,
                        GHashTable    *detail);

/* Response processing */
gchar *
htmx_extension_transform_response(HtmxExtension *self,
                                  const gchar   *content,
                                  const gchar   *content_type);

/* Swap handling */
gboolean
htmx_extension_is_inline_swap(HtmxExtension *self,
                              const gchar   *swap_style);

gboolean
htmx_extension_handle_swap(HtmxExtension *self,
                           const gchar   *swap_style,
                           gpointer       target,
                           const gchar   *fragment);

/* Request building */
void
htmx_extension_encode_parameters(HtmxExtension *self,
                                 gpointer       element,
                                 GHashTable    *parameters);

GPtrArray *
htmx_extension_get_selectors(HtmxExtension *self);

/* Header processing */
void
htmx_extension_header_filter(HtmxExtension *self,
                             GHashTable    *headers,
                             gboolean       is_request);

/* Request/Response hooks */
void
htmx_extension_on_request(HtmxExtension *self,
                          gpointer       request);

void
htmx_extension_on_response(HtmxExtension *self,
                           gpointer       response);

void
htmx_extension_on_error(HtmxExtension *self,
                        const gchar   *message,
                        gpointer       request);

/* Swap lifecycle */
gboolean
htmx_extension_on_before_swap(HtmxExtension *self,
                              gpointer       response);

void
htmx_extension_on_after_swap(HtmxExtension *self,
                             gpointer       target);

gboolean
htmx_extension_on_before_settle(HtmxExtension *self,
                                gpointer       target);

void
htmx_extension_on_after_settle(HtmxExtension *self,
                               gpointer       target);

G_END_DECLS

#endif /* HTMX_EXTENSION_H */
