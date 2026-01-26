/*
 * htmx-server.h - HTTP server wrapper
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef HTMX_SERVER_H
#define HTMX_SERVER_H

#include <glib-object.h>
#include <libsoup/soup.h>
#include "htmx-router.h"
#include "htmx-config.h"

G_BEGIN_DECLS

#define HTMX_TYPE_SERVER (htmx_server_get_type())
G_DECLARE_FINAL_TYPE(HtmxServer, htmx_server, HTMX, SERVER, GObject)

/**
 * htmx_server_new:
 *
 * Creates a new #HtmxServer with default configuration.
 *
 * Returns: (transfer full): a new #HtmxServer
 */
HtmxServer *
htmx_server_new(void);

/**
 * htmx_server_new_with_port:
 * @port: the port to listen on
 *
 * Creates a new #HtmxServer listening on the specified port.
 *
 * Returns: (transfer full): a new #HtmxServer
 */
HtmxServer *
htmx_server_new_with_port(guint16 port);

/**
 * htmx_server_new_with_config:
 * @config: server configuration
 *
 * Creates a new #HtmxServer with the given configuration.
 *
 * Returns: (transfer full): a new #HtmxServer
 */
HtmxServer *
htmx_server_new_with_config(HtmxConfig *config);

/**
 * htmx_server_get_router:
 * @self: an #HtmxServer
 *
 * Gets the router for adding routes.
 *
 * Returns: (transfer none): the #HtmxRouter
 */
HtmxRouter *
htmx_server_get_router(HtmxServer *self);

/**
 * htmx_server_get_soup_server:
 * @self: an #HtmxServer
 *
 * Gets the underlying SoupServer.
 *
 * Returns: (transfer none): the #SoupServer
 */
SoupServer *
htmx_server_get_soup_server(HtmxServer *self);

/**
 * htmx_server_start:
 * @self: an #HtmxServer
 * @error: (nullable): return location for an error
 *
 * Starts the server listening for connections.
 *
 * Returns: %TRUE on success
 */
gboolean
htmx_server_start(
	HtmxServer *self,
	GError    **error
);

/**
 * htmx_server_stop:
 * @self: an #HtmxServer
 *
 * Stops the server.
 */
void
htmx_server_stop(HtmxServer *self);

/**
 * htmx_server_run:
 * @self: an #HtmxServer
 *
 * Runs the server's main loop (blocking).
 */
void
htmx_server_run(HtmxServer *self);

/**
 * htmx_server_is_running:
 * @self: an #HtmxServer
 *
 * Checks if the server is currently running.
 *
 * Returns: %TRUE if running
 */
gboolean
htmx_server_is_running(HtmxServer *self);

G_END_DECLS

#endif /* HTMX_SERVER_H */
