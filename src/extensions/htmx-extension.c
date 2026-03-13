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
	(void)iface;
}

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

void
htmx_extension_init_ext(HtmxExtension *self)
{
	HtmxExtensionInterface *iface;

	g_return_if_fail(HTMX_IS_EXTENSION(self));

	iface = HTMX_EXTENSION_GET_IFACE(self);
	if (iface->init != NULL) {
		iface->init(self);
	}
}

gboolean
htmx_extension_on_event(HtmxExtension *self,
                        const gchar   *event_name,
                        GHashTable    *detail)
{
	HtmxExtensionInterface *iface;

	g_return_val_if_fail(HTMX_IS_EXTENSION(self), TRUE);

	iface = HTMX_EXTENSION_GET_IFACE(self);
	if (iface->on_event != NULL) {
		return iface->on_event(self, event_name, detail);
	}

	return TRUE;
}

gchar *
htmx_extension_transform_response(HtmxExtension *self,
                                  const gchar   *content,
                                  const gchar   *content_type)
{
	HtmxExtensionInterface *iface;

	g_return_val_if_fail(HTMX_IS_EXTENSION(self), NULL);

	iface = HTMX_EXTENSION_GET_IFACE(self);
	if (iface->transform_response != NULL) {
		return iface->transform_response(self, content, content_type);
	}

	return NULL;
}

gboolean
htmx_extension_is_inline_swap(HtmxExtension *self,
                              const gchar   *swap_style)
{
	HtmxExtensionInterface *iface;

	g_return_val_if_fail(HTMX_IS_EXTENSION(self), FALSE);

	iface = HTMX_EXTENSION_GET_IFACE(self);
	if (iface->is_inline_swap != NULL) {
		return iface->is_inline_swap(self, swap_style);
	}

	return FALSE;
}

gboolean
htmx_extension_handle_swap(HtmxExtension *self,
                           const gchar   *swap_style,
                           gpointer       target,
                           const gchar   *fragment)
{
	HtmxExtensionInterface *iface;

	g_return_val_if_fail(HTMX_IS_EXTENSION(self), FALSE);

	iface = HTMX_EXTENSION_GET_IFACE(self);
	if (iface->handle_swap != NULL) {
		return iface->handle_swap(self, swap_style, target, fragment);
	}

	return FALSE;
}

void
htmx_extension_encode_parameters(HtmxExtension *self,
                                 gpointer       element,
                                 GHashTable    *parameters)
{
	HtmxExtensionInterface *iface;

	g_return_if_fail(HTMX_IS_EXTENSION(self));

	iface = HTMX_EXTENSION_GET_IFACE(self);
	if (iface->encode_parameters != NULL) {
		iface->encode_parameters(self, element, parameters);
	}
}

GPtrArray *
htmx_extension_get_selectors(HtmxExtension *self)
{
	HtmxExtensionInterface *iface;

	g_return_val_if_fail(HTMX_IS_EXTENSION(self), NULL);

	iface = HTMX_EXTENSION_GET_IFACE(self);
	if (iface->get_selectors != NULL) {
		return iface->get_selectors(self);
	}

	return NULL;
}

void
htmx_extension_header_filter(HtmxExtension *self,
                             GHashTable    *headers,
                             gboolean       is_request)
{
	HtmxExtensionInterface *iface;

	g_return_if_fail(HTMX_IS_EXTENSION(self));

	iface = HTMX_EXTENSION_GET_IFACE(self);
	if (iface->header_filter != NULL) {
		iface->header_filter(self, headers, is_request);
	}
}

void
htmx_extension_on_request(HtmxExtension *self,
                          gpointer       request)
{
	HtmxExtensionInterface *iface;

	g_return_if_fail(HTMX_IS_EXTENSION(self));

	iface = HTMX_EXTENSION_GET_IFACE(self);
	if (iface->on_request != NULL) {
		iface->on_request(self, request);
	}
}

void
htmx_extension_on_response(HtmxExtension *self,
                           gpointer       response)
{
	HtmxExtensionInterface *iface;

	g_return_if_fail(HTMX_IS_EXTENSION(self));

	iface = HTMX_EXTENSION_GET_IFACE(self);
	if (iface->on_response != NULL) {
		iface->on_response(self, response);
	}
}

void
htmx_extension_on_error(HtmxExtension *self,
                        const gchar   *message,
                        gpointer       request)
{
	HtmxExtensionInterface *iface;

	g_return_if_fail(HTMX_IS_EXTENSION(self));

	iface = HTMX_EXTENSION_GET_IFACE(self);
	if (iface->on_error != NULL) {
		iface->on_error(self, message, request);
	}
}

gboolean
htmx_extension_on_before_swap(HtmxExtension *self,
                              gpointer       response)
{
	HtmxExtensionInterface *iface;

	g_return_val_if_fail(HTMX_IS_EXTENSION(self), TRUE);

	iface = HTMX_EXTENSION_GET_IFACE(self);
	if (iface->on_before_swap != NULL) {
		return iface->on_before_swap(self, response);
	}

	return TRUE;
}

void
htmx_extension_on_after_swap(HtmxExtension *self,
                             gpointer       target)
{
	HtmxExtensionInterface *iface;

	g_return_if_fail(HTMX_IS_EXTENSION(self));

	iface = HTMX_EXTENSION_GET_IFACE(self);
	if (iface->on_after_swap != NULL) {
		iface->on_after_swap(self, target);
	}
}

gboolean
htmx_extension_on_before_settle(HtmxExtension *self,
                                gpointer       target)
{
	HtmxExtensionInterface *iface;

	g_return_val_if_fail(HTMX_IS_EXTENSION(self), TRUE);

	iface = HTMX_EXTENSION_GET_IFACE(self);
	if (iface->on_before_settle != NULL) {
		return iface->on_before_settle(self, target);
	}

	return TRUE;
}

void
htmx_extension_on_after_settle(HtmxExtension *self,
                               gpointer       target)
{
	HtmxExtensionInterface *iface;

	g_return_if_fail(HTMX_IS_EXTENSION(self));

	iface = HTMX_EXTENSION_GET_IFACE(self);
	if (iface->on_after_settle != NULL) {
		iface->on_after_settle(self, target);
	}
}
