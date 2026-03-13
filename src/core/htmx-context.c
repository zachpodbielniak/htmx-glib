/*
 * htmx-context.c - Per-request context implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-context.h"

struct _HtmxContext {
	GObject parent_instance;

	HtmxRequest  *request;
	HtmxResponse *response;
	GHashTable   *params;
};

G_DEFINE_FINAL_TYPE(HtmxContext, htmx_context, G_TYPE_OBJECT)

static void
htmx_context_finalize(GObject *object)
{
	HtmxContext *self = HTMX_CONTEXT(object);

	g_clear_object(&self->request);
	g_clear_object(&self->response);
	g_clear_pointer(&self->params, g_hash_table_unref);

	G_OBJECT_CLASS(htmx_context_parent_class)->finalize(object);
}

static void
htmx_context_class_init(HtmxContextClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);

	object_class->finalize = htmx_context_finalize;
}

static void
htmx_context_init(HtmxContext *self)
{
	self->request = NULL;
	self->response = NULL;
	self->params = NULL;
}

HtmxContext *
htmx_context_new(HtmxRequest *request)
{
	HtmxContext *self;

	g_return_val_if_fail(HTMX_IS_REQUEST(request), NULL);

	self = g_object_new(HTMX_TYPE_CONTEXT, NULL);
	self->request = g_object_ref(request);

	return self;
}

HtmxRequest *
htmx_context_get_request(HtmxContext *self)
{
	g_return_val_if_fail(HTMX_IS_CONTEXT(self), NULL);

	return self->request;
}

HtmxResponse *
htmx_context_get_response(HtmxContext *self)
{
	g_return_val_if_fail(HTMX_IS_CONTEXT(self), NULL);

	return self->response;
}

void
htmx_context_set_response(
	HtmxContext  *self,
	HtmxResponse *response
){
	g_return_if_fail(HTMX_IS_CONTEXT(self));

	g_clear_object(&self->response);
	self->response = response; /* takes ownership (transfer full) */
}

GHashTable *
htmx_context_get_params(HtmxContext *self)
{
	g_return_val_if_fail(HTMX_IS_CONTEXT(self), NULL);

	return self->params;
}

void
htmx_context_set_params(
	HtmxContext *self,
	GHashTable  *params
){
	g_return_if_fail(HTMX_IS_CONTEXT(self));

	g_clear_pointer(&self->params, g_hash_table_unref);
	self->params = params; /* takes ownership (transfer full) */
}

const gchar *
htmx_context_get_param(
	HtmxContext *self,
	const gchar *name
){
	g_return_val_if_fail(HTMX_IS_CONTEXT(self), NULL);
	g_return_val_if_fail(name != NULL, NULL);

	if (self->params == NULL) {
		return NULL;
	}

	return (const gchar *)g_hash_table_lookup(self->params, name);
}

void
htmx_context_set_data(
	HtmxContext    *self,
	const gchar    *key,
	gpointer        data,
	GDestroyNotify  destroy
){
	g_return_if_fail(HTMX_IS_CONTEXT(self));
	g_return_if_fail(key != NULL);

	g_object_set_data_full(G_OBJECT(self), key, data, destroy);
}

gpointer
htmx_context_get_data(
	HtmxContext *self,
	const gchar *key
){
	g_return_val_if_fail(HTMX_IS_CONTEXT(self), NULL);
	g_return_val_if_fail(key != NULL, NULL);

	return g_object_get_data(G_OBJECT(self), key);
}
