/*
 * test-streaming.c - Server-Sent Events, and listening on more than one address
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * These two go together because they are the two things a server pushing
 * updates to a browser needs and did not have: a route that can stream at
 * all, and a way to be reachable from one private network and the local
 * machine without also appearing on every interface.
 */

#include <htmx-glib.h>

#include <string.h>
#include <glib/gstdio.h>

typedef struct {
	HtmxServer        *server;
	HtmxSseConnection *conn;
	GMainLoop         *loop;
	GString           *received;
	guint              sent;
	guint              closed_count;
	guint16            port;
} Fixture;

/* ── A route that streams ────────────────────────────────────────── */

static void
on_conn_closed(HtmxSseConnection *conn, gpointer user_data)
{
	Fixture *fix = user_data;

	(void)conn;
	fix->closed_count++;
}

static gboolean
push_one(gpointer user_data)
{
	Fixture *fix = user_data;
	g_autofree gchar *body = NULL;

	if (!htmx_sse_connection_is_connected(fix->conn)) {
		return G_SOURCE_REMOVE;
	}

	fix->sent++;
	body = g_strdup_printf("tick %u", fix->sent);
	htmx_sse_connection_send_event(fix->conn, "tick", body, NULL);

	if (fix->sent >= 3) {
		htmx_sse_connection_close(fix->conn);
		return G_SOURCE_REMOVE;
	}

	return G_SOURCE_CONTINUE;
}

static HtmxResponse *
on_stream(HtmxRequest *request, GHashTable *params, gpointer user_data)
{
	Fixture *fix = user_data;

	(void)params;

	fix->conn = htmx_sse_connection_new(htmx_request_get_message(request));
	g_signal_connect(fix->conn, "closed", G_CALLBACK(on_conn_closed), fix);

	/*
	 * Nothing is written inline on purpose. An event sent from the
	 * handler arrives even when streaming is broken -- that is exactly
	 * how the missing chunked encoding stayed hidden -- so the whole
	 * assertion here is about events produced after the handler returned.
	 */
	g_timeout_add(20, push_one, fix);

	return htmx_response_new_streaming();
}

/* ── Fixture ─────────────────────────────────────────────────────── */

static void
fixture_set_up(Fixture *fix, gconstpointer user_data)
{
	g_autoptr(GError) error = NULL;
	GSList *uris;

	(void)user_data;

	fix->server = htmx_server_new();
	fix->loop = g_main_loop_new(NULL, FALSE);
	fix->received = g_string_new(NULL);
	fix->sent = 0;
	fix->closed_count = 0;
	fix->conn = NULL;

	htmx_router_get(htmx_server_get_router(fix->server), "/stream",
	                on_stream, fix);

	/* Port 0: the kernel picks one, so a machine already running
	 * something on a fixed port does not fail the suite. */
	g_assert_true(htmx_server_listen_on(fix->server, "127.0.0.1", 0, &error));
	g_assert_no_error(error);

	uris = htmx_server_get_listen_uris(fix->server);
	g_assert_nonnull(uris);
	{
		g_autoptr(GUri) uri = g_uri_parse(uris->data, SOUP_HTTP_URI_FLAGS, NULL);
		fix->port = (guint16)g_uri_get_port(uri);
	}
	g_slist_free_full(uris, g_free);
}

static void
fixture_tear_down(Fixture *fix, gconstpointer user_data)
{
	(void)user_data;

	g_clear_object(&fix->conn);
	htmx_server_stop(fix->server);
	g_clear_object(&fix->server);
	g_clear_pointer(&fix->loop, g_main_loop_unref);
	g_string_free(fix->received, TRUE);
}

/* ── The streaming response itself ───────────────────────────────── */

static void
test_streaming_response_is_marked(void)
{
	g_autoptr(HtmxResponse) streaming = htmx_response_new_streaming();
	g_autoptr(HtmxResponse) ordinary = htmx_response_new();

	g_assert_true(htmx_response_is_streaming(streaming));
	g_assert_false(htmx_response_is_streaming(ordinary));
}

/* ── The stream ──────────────────────────────────────────────────── */

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

		g_string_append_len(fix->received, data, (gssize)size);
	}

	g_main_loop_quit(fix->loop);
}

