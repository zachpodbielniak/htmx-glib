/*
 * htmx-compression.c - Response compression middleware implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-compression.h"
#include "htmx-request.h"
#include "htmx-response.h"
#include <gio/gio.h>
#include <string.h>

struct _HtmxCompression {
	GObject parent_instance;
	gsize   min_size;
};

G_DEFINE_FINAL_TYPE(HtmxCompression, htmx_compression, G_TYPE_OBJECT)

static void
htmx_compression_class_init(HtmxCompressionClass *klass)
{
	(void)klass;
}

static void
htmx_compression_init(HtmxCompression *self)
{
	self->min_size = 256;
}

HtmxCompression *
htmx_compression_new(void)
{
	return g_object_new(HTMX_TYPE_COMPRESSION, NULL);
}

void
htmx_compression_set_min_size(HtmxCompression *self,
                               gsize            min_size)
{
	g_return_if_fail(HTMX_IS_COMPRESSION(self));
	self->min_size = min_size;
}

gsize
htmx_compression_get_min_size(HtmxCompression *self)
{
	g_return_val_if_fail(HTMX_IS_COMPRESSION(self), 0);
	return self->min_size;
}

/*
 * Check if the client accepts a given encoding.
 */
static gboolean
accepts_encoding(HtmxRequest *request, const gchar *encoding)
{
	SoupServerMessage *msg;
	SoupMessageHeaders *headers;
	const gchar *accept;

	msg = htmx_request_get_message(request);
	if (msg == NULL)
		return FALSE;

	headers = soup_server_message_get_request_headers(msg);
	accept = soup_message_headers_get_one(headers, "Accept-Encoding");

	if (accept == NULL)
		return FALSE;

	return strstr(accept, encoding) != NULL;
}

/*
 * Compress data using GZlibCompressor (gzip or deflate).
 * Returns NULL on failure.
 *
 * The loop ends on G_CONVERTER_FINISHED and on nothing else.  It used to
 * run `while (total_read < data_len)` and take the input being consumed
 * as the work being done, which is a different question: a deflate stream
 * is not finished until the converter has emitted its final block, and a
 * gzip stream is not finished until the eight-byte trailer after that.
 *
 * The old G_CONVERTER_INPUT_AT_END flag was chosen by
 * `total_read + sizeof(out_buf) >= data_len`, which compares input
 * consumed plus the *output* buffer's size against the input's length.
 * For a body larger than 64 KB the first pass was therefore told the
 * input had not ended -- and a body that compresses well is swallowed
 * whole on that pass, so the loop exited immediately with an unterminated
 * stream.  A megabyte of ordinary HTML does this, and the result is
 * within a few hundred bytes of the right length, so it survives every
 * check but decompressing it.
 */
