/*
 * htmx-server.c - HTTP server wrapper implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-server.h"
#include "htmx-secret.h"
#include "htmx-response-private.h"

#include <errno.h>
#include <sys/socket.h>

struct _HtmxServer {
	GObject parent_instance;

	SoupServer     *soup_server;
	HtmxRouter     *router;
	HtmxConfig     *config;
	HtmxMiddleware *middleware;
	GMainLoop      *main_loop;
	gboolean        is_running;
	gboolean        explicit_listen;  /* htmx_server_listen_on() was used */
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
	} else if (htmx_response_is_deferred(response)) {
		/* Paused until the answer arrives; the loop serves others. */
		htmx_response_attach_message(response, msg);
	} else {
		htmx_response_apply(response, msg);
	}
}

/*
 * libsoup's copy of a request body -- the chunks read off the socket, and
 * the flattened buffer made from them -- is freed with the message, and
 * freeing does not clear it.  HtmxRequest's own copy wipes itself; this is
 * the other half.  "request-finished" is after the response is written,
 * when nothing reads the request again; libsoup emits "request-aborted"
 * instead for a dropped connection or a handler's 500, which is connected
 * to the same function so a failed login is no exception.
 *
 * Connected in htmx_server_new(), before anything an application adds to
 * the soup server, so it runs ahead of a handler that truncates the body:
 * truncating first would release the chunks before they were cleared.
 */
static void
wipe_request_body(
	SoupServer        *server,
	SoupServerMessage *msg,
	gpointer           user_data
){
	(void)server;
	(void)user_data;

	htmx_secret_wipe_message_body(soup_server_message_get_request_body(msg));
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
	self->explicit_listen = FALSE;
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
	g_signal_connect(self->soup_server, "request-finished",
	                 G_CALLBACK(wipe_request_body), NULL);
	g_signal_connect(self->soup_server, "request-aborted",
	                 G_CALLBACK(wipe_request_body), NULL);

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

/*
 * Give every listening socket the configured backlog.
 *
 * libsoup binds with GSocket's default of 10 and offers no way to ask for
 * more, and GSocket refuses g_socket_set_listen_backlog() once a socket
 * listens.  So the descriptor is told to listen again: on Linux and the
 * BSDs a second listen() on a listening socket changes only its backlog,
 * and every other choice libsoup made about binding stands.
 */
void
htmx_server_apply_listen_backlog(HtmxServer *self)
{
	GSList *listeners;
	GSList *iter;
	gint backlog;

	g_return_if_fail(HTMX_IS_SERVER(self));

	backlog = htmx_config_get_listen_backlog(self->config);
	listeners = soup_server_get_listeners(self->soup_server);
	for (iter = listeners; iter != NULL; iter = iter->next) {
		GSocket *socket = G_SOCKET(iter->data);

		if (listen(g_socket_get_fd(socket), backlog) != 0) {
			gint saved = errno;

			g_message("htmx-server: could not raise the listen backlog to %d: %s",
			          backlog, g_strerror(saved));
		}
	}
	g_slist_free(listeners);
}

/*
 * Bind one address, in whichever of soup's three forms fits it.
 */
static gboolean
bind_one(
	HtmxServer  *self,
	const gchar *host,
	guint16      port,
	GError     **error
){
	gboolean all = (host == NULL ||
	                g_strcmp0(host, "0.0.0.0") == 0 ||
	                g_strcmp0(host, "::") == 0);

	if (all) {
		return soup_server_listen_all(self->soup_server, port, 0, error);
	}

	if (g_strcmp0(host, "127.0.0.1") == 0 ||
	    g_strcmp0(host, "::1") == 0 ||
	    g_strcmp0(host, "localhost") == 0) {
		return soup_server_listen_local(self->soup_server, port, 0, error);
	}

	{
		GInetAddress   *inet_addr;
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

		return ok;
	}
}

static gboolean
listen_one(
	HtmxServer  *self,
	const gchar *host,
	guint16      port,
	GError     **error
){
	if (!bind_one(self, host, port, error)) {
		return FALSE;
	}
	htmx_server_apply_listen_backlog(self);
	return TRUE;
}

gboolean
htmx_server_listen_on(
	HtmxServer  *self,
	const gchar *host,
	guint16      port,
	GError     **error
){
	g_return_val_if_fail(HTMX_IS_SERVER(self), FALSE);

	if (!listen_one(self, host, port, error)) {
		return FALSE;
	}

	/*
	 * Recorded so that start() does not then bind the configured address
	 * as well.  A server told exactly where to listen must not also
	 * acquire a port nobody asked for -- which for the default host of
	 * "0.0.0.0" would be every interface on the machine.
	 */
	self->explicit_listen = TRUE;
	self->is_running = TRUE;

	return TRUE;
}

GSList *
htmx_server_get_listen_uris(HtmxServer *self)
{
	GSList *uris;
	GSList *out = NULL;
	GSList *iter;

	g_return_val_if_fail(HTMX_IS_SERVER(self), NULL);

	uris = soup_server_get_uris(self->soup_server);

	for (iter = uris; iter != NULL; iter = iter->next) {
		out = g_slist_prepend(out, g_uri_to_string(iter->data));
	}

	g_slist_free_full(uris, (GDestroyNotify)g_uri_unref);

	return g_slist_reverse(out);
}

gboolean
htmx_server_start(
	HtmxServer *self,
	GError    **error
){
	g_return_val_if_fail(HTMX_IS_SERVER(self), FALSE);

	if (self->is_running) {
		return TRUE;
	}

	if (!listen_one(self,
	                htmx_config_get_host(self->config),
	                htmx_config_get_port(self->config),
	                error)) {
		return FALSE;
	}

	self->is_running = TRUE;

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
	self->explicit_listen = FALSE;

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
