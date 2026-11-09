/*
 * htmx-config.h - Server configuration
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef HTMX_CONFIG_H
#define HTMX_CONFIG_H

#include <glib-object.h>

G_BEGIN_DECLS

#define HTMX_TYPE_CONFIG (htmx_config_get_type())
G_DECLARE_FINAL_TYPE(HtmxConfig, htmx_config, HTMX, CONFIG, GObject)

/**
 * htmx_config_new:
 *
 * Creates a new #HtmxConfig with default values.
 *
 * Returns: (transfer full): a new #HtmxConfig
 */
HtmxConfig *
htmx_config_new(void);

/**
 * htmx_config_set_port:
 * @self: an #HtmxConfig
 * @port: the port number
 *
 * Sets the server port.
 */
void
htmx_config_set_port(
	HtmxConfig *self,
	guint16     port
);

/**
 * htmx_config_get_port:
 * @self: an #HtmxConfig
 *
 * Gets the server port.
 *
 * Returns: the port number
 */
guint16
htmx_config_get_port(HtmxConfig *self);

/**
 * htmx_config_set_host:
 * @self: an #HtmxConfig
 * @host: the host address
 *
 * Sets the host address to bind to.
 */
void
htmx_config_set_host(
	HtmxConfig  *self,
	const gchar *host
);

/**
 * htmx_config_get_host:
 * @self: an #HtmxConfig
 *
 * Gets the host address.
 *
 * Returns: (transfer none): the host address
 */
const gchar *
htmx_config_get_host(HtmxConfig *self);

/**
 * htmx_config_set_static_dir:
 * @self: an #HtmxConfig
 * @directory: the static files directory
 *
 * Sets the directory for static file serving.
 */
void
htmx_config_set_static_dir(
	HtmxConfig  *self,
	const gchar *directory
);

/**
 * htmx_config_get_static_dir:
 * @self: an #HtmxConfig
 *
 * Gets the static files directory.
 *
 * Returns: (transfer none) (nullable): the directory, or %NULL
 */
const gchar *
htmx_config_get_static_dir(HtmxConfig *self);

/**
 * HTMX_CONFIG_DEFAULT_LISTEN_BACKLOG:
 *
 * The listen backlog a server asks for unless told otherwise.  GSocket's
 * own default is 10, which a burst of clients connecting at once
 * overflows: the kernel drops their SYNs and they wait a second or more
 * to retry.  The kernel clamps the value to net.core.somaxconn.
 */
#define HTMX_CONFIG_DEFAULT_LISTEN_BACKLOG (1024)

/**
 * htmx_config_set_listen_backlog:
 * @self: an #HtmxConfig
 * @backlog: connections the kernel may queue before they are accepted
 *
 * Sets the listen backlog of every socket the server binds.
 */
void
htmx_config_set_listen_backlog(
	HtmxConfig *self,
	gint        backlog
);

/**
 * htmx_config_get_listen_backlog:
 * @self: an #HtmxConfig
 *
 * Returns: the listen backlog
 */
gint
htmx_config_get_listen_backlog(HtmxConfig *self);

G_END_DECLS

#endif /* HTMX_CONFIG_H */
