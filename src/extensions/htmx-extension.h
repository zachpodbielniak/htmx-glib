/*
 * htmx-extension.h - Extension interface
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef HTMX_EXTENSION_H
#define HTMX_EXTENSION_H

#include <glib-object.h>

G_BEGIN_DECLS

#define HTMX_TYPE_EXTENSION (htmx_extension_get_type())
G_DECLARE_INTERFACE(HtmxExtension, htmx_extension, HTMX, EXTENSION, GObject)

struct _HtmxExtensionInterface {
	GTypeInterface parent_iface;

	const gchar *(*get_name)(HtmxExtension *self);
	void (*init_element)(HtmxExtension *self, gpointer element);
};

const gchar *htmx_extension_get_name(HtmxExtension *self);
void htmx_extension_init_element(HtmxExtension *self, gpointer element);

G_END_DECLS

#endif /* HTMX_EXTENSION_H */