static void
test_events_arrive_after_the_handler_returned(Fixture *fix,
                                              gconstpointer user_data)
{
	g_autoptr(SoupSession) session = soup_session_new();
	g_autofree gchar *url = NULL;
	SoupMessage *message;

	(void)user_data;

	url = g_strdup_printf("http://127.0.0.1:%u/stream", fix->port);
	message = soup_message_new("GET", url);

	soup_session_send_and_read_async(session, message, G_PRIORITY_DEFAULT,
	                                 NULL, on_body_read, fix);
	g_main_loop_run(fix->loop);
	g_object_unref(message);

	/*
	 * All three, in order. Before the chunked encoding was stated the
	 * response ended when the handler returned, so this body was empty --
	 * a stream that had delivered nothing at all, from a route that
	 * looked healthy.
	 */
	g_assert_cmpuint(fix->sent, ==, 3);
	g_assert_nonnull(strstr(fix->received->str, "data: tick 1"));
	g_assert_nonnull(strstr(fix->received->str, "data: tick 2"));
	g_assert_nonnull(strstr(fix->received->str, "data: tick 3"));
	g_assert_nonnull(strstr(fix->received->str, "event: tick"));
}

static void
test_close_is_reported_once(Fixture *fix, gconstpointer user_data)
{
	g_autoptr(SoupSession) session = soup_session_new();
	g_autofree gchar *url = NULL;
	SoupMessage *message;

	(void)user_data;

	url = g_strdup_printf("http://127.0.0.1:%u/stream", fix->port);
	message = soup_message_new("GET", url);

	soup_session_send_and_read_async(session, message, G_PRIORITY_DEFAULT,
	                                 NULL, on_body_read, fix);
	g_main_loop_run(fix->loop);
	g_object_unref(message);

	/*
	 * Exactly once, though both ::finished and ::disconnected reach the
	 * same handler and a local close runs through it too. A subscriber
	 * list that removed an entry twice would take somebody else's with
	 * it.
	 */
	g_assert_cmpuint(fix->closed_count, ==, 1);
	g_assert_false(htmx_sse_connection_is_connected(fix->conn));
}

/* ── Listening ───────────────────────────────────────────────────── */

static void
test_two_addresses_are_both_bound(void)
{
	g_autoptr(HtmxServer) server = htmx_server_new();
	g_autoptr(GError) error = NULL;
	GSList *uris;

	g_assert_true(htmx_server_listen_on(server, "127.0.0.1", 0, &error));
	g_assert_no_error(error);
	g_assert_true(htmx_server_listen_on(server, "::1", 0, &error));
	g_assert_no_error(error);

	uris = htmx_server_get_listen_uris(server);
	g_assert_cmpuint(g_slist_length(uris), >=, 2);
	g_slist_free_full(uris, g_free);

	htmx_server_stop(server);
}

static void
test_start_after_listen_on_adds_nothing(void)
{
	g_autoptr(HtmxServer) server = htmx_server_new();
	g_autoptr(GError) error = NULL;
	GSList *before;
	GSList *after;
	guint n_before;
	guint n_after;

	g_assert_true(htmx_server_listen_on(server, "127.0.0.1", 0, &error));
	g_assert_no_error(error);

	before = htmx_server_get_listen_uris(server);
	n_before = g_slist_length(before);
	g_slist_free_full(before, g_free);

	/*
	 * The configured host defaults to every interface. A server told
	 * exactly where to listen and then started must not pick that up as
	 * well -- it would be reachable from places nobody named, which for
	 * a client with no authentication of its own is the whole risk.
	 */
	g_assert_true(htmx_server_start(server, &error));
	g_assert_no_error(error);

	after = htmx_server_get_listen_uris(server);
	n_after = g_slist_length(after);
	g_slist_free_full(after, g_free);

	g_assert_cmpuint(n_after, ==, n_before);

	htmx_server_stop(server);
}

/* ── Static files ────────────────────────────────────────────────── */

/*
 * The router turns a request path into a filename, which makes it the
 * one place that has to answer "is this still inside the directory I was
 * given". It could not: it built the path and read it. Nothing had ever
 * got through, because libsoup normalises a literal "../" out of the
 * request and answers an encoded one with 400 -- but that is a guarantee
 * made by a version of libsoup we do not pin, about the one function
 * whose whole job is turning a request into a filename.
 *
 * Which is also why this goes through htmx_request_new_for_path(): the
 * case worth testing hardest is the one a real client cannot send.
 */
