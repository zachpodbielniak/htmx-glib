/*
 * test-session.c - Set-Cookie is a header whose job is to repeat
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * htmx_response_add_header() does a g_hash_table_insert() into
 * custom_headers, so the last writer for a name wins -- which is right
 * for Content-Type and wrong for the one header in HTTP whose entire
 * design is that there is more than one of it.
 * htmx_response_append_header() exists for exactly that, was added in a
 * commit whose message says so, and had no caller anywhere in the
 * library: the single place htmx-glib emits a repeatable header used the
 * other one.
 *
 * The result is silent in both directions.  A handler that sets a cookie
 * loses it to the session middleware, which runs after it; or the
 * session cookie is the one lost, and the browser then has no session,
 * so the next request makes another and nothing ever persists.
 */

#include <htmx-glib.h>
#include <string.h>

typedef struct {
	HtmxServer  *server;
	HtmxSession *session;
	GMainLoop   *loop;
	GPtrArray   *set_cookies;   /* gchar*, one per Set-Cookie header */
	const gchar *path;          /* which route to fetch */
	guint16      port;
} Fixture;

/* A route that sets a cookie of its own, the way any login handler does. */
static HtmxResponse *
on_page(HtmxRequest *request, GHashTable *params, gpointer user_data)
{
	HtmxResponse *response;

	(void)request;
	(void)params;
	(void)user_data;

	response = htmx_response_new_with_content("<p>page</p>");
	htmx_response_add_header(response, "Set-Cookie",
	                         "flavour=chocolate; Path=/");

	return response;
}

/* And one that sets two of its own, through the appending setter. */
static HtmxResponse *
on_two(HtmxRequest *request, GHashTable *params, gpointer user_data)
{
	HtmxResponse *response;

	(void)request;
	(void)params;
	(void)user_data;

	response = htmx_response_new_with_content("<p>two</p>");
	htmx_response_append_header(response, "Set-Cookie", "first=1; Path=/");
	htmx_response_append_header(response, "Set-Cookie", "second=2; Path=/");

	return response;
}

static void
fixture_set_up(Fixture *fix, gconstpointer user_data)
{
	g_autoptr(GError) error = NULL;
	GSList *uris;

	(void)user_data;

	fix->server = htmx_server_new();
	fix->session = htmx_session_new();
	fix->loop = g_main_loop_new(NULL, FALSE);
	fix->set_cookies = g_ptr_array_new_with_free_func(g_free);
	fix->path = "/page";

	htmx_router_get(htmx_server_get_router(fix->server), "/page",
	                on_page, NULL);
	htmx_router_get(htmx_server_get_router(fix->server), "/two",
	                on_two, NULL);
	htmx_server_use(fix->server, htmx_session_middleware,
	                g_object_ref(fix->session), g_object_unref);

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
	g_clear_object(&fix->session);
	g_clear_pointer(&fix->loop, g_main_loop_unref);
	g_ptr_array_unref(fix->set_cookies);
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
 * Collect the Set-Cookie headers one at a time.
 *
 * Not soup_message_headers_get_list(), which joins repeated headers with
 * a comma -- and a cookie's Expires attribute contains a comma, so the
 * joined form cannot be split back apart and a count taken from it would
 * be a count of something else.
 */
static void
fetch(Fixture *fix)
{
	g_autoptr(SoupSession) session = soup_session_new();
	g_autoptr(SoupMessage) message = NULL;
	g_autofree gchar *url = NULL;
	SoupMessageHeadersIter iter;
	const gchar *name;
	const gchar *value;

	/*
	 * No cookie jar: it consumes Set-Cookie and would answer a question
	 * about its own storage rather than about what was sent.
	 */
	soup_session_remove_feature_by_type(session, SOUP_TYPE_COOKIE_JAR);

	url = g_strdup_printf("http://127.0.0.1:%u%s", fix->port, fix->path);
	message = soup_message_new("GET", url);

	soup_session_send_and_read_async(session, message, G_PRIORITY_DEFAULT,
	                                 NULL, on_body_read, fix);
	g_main_loop_run(fix->loop);

	soup_message_headers_iter_init(&iter,
	                               soup_message_get_response_headers(message));

	while (soup_message_headers_iter_next(&iter, &name, &value)) {
		if (g_ascii_strcasecmp(name, "Set-Cookie") == 0) {
			g_ptr_array_add(fix->set_cookies, g_strdup(value));
		}
	}
}

static gboolean
any_contains(GPtrArray *values, const gchar *needle)
{
	guint i;

	for (i = 0; i < values->len; i++) {
		if (strstr(g_ptr_array_index(values, i), needle) != NULL) {
			return TRUE;
		}
	}

	return FALSE;
}

/*
 * The handler's cookie and the session's both reach the browser.
 *
 * Through add_header() there was one Set-Cookie, and it was the session
 * middleware's: it runs after the handler on the way out, so the
 * hash-table insert overwrote the handler's.
 */
static void
test_both_cookies_are_sent(Fixture *fix, gconstpointer user_data)
{
	(void)user_data;

	fetch(fix);

	g_assert_cmpuint(fix->set_cookies->len, ==, 2);
	g_assert_true(any_contains(fix->set_cookies, "flavour=chocolate"));
	g_assert_true(any_contains(fix->set_cookies, "htmx_sid="));
}

/*
 * And a guard on the setter itself.
 *
 * htmx_response_append_header() had no caller and no test anywhere in the
 * library, which is how the one place that needed it came to use
 * add_header() instead: nothing had ever demonstrated that the two
 * differ.  This case passes either way -- it is here so that the function
 * the fix depends on is exercised rather than assumed.  Three cookies,
 * because two would also be the answer if the first of the handler's pair
 * were being dropped.
 */
static void
test_append_header_repeats(Fixture *fix, gconstpointer user_data)
{
	(void)user_data;

	fix->path = "/two";
	fetch(fix);

	g_assert_cmpuint(fix->set_cookies->len, ==, 3);
	g_assert_true(any_contains(fix->set_cookies, "first=1"));
	g_assert_true(any_contains(fix->set_cookies, "second=2"));
	g_assert_true(any_contains(fix->set_cookies, "htmx_sid="));
}

int
main(int argc, char *argv[])
{
	g_test_init(&argc, &argv, NULL);

	g_test_add("/session/both-cookies-are-sent",
	           Fixture, NULL, fixture_set_up,
	           test_both_cookies_are_sent, fixture_tear_down);
	g_test_add("/session/append-header-repeats",
	           Fixture, NULL, fixture_set_up,
	           test_append_header_repeats, fixture_tear_down);

	return g_test_run();
}
