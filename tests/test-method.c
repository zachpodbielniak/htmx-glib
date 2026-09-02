/*
 * test-method.c - A method this library cannot name is not a GET
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * HtmxMethod had five members and the string-to-enum mapping in
 * htmx_request_new_from_message() ended `return HTMX_METHOD_GET`, so
 * every method nobody had listed became a GET before anything downstream
 * saw it.  `FROB /page` matched a route registered with
 * htmx_router_get(), ran that handler, and came back 200; `FROB
 * /static/secret.txt` was served the file.  htmx_request_get_method()
 * reported HTMX_METHOD_GET to any middleware that asked, with nothing
 * left in the request to say the client had never sent one.
 *
 * The CSRF fail-open this caused was closed separately, by reading the
 * method string off the SoupServerMessage rather than the enum.  These
 * tests are about the value: the enum has HTMX_METHOD_UNKNOWN now, and
 * the requests above have to reach nothing.
 */

#include <htmx-glib.h>
#include <glib/gstdio.h>
#include <string.h>

typedef struct {
	HtmxServer *server;
	GMainLoop  *loop;
	gchar      *static_dir;
	gboolean    get_handler_ran;
	gboolean    saw_a_request;   /* the middleware ran at all */
	HtmxMethod  method_seen;     /* what the request reported to it */
	guint       status;
	gchar      *body;
	guint16     port;
} Fixture;

/*
 * A route registered for GET.  Reaching this at all with anything but a
 * GET is the defect.
 */
static HtmxResponse *
on_get_page(HtmxRequest *request, GHashTable *params, gpointer user_data)
{
	Fixture *fix = user_data;

	(void)request;
	(void)params;

	fix->get_handler_ran = TRUE;

	return htmx_response_new_with_content("<p>the page</p>");
}

/*
 * Middleware runs for every request, routed or not, which is the only
 * place a request that matches nothing can still be asked what it is.
 */
static void
record_method(
	HtmxContext        *context,
	HtmxMiddlewareNext  next,
	gpointer            next_data,
	gpointer            user_data
){
	Fixture *fix = user_data;

	fix->saw_a_request = TRUE;
	fix->method_seen = htmx_request_get_method(
		htmx_context_get_request(context));

	next(context, next_data);
}

static void
fixture_set_up(Fixture *fix, gconstpointer user_data)
{
	g_autoptr(GError) error = NULL;
	g_autofree gchar *secret = NULL;
	GSList *uris;

	(void)user_data;

	fix->server = htmx_server_new();
	fix->loop = g_main_loop_new(NULL, FALSE);
	fix->get_handler_ran = FALSE;
	fix->saw_a_request = FALSE;
	fix->method_seen = HTMX_METHOD_UNKNOWN;
	fix->status = 0;
	fix->body = NULL;

	fix->static_dir = g_dir_make_tmp("htmx-method-XXXXXX", &error);
	g_assert_no_error(error);
	g_assert_nonnull(fix->static_dir);

	secret = g_build_filename(fix->static_dir, "secret.txt", NULL);
	g_assert_true(g_file_set_contents(secret, "the file", -1, &error));
	g_assert_no_error(error);

	htmx_router_get(htmx_server_get_router(fix->server), "/page",
	                on_get_page, fix);
	htmx_router_serve_static(htmx_server_get_router(fix->server),
	                         "/static", fix->static_dir);

	htmx_middleware_use(htmx_server_get_middleware(fix->server),
	                    record_method, fix, NULL);

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
	g_autofree gchar *secret = NULL;

	(void)user_data;

	htmx_server_stop(fix->server);
	g_clear_object(&fix->server);
	g_clear_pointer(&fix->loop, g_main_loop_unref);
	g_clear_pointer(&fix->body, g_free);

	/*
	 * Both the file and the directory: g_rmdir() does nothing to a
	 * directory that still has something in it, and a green run that
	 * leaves one behind leaves one behind every time.
	 */
	if (fix->static_dir != NULL) {
		secret = g_build_filename(fix->static_dir, "secret.txt", NULL);
		g_unlink(secret);
		g_rmdir(fix->static_dir);
	}

	g_clear_pointer(&fix->static_dir, g_free);
}

