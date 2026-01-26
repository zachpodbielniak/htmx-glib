/*
 * htmx-sse.c - Server-Sent Events extension implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-sse.h"

struct _HtmxSseConnection {
	GObject parent_instance;

	SoupServerMessage *message;
	GOutputStream     *output;
	gboolean           connected;
};

G_DEFINE_FINAL_TYPE(HtmxSseConnection, htmx_sse_connection, G_TYPE_OBJECT)

static void
htmx_sse_connection_dispose(GObject *object)
{
	HtmxSseConnection *self = HTMX_SSE_CONNECTION(object);

	if (self->connected) {
		htmx_sse_connection_close(self);
	}

	g_clear_object(&self->message);
	g_clear_object(&self->output);

	G_OBJECT_CLASS(htmx_sse_connection_parent_class)->dispose(object);
}

static void
htmx_sse_connection_class_init(HtmxSseConnectionClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);

	object_class->dispose = htmx_sse_connection_dispose;
}

static void
htmx_sse_connection_init(HtmxSseConnection *self)
{
	self->message = NULL;
	self->output = NULL;
	self->connected = FALSE;
}

/**
 * htmx_sse_connection_new:
 * @msg: (transfer none): the #SoupServerMessage for this connection
 *
 * Creates a new SSE connection from a server message.
 * Sets up the response headers for SSE streaming.
 *
 * Returns: (transfer full): a new #HtmxSseConnection
 */
HtmxSseConnection *
htmx_sse_connection_new(SoupServerMessage *msg)
{
	HtmxSseConnection *self;
	SoupMessageHeaders *headers;
	SoupMessageBody *body;

	g_return_val_if_fail(SOUP_IS_SERVER_MESSAGE(msg), NULL);

	self = g_object_new(HTMX_TYPE_SSE_CONNECTION, NULL);
	self->message = g_object_ref(msg);

	/* Set SSE headers */
	headers = soup_server_message_get_response_headers(msg);
	soup_message_headers_set_content_type(headers, "text/event-stream", NULL);
	soup_message_headers_append(headers, "Cache-Control", "no-cache");
	soup_message_headers_append(headers, "Connection", "keep-alive");
	soup_message_headers_append(headers, "X-Accel-Buffering", "no");

	/* Get response body for streaming */
	body = soup_server_message_get_response_body(msg);
	soup_message_body_set_accumulate(body, FALSE);

	self->connected = TRUE;

	return self;
}

/*
 * Write data to the SSE stream
 */
static gboolean
write_to_stream(HtmxSseConnection *self, const gchar *data)
{
	SoupMessageBody *body;

	if (!self->connected || self->message == NULL) {
		return FALSE;
	}

	body = soup_server_message_get_response_body(self->message);
	soup_message_body_append(body, SOUP_MEMORY_COPY, data, strlen(data));

	return TRUE;
}

/**
 * htmx_sse_connection_send_event:
 * @self: an #HtmxSseConnection
 * @event: (nullable): the event type
 * @data: the event data
 * @id: (nullable): the event ID
 *
 * Sends an event to the connected client.
 * The event format follows the SSE specification.
 */
void
htmx_sse_connection_send_event(HtmxSseConnection *self,
                               const gchar       *event,
                               const gchar       *data,
                               const gchar       *id)
{
	GString *message;

	g_return_if_fail(HTMX_IS_SSE_CONNECTION(self));
	g_return_if_fail(data != NULL);

	message = g_string_new("");

	if (id != NULL) {
		g_string_append_printf(message, "id: %s\n", id);
	}

	if (event != NULL) {
		g_string_append_printf(message, "event: %s\n", event);
	}

	/* Handle multi-line data */
	{
		g_auto(GStrv) lines = g_strsplit(data, "\n", -1);
		gchar **line;

		for (line = lines; *line != NULL; line++) {
			g_string_append_printf(message, "data: %s\n", *line);
		}
	}

	/* End of event */
	g_string_append_c(message, '\n');

	write_to_stream(self, message->str);

	g_string_free(message, TRUE);
}

/**
 * htmx_sse_connection_send_data:
 * @self: an #HtmxSseConnection
 * @data: the data to send
 *
 * Sends a simple data-only event without event type or ID.
 */
void
htmx_sse_connection_send_data(HtmxSseConnection *self,
                              const gchar       *data)
{
	g_return_if_fail(HTMX_IS_SSE_CONNECTION(self));
	g_return_if_fail(data != NULL);

	htmx_sse_connection_send_event(self, NULL, data, NULL);
}

/**
 * htmx_sse_connection_send_comment:
 * @self: an #HtmxSseConnection
 * @comment: the comment text
 *
 * Sends a comment to keep the connection alive.
 * Comments are prefixed with ':' and ignored by the client.
 */
void
htmx_sse_connection_send_comment(HtmxSseConnection *self,
                                 const gchar       *comment)
{
	g_autofree gchar *message = NULL;

	g_return_if_fail(HTMX_IS_SSE_CONNECTION(self));

	if (comment != NULL) {
		message = g_strdup_printf(": %s\n\n", comment);
	} else {
		message = g_strdup(":\n\n");
	}

	write_to_stream(self, message);
}

/**
 * htmx_sse_connection_set_retry:
 * @self: an #HtmxSseConnection
 * @retry_ms: reconnection time in milliseconds
 *
 * Sends a retry directive to tell the client how long to wait
 * before reconnecting if the connection is lost.
 */
void
htmx_sse_connection_set_retry(HtmxSseConnection *self,
                              guint              retry_ms)
{
	g_autofree gchar *message = NULL;

	g_return_if_fail(HTMX_IS_SSE_CONNECTION(self));

	message = g_strdup_printf("retry: %u\n\n", retry_ms);
	write_to_stream(self, message);
}

/**
 * htmx_sse_connection_close:
 * @self: an #HtmxSseConnection
 *
 * Closes the SSE connection.
 */
void
htmx_sse_connection_close(HtmxSseConnection *self)
{
	SoupMessageBody *body;

	g_return_if_fail(HTMX_IS_SSE_CONNECTION(self));

	if (!self->connected) {
		return;
	}

	if (self->message != NULL) {
		body = soup_server_message_get_response_body(self->message);
		soup_message_body_complete(body);
	}

	self->connected = FALSE;
}

/**
 * htmx_sse_connection_is_connected:
 * @self: an #HtmxSseConnection
 *
 * Checks if the SSE connection is still active.
 *
 * Returns: %TRUE if connected, %FALSE otherwise
 */
gboolean
htmx_sse_connection_is_connected(HtmxSseConnection *self)
{
	g_return_val_if_fail(HTMX_IS_SSE_CONNECTION(self), FALSE);

	return self->connected;
}
