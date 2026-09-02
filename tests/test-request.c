/*
 * test-request.c - Decoding application/x-www-form-urlencoded
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * A query string and a POST body are the same encoding, and only one of
 * the two readers knew it.  The '+' substitution was added to
 * parse_form_data(), with a careful comment about doing it before
 * unescaping so that a genuine plus sent as %2B survives -- and
 * parse_query_string(), which is what every GET form and every hx-get
 * goes through, was left as it was.  So ?q=a+b came back as the literal
 * "a+b" while q=a+b in a POST body came back as "a b", from the same
 * server, for the same form, depending on its method.
 */

#include <htmx-glib.h>
#include <string.h>

static void
test_a_plus_in_a_query_is_a_space(void)
{
	g_autoptr(HtmxRequest) request =
		htmx_request_new_for_path(HTMX_METHOD_GET, "/s?q=a+b");

	g_assert_cmpstr(htmx_request_get_query_param(request, "q"), ==, "a b");
}

/*
 * And a plus that was meant survives, because the substitution happens
 * before the %XX unescaping rather than after it.  Doing it the other way
 * round turns every %2B into a space, which is the bug this ordering was
 * written to avoid and which no test had ever pinned.
 */
static void
test_an_escaped_plus_stays_a_plus(void)
{
	g_autoptr(HtmxRequest) request =
		htmx_request_new_for_path(HTMX_METHOD_GET,
		                          "/s?plus=%2B&both=a+%2Bb");

	g_assert_cmpstr(htmx_request_get_query_param(request, "plus"), ==, "+");
	g_assert_cmpstr(htmx_request_get_query_param(request, "both"), ==,
	                "a +b");
}

/* The key is encoded the same way as the value. */
static void
test_a_plus_in_a_key_is_a_space(void)
{
	g_autoptr(HtmxRequest) request =
		htmx_request_new_for_path(HTMX_METHOD_GET, "/s?first+name=ada");

	g_assert_cmpstr(htmx_request_get_query_param(request, "first name"),
	                ==, "ada");
	g_assert_null(htmx_request_get_query_param(request, "first+name"));
}

/* A percent-encoded space keeps working. */
static void
test_a_percent_twenty_is_still_a_space(void)
{
	g_autoptr(HtmxRequest) request =
		htmx_request_new_for_path(HTMX_METHOD_GET, "/s?q=a%20b");

	g_assert_cmpstr(htmx_request_get_query_param(request, "q"), ==, "a b");
}

/* ── The POST body, which is the half that already worked ────────── */

typedef struct {
	HtmxServer *server;
	GMainLoop  *loop;
	gchar      *seen;      /* the "q" the handler was given */
	guint16     port;
} Fixture;

static HtmxResponse *
on_submit(HtmxRequest *request, GHashTable *params, gpointer user_data)
{
	Fixture *fix = user_data;

	(void)params;

	g_clear_pointer(&fix->seen, g_free);
	fix->seen = g_strdup(htmx_request_get_form_value(request, "q"));

	return htmx_response_new_with_content("<p>ok</p>");
}

static void
fixture_set_up(Fixture *fix, gconstpointer user_data)
{
	g_autoptr(GError) error = NULL;
	GSList *uris;

	(void)user_data;

	fix->server = htmx_server_new();
	fix->loop = g_main_loop_new(NULL, FALSE);
	fix->seen = NULL;

	htmx_router_post(htmx_server_get_router(fix->server), "/submit",
	                 on_submit, fix);

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
	g_clear_pointer(&fix->seen, g_free);
}

static void
on_body_read(GObject *source, GAsyncResult *result, gpointer user_data)
{
	Fixture *fix = user_data;
	g_autoptr(GError) error = NULL;
	g_autoptr(GBytes) bytes = NULL;

	bytes = soup_session_send_and_read_finish(SOUP_SESSION(source), result,
	                                          &error);
	(void)bytes;

	g_main_loop_quit(fix->loop);
}

/*
 * The POST path, unchanged: the substitution moved into the one function
 * both readers go through, so this is what says the move did not lose it.
 */
static void
test_a_plus_in_a_form_body_is_still_a_space(Fixture *fix,
                                            gconstpointer user_data)
{
	g_autoptr(SoupSession) session = soup_session_new();
	g_autoptr(SoupMessage) message = NULL;
	g_autofree gchar *url = NULL;
	g_autoptr(GBytes) body = NULL;

	(void)user_data;

	url = g_strdup_printf("http://127.0.0.1:%u/submit", fix->port);
	message = soup_message_new("POST", url);

	body = g_bytes_new_static("q=a+b&plus=%2B", strlen("q=a+b&plus=%2B"));
	soup_message_set_request_body_from_bytes(
		message, "application/x-www-form-urlencoded", body);

	soup_session_send_and_read_async(session, message, G_PRIORITY_DEFAULT,
	                                 NULL, on_body_read, fix);
	g_main_loop_run(fix->loop);

	g_assert_cmpstr(fix->seen, ==, "a b");
}

int
main(int argc, char *argv[])
{
	g_test_init(&argc, &argv, NULL);

	g_test_add_func("/request/a-plus-in-a-query-is-a-space",
	                test_a_plus_in_a_query_is_a_space);
	g_test_add_func("/request/an-escaped-plus-stays-a-plus",
	                test_an_escaped_plus_stays_a_plus);
	g_test_add_func("/request/a-plus-in-a-key-is-a-space",
	                test_a_plus_in_a_key_is_a_space);
	g_test_add_func("/request/a-percent-twenty-is-still-a-space",
	                test_a_percent_twenty_is_still_a_space);
	g_test_add("/request/a-plus-in-a-form-body-is-still-a-space",
	           Fixture, NULL, fixture_set_up,
	           test_a_plus_in_a_form_body_is_still_a_space,
	           fixture_tear_down);

	return g_test_run();
}