static void
test_static_stays_inside_its_directory(void)
{
	g_autoptr(HtmxRouter) router = htmx_router_new();
	g_autoptr(GError) error = NULL;
	g_autofree gchar *dir = NULL;
	g_autofree gchar *asset = NULL;
	g_autofree gchar *outside = NULL;

	dir = g_dir_make_tmp("htmx-static-XXXXXX", &error);
	g_assert_no_error(error);

	asset = g_build_filename(dir, "ok.txt", NULL);
	g_assert_true(g_file_set_contents(asset, "asset", -1, &error));
	g_assert_no_error(error);

	outside = g_build_filename(dir, "..", "htmx-secret-probe", NULL);
	g_assert_true(g_file_set_contents(outside, "secret", -1, &error));
	g_assert_no_error(error);

	htmx_router_serve_static(router, "/static", dir);

	{
		g_autoptr(HtmxRequest) good =
			htmx_request_new_for_path(HTMX_METHOD_GET, "/static/ok.txt");
		GHashTable *params = NULL;
		HtmxResponse *response;

		response = htmx_router_match(router, good, &params);
		g_assert_nonnull(response);
		g_assert_cmpuint(htmx_response_get_status(response), ==, 200);

		/*
		 * Bytes, not content.  A static route serves a file, and a
		 * file is not a string: reading it into a NUL-terminated
		 * one truncated every binary asset at its first zero byte,
		 * eight in for a PNG, beside a MIME table naming image/png.
		 * This assertion is what said the old behaviour was
		 * intended, and it only ever served text.
		 */
		{
			GBytes *body = htmx_response_get_bytes(response);
			gsize size = 0;
			const gchar *data;

			g_assert_nonnull(body);
			data = g_bytes_get_data(body, &size);
			g_assert_cmpuint(size, ==, strlen("asset"));
			g_assert_cmpint(memcmp(data, "asset", size), ==, 0);
		}

		g_object_unref(response);
		g_clear_pointer(&params, g_hash_table_unref);
	}

	{
		g_autoptr(HtmxRequest) bad =
			htmx_request_new_for_path(HTMX_METHOD_GET,
			                          "/static/../htmx-secret-probe");
		GHashTable *params = NULL;
		HtmxResponse *response;

		g_test_expect_message(NULL, G_LOG_LEVEL_WARNING, "htmx: refusing*");
		response = htmx_router_match(router, bad, &params);
		g_test_assert_expected_messages();

		g_assert_nonnull(response);
		g_assert_cmpuint(htmx_response_get_status(response), ==, 404);
		g_assert_cmpstr(htmx_response_get_content(response), !=, "secret");
		g_object_unref(response);
		g_clear_pointer(&params, g_hash_table_unref);
	}

	g_unlink(asset);
	g_unlink(outside);
	g_rmdir(dir);
}

static void
test_a_bad_address_is_refused(void)
{
	g_autoptr(HtmxServer) server = htmx_server_new();
	g_autoptr(GError) error = NULL;

	g_assert_false(htmx_server_listen_on(server, "not-an-address", 0, &error));
	g_assert_nonnull(error);
}

int
main(int argc, char *argv[])
{
	g_test_init(&argc, &argv, NULL);

	g_test_add_func("/streaming/response-is-marked",
	                test_streaming_response_is_marked);
	g_test_add("/streaming/events-arrive-after-the-handler-returned",
	           Fixture, NULL, fixture_set_up,
	           test_events_arrive_after_the_handler_returned,
	           fixture_tear_down);
	g_test_add("/streaming/close-is-reported-once",
	           Fixture, NULL, fixture_set_up,
	           test_close_is_reported_once, fixture_tear_down);
	g_test_add_func("/streaming/two-addresses-are-both-bound",
	                test_two_addresses_are_both_bound);
	g_test_add_func("/streaming/start-after-listen-on-adds-nothing",
	                test_start_after_listen_on_adds_nothing);
	g_test_add_func("/streaming/a-bad-address-is-refused",
	                test_a_bad_address_is_refused);
	g_test_add_func("/static/stays-inside-its-directory",
	                test_static_stays_inside_its_directory);

	return g_test_run();
}
