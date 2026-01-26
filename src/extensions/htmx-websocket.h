/*
 * htmx-websocket.h - WebSocket extension
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef HTMX_WEBSOCKET_H
#define HTMX_WEBSOCKET_H

#include <glib-object.h>
#include <gio/gio.h>
#include <libsoup/soup.h>

G_BEGIN_DECLS

#define HTMX_TYPE_WEBSOCKET_CONNECTION (htmx_websocket_connection_get_type())
G_DECLARE_FINAL_TYPE(HtmxWebSocketConnection, htmx_websocket_connection, HTMX, WEBSOCKET_CONNECTION, GObject)

/**
 * HtmxWebSocketConnection:
 *
 * Represents a WebSocket connection to a client.
 *
 * WebSocket connections allow bidirectional communication between
 * the server and connected clients in real-time.
 */

/**
 * HtmxWebSocketMessageCallback:
 * @connection: the #HtmxWebSocketConnection
 * @message: the received message
 * @user_data: user data passed to the callback
 *
 * Callback for receiving WebSocket messages.
 */
typedef void (*HtmxWebSocketMessageCallback)(HtmxWebSocketConnection *connection,
                                             const gchar             *message,
                                             gpointer                 user_data);

/**
 * HtmxWebSocketCloseCallback:
 * @connection: the #HtmxWebSocketConnection
 * @code: the close code
 * @reason: the close reason (may be %NULL)
 * @user_data: user data passed to the callback
 *
 * Callback for WebSocket connection close events.
 */
typedef void (*HtmxWebSocketCloseCallback)(HtmxWebSocketConnection *connection,
                                           guint                    code,
                                           const gchar             *reason,
                                           gpointer                 user_data);

HtmxWebSocketConnection *htmx_websocket_connection_new(SoupServerMessage *msg,
                                                       const gchar       *origin);

void htmx_websocket_connection_send_text(HtmxWebSocketConnection *self,
                                         const gchar             *message);

void htmx_websocket_connection_send_binary(HtmxWebSocketConnection *self,
                                           GBytes                  *data);

void htmx_websocket_connection_close(HtmxWebSocketConnection *self,
                                     guint                    code,
                                     const gchar             *reason);

gboolean htmx_websocket_connection_is_connected(HtmxWebSocketConnection *self);

void htmx_websocket_connection_set_message_callback(HtmxWebSocketConnection      *self,
                                                    HtmxWebSocketMessageCallback  callback,
                                                    gpointer                      user_data,
                                                    GDestroyNotify                destroy);

void htmx_websocket_connection_set_close_callback(HtmxWebSocketConnection    *self,
                                                  HtmxWebSocketCloseCallback  callback,
                                                  gpointer                    user_data,
                                                  GDestroyNotify              destroy);

const gchar *htmx_websocket_connection_get_origin(HtmxWebSocketConnection *self);

G_END_DECLS

#endif /* HTMX_WEBSOCKET_H */
