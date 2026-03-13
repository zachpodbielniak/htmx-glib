/*
 * htmx-middleware.c - Middleware pipeline implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-middleware.h"

typedef struct {
	HtmxMiddlewareFunc func;
	gpointer           user_data;
	GDestroyNotify     destroy;
} MiddlewareEntry;

/*
 * State passed through the recursive middleware chain.
 * Each level captures the remaining middleware list and
 * the terminal handler.
 */
typedef struct {
	GList              *remaining;
	HtmxMiddlewareFunc  handler;
	gpointer            handler_data;
} ChainState;

struct _HtmxMiddleware {
	GObject parent_instance;

	GList *entries; /* list of MiddlewareEntry* */
};

G_DEFINE_FINAL_TYPE(HtmxMiddleware, htmx_middleware, G_TYPE_OBJECT)

static void
middleware_entry_free(MiddlewareEntry *entry)
{
	if (entry == NULL) {
		return;
	}

	if (entry->destroy != NULL && entry->user_data != NULL) {
		entry->destroy(entry->user_data);
	}

	g_slice_free(MiddlewareEntry, entry);
}

static void
htmx_middleware_finalize(GObject *object)
{
	HtmxMiddleware *self = HTMX_MIDDLEWARE(object);

	g_list_free_full(self->entries, (GDestroyNotify)middleware_entry_free);

	G_OBJECT_CLASS(htmx_middleware_parent_class)->finalize(object);
}

static void
htmx_middleware_class_init(HtmxMiddlewareClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);

	object_class->finalize = htmx_middleware_finalize;
}

static void
htmx_middleware_init(HtmxMiddleware *self)
{
	self->entries = NULL;
}

/*
 * No-op continuation for the terminal handler.
 */
static void
noop_next(HtmxContext *context, gpointer data)
{
	(void)context;
	(void)data;
}

/*
 * Recursive chain runner. Each call pops the head of the remaining
 * list, calls that middleware, and passes a chain_next pointing
 * at the tail as the "next" continuation. When the list is
 * exhausted, the terminal handler is invoked.
 */
static void
chain_next(HtmxContext *context, gpointer data)
{
	ChainState *state = (ChainState *)data;
	MiddlewareEntry *entry;
	ChainState next_state;

	/* If a response was already set, stop the chain */
	if (htmx_context_get_response(context) != NULL) {
		return;
	}

	/* End of middleware list — call terminal handler */
	if (state->remaining == NULL) {
		if (state->handler != NULL) {
			state->handler(context, noop_next, NULL, state->handler_data);
		}
		return;
	}

	entry = (MiddlewareEntry *)state->remaining->data;
	next_state.remaining = state->remaining->next;
	next_state.handler = state->handler;
	next_state.handler_data = state->handler_data;

	entry->func(context, chain_next, &next_state, entry->user_data);
}

HtmxMiddleware *
htmx_middleware_new(void)
{
	return g_object_new(HTMX_TYPE_MIDDLEWARE, NULL);
}

void
htmx_middleware_use(
	HtmxMiddleware     *self,
	HtmxMiddlewareFunc  func,
	gpointer            user_data,
	GDestroyNotify      destroy
){
	MiddlewareEntry *entry;

	g_return_if_fail(HTMX_IS_MIDDLEWARE(self));
	g_return_if_fail(func != NULL);

	entry = g_slice_new0(MiddlewareEntry);
	entry->func = func;
	entry->user_data = user_data;
	entry->destroy = destroy;

	self->entries = g_list_append(self->entries, entry);
}

void
htmx_middleware_use_before(
	HtmxMiddleware     *self,
	HtmxMiddlewareFunc  func,
	gpointer            user_data,
	GDestroyNotify      destroy
){
	MiddlewareEntry *entry;

	g_return_if_fail(HTMX_IS_MIDDLEWARE(self));
	g_return_if_fail(func != NULL);

	entry = g_slice_new0(MiddlewareEntry);
	entry->func = func;
	entry->user_data = user_data;
	entry->destroy = destroy;

	self->entries = g_list_prepend(self->entries, entry);
}

void
htmx_middleware_run(
	HtmxMiddleware     *self,
	HtmxContext        *context,
	HtmxMiddlewareFunc  handler,
	gpointer            handler_data
){
	ChainState state;

	g_return_if_fail(HTMX_IS_MIDDLEWARE(self));
	g_return_if_fail(HTMX_IS_CONTEXT(context));

	state.remaining = self->entries;
	state.handler = handler;
	state.handler_data = handler_data;

	chain_next(context, &state);
}

guint
htmx_middleware_get_length(HtmxMiddleware *self)
{
	g_return_val_if_fail(HTMX_IS_MIDDLEWARE(self), 0);

	return g_list_length(self->entries);
}
