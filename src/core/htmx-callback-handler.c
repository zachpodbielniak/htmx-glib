/*
 * htmx-callback-handler.c - Closure-based handler implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-callback-handler.h"

static void htmx_callback_handler_iface_init(HtmxHandlerInterface *iface);

struct _HtmxCallbackHandler {
	GObject            parent_instance;
	HtmxRouteCallback  callback;
	gpointer           user_data;
	GDestroyNotify     destroy;
};

G_DEFINE_FINAL_TYPE_WITH_CODE(HtmxCallbackHandler, htmx_callback_handler, G_TYPE_OBJECT,
	G_IMPLEMENT_INTERFACE(HTMX_TYPE_HANDLER, htmx_callback_handler_iface_init))

static void
htmx_callback_handler_finalize(GObject *object)
{
	HtmxCallbackHandler *self = HTMX_CALLBACK_HANDLER(object);

	if (self->destroy != NULL && self->user_data != NULL)
		self->destroy(self->user_data);

	G_OBJECT_CLASS(htmx_callback_handler_parent_class)->finalize(object);
}

static void
htmx_callback_handler_class_init(HtmxCallbackHandlerClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);
	object_class->finalize = htmx_callback_handler_finalize;
}

static void
htmx_callback_handler_init(HtmxCallbackHandler *self)
{
	self->callback = NULL;
	self->user_data = NULL;
	self->destroy = NULL;
}

static HtmxResponse *
htmx_callback_handler_handle(HtmxHandler *handler,
                              HtmxRequest *request,
                              GHashTable  *params)
{
	HtmxCallbackHandler *self = HTMX_CALLBACK_HANDLER(handler);

	g_return_val_if_fail(self->callback != NULL, NULL);

	return self->callback(request, params, self->user_data);
}

static void
htmx_callback_handler_iface_init(HtmxHandlerInterface *iface)
{
	iface->handle = htmx_callback_handler_handle;
}

HtmxCallbackHandler *
htmx_callback_handler_new(HtmxRouteCallback  callback,
                           gpointer           user_data,
                           GDestroyNotify     destroy)
{
	HtmxCallbackHandler *self;

	g_return_val_if_fail(callback != NULL, NULL);

	self = g_object_new(HTMX_TYPE_CALLBACK_HANDLER, NULL);
	self->callback = callback;
	self->user_data = user_data;
	self->destroy = destroy;

	return self;
}
