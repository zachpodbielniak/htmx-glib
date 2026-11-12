/*
 * test-wipe.c - A password in a request body does not outlive the request
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * A login form posts its password in the body.  Every copy of that body
 * -- libsoup's chunks, the flattened buffer libsoup makes of them, the
 * HtmxRequest's own string and bytes, the decoded form values -- went back
 * to the allocator with g_free() and stayed readable in the heap until the
 * block was reused.  A core dump or an unrelated heap over-read then had
 * the plaintext in it.
 *
 * Freed memory cannot be inspected without undefined behaviour, so these
 * tests keep a reference to the storage they look at: a GBytes the test
 * holds stays allocated, and what the library wrote into it is legitimately
 * readable afterwards.  That is enough for libsoup's copies, which the
 * server clears in place.  HtmxRequest's own copy clears itself when its
 * last reference goes, which no reference-holding test can watch; the
 * secret-bytes test below pins the function that does it.
 */

#include <htmx-glib.h>

#include <string.h>

#define PASSWORD "correct-horse-battery-staple"

/* Whether @size bytes at @data are all zero. */
static gboolean
all_zero(
	gconstpointer data,
	gsize         size
){
	const guint8 *bytes = data;
	gsize i;

	for (i = 0; i < size; i++) {
		if (bytes[i] != 0) {
			return FALSE;
		}
	}

	return TRUE;
}

/* ── The helpers ──────────────────────────────────────────────────── */

static void
test_wipe_clears_and_tolerates_nothing(void)
{
	gchar buffer[] = PASSWORD;

	htmx_secret_wipe(buffer, sizeof buffer - 1);
	g_assert_true(all_zero(buffer, sizeof buffer));

	/* Neither of these may touch anything. */
	htmx_secret_wipe(NULL, 32);
	htmx_secret_wipe(buffer, 0);
	htmx_secret_free(NULL);
}

/*
 * The request's body string is the secret bytes read as a C string, so the
 * bytes must carry a NUL one past their end, must keep embedded NULs
 * (a multipart body is full of them), and must report the size given.
 */
static void
test_secret_bytes_are_a_terminated_copy(void)
{
	static const guint8 input[] = { 'a', '\0', 'b', 'c' };
	g_autoptr(GBytes) bytes = NULL;
	g_autoptr(GBytes) empty = NULL;
	const guint8 *data;
	gsize size = 0;

	bytes = htmx_secret_bytes_new(input, sizeof input);
	data = g_bytes_get_data(bytes, &size);

	g_assert_cmpuint(size, ==, sizeof input);
	g_assert_true(data != input);
	g_assert_cmpmem(data, size, input, sizeof input);
	g_assert_cmpuint(data[size], ==, 0);

	empty = htmx_secret_bytes_new(NULL, 0);
	g_assert_nonnull(empty);
	g_assert_cmpuint(g_bytes_get_size(empty), ==, 0);
}

/*
 * A body in two chunks, then flattened: three places the password sits.
 * soup_message_body_truncate() alone releases all three uncleared.
 */
static void
test_a_message_body_is_cleared_in_every_copy(void)
{
	SoupMessageBody *body = soup_message_body_new();
	g_autoptr(GBytes) first = g_bytes_new("password=", 9);
	g_autoptr(GBytes) second = g_bytes_new(PASSWORD, strlen(PASSWORD));
	g_autoptr(GBytes) flat = NULL;
	gsize size = 0;
	gconstpointer data;

	soup_message_body_append_bytes(body, first);
	soup_message_body_append_bytes(body, second);
	flat = soup_message_body_flatten(body);

	htmx_secret_wipe_message_body(body);

	g_assert_cmpint(body->length, ==, 0);
	data = g_bytes_get_data(first, &size);
	g_assert_true(all_zero(data, size));
	data = g_bytes_get_data(second, &size);
	g_assert_true(all_zero(data, size));
	data = g_bytes_get_data(flat, &size);
	g_assert_true(all_zero(data, size));

	/* Twice, and on an empty body, is harmless. */
	htmx_secret_wipe_message_body(body);
	htmx_secret_wipe_message_body(NULL);

	soup_message_body_unref(body);
}

/* ── Through a real server ────────────────────────────────────────── */

typedef struct {
	HtmxServer  *server;
	SoupSession *session;
	gchar       *base;
	GPtrArray   *held;       /* libsoup's body storage, kept alive */
	gchar       *seen;       /* the password as the handler read it */
	gboolean     finished;
} Fixture;

static HtmxResponse *
on_login(
	HtmxRequest *request,
	GHashTable  *params,
	gpointer     user_data
){
	Fixture *fix = user_data;
	SoupMessageBody *body;
	goffset offset = 0;
	(void)params;

	fix->seen = g_strdup(htmx_request_get_form_value(request, "password"));

	/*
	 * Hold every piece of storage libsoup keeps the body in, so the test
	 * can look at it after the request is over.
	 */
	body = soup_server_message_get_request_body(htmx_request_get_message(request));
	while (offset < body->length) {
		GBytes *chunk = soup_message_body_get_chunk(body, offset);

		if (chunk == NULL || g_bytes_get_size(chunk) == 0) {
			g_clear_pointer(&chunk, g_bytes_unref);
			break;
		}
		offset += (goffset)g_bytes_get_size(chunk);
		g_ptr_array_add(fix->held, chunk);
	}
	g_ptr_array_add(fix->held, soup_message_body_flatten(body));

	return htmx_response_new_with_content("welcome");
}

