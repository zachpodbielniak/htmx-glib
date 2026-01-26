/*
 * htmx-websocket.c - WebSocket extension implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-websocket.h"

struct _HtmxWebSocketConnection {
	GObject parent_instance;

	SoupWebsocketConnection *websocket;
	gchar                   *origin;
	gboolean                 connected;

	HtmxWebSocketMessageCallback message_callback;
	gpointer                     message_user_data;
	GDestroyNotify               message_destroy;

	HtmxWebSocketCloseCallback close_callback;
	gpointer                   close_user_data;
	GDestroyNotify             close_destroy;
};

G_DEFINE_FINAL_TYPE(HtmxWebSocketConnection, htmx_websocket_connection, G_TYPE_OBJECT)

static void
on_websocket_message(SoupWebsocketConnection *ws,
                     SoupWebsocketDataType    type,
                     GBytes                  *message,
                     gpointer                 user_data)
{
	HtmxWebSocketConnection *self = HTMX_WEBSOCKET_CONNECTION(user_data);
	gconstpointer data;
	gsize size;

	if (type == SOUP_WEBSOCKET_DATA_TEXT && self->message_callback != NULL) {
		data = g_bytes_get_data(message, &size);
		/* Create null-terminated string */
		g_autofree gchar *text = g_strndup(data, size);
		self->message_callback(self, text, self->message_user_data);
	}
}

static void
on_websocket_closed(SoupWebsocketConnection *ws, gpointer user_data)
{
	HtmxWebSocketConnection *self = HTMX_WEBSOCKET_CONNECTION(user_data);
	guint code;
	const gchar *reason;

	self->connected = FALSE;

	if (self->close_callback != NULL) {
		code = soup_websocket_connection_get_close_code(ws);
		reason = soup_websocket_connection_get_close_data(ws);
		self->close_callback(self, code, reason, self->close_user_data);
	}
}

static void
htmx_websocket_connection_dispose(GObject *object)
{
	HtmxWebSocketConnection *self = HTMX_WEBSOCKET_CONNECTION(object);

	if (self->connected && self->websocket != NULL) {
		soup_websocket_connection_close(self->websocket,
		                                SOUP_WEBSOCKET_CLOSE_NORMAL,
		                                NULL);
	}

	if (self->message_destroy != NULL && self->message_user_data != NULL) {
		self->message_destroy(self->message_user_data);
		self->message_user_data = NULL;
	}

	if (self->close_destroy != NULL && self->close_user_data != NULL) {
		self->close_destroy(self->close_user_data);
		self->close_user_data = NULL;
	}

	g_clear_object(&self->websocket);

	G_OBJECT_CLASS(htmx_websocket_connection_parent_class)->dispose(object);
}

static void
htmx_websocket_connection_finalize(GObject *object)
{
	HtmxWebSocketConnection *self = HTMX_WEBSOCKET_CONNECTION(object);

	g_free(self->origin);

	G_OBJECT_CLASS(htmx_websocket_connection_parent_class)->finalize(object);
}

static void
htmx_websocket_connection_class_init(HtmxWebSocketConnectionClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);

	object_class->dispose = htmx_websocket_connection_dispose;
	object_class->finalize = htmx_websocket_connection_finalize;
}

static void
htmx_websocket_connection_init(HtmxWebSocketConnection *self)
{
	self->websocket = NULL;
	self->origin = NULL;
	self->connected = FALSE;
	self->message_callback = NULL;
	self->message_user_data = NULL;
	self->message_destroy = NULL;
	self->close_callback = NULL;
	self->close_user_data = NULL;
	self->close_destroy = NULL;
}

/**
 * htmx_websocket_connection_new:
 * @msg: (transfer none): the #SoupServerMessage for the WebSocket upgrade
 * @origin: (nullable): the expected origin for the connection
 *
 * Creates a new WebSocket connection from a server message.
 * The message must be a valid WebSocket upgrade request.
 *
 * Returns: (transfer full) (nullable): a new #HtmxWebSocketConnection,
 *          or %NULL if the upgrade failed
 */
HtmxWebSocketConnection *
htmx_websocket_connection_new(SoupServerMessage *msg, const gchar *origin)
{
	HtmxWebSocketConnection *self;
	SoupMessageHeaders *req_headers;
	const gchar *connection_header;
	const gchar *upgrade_header;

	g_return_val_if_fail(SOUP_IS_SERVER_MESSAGE(msg), NULL);

	/* Verify this is a WebSocket upgrade request */
	req_headers = soup_server_message_get_request_headers(msg);
	connection_header = soup_message_headers_get_one(req_headers, "Connection");
	upgrade_header = soup_message_headers_get_one(req_headers, "Upgrade");

	if (connection_header == NULL ||
	    g_ascii_strcasecmp(connection_header, "upgrade") != 0) {
		return NULL;
	}

	if (upgrade_header == NULL ||
	    g_ascii_strcasecmp(upgrade_header, "websocket") != 0) {
		return NULL;
	}

	self = g_object_new(HTMX_TYPE_WEBSOCKET_CONNECTION, NULL);
	self->origin = g_strdup(origin);

	/*
	 * Note: Actual WebSocket connection setup requires using
	 * soup_server_add_websocket_handler() on the SoupServer.
	 * This object wraps an existing SoupWebsocketConnection.
	 * The actual upgrade happens in the server callback.
	 */

	return self;
}

/**
 * htmx_websocket_connection_send_text:
 * @self: an #HtmxWebSocketConnection
 * @message: the text message to send
 *
 * Sends a text message to the connected WebSocket client.
 */
