/*
 * htmx-server.c - HTTP server wrapper implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-server.h"

struct _HtmxServer {
	GObject parent_instance;

	SoupServer     *soup_server;
	HtmxRouter     *router;
	HtmxConfig     *config;
	HtmxMiddleware *middleware;
	GMainLoop      *main_loop;
	gboolean        is_running;
};

G_DEFINE_FINAL_TYPE(HtmxServer, htmx_server, G_TYPE_OBJECT)

/*
 * Terminal middleware: dispatches the request through the router.
 * This is called as the final step in the middleware chain.
 */
static void
router_dispatch(
	HtmxContext        *context,
	HtmxMiddlewareNext  next,
	gpointer            next_data,
	gpointer            user_data
){
	HtmxRouter *router = HTMX_ROUTER(user_data);
	HtmxRequest *request;
	HtmxResponse *response;
	GHashTable *params = NULL;

	(void)next;
	(void)next_data;

	request = htmx_context_get_request(context);
	response = htmx_router_match(router, request, &params);

	if (params != NULL) {
		htmx_context_set_params(context, params);
	}

	if (response != NULL) {
		htmx_context_set_response(context, response);
	}
}

/*
 * Handle incoming requests through the middleware pipeline.
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
	g_autoptr(HtmxContext) context = NULL;
	HtmxResponse *response;

	(void)server;
	(void)path;
	(void)query;

	request = htmx_request_new_from_message(msg);
	context = htmx_context_new(request);

	/* Run middleware pipeline with router dispatch as terminal handler */
	htmx_middleware_run(self->middleware, context,
	                    router_dispatch, self->router);

	response = htmx_context_get_response(context);
	if (response == NULL) {
		g_autoptr(HtmxResponse) not_found = htmx_response_not_found();
		htmx_response_apply(not_found, msg);
	} else {
		htmx_response_apply(response, msg);
	}
}

static void
htmx_server_finalize(GObject *object)
{
	HtmxServer *self = HTMX_SERVER(object);

	g_clear_object(&self->soup_server);
	g_clear_object(&self->router);
	g_clear_object(&self->config);
	g_clear_object(&self->middleware);
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
	self->middleware = htmx_middleware_new();
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

HtmxMiddleware *
htmx_server_get_middleware(HtmxServer *self)
{
	g_return_val_if_fail(HTMX_IS_SERVER(self), NULL);

	return self->middleware;
}

void
htmx_server_use(
	HtmxServer         *self,
	HtmxMiddlewareFunc  func,
	gpointer            user_data,
	GDestroyNotify      destroy
){
	g_return_if_fail(HTMX_IS_SERVER(self));
	g_return_if_fail(func != NULL);

	htmx_middleware_use(self->middleware, func, user_data, destroy);
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

	{
		const gchar *host = htmx_config_get_host(self->config);
		gboolean all = (host == NULL ||
		                g_strcmp0(host, "0.0.0.0") == 0 ||
		                g_strcmp0(host, "::") == 0);

		if (all) {
			if (!soup_server_listen_all(self->soup_server, port, 0, error))
				return FALSE;
		} else if (g_strcmp0(host, "127.0.0.1") == 0 ||
		           g_strcmp0(host, "localhost") == 0) {
			if (!soup_server_listen_local(self->soup_server, port, 0, error))
				return FALSE;
		} else {
			GInetAddress  *inet_addr;
			GSocketAddress *sock_addr;
			gboolean        ok;

			inet_addr = g_inet_address_new_from_string(host);
			if (inet_addr == NULL) {
				g_set_error(error,
				            G_IO_ERROR, G_IO_ERROR_INVALID_ARGUMENT,
				            "Invalid host address: %s", host);
				return FALSE;
			}
			sock_addr = g_inet_socket_address_new(inet_addr, port);
			ok = soup_server_listen(self->soup_server, sock_addr, 0, error);
			g_object_unref(sock_addr);
			g_object_unref(inet_addr);
			if (!ok)
				return FALSE;
		}
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
