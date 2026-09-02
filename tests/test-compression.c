/*
 * test-compression.c - Tests for HtmxCompression middleware
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * The middleware compressed correctly and then stored the result with
 * htmx_response_set_content() -- the g_strdup()/strlen() path -- so the
 * body a browser received was cut at the gzip header's first NUL, which
 * is byte three, under a Content-Encoding: gzip that promised the rest.
 * Its own comment said the real bytes were applied to the
 * SoupServerMessage by the server; htmx-server.c has never mentioned
 * compression.
 *
 * The second defect is in compress_data() and is invisible to a test
 * that only checks the length: the loop ran while total_read < data_len
 * and stopped as soon as the input was consumed, without ever requiring
 * G_CONVERTER_FINISHED.  A body zlib can swallow in one pass leaves that
 * loop with a deflate stream that was never flushed and a gzip trailer
 * that was never written.  So these tests decompress what arrived rather
 * than measuring it.
 */

#include <htmx-glib.h>
#include <string.h>

static void
test_compression_new(void)
{
	g_autoptr(HtmxCompression) comp = htmx_compression_new();

	g_assert_nonnull(comp);
	g_assert_true(HTMX_IS_COMPRESSION(comp));
}

static void
test_compression_default_min_size(void)
{
	g_autoptr(HtmxCompression) comp = htmx_compression_new();

	g_assert_cmpuint(htmx_compression_get_min_size(comp), ==, 256);
}

static void
test_compression_set_min_size(void)
{
	g_autoptr(HtmxCompression) comp = htmx_compression_new();

	htmx_compression_set_min_size(comp, 1024);
	g_assert_cmpuint(htmx_compression_get_min_size(comp), ==, 1024);

	htmx_compression_set_min_size(comp, 0);
	g_assert_cmpuint(htmx_compression_get_min_size(comp), ==, 0);
}

static void
test_compression_get_type(void)
{
	GType type = htmx_compression_get_type();
	g_assert_true(g_type_is_a(type, G_TYPE_OBJECT));
}

/* ── What a browser actually receives ────────────────────────────── */

typedef struct {
	HtmxServer *server;
	GMainLoop  *loop;
	GString    *body;         /* the raw bytes, still encoded */
	gchar      *encoding;     /* the Content-Encoding it claimed */
	guint       status;
	GString    *page;         /* what the handler wrote */
	guint16     port;
} Fixture;

static HtmxResponse *
on_page(HtmxRequest *request, GHashTable *params, gpointer user_data)
{
	Fixture *fix = user_data;

	(void)request;
	(void)params;

	return htmx_response_new_with_content(fix->page->str);
}

static void
fixture_set_up(Fixture *fix, gconstpointer user_data)
{
	g_autoptr(GError) error = NULL;
	g_autoptr(HtmxCompression) compression = htmx_compression_new();
	GSList *uris;

	(void)user_data;

	fix->server = htmx_server_new();
	fix->loop = g_main_loop_new(NULL, FALSE);
	fix->body = g_string_new(NULL);
	fix->page = g_string_new(NULL);
	fix->encoding = NULL;
	fix->status = 0;

	htmx_router_get(htmx_server_get_router(fix->server), "/page",
	                on_page, fix);
	htmx_server_use(fix->server, htmx_compression_middleware,
	                g_object_ref(compression), g_object_unref);

	g_assert_true(htmx_server_listen_on(fix->server, "127.0.0.1", 0,
	                                    &error));
	g_assert_no_error(error);

	uris = htmx_server_get_listen_uris(fix->server);
	g_assert_nonnull(uris);
	{
		g_autoptr(GUri) uri = g_uri_parse(uris->data,
		                                  SOUP_HTTP_URI_FLAGS, NULL);
		fix->port = (guint16)g_uri_get_port(uri);
	}
	g_slist_free_full(uris, g_free);
}

static void
fixture_tear_down(Fixture *fix, gconstpointer user_data)
{
	(void)user_data;

	htmx_server_stop(fix->server);
	g_clear_object(&fix->server);
	g_clear_pointer(&fix->loop, g_main_loop_unref);
	g_string_free(fix->body, TRUE);
	g_string_free(fix->page, TRUE);
	g_clear_pointer(&fix->encoding, g_free);
}

static void
on_body_read(GObject *source, GAsyncResult *result, gpointer user_data)
{
	Fixture *fix = user_data;
	g_autoptr(GError) error = NULL;
	g_autoptr(GBytes) bytes = NULL;

	bytes = soup_session_send_and_read_finish(SOUP_SESSION(source), result,
	                                          &error);

	if (bytes != NULL) {
		gsize size = 0;
		const gchar *data = g_bytes_get_data(bytes, &size);

		g_string_append_len(fix->body, data, (gssize)size);
	}

	g_main_loop_quit(fix->loop);
}

/*
 * Fetch /page with Accept-Encoding: gzip and keep the bytes as they came.
 *
 * SoupContentDecoder is removed on purpose.  Left in, it decompresses the
 * body and drops Content-Encoding before anything here sees either, so
 * every assertion below would be made about a body libsoup repaired --
 * and it is also what sends Accept-Encoding, which is why that header is
 * set by hand.
 */
