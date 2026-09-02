/*
 * test-csrf.c - The token a page is holding, and which methods need one
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * htmx_csrf_middleware() called htmx_csrf_generate_token() on every safe
 * request, and there is exactly one token slot per session -- so the
 * second GET a page makes, for a favicon or an hx-get fragment or in a
 * second tab, replaced the token sitting in the form the reader is about
 * to submit.  The submit then came back 403.  Nothing about that looks
 * like a CSRF bug from either end: the form was rendered by the server,
 * the token was correct when it was written, and the request that
 * invalidated it succeeded.
 *
 * The method half is the other way round from how it reads.
 * is_safe_method() named only HTMX_METHOD_GET, but HtmxMethod has no
 * HEAD and no OPTIONS at all and parse_method() answers HTMX_METHOD_GET
 * for every string it does not recognise -- so those two were already
 * safe, and so was every method nobody has heard of.  The decision was
 * being taken from a value that cannot represent the request.
 */

#include <htmx-glib.h>
#include <string.h>

typedef struct {
	HtmxServer  *server;
	HtmxSession *session;
	HtmxCsrf    *csrf;
	GMainLoop   *loop;
	gchar       *token;      /* what the middleware put on the context */
	gchar       *cookie;     /* name=value from the first Set-Cookie */
	guint        status;
	guint16      port;
} Fixture;

static HtmxResponse *
on_page(HtmxRequest *request, GHashTable *params, gpointer user_data)
{
	(void)request;
	(void)params;
	(void)user_data;

	return htmx_response_new_with_content("<p>page</p>");
}

/*
 * Registered after the CSRF middleware, so it runs inside its next() and
 * sees the token that request was given.  This is also the only way a
 * route handler can reach it: HtmxRouteCallback is handed the request and
 * the route parameters, never the context.
 */
static void
capture_token(HtmxContext        *context,
              HtmxMiddlewareNext  next,
              gpointer            next_data,
              gpointer            user_data)
{
	Fixture *fix = user_data;

	g_clear_pointer(&fix->token, g_free);
	fix->token = g_strdup((const gchar *)htmx_context_get_data(
		context, "htmx_csrf_token"));

	next(context, next_data);
}