void
htmx_websocket_connection_send_text(HtmxWebSocketConnection *self,
                                    const gchar             *message)
{
	g_return_if_fail(HTMX_IS_WEBSOCKET_CONNECTION(self));
	g_return_if_fail(message != NULL);

	if (!self->connected || self->websocket == NULL) {
		return;
	}

	soup_websocket_connection_send_text(self->websocket, message);
}

/**
 * htmx_websocket_connection_send_binary:
 * @self: an #HtmxWebSocketConnection
 * @data: the binary data to send
 *
 * Sends binary data to the connected WebSocket client.
 */
void
htmx_websocket_connection_send_binary(HtmxWebSocketConnection *self,
                                      GBytes                  *data)
{
	gconstpointer bytes_data;
	gsize bytes_size;

	g_return_if_fail(HTMX_IS_WEBSOCKET_CONNECTION(self));
	g_return_if_fail(data != NULL);

	if (!self->connected || self->websocket == NULL) {
		return;
	}

	bytes_data = g_bytes_get_data(data, &bytes_size);
	soup_websocket_connection_send_binary(self->websocket, bytes_data, bytes_size);
}

/**
 * htmx_websocket_connection_close:
 * @self: an #HtmxWebSocketConnection
 * @code: the close code (e.g., 1000 for normal closure)
 * @reason: (nullable): the close reason
 *
 * Closes the WebSocket connection with the specified code and reason.
 */
void
htmx_websocket_connection_close(HtmxWebSocketConnection *self,
                                guint                    code,
                                const gchar             *reason)
{
	g_return_if_fail(HTMX_IS_WEBSOCKET_CONNECTION(self));

	if (!self->connected || self->websocket == NULL) {
		return;
	}

	soup_websocket_connection_close(self->websocket, (gushort)code, reason);
	self->connected = FALSE;
}

/**
 * htmx_websocket_connection_is_connected:
 * @self: an #HtmxWebSocketConnection
 *
 * Checks if the WebSocket connection is still active.
 *
 * Returns: %TRUE if connected, %FALSE otherwise
 */
gboolean
htmx_websocket_connection_is_connected(HtmxWebSocketConnection *self)
{
	SoupWebsocketState state;

	g_return_val_if_fail(HTMX_IS_WEBSOCKET_CONNECTION(self), FALSE);

	if (!self->connected || self->websocket == NULL) {
		return FALSE;
	}

	state = soup_websocket_connection_get_state(self->websocket);
	return state == SOUP_WEBSOCKET_STATE_OPEN;
}

/**
 * htmx_websocket_connection_set_message_callback:
 * @self: an #HtmxWebSocketConnection
 * @callback: (nullable): the callback function
 * @user_data: (nullable): data to pass to the callback
 * @destroy: (nullable): function to free user_data
 *
 * Sets the callback for receiving text messages.
 */
void
htmx_websocket_connection_set_message_callback(HtmxWebSocketConnection      *self,
                                               HtmxWebSocketMessageCallback  callback,
                                               gpointer                      user_data,
                                               GDestroyNotify                destroy)
{
	g_return_if_fail(HTMX_IS_WEBSOCKET_CONNECTION(self));

	/* Free existing user data */
	if (self->message_destroy != NULL && self->message_user_data != NULL) {
		self->message_destroy(self->message_user_data);
	}

	self->message_callback = callback;
	self->message_user_data = user_data;
	self->message_destroy = destroy;
}

/**
 * htmx_websocket_connection_set_close_callback:
 * @self: an #HtmxWebSocketConnection
 * @callback: (nullable): the callback function
 * @user_data: (nullable): data to pass to the callback
 * @destroy: (nullable): function to free user_data
 *
 * Sets the callback for connection close events.
 */
void
htmx_websocket_connection_set_close_callback(HtmxWebSocketConnection    *self,
                                             HtmxWebSocketCloseCallback  callback,
                                             gpointer                    user_data,
                                             GDestroyNotify              destroy)
{
	g_return_if_fail(HTMX_IS_WEBSOCKET_CONNECTION(self));

	/* Free existing user data */
	if (self->close_destroy != NULL && self->close_user_data != NULL) {
		self->close_destroy(self->close_user_data);
	}

	self->close_callback = callback;
	self->close_user_data = user_data;
	self->close_destroy = destroy;
}

/**
 * htmx_websocket_connection_get_origin:
 * @self: an #HtmxWebSocketConnection
 *
 * Gets the origin of the WebSocket connection.
 *
 * Returns: (transfer none) (nullable): the connection origin
 */
const gchar *
htmx_websocket_connection_get_origin(HtmxWebSocketConnection *self)
{
	g_return_val_if_fail(HTMX_IS_WEBSOCKET_CONNECTION(self), NULL);

	return self->origin;
}

/*
 * Internal: Attach a SoupWebsocketConnection
 * Called by the server when the WebSocket handshake completes.
 */
void
htmx_websocket_connection_attach(HtmxWebSocketConnection *self,
                                 SoupWebsocketConnection *websocket)
{
	g_return_if_fail(HTMX_IS_WEBSOCKET_CONNECTION(self));
	g_return_if_fail(SOUP_IS_WEBSOCKET_CONNECTION(websocket));

	g_clear_object(&self->websocket);
	self->websocket = g_object_ref(websocket);
	self->connected = TRUE;

	g_signal_connect(websocket, "message",
	                 G_CALLBACK(on_websocket_message), self);
	g_signal_connect(websocket, "closed",
	                 G_CALLBACK(on_websocket_closed), self);
}