static void
fetch(Fixture *fix)
{
	g_autoptr(SoupSession) session = soup_session_new();
	g_autoptr(SoupMessage) message = NULL;
	g_autofree gchar *url = NULL;

	soup_session_remove_feature_by_type(session, SOUP_TYPE_CONTENT_DECODER);

	url = g_strdup_printf("http://127.0.0.1:%u/page", fix->port);
	message = soup_message_new("GET", url);
	soup_message_headers_replace(soup_message_get_request_headers(message),
	                             "Accept-Encoding", "gzip");

	soup_session_send_and_read_async(session, message, G_PRIORITY_DEFAULT,
	                                 NULL, on_body_read, fix);
	g_main_loop_run(fix->loop);

	fix->status = soup_message_get_status(message);
	fix->encoding = g_strdup(soup_message_headers_get_one(
		soup_message_get_response_headers(message),
		"Content-Encoding"));
}

/*
 * Inflate a gzip stream, refusing to call a truncated one a success.
 *
 * GZlibDecompressor reports G_IO_ERROR_PARTIAL_INPUT when the stream ends
 * before its trailer, which is the whole point of decompressing here
 * rather than comparing lengths.
 */
static gchar *
gunzip(const gchar *data, gsize length, GError **error)
{
	g_autoptr(GZlibDecompressor) decompressor =
		g_zlib_decompressor_new(G_ZLIB_COMPRESSOR_FORMAT_GZIP);
	g_autoptr(GInputStream) memory = NULL;
	g_autoptr(GInputStream) stream = NULL;
	GString *out = g_string_new(NULL);
	gchar buffer[16384];

	memory = g_memory_input_stream_new_from_data(g_memdup2(data, length),
	                                             (gssize)length, g_free);
	stream = g_converter_input_stream_new(memory,
	                                      G_CONVERTER(decompressor));

	for (;;) {
		gssize n = g_input_stream_read(stream, buffer, sizeof(buffer),
		                               NULL, error);

		if (n < 0) {
			g_string_free(out, TRUE);
			return NULL;
		}

		if (n == 0) {
			break;
		}

		g_string_append_len(out, buffer, n);
	}

	return g_string_free(out, FALSE);
}

/*
 * An ordinary page, compressed: the body has to be the whole gzip stream.
 *
 * htmx_response_set_content() ended it at the header's first NUL, so this
 * arrived as three bytes with a Content-Length of 3.
 */
static void
test_a_compressed_body_is_not_cut_at_the_first_nul(Fixture *fix,
                                                   gconstpointer user_data)
{
	g_autoptr(GError) error = NULL;
	g_autofree gchar *plain = NULL;
	guint i;

	(void)user_data;

	for (i = 0; i < 200; i++) {
		g_string_append(fix->page,
		                "the quick brown fox jumps over the lazy dog. ");
	}

	fetch(fix);

	g_assert_cmpuint(fix->status, ==, 200);
	g_assert_cmpstr(fix->encoding, ==, "gzip");

	/* Three is what strlen() said about a gzip header. */
	g_assert_cmpuint(fix->body->len, >, 3);

	plain = gunzip(fix->body->str, fix->body->len, &error);
	g_assert_no_error(error);
	g_assert_cmpstr(plain, ==, fix->page->str);
}

/*
 * A body zlib consumes in a single pass.
 *
 * compress_data() chose its G_CONVERTER_INPUT_AT_END flag from
 * `total_read + sizeof(out_buf) >= data_len`, so for anything larger than
 * the 64 KB output buffer the first pass was told the input had not
 * ended -- and a megabyte of repeated text compresses small enough that
 * the first pass consumes all of it.  `while (total_read < data_len)`
 * was then false, the loop ended without G_CONVERTER_FINISHED, and the
 * stream went out with neither its final block nor its gzip trailer.
 *
 * A length check cannot see this: the body is nearly the right size.
 */
static void
test_a_body_consumed_in_one_pass_still_ends(Fixture *fix,
                                            gconstpointer user_data)
{
	g_autoptr(GError) error = NULL;
	g_autofree gchar *plain = NULL;
	guint i;

	(void)user_data;

	for (i = 0; i < 24000; i++) {
		g_string_append(fix->page,
		                "the quick brown fox jumps over the lazy dog. ");
	}
	g_assert_cmpuint(fix->page->len, >, 65536);

	fetch(fix);

	g_assert_cmpuint(fix->status, ==, 200);
	g_assert_cmpstr(fix->encoding, ==, "gzip");

	plain = gunzip(fix->body->str, fix->body->len, &error);
	g_assert_no_error(error);
	g_assert_cmpstr(plain, ==, fix->page->str);
}

int
main(int argc, char *argv[])
{
	g_test_init(&argc, &argv, NULL);

	g_test_add_func("/compression/new", test_compression_new);
	g_test_add_func("/compression/default_min_size", test_compression_default_min_size);
	g_test_add_func("/compression/set_min_size", test_compression_set_min_size);
	g_test_add_func("/compression/get_type", test_compression_get_type);

	g_test_add("/compression/body-is-not-cut-at-the-first-nul",
	           Fixture, NULL, fixture_set_up,
	           test_a_compressed_body_is_not_cut_at_the_first_nul,
	           fixture_tear_down);
	g_test_add("/compression/a-body-consumed-in-one-pass-still-ends",
	           Fixture, NULL, fixture_set_up,
	           test_a_body_consumed_in_one_pass_still_ends,
	           fixture_tear_down);

	return g_test_run();
}