static void
fixture_set_up(Fixture *fix, gconstpointer user_data)
{
	g_autoptr(GError) error = NULL;
	GSList *uris;

	(void)user_data;

	fix->server = htmx_server_new();
	fix->session = htmx_session_new();
	fix->csrf = htmx_csrf_new(fix->session);
	fix->loop = g_main_loop_new(NULL, FALSE);
	fix->token = NULL;
	fix->cookie = NULL;
	fix->status = 0;

	htmx_router_get(htmx_server_get_router(fix->server), "/page",
	                on_page, NULL);
	htmx_router_post(htmx_server_get_router(fix->server), "/page",
	                 on_page, NULL);

	htmx_server_use(fix->server, htmx_session_middleware,
	                g_object_ref(fix->session), g_object_unref);
	htmx_server_use(fix->server, htmx_csrf_middleware,
	                g_object_ref(fix->csrf), g_object_unref);
	htmx_server_use(fix->server, capture_token, fix, NULL);

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
	g_clear_object(&fix->csrf);
	g_clear_object(&fix->session);
	g_clear_pointer(&fix->loop, g_main_loop_unref);
	g_clear_pointer(&fix->token, g_free);
	g_clear_pointer(&fix->cookie, g_free);
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
 * One request, with the session cookie carried by hand.
 *
 * By hand rather than through a SoupCookieJar because the point of
 * several of these is that two requests share one session, and a jar
 * that quietly failed to store the cookie would make them pass while
 * describing something else.
 */
static void
request(Fixture *fix, const gchar *method, const gchar *csrf_token)
{
	g_autoptr(SoupSession) session = soup_session_new();
	g_autoptr(SoupMessage) message = NULL;
	g_autofree gchar *url = NULL;

	url = g_strdup_printf("http://127.0.0.1:%u/page", fix->port);
	message = soup_message_new(method, url);

	if (fix->cookie != NULL) {
		soup_message_headers_replace(
			soup_message_get_request_headers(message),
			"Cookie", fix->cookie);
	}

	if (csrf_token != NULL) {
		soup_message_headers_replace(
			soup_message_get_request_headers(message),
			"X-CSRF-Token", csrf_token);
	}

	soup_session_send_and_read_async(session, message, G_PRIORITY_DEFAULT,
	                                 NULL, on_body_read, fix);
	g_main_loop_run(fix->loop);

	fix->status = soup_message_get_status(message);

	if (fix->cookie == NULL) {
		const gchar *set = soup_message_headers_get_one(
			soup_message_get_response_headers(message),
			"Set-Cookie");

		if (set != NULL) {
			const gchar *semi = strchr(set, ';');

			fix->cookie = semi != NULL
				? g_strndup(set, (gsize)(semi - set))
				: g_strdup(set);
		}
	}
}

/*
 * A second safe request must not take the token away from the first.
 *
 * Every real page makes more than one: a favicon, a stylesheet through
 * the same middleware stack, an hx-get that fills a panel.  The form is
 * rendered once and submitted later.
 */
static void
test_a_second_get_does_not_replace_the_token(Fixture *fix,
                                             gconstpointer user_data)
{
	g_autofree gchar *from_the_page = NULL;

	(void)user_data;

	request(fix, "GET", NULL);
	g_assert_cmpuint(fix->status, ==, 200);
	g_assert_nonnull(fix->token);
	from_the_page = g_strdup(fix->token);

	request(fix, "GET", NULL);
	g_assert_cmpuint(fix->status, ==, 200);
	g_assert_cmpstr(fix->token, ==, from_the_page);

	/* And the form written from the first one still submits. */
	request(fix, "POST", from_the_page);
	g_assert_cmpuint(fix->status, ==, 200);
}

/*
 * Submitting does not invalidate the token either -- there is one slot
 * per session, so rotating it here breaks the second open form for
 * exactly the same reason.
 */
static void
test_a_post_does_not_replace_the_token(Fixture *fix,
                                       gconstpointer user_data)
{
	g_autofree gchar *from_the_page = NULL;

	(void)user_data;

	request(fix, "GET", NULL);
	from_the_page = g_strdup(fix->token);

	request(fix, "POST", from_the_page);
	g_assert_cmpuint(fix->status, ==, 200);

	request(fix, "POST", from_the_page);
	g_assert_cmpuint(fix->status, ==, 200);
}

/* The refusal still happens for a token that is wrong or absent. */
static void
test_a_bad_token_is_still_refused(Fixture *fix, gconstpointer user_data)
{
	(void)user_data;

	request(fix, "GET", NULL);

	request(fix, "POST", "not-the-token");
	g_assert_cmpuint(fix->status, ==, 403);

	request(fix, "POST", NULL);
	g_assert_cmpuint(fix->status, ==, 403);
}

/*
 * HEAD, OPTIONS and TRACE are the safe methods RFC 9110 names alongside
 * GET, and they must not need a token.  They did not before either --
 * but only because parse_method() could not tell them apart from GET,
 * which is the same reason FROB below got through.
 */
static void
test_the_other_safe_methods_need_no_token(Fixture *fix,
                                          gconstpointer user_data)
{
	(void)user_data;

	request(fix, "GET", NULL);

	request(fix, "HEAD", NULL);
	g_assert_cmpuint(fix->status, ==, 200);

	request(fix, "OPTIONS", NULL);
	g_assert_cmpuint(fix->status, ==, 200);
}

/*
 * A method nobody has heard of is not safe.
 *
 * HtmxMethod has five members and parse_method() returns HTMX_METHOD_GET
 * for anything else, so asking the enum whether a request is safe gets
 * "yes" for every unrecognised method -- a fail-open in the one function
 * whose answer decides whether a credential is required.
 */
static void
test_an_unknown_method_is_not_safe(Fixture *fix, gconstpointer user_data)
{
	(void)user_data;

	request(fix, "GET", NULL);

	request(fix, "FROB", NULL);
	g_assert_cmpuint(fix->status, ==, 403);
}

/*
 * htmx_csrf_generate_token() keeps meaning what it says: mint a new one.
 * It is the explicit rotation an application performs when the identity
 * behind the session changes; the middleware is what stopped calling it
 * per request.
 */
static void
test_generate_token_still_rotates(void)
{
	g_autoptr(HtmxSession) session = htmx_session_new();
	g_autoptr(HtmxCsrf) csrf = htmx_csrf_new(session);
	g_autofree gchar *first = NULL;

	first = g_strdup(htmx_csrf_generate_token(csrf, "a-session"));
	g_assert_nonnull(first);

	g_assert_cmpstr(first, !=, htmx_csrf_generate_token(csrf, "a-session"));
	g_assert_false(htmx_csrf_validate_token(csrf, "a-session", first));
}

int
main(int argc, char *argv[])
{
	g_test_init(&argc, &argv, NULL);

	g_test_add("/csrf/a-second-get-does-not-replace-the-token",
	           Fixture, NULL, fixture_set_up,
	           test_a_second_get_does_not_replace_the_token,
	           fixture_tear_down);
	g_test_add("/csrf/a-post-does-not-replace-the-token",
	           Fixture, NULL, fixture_set_up,
	           test_a_post_does_not_replace_the_token, fixture_tear_down);
	g_test_add("/csrf/a-bad-token-is-still-refused",
	           Fixture, NULL, fixture_set_up,
	           test_a_bad_token_is_still_refused, fixture_tear_down);
	g_test_add("/csrf/the-other-safe-methods-need-no-token",
	           Fixture, NULL, fixture_set_up,
	           test_the_other_safe_methods_need_no_token,
	           fixture_tear_down);
	g_test_add("/csrf/an-unknown-method-is-not-safe",
	           Fixture, NULL, fixture_set_up,
	           test_an_unknown_method_is_not_safe, fixture_tear_down);
	g_test_add_func("/csrf/generate-token-still-rotates",
	                test_generate_token_still_rotates);

	return g_test_run();
}
