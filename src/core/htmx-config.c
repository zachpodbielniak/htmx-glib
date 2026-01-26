/*
 * htmx-config.c - Server configuration implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-config.h"

struct _HtmxConfig {
	GObject parent_instance;

	guint16  port;
	gchar   *host;
	gchar   *static_dir;
};

G_DEFINE_FINAL_TYPE(HtmxConfig, htmx_config, G_TYPE_OBJECT)

static void
htmx_config_finalize(GObject *object)
{
	HtmxConfig *self = HTMX_CONFIG(object);

	g_free(self->host);
	g_free(self->static_dir);

	G_OBJECT_CLASS(htmx_config_parent_class)->finalize(object);
}

static void
htmx_config_class_init(HtmxConfigClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);

	object_class->finalize = htmx_config_finalize;
}

static void
htmx_config_init(HtmxConfig *self)
{
	self->port = 8080;
	self->host = g_strdup("127.0.0.1");
	self->static_dir = NULL;
}

HtmxConfig *
htmx_config_new(void)
{
	return g_object_new(HTMX_TYPE_CONFIG, NULL);
}

void
htmx_config_set_port(
	HtmxConfig *self,
	guint16     port
){
	g_return_if_fail(HTMX_IS_CONFIG(self));

	self->port = port;
}

guint16
htmx_config_get_port(HtmxConfig *self)
{
	g_return_val_if_fail(HTMX_IS_CONFIG(self), 8080);

	return self->port;
}

void
htmx_config_set_host(
	HtmxConfig  *self,
	const gchar *host
){
	g_return_if_fail(HTMX_IS_CONFIG(self));

	g_free(self->host);
	self->host = g_strdup(host);
}

const gchar *
htmx_config_get_host(HtmxConfig *self)
{
	g_return_val_if_fail(HTMX_IS_CONFIG(self), NULL);

	return self->host;
}

void
htmx_config_set_static_dir(
	HtmxConfig  *self,
	const gchar *directory
){
	g_return_if_fail(HTMX_IS_CONFIG(self));

	g_free(self->static_dir);
	self->static_dir = g_strdup(directory);
}

const gchar *
htmx_config_get_static_dir(HtmxConfig *self)
{
	g_return_val_if_fail(HTMX_IS_CONFIG(self), NULL);

	return self->static_dir;
}