static GBytes *
compress_data(const gchar *data, gsize data_len, gboolean use_gzip)
{
	GZlibCompressor *compressor;
	GConverter *converter;
	GConverterResult result;
	guint8 out_buf[65536];
	GByteArray *output;
	gsize total_read;
	gsize bytes_read;
	gsize bytes_written;
	GError *error = NULL;

	compressor = g_zlib_compressor_new(
		use_gzip ? G_ZLIB_COMPRESSOR_FORMAT_GZIP : G_ZLIB_COMPRESSOR_FORMAT_ZLIB,
		-1);
	converter = G_CONVERTER(compressor);
	output = g_byte_array_new();
	total_read = 0;

	for (;;) {
		/*
		 * Told the input has ended only once it actually has, which
		 * is what makes the flush pass -- zero bytes in, the trailer
		 * out -- reachable at all.
		 */
		GConverterFlags flags = (total_read == data_len)
			? G_CONVERTER_INPUT_AT_END
			: G_CONVERTER_NO_FLAGS;

		result = g_converter_convert(
			converter,
			data + total_read, data_len - total_read,
			out_buf, sizeof(out_buf),
			flags,
			&bytes_read, &bytes_written,
			&error);

		if (result == G_CONVERTER_ERROR) {
			g_warning("Compression failed: %s", error->message);
			g_error_free(error);
			g_byte_array_unref(output);
			g_object_unref(compressor);
			return NULL;
		}

		total_read += bytes_read;
		/*
		 * bytes_written is the converter's own count for this pass and
		 * is used through `output`; there is no running total, because
		 * nothing ever read one.  It was accumulated and discarded.
		 */
		g_byte_array_append(output, out_buf, (guint)bytes_written);

		if (result == G_CONVERTER_FINISHED)
			break;

		/*
		 * A pass that neither consumed nor produced anything cannot
		 * be followed by one that does, and this loop no longer has
		 * an input-length bound to fall out of.  Refuse rather than
		 * spin: a compressor that has stopped answering is not a body
		 * to send.
		 */
		if (bytes_read == 0 && bytes_written == 0) {
			g_warning("Compression stalled after %" G_GSIZE_FORMAT
			          " of %" G_GSIZE_FORMAT " bytes",
			          total_read, data_len);
			g_byte_array_unref(output);
			g_object_unref(compressor);
			return NULL;
		}
	}

	g_object_unref(compressor);

	{
		GBytes *result_bytes;
		result_bytes = g_byte_array_free_to_bytes(output);
		return result_bytes;
	}
}

void
htmx_compression_middleware(HtmxContext        *context,
                             HtmxMiddlewareNext  next,
                             gpointer            next_data,
                             gpointer            user_data)
{
	HtmxCompression *self = HTMX_COMPRESSION(user_data);
	HtmxRequest *request;
	HtmxResponse *response;
	const gchar *content;
	gsize content_len;
	gboolean use_gzip;
	GBytes *compressed;
	gsize compressed_len;

	/* Run the rest of the pipeline first */
	next(context, next_data);

	response = htmx_context_get_response(context);
	if (response == NULL)
		return;

	content = htmx_response_get_content(response);
	if (content == NULL)
		return;

	content_len = strlen(content);
	if (content_len < self->min_size)
		return;

	request = htmx_context_get_request(context);

	/* Check what the client accepts */
	if (accepts_encoding(request, "gzip"))
		use_gzip = TRUE;
	else if (accepts_encoding(request, "deflate"))
		use_gzip = FALSE;
	else
		return;

	compressed = compress_data(content, content_len, use_gzip);
	if (compressed == NULL)
		return;

	g_bytes_get_data(compressed, &compressed_len);

	/* Only use compressed version if it's actually smaller */
	if (compressed_len >= content_len) {
		g_bytes_unref(compressed);
		return;
	}

	/*
	 * Bytes, not content.
	 *
	 * This stored the compressed body with htmx_response_set_content(),
	 * which g_strdup()s it and is written out with strlen() -- and a
	 * gzip stream's third byte is the deflate compression method, which
	 * is zero.  So every compressed response was a three-byte body under
	 * a Content-Encoding: gzip promising the rest, and a deflate one was
	 * cut at whatever offset its first zero byte happened to land on.
	 *
	 * The comment that used to stand here said the real bytes were
	 * applied to the SoupServerMessage by the server "in a real
	 * integration, which the server does".  htmx-server.c has never
	 * mentioned compression; htmx_response_set_bytes() is what applies a
	 * body that is not a string, and htmx_response_apply() prefers it
	 * over content for exactly this reason.
	 */
	htmx_response_set_bytes(response, compressed);

	/*
	 * And the uncompressed copy goes, because the body is the bytes now.
	 * Leaving it would be two answers to one question -- and a
	 * middleware unwinding after this one reads get_content() to decide
	 * whether there is anything to encode, so a stale copy invites a
	 * second encoding of a body that already has a Content-Encoding.
	 */
	htmx_response_set_content(response, NULL);

	htmx_response_add_header(response, "Content-Encoding",
	                         use_gzip ? "gzip" : "deflate");
	htmx_response_add_header(response, "Vary", "Accept-Encoding");

	g_bytes_unref(compressed);
}