static void
on_body_read(GObject *source, GAsyncResult *result, gpointer user_data)
{
	Fixture *fix = user_data;
	g_autoptr(GError) error = NULL;
	g_autoptr(GBytes) bytes = NULL;

	bytes = soup_session_send_and_read_finish(SOUP_SESSION(source), result,
	                                          &error);

	g_clear_pointer(&fix->body, g_free);
	if (bytes != NULL) {
		gsize length = 0;
		const gchar *data = g_bytes_get_data(bytes, &length);

		fix->body = g_strndup(data, length);
	}

	g_main_loop_quit(fix->loop);
}

/*
 * Send @method to @path and park until the reply is in.  Returns nothing;
 * the fixture carries the status and the body.
 */
static void
request(Fixture *fix, const gchar *method, const gchar *path)
{
	g_autoptr(SoupSession) session = soup_session_new();
	g_autoptr(SoupMessage) message = NULL;
	g_autofree gchar *url = NULL;

	url = g_strdup_printf("http://127.0.0.1:%u%s", fix->port, path);
	message = soup_message_new(method, url);
	g_assert_nonnull(message);

	soup_session_send_and_read_async(session, message, G_PRIORITY_DEFAULT,
	                                 NULL, on_body_read, fix);
	g_main_loop_run(fix->loop);

	fix->status = soup_message_get_status(message);
}

/*
 * The whole defect in one assertion: the route was registered for GET and
 * the request was not a GET, so the handler must not have run.  Before
 * the fix the method arrived as HTMX_METHOD_GET, the route matched, and
 * this came back 200 with the page in it.
 */
static void
test_an_unknown_method_does_not_reach_a_get_route(Fixture *fix,
                                                  gconstpointer user_data)
{
	(void)user_data;

	request(fix, "FROB", "/page");

	g_assert_false(fix->get_handler_ran);
	g_assert_cmpint(fix->status, ==, SOUP_STATUS_NOT_FOUND);
}

/*
 * htmx_router_match() gates the static tree on `method == HTMX_METHOD_GET`,
 * so the same substitution handed a file to anything that asked for one
 * under a name nobody recognised.
 */
static void
test_an_unknown_method_is_not_served_a_static_file(Fixture *fix,
                                                   gconstpointer user_data)
{
	(void)user_data;

	request(fix, "FROB", "/static/secret.txt");

	g_assert_cmpint(fix->status, ==, SOUP_STATUS_NOT_FOUND);
	g_assert_cmpstr(fix->body, !=, "the file");
}

/*
 * And the value itself, which is what every other consumer of
 * htmx_request_get_method() reads.  A middleware sees the request whether
 * or not it routes, so this is the assertion the two above cannot make:
 * not merely that the request went nowhere, but that what it reports is
 * distinguishable from a GET.
 */
static void
test_the_request_reports_the_method_as_unknown(Fixture *fix,
                                               gconstpointer user_data)
{
	(void)user_data;

	request(fix, "FROB", "/page");

	g_assert_true(fix->saw_a_request);
	g_assert_cmpint(fix->method_seen, ==, HTMX_METHOD_UNKNOWN);
	g_assert_cmpint(fix->method_seen, !=, HTMX_METHOD_GET);
}

/* A real GET is untouched by any of this. */
static void
test_a_get_still_reaches_its_route(Fixture *fix, gconstpointer user_data)
{
	(void)user_data;

	request(fix, "GET", "/page");

	g_assert_true(fix->get_handler_ran);
	g_assert_cmpint(fix->method_seen, ==, HTMX_METHOD_GET);
	g_assert_cmpint(fix->status, ==, SOUP_STATUS_OK);
}

