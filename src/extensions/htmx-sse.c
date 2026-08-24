/*
 * htmx-sse.c - Server-Sent Events extension implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-sse.h"

#include <string.h>

struct _HtmxSseConnection {
	GObject parent_instance;

	SoupServerMessage *message;
	gboolean           connected;
	gboolean           ended;      /* the message is finished, either way */
};

enum {
	SIGNAL_CLOSED,
	N_SIGNALS
};

static guint signals[N_SIGNALS];

G_DEFINE_FINAL_TYPE(HtmxSseConnection, htmx_sse_connection, G_TYPE_OBJECT)

static void
htmx_sse_connection_dispose(GObject *object)
{
	HtmxSseConnection *self = HTMX_SSE_CONNECTION(object);

	if (self->connected) {
		htmx_sse_connection_close(self);
	}

	g_clear_object(&self->message);

	G_OBJECT_CLASS(htmx_sse_connection_parent_class)->dispose(object);
}

static void
htmx_sse_connection_class_init(HtmxSseConnectionClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);

	object_class->dispose = htmx_sse_connection_dispose;

	/**
	 * HtmxSseConnection::closed:
	 * @self: the connection
	 *
	 * The stream has ended, whether because the peer went away or because
	 * htmx_sse_connection_close() was called.
	 *
	 * A server pushing events holds state per connection -- a timer, a
	 * place in a queue, a row in a table of subscribers.  Without this
	 * there is nothing to tell it that a browser tab was closed, so every
	 * closed tab leaves that state behind for the life of the process.
	 */
	signals[SIGNAL_CLOSED] =
		g_signal_new("closed",
		             G_TYPE_FROM_CLASS(klass),
		             G_SIGNAL_RUN_LAST,
		             0, NULL, NULL, NULL,
		             G_TYPE_NONE, 0);
}

static void
htmx_sse_connection_init(HtmxSseConnection *self)
{
	self->message = NULL;
	self->connected = FALSE;
	self->ended = FALSE;
}

/*
 * The peer went away, or soup finished the message on its own.
 *
 * Nothing may be written after this: the body still accepts an append,
 * and it goes nowhere.
 */
static void
on_message_ended(SoupServerMessage *msg, gpointer user_data)
{
	HtmxSseConnection *self = HTMX_SSE_CONNECTION(user_data);

	(void)msg;

	/*
	 * Once, whichever of the three routes gets here first: the peer
	 * hanging up, soup finishing the message, or our own close. All
	 * three end the same stream, and a subscriber list told twice that
	 * one connection went would remove somebody else's entry with the
	 * second. Measured before this: a local close emitted "closed", and
	 * soup's ::finished emitted it again microseconds later.
	 */
	if (self->ended) {
		return;
	}

	self->ended = TRUE;
	self->connected = FALSE;

	g_signal_emit(self, signals[SIGNAL_CLOSED], 0);
}

/**
 * htmx_sse_connection_new:
 * @msg: (transfer none): the #SoupServerMessage for this connection
 *
 * Creates a new SSE connection from a server message.
 * Sets up the response status and headers for SSE streaming.
 *
 * The handler that calls this must return htmx_response_new_streaming(),
 * so that the router leaves the message alone rather than applying an
 * ordinary response over the top of these headers.
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

	/*
	 * The status is set here rather than left to the caller.  A message
	 * that reaches the write path with no status set is sent as an
	 * internal error, so an SSE route that forgot it answered 500 with a
	 * perfectly formed event stream in the body.
	 */
	soup_server_message_set_status(msg, SOUP_STATUS_OK, NULL);

	headers = soup_server_message_get_response_headers(msg);
	soup_message_headers_set_content_type(headers, "text/event-stream", NULL);
	soup_message_headers_append(headers, "Cache-Control", "no-cache");
	soup_message_headers_append(headers, "Connection", "keep-alive");
	soup_message_headers_append(headers, "X-Accel-Buffering", "no");

	/*
	 * Chunked, stated outright.  Left to infer, soup takes the body's
	 * length at the moment the handler returns as the Content-Length,
	 * writes exactly that many bytes and finishes -- so a stream
	 * delivered whatever the handler wrote inline and nothing after it,
	 * which reads from the browser as a server that pushes one event and
	 * dies.  Measured against a real client: five later events arrive
	 * with this line and none without it.
	 */
	soup_message_headers_set_encoding(headers, SOUP_ENCODING_CHUNKED);

	/* Not accumulated, so appends go out as they are made */
	body = soup_server_message_get_response_body(msg);
	soup_message_body_set_accumulate(body, FALSE);

	/*
	 * Paused before the handler returns, and this is the line that makes
	 * streaming work at all.  soup finishes a message whose body it has
	 * run out of, and a handler returns long before the first pushed
	 * event exists -- so without the pause the stream ended immediately,
	 * having delivered only whatever the handler wrote inline.  Measured:
	 * the one synchronous event arrived and not a single later one did,
	 * with ::finished firing before the first timer tick.
	 */
	soup_server_message_pause(msg);

	/*
	 * Connected with g_signal_connect_object so the handlers go when this
	 * connection does.  A SoupServerMessage outlives a connection the
	 * application has dropped, and a callback into freed memory is the
	 * usual way that ends.
	 */
	g_signal_connect_object(msg, "disconnected",
	                        G_CALLBACK(on_message_ended), self, 0);
	g_signal_connect_object(msg, "finished",
	                        G_CALLBACK(on_message_ended), self, 0);

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

	if (!self->connected || self->ended || self->message == NULL) {
		return FALSE;
	}

	body = soup_server_message_get_response_body(self->message);
	soup_message_body_append(body, SOUP_MEMORY_COPY, data, strlen(data));

	soup_server_message_unpause(self->message);

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
 * @comment: (nullable): the comment text
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

	if (!self->connected || self->ended) {
		return;
	}

	self->connected = FALSE;

	/*
	 * Completing a body whose peer has already gone is not merely
	 * pointless -- soup has finished with the message by then, and it is
	 * the ::finished handler above that told us so.
	 */
	if (self->message != NULL) {
		body = soup_server_message_get_response_body(self->message);
		soup_message_body_complete(body);
		soup_server_message_unpause(self->message);
	}

	/*
	 * Through the same door as the other two, so "closed" is emitted
	 * exactly once however the stream ended.
	 */
	on_message_ended(self->message, self);
}

/**
 * htmx_sse_connection_is_connected:
 * @self: an #HtmxSseConnection
 *
 * Checks if the SSE connection is still active.
 *
 * This reports the peer going away as well as a local close: the
 * connection listens for the message ending, so a browser tab that was
 * closed is visible here rather than only at the next failed write.
 *
 * Returns: %TRUE if connected, %FALSE otherwise
 */
gboolean
htmx_sse_connection_is_connected(HtmxSseConnection *self)
{
	g_return_val_if_fail(HTMX_IS_SSE_CONNECTION(self), FALSE);

	return self->connected;
}
