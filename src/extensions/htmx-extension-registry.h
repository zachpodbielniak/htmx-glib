/*
 * htmx-extension-registry.h - Extension registry
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * HtmxExtensionRegistry manages registration and lookup of
 * HtmxExtension implementations. It provides batch invocation
 * of extension hooks across all registered extensions.
 */

#ifndef HTMX_EXTENSION_REGISTRY_H
#define HTMX_EXTENSION_REGISTRY_H

#include <glib-object.h>
#include "htmx-extension.h"

G_BEGIN_DECLS

#define HTMX_TYPE_EXTENSION_REGISTRY (htmx_extension_registry_get_type())
G_DECLARE_FINAL_TYPE(HtmxExtensionRegistry, htmx_extension_registry, HTMX, EXTENSION_REGISTRY, GObject)

/**
 * htmx_extension_registry_new:
 *
 * Creates a new extension registry.
 *
 * Returns: (transfer full): a new #HtmxExtensionRegistry
 */
HtmxExtensionRegistry *
htmx_extension_registry_new(void);

/**
 * htmx_extension_registry_register:
 * @self: an #HtmxExtensionRegistry
 * @extension: the extension to register
 *
 * Registers an extension. The extension's init() hook is called
 * immediately. Extensions are identified by name — registering
 * a new extension with the same name replaces the previous one.
 */
void
htmx_extension_registry_register(HtmxExtensionRegistry *self,
                                 HtmxExtension         *extension);

/**
 * htmx_extension_registry_unregister:
 * @self: an #HtmxExtensionRegistry
 * @name: the extension name to remove
 *
 * Removes a registered extension by name.
 *
 * Returns: %TRUE if the extension was found and removed
 */
gboolean
htmx_extension_registry_unregister(HtmxExtensionRegistry *self,
                                   const gchar           *name);

/**
 * htmx_extension_registry_get:
 * @self: an #HtmxExtensionRegistry
 * @name: the extension name
 *
 * Looks up a registered extension by name.
 *
 * Returns: (transfer none) (nullable): the extension, or %NULL
 */
HtmxExtension *
htmx_extension_registry_get(HtmxExtensionRegistry *self,
                            const gchar           *name);

/**
 * htmx_extension_registry_get_all:
 * @self: an #HtmxExtensionRegistry
 *
 * Returns all registered extensions.
 *
 * Returns: (transfer none) (element-type HtmxExtension): the extensions
 */
GPtrArray *
htmx_extension_registry_get_all(HtmxExtensionRegistry *self);

/**
 * htmx_extension_registry_get_count:
 * @self: an #HtmxExtensionRegistry
 *
 * Returns the number of registered extensions.
 *
 * Returns: the count
 */
guint
htmx_extension_registry_get_count(HtmxExtensionRegistry *self);

/**
 * htmx_extension_registry_fire_event:
 * @self: an #HtmxExtensionRegistry
 * @event_name: the event name
 * @detail: (element-type utf8 utf8) (nullable): event detail
 *
 * Fires an event on all registered extensions. Returns %FALSE
 * if any extension cancels the event.
 *
 * Returns: %TRUE if all extensions allow the event
 */
gboolean
htmx_extension_registry_fire_event(HtmxExtensionRegistry *self,
                                   const gchar           *event_name,
                                   GHashTable            *detail);

/**
 * htmx_extension_registry_transform_response:
 * @self: an #HtmxExtensionRegistry
 * @content: the response content
 * @content_type: the content type
 *
 * Runs the response through all extensions' transform_response hooks.
 * Each extension receives the output of the previous one.
 *
 * Returns: (transfer full): the transformed content
 */
gchar *
htmx_extension_registry_transform_response(HtmxExtensionRegistry *self,
                                           const gchar           *content,
                                           const gchar           *content_type);

/**
 * htmx_extension_registry_filter_headers:
 * @self: an #HtmxExtensionRegistry
 * @headers: (element-type utf8 utf8): the headers to filter
 * @is_request: %TRUE for request headers, %FALSE for response
 *
 * Runs headers through all extensions' header_filter hooks.
 */
void
htmx_extension_registry_filter_headers(HtmxExtensionRegistry *self,
                                       GHashTable            *headers,
                                       gboolean               is_request);

G_END_DECLS

#endif /* HTMX_EXTENSION_REGISTRY_H */
