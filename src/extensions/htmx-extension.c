/*
 * htmx-extension.c - Extension interface implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-extension.h"

G_DEFINE_INTERFACE(HtmxExtension, htmx_extension, G_TYPE_OBJECT)

static void
htmx_extension_default_init(HtmxExtensionInterface *iface)
{
	/* Default implementations are NULL */
}

/**
 * htmx_extension_get_name:
 * @self: an #HtmxExtension
 *
 * Gets the name of the extension. This is used for the hx-ext attribute.
 *
 * Returns: (transfer none): the extension name
 */
const gchar *
htmx_extension_get_name(HtmxExtension *self)
{
	HtmxExtensionInterface *iface;

	g_return_val_if_fail(HTMX_IS_EXTENSION(self), NULL);

	iface = HTMX_EXTENSION_GET_IFACE(self);
	if (iface->get_name != NULL) {
		return iface->get_name(self);
	}

	return NULL;
}

/**
 * htmx_extension_init_element:
 * @self: an #HtmxExtension
 * @element: the element to initialize
 *
 * Initializes an element with extension-specific behavior.
 */
void
htmx_extension_init_element(HtmxExtension *self, gpointer element)
{
	HtmxExtensionInterface *iface;

	g_return_if_fail(HTMX_IS_EXTENSION(self));

	iface = HTMX_EXTENSION_GET_IFACE(self);
	if (iface->init_element != NULL) {
		iface->init_element(self, element);
	}
}
