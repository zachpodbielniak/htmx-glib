/*
 * htmx-sse.h - Server-Sent Events extension
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef HTMX_SSE_H
#define HTMX_SSE_H

#include <glib-object.h>
#include <gio/gio.h>
#include <libsoup/soup.h>

G_BEGIN_DECLS

#define HTMX_TYPE_SSE_CONNECTION (htmx_sse_connection_get_type())
G_DECLARE_FINAL_TYPE(HtmxSseConnection, htmx_sse_connection, HTMX, SSE_CONNECTION, GObject)

/**
 * HtmxSseConnection:
 *
 * Represents a Server-Sent Events connection to a client.
 *
 * SSE connections allow the server to push updates to connected clients
 * in real-time. Events can have an event type, data, and optional ID.
 */

HtmxSseConnection *htmx_sse_connection_new(SoupServerMessage *msg);

void htmx_sse_connection_send_event(HtmxSseConnection *self,
                                    const gchar       *event,
                                    const gchar       *data,
                                    const gchar       *id);

void htmx_sse_connection_send_data(HtmxSseConnection *self,
                                   const gchar       *data);

void htmx_sse_connection_send_comment(HtmxSseConnection *self,
                                      const gchar       *comment);

void htmx_sse_connection_set_retry(HtmxSseConnection *self,
                                   guint              retry_ms);

void htmx_sse_connection_close(HtmxSseConnection *self);

gboolean htmx_sse_connection_is_connected(HtmxSseConnection *self);

G_END_DECLS

#endif /* HTMX_SSE_H */
