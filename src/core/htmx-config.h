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

G_END_DECLS

#endif /* HTMX_CONFIG_H */