/* And a real GET is still served the static file. */
static void
test_a_get_is_still_served_a_static_file(Fixture *fix,
                                         gconstpointer user_data)
{
	(void)user_data;

	request(fix, "GET", "/static/secret.txt");

	g_assert_cmpint(fix->status, ==, SOUP_STATUS_OK);
	g_assert_cmpstr(fix->body, ==, "the file");
}

/*
 * HEAD is the half of the fall-through that was right, and the reason
 * HTMX_METHOD_HEAD is named rather than left to fall into
 * HTMX_METHOD_UNKNOWN.  RFC 9110 section 9.3.2 makes it identical to GET
 * but for the content, so it still reaches the GET route -- deliberately
 * now, through route_method_matches(), rather than because the parser
 * could not tell it from a GET.
 *
 * Without that clause every HEAD in every application on this library
 * becomes a 404, which is a regression the FROB tests above cannot see.
 */
static void
test_head_still_reaches_a_get_route(Fixture *fix, gconstpointer user_data)
{
	(void)user_data;

	request(fix, "HEAD", "/page");

	g_assert_true(fix->get_handler_ran);
	g_assert_cmpint(fix->status, ==, SOUP_STATUS_OK);

	/* Reaching the route is not the same as being called a GET. */
	g_assert_cmpint(fix->method_seen, ==, HTMX_METHOD_HEAD);
	g_assert_cmpint(fix->method_seen, !=, HTMX_METHOD_GET);
}

/* And the static tree, for the same reason and through the same gate. */
static void
test_head_is_still_served_a_static_file(Fixture *fix,
                                        gconstpointer user_data)
{
	(void)user_data;

	request(fix, "HEAD", "/static/secret.txt");

	g_assert_cmpint(fix->status, ==, SOUP_STATUS_OK);
}

/*
 * OPTIONS is the case HEAD is not.  It is a real method with its own
 * meaning, it has no name in this enum, and answering it with whatever a
 * GET route renders was never right -- so it becomes
 * HTMX_METHOD_UNKNOWN, matches nothing, and 404s.
 */
static void
test_options_does_not_reach_a_get_route(Fixture *fix,
                                        gconstpointer user_data)
{
	(void)user_data;

	request(fix, "OPTIONS", "/page");

	g_assert_false(fix->get_handler_ran);
	g_assert_cmpint(fix->method_seen, ==, HTMX_METHOD_UNKNOWN);
}

int
main(int argc, char *argv[])
{
	g_test_init(&argc, &argv, NULL);

	g_test_add("/method/an-unknown-method-does-not-reach-a-get-route",
	           Fixture, NULL, fixture_set_up,
	           test_an_unknown_method_does_not_reach_a_get_route,
	           fixture_tear_down);
	g_test_add("/method/an-unknown-method-is-not-served-a-static-file",
	           Fixture, NULL, fixture_set_up,
	           test_an_unknown_method_is_not_served_a_static_file,
	           fixture_tear_down);
	g_test_add("/method/the-request-reports-the-method-as-unknown",
	           Fixture, NULL, fixture_set_up,
	           test_the_request_reports_the_method_as_unknown,
	           fixture_tear_down);
	g_test_add("/method/a-get-still-reaches-its-route",
	           Fixture, NULL, fixture_set_up,
	           test_a_get_still_reaches_its_route,
	           fixture_tear_down);
	g_test_add("/method/a-get-is-still-served-a-static-file",
	           Fixture, NULL, fixture_set_up,
	           test_a_get_is_still_served_a_static_file,
	           fixture_tear_down);
	g_test_add("/method/head-still-reaches-a-get-route",
	           Fixture, NULL, fixture_set_up,
	           test_head_still_reaches_a_get_route,
	           fixture_tear_down);
	g_test_add("/method/head-is-still-served-a-static-file",
	           Fixture, NULL, fixture_set_up,
	           test_head_is_still_served_a_static_file,
	           fixture_tear_down);
	g_test_add("/method/options-does-not-reach-a-get-route",
	           Fixture, NULL, fixture_set_up,
	           test_options_does_not_reach_a_get_route,
	           fixture_tear_down);

	return g_test_run();
}
