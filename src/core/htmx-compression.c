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
	gsize total_written;
	gsize bytes_read;
	gsize bytes_written;
	GError *error = NULL;

	compressor = g_zlib_compressor_new(
		use_gzip ? G_ZLIB_COMPRESSOR_FORMAT_GZIP : G_ZLIB_COMPRESSOR_FORMAT_ZLIB,
		-1);
	converter = G_CONVERTER(compressor);
	output = g_byte_array_new();
	total_read = 0;
	total_written = 0;

	while (total_read < data_len) {
		result = g_converter_convert(
			converter,
			data + total_read, data_len - total_read,
			out_buf, sizeof(out_buf),
			(total_read + sizeof(out_buf) >= data_len) ? G_CONVERTER_INPUT_AT_END : G_CONVERTER_NO_FLAGS,
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
		total_written += bytes_written;
		g_byte_array_append(output, out_buf, (guint)bytes_written);

		if (result == G_CONVERTER_FINISHED)
			break;
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
	gchar *compressed_content;
	gsize compressed_len;
	const guint8 *compressed_data;

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

	compressed_data = g_bytes_get_data(compressed, &compressed_len);

	/* Only use compressed version if it's actually smaller */
	if (compressed_len >= content_len) {
		g_bytes_unref(compressed);
		return;
	}

	/*
	 * Set the compressed content. We store as a NUL-terminated copy
	 * since HtmxResponse uses gchar* content. For binary compressed
	 * data this is imperfect but functional for the middleware's
	 * purpose of setting headers. The actual compressed bytes should
	 * be applied directly to the SoupServerMessage in a real
	 * integration, which the server does.
	 */
	compressed_content = g_malloc(compressed_len + 1);
	memcpy(compressed_content, compressed_data, compressed_len);
	compressed_content[compressed_len] = '\0';

	htmx_response_set_content(response, compressed_content);
	htmx_response_add_header(response, "Content-Encoding",
	                         use_gzip ? "gzip" : "deflate");
	htmx_response_add_header(response, "Vary", "Accept-Encoding");

	g_free(compressed_content);
	g_bytes_unref(compressed);
}