/* Connected after the server's own handler, so it runs after the wipe. */
static void
on_finished(
	SoupServer        *server,
	SoupServerMessage *msg,
	gpointer           user_data
){
	Fixture *fix = user_data;
	(void)server;
	(void)msg;

	fix->finished = TRUE;
}

static void
fixture_set_up(
	Fixture       *fix,
	gconstpointer  user_data
){
	g_autoptr(GError) error = NULL;
	GSList *uris;
	(void)user_data;

	memset(fix, 0, sizeof *fix);
	fix->server = htmx_server_new();
	fix->session = soup_session_new();
	fix->held = g_ptr_array_new_with_free_func((GDestroyNotify)g_bytes_unref);

	htmx_router_post(htmx_server_get_router(fix->server), "/login", on_login, fix);
	g_signal_connect(htmx_server_get_soup_server(fix->server), "request-finished",
	                 G_CALLBACK(on_finished), fix);

	g_assert_true(htmx_server_listen_on(fix->server, "127.0.0.1", 0, &error));
	g_assert_no_error(error);
	uris = htmx_server_get_listen_uris(fix->server);
	fix->base = g_strdup(uris->data);
	if (g_str_has_suffix(fix->base, "/")) {
		fix->base[strlen(fix->base) - 1] = '\0';
	}
	g_slist_free_full(uris, g_free);
}

static void
fixture_tear_down(
	Fixture       *fix,
	gconstpointer  user_data
){
	(void)user_data;

	htmx_server_stop(fix->server);
	g_clear_object(&fix->server);
	g_clear_object(&fix->session);
	g_clear_pointer(&fix->held, g_ptr_array_unref);
	g_free(fix->seen);
	g_free(fix->base);
}

static void
sent(
	GObject      *source,
	GAsyncResult *result,
	gpointer      user_data
){
	GBytes **answer = user_data;

	*answer = soup_session_send_and_read_finish(SOUP_SESSION(source), result, NULL);
}

/*
 * The handler still reads the password -- wiping must not cost the
 * application its input -- and once the request is finished every byte
 * libsoup held it in is zero.  Without the server's request-finished
 * handler the chunks and the flattened copy still spell the password.
 */
static void
test_a_posted_password_is_wiped_from_libsoup(
	Fixture       *fix,
	gconstpointer  user_data
){
	g_autoptr(SoupMessage) message = NULL;
	g_autoptr(GBytes) form = NULL;
	g_autoptr(GBytes) answer = NULL;
	g_autofree gchar *uri = NULL;
	gint64 deadline;
	guint i;
	(void)user_data;

	uri = g_strconcat(fix->base, "/login", NULL);
	message = soup_message_new("POST", uri);
	form = g_bytes_new_static("username=ada&password=" PASSWORD,
	                          strlen("username=ada&password=" PASSWORD));
	soup_message_set_request_body_from_bytes(message,
	                                         "application/x-www-form-urlencoded",
	                                         form);
	soup_session_send_and_read_async(fix->session, message, G_PRIORITY_DEFAULT,
	                                 NULL, sent, &answer);

	deadline = g_get_monotonic_time() + 10 * G_TIME_SPAN_SECOND;
	while ((answer == NULL || !fix->finished) &&
	       g_get_monotonic_time() < deadline) {
		g_main_context_iteration(NULL, TRUE);
	}

	g_assert_nonnull(answer);
	g_assert_true(fix->finished);
	g_assert_cmpuint(soup_message_get_status(message), ==, 200);
	g_assert_cmpstr(fix->seen, ==, PASSWORD);

	g_assert_cmpuint(fix->held->len, >, 0);
	for (i = 0; i < fix->held->len; i++) {
		GBytes *piece = g_ptr_array_index(fix->held, i);
		gsize size = 0;
		gconstpointer data = g_bytes_get_data(piece, &size);

		g_assert_cmpuint(size, >, 0);
		g_assert_true(all_zero(data, size));
	}
}

int
main(
	int   argc,
	char *argv[]
){
	g_test_init(&argc, &argv, NULL);

	g_test_add_func("/wipe/clears-and-tolerates-nothing",
	                test_wipe_clears_and_tolerates_nothing);
	g_test_add_func("/wipe/secret-bytes-are-a-terminated-copy",
	                test_secret_bytes_are_a_terminated_copy);
	g_test_add_func("/wipe/message-body-cleared-in-every-copy",
	                test_a_message_body_is_cleared_in_every_copy);
	g_test_add("/wipe/posted-password-wiped-from-libsoup", Fixture, NULL,
	           fixture_set_up, test_a_posted_password_is_wiped_from_libsoup,
	           fixture_tear_down);

	return g_test_run();
}
