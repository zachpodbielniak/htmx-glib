/*
 * htmx-server.c - HTTP server wrapper implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-server.h"

struct _HtmxServer {
	GObject parent_instance;

	SoupServer *soup_server;
	HtmxRouter *router;
	HtmxConfig *config;
	GMainLoop  *main_loop;
	gboolean    is_running;
};

G_DEFINE_FINAL_TYPE(HtmxServer, htmx_server, G_TYPE_OBJECT)

/*
 * Handle incoming requests
 */
static void
handle_request(
	SoupServer        *server,
	SoupServerMessage *msg,
	const char        *path,
	GHashTable        *query,
	gpointer           user_data
){
	HtmxServer *self = HTMX_SERVER(user_data);
	g_autoptr(HtmxRequest) request = NULL;
	g_autoptr(HtmxResponse) response = NULL;
	GHashTable *params = NULL;

	request = htmx_request_new_from_message(msg);
	response = htmx_router_match(self->router, request, &params);

	if (response == NULL) {
		response = htmx_response_not_found();
	}

	htmx_response_apply(response, msg);

	if (params != NULL) {
		g_hash_table_unref(params);
	}
}

static void
htmx_server_finalize(GObject *object)
{
	HtmxServer *self = HTMX_SERVER(object);

	g_clear_object(&self->soup_server);
	g_clear_object(&self->router);
	g_clear_object(&self->config);
	g_clear_pointer(&self->main_loop, g_main_loop_unref);

	G_OBJECT_CLASS(htmx_server_parent_class)->finalize(object);
}

static void
htmx_server_class_init(HtmxServerClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);

	object_class->finalize = htmx_server_finalize;
}

static void
htmx_server_init(HtmxServer *self)
{
	self->soup_server = NULL;
	self->router = htmx_router_new();
	self->config = htmx_config_new();
	self->main_loop = NULL;
	self->is_running = FALSE;
}

HtmxServer *
htmx_server_new(void)
{
	HtmxServer *self;

	self = g_object_new(HTMX_TYPE_SERVER, NULL);
	self->soup_server = soup_server_new(NULL, NULL);

	soup_server_add_handler(self->soup_server, NULL, handle_request, self, NULL);

	return self;
}

HtmxServer *
htmx_server_new_with_port(guint16 port)
{
	HtmxServer *self;

	self = htmx_server_new();
	htmx_config_set_port(self->config, port);

	return self;
}

HtmxServer *
htmx_server_new_with_config(HtmxConfig *config)
{
	HtmxServer *self;

	g_return_val_if_fail(HTMX_IS_CONFIG(config), NULL);

	self = htmx_server_new();
	g_object_unref(self->config);
	self->config = g_object_ref(config);

	return self;
}

HtmxRouter *
htmx_server_get_router(HtmxServer *self)
{
	g_return_val_if_fail(HTMX_IS_SERVER(self), NULL);

	return self->router;
}

SoupServer *
htmx_server_get_soup_server(HtmxServer *self)
{
	g_return_val_if_fail(HTMX_IS_SERVER(self), NULL);

	return self->soup_server;
}

gboolean
htmx_server_start(
	HtmxServer *self,
	GError    **error
){
	guint16 port;
	GSList *uris;

	g_return_val_if_fail(HTMX_IS_SERVER(self), FALSE);

	if (self->is_running) {
		return TRUE;
	}

	port = htmx_config_get_port(self->config);

	if (!soup_server_listen_local(self->soup_server, port, 0, error)) {
		return FALSE;
	}

	self->is_running = TRUE;

	uris = soup_server_get_uris(self->soup_server);
	if (uris != NULL) {
		g_autofree gchar *uri_str = g_uri_to_string(uris->data);
		g_print("Server listening on %s\n", uri_str);
		g_slist_free_full(uris, (GDestroyNotify)g_uri_unref);
	}

	return TRUE;
}

void
htmx_server_stop(HtmxServer *self)
{
	g_return_if_fail(HTMX_IS_SERVER(self));

	if (!self->is_running) {
		return;
	}

	soup_server_disconnect(self->soup_server);
	self->is_running = FALSE;

	if (self->main_loop != NULL && g_main_loop_is_running(self->main_loop)) {
		g_main_loop_quit(self->main_loop);
	}
}

void
htmx_server_run(HtmxServer *self)
{
	GError *error = NULL;

	g_return_if_fail(HTMX_IS_SERVER(self));

	if (!htmx_server_start(self, &error)) {
		g_warning("Failed to start server: %s", error->message);
		g_error_free(error);
		return;
	}

	self->main_loop = g_main_loop_new(NULL, FALSE);
	g_main_loop_run(self->main_loop);
}

gboolean
htmx_server_is_running(HtmxServer *self)
{
	g_return_val_if_fail(HTMX_IS_SERVER(self), FALSE);

	return self->is_running;
}
