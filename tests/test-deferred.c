/*
 * test-deferred.c - Answers that arrive later, and answers made on threads
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * A handler that had to wait -- on another service, on a slow query --
 * used to hold the whole server, because the answer was whatever it
 * returned.  A deferred response pauses its message and lets the loop go
 * on; a threaded one computes the answer on a worker.  These tests hold
 * the two promises that make that worth having: a waiting request never
 * delays another, and threaded handlers really run side by side.
 */

#include <htmx-glib.h>

#include <string.h>

#define RENDEZVOUS (3)

typedef struct {
	HtmxServer   *server;
	SoupSession  *session;
	gchar        *base;
	HtmxResponse *pending;      /* the /slow placeholder, kept by the test */
	GPtrArray    *order;        /* paths, in the order their answers arrived */
	guint         outstanding;

	/* The threaded rendezvous */
	GMutex        lock;
	GCond         cond;
	guint         arrived;
	guint         max_together;

	/* Abandonment */
	gboolean      cancelled;
} Fixture;

/* ── Handlers ─────────────────────────────────────────────────────── */

static HtmxResponse *
on_fast(HtmxRequest *request, GHashTable *params, gpointer user_data)
{
	Fixture *fix = user_data;
	(void)request;
	(void)params;

	/* The slow request is still paused when this one is answered: that
	 * is the whole point.  Answer it now that the fast one has gone. */
	if (fix->pending != NULL) {
		g_autoptr(HtmxResponse) late = htmx_response_new_with_content("slow");
		g_autoptr(HtmxResponse) pending = g_steal_pointer(&fix->pending);

		htmx_response_complete(pending, late);
	}
	return htmx_response_new_with_content("fast");
}

static HtmxResponse *
on_slow(HtmxRequest *request, GHashTable *params, gpointer user_data)
{
	Fixture *fix = user_data;
	(void)request;
	(void)params;

	fix->pending = htmx_response_new_deferred();
	return g_object_ref(fix->pending);
}

static HtmxResponse *
on_now(HtmxRequest *request, GHashTable *params, gpointer user_data)
{
	g_autoptr(HtmxResponse) answer = htmx_response_new_with_content("now");
	HtmxResponse *deferred;
	(void)request;
	(void)params;
	(void)user_data;

	/* Completed before the handler even returns. */
	deferred = htmx_response_new_deferred();
	htmx_response_complete(deferred, answer);
	return deferred;
}

static HtmxResponse *
rendezvous(GCancellable *cancellable, gpointer user_data)
{
	Fixture *fix = user_data;
	gint64 deadline = g_get_monotonic_time() + 5 * G_TIME_SPAN_SECOND;
	(void)cancellable;

	/*
	 * Each worker waits until RENDEZVOUS of them are inside at once.  Run
	 * one at a time, the first would wait out the deadline alone and the
	 * count would never pass one -- so this proves concurrency without
	 * measuring anybody's clock.
	 */
	g_mutex_lock(&fix->lock);
	fix->arrived++;
	fix->max_together = MAX(fix->max_together, fix->arrived);
	g_cond_broadcast(&fix->cond);
	while (fix->max_together < RENDEZVOUS)
		if (!g_cond_wait_until(&fix->cond, &fix->lock, deadline))
			break;
	g_mutex_unlock(&fix->lock);

	return htmx_response_new_with_content("threaded");
}

static HtmxResponse *
on_threaded(HtmxRequest *request, GHashTable *params, gpointer user_data)
{
	(void)request;
	(void)params;
	return htmx_response_new_threaded(rendezvous, user_data, NULL);
}

static void
hook_header(HtmxResponse *deferred, HtmxResponse *response, gpointer user_data)
{
	(void)deferred;
	htmx_response_append_header(response, "X-Hooks", user_data);
}

static void
hooking_middleware(HtmxContext *context, HtmxMiddlewareNext next, gpointer next_data, gpointer user_data)
{
	HtmxResponse *response;

	next(context, next_data);
	response = htmx_context_get_response(context);
	if (response != NULL && htmx_response_is_deferred(response))
		htmx_response_on_complete(response, hook_header, user_data, NULL);
}

static gboolean
complete_late(gpointer user_data)
{
	Fixture *fix = user_data;
	g_autoptr(HtmxResponse) late = htmx_response_new_with_content("too late");

	/* The client has gone; whether libsoup noticed yet or not, writing
	 * the answer must be harmless. */
	htmx_response_complete(fix->pending, late);
	g_clear_object(&fix->pending);
	fix->cancelled = TRUE;
	return G_SOURCE_REMOVE;
}

/* ── Fixture ──────────────────────────────────────────────────────── */

static void
fixture_set_up(Fixture *fix, gconstpointer user_data)
{
	g_autoptr(GError) error = NULL;
	HtmxRouter *router;
	GSList *uris;
	(void)user_data;

	memset(fix, 0, sizeof *fix);
	g_mutex_init(&fix->lock);
	g_cond_init(&fix->cond);
	fix->server = htmx_server_new();
	/* libsoup's default of two connections per host would queue the
	 * third request in the client and hide what the server does. */
	fix->session = soup_session_new_with_options("max-conns-per-host", 16, NULL);
	fix->order = g_ptr_array_new_with_free_func(g_free);

	router = htmx_server_get_router(fix->server);
	htmx_router_get(router, "/fast", on_fast, fix);
	htmx_router_get(router, "/slow", on_slow, fix);
	htmx_router_get(router, "/now", on_now, fix);
	htmx_router_get(router, "/threaded", on_threaded, fix);
	htmx_server_use(fix->server, hooking_middleware, (gpointer)"a", NULL);
	htmx_server_use(fix->server, hooking_middleware, (gpointer)"b", NULL);

	g_assert_true(htmx_server_listen_on(fix->server, "127.0.0.1", 0, &error));
	g_assert_no_error(error);
	uris = htmx_server_get_listen_uris(fix->server);
	fix->base = g_strdup(uris->data);
	if (g_str_has_suffix(fix->base, "/"))
		fix->base[strlen(fix->base) - 1] = '\0';
	g_slist_free_full(uris, g_free);
}

static void
fixture_tear_down(Fixture *fix, gconstpointer user_data)
{
	(void)user_data;

	g_clear_object(&fix->pending);
	htmx_server_stop(fix->server);
	g_clear_object(&fix->server);
	g_clear_object(&fix->session);
	g_clear_pointer(&fix->order, g_ptr_array_unref);
	g_free(fix->base);
	g_mutex_clear(&fix->lock);
	g_cond_clear(&fix->cond);
}

typedef struct {
	Fixture     *fix;
	gchar       *path;
	SoupMessage *message;
	GBytes      *body;
} Call;

static void
call_done(GObject *source, GAsyncResult *result, gpointer user_data)
{
	Call *call = user_data;

	call->body = soup_session_send_and_read_finish(SOUP_SESSION(source), result, NULL);
	g_ptr_array_add(call->fix->order, g_strdup(call->path));
	call->fix->outstanding--;
}

static Call *
call_start(Fixture *fix, const gchar *path)
{
	Call *call = g_new0(Call, 1);
	g_autofree gchar *uri = g_strconcat(fix->base, path, NULL);

	call->fix = fix;
	call->path = g_strdup(path);
	call->message = soup_message_new("GET", uri);
	fix->outstanding++;
	soup_session_send_and_read_async(fix->session, call->message, G_PRIORITY_DEFAULT, NULL, call_done, call);
	return call;
}

static void
call_free(Call *call)
{
	g_free(call->path);
	g_clear_object(&call->message);
	g_clear_pointer(&call->body, g_bytes_unref);
	g_free(call);
}

static void
drain(Fixture *fix)
{
	gint64 deadline = g_get_monotonic_time() + 10 * G_TIME_SPAN_SECOND;

	while (fix->outstanding > 0 && g_get_monotonic_time() < deadline)
		g_main_context_iteration(NULL, TRUE);
	g_assert_cmpuint(fix->outstanding, ==, 0);
}

static void
assert_body(Call *call, const gchar *expected)
{
	gsize size = 0;
	const gchar *data;
	g_autofree gchar *text = NULL;

	g_assert_nonnull(call->body);
	data = g_bytes_get_data(call->body, &size);
	text = g_strndup(data, size);
	g_assert_cmpstr(text, ==, expected);
}

/* ── Tests ────────────────────────────────────────────────────────── */

/* Breaks if a waiting request holds the loop: /fast would never be
 * served, so /slow would never be completed, and drain() would fail. */
static void
test_waiting_does_not_block(Fixture *fix, gconstpointer user_data)
{
	Call *slow;
	Call *fast;
	gint64 deadline = g_get_monotonic_time() + 5 * G_TIME_SPAN_SECOND;
	(void)user_data;

	slow = call_start(fix, "/slow");
	while (fix->pending == NULL && g_get_monotonic_time() < deadline)
		g_main_context_iteration(NULL, TRUE);
	g_assert_nonnull(fix->pending);
	fast = call_start(fix, "/fast");
	drain(fix);

	g_assert_cmpuint(fix->order->len, ==, 2);
	assert_body(slow, "slow");
	assert_body(fast, "fast");
	g_assert_cmpuint(soup_message_get_status(slow->message), ==, 200);
	call_free(slow);
	call_free(fast);
}

/* Breaks if threaded handlers are serialised: no worker would ever see
 * RENDEZVOUS of them inside at once. */
static void
test_threads_run_together(Fixture *fix, gconstpointer user_data)
{
	Call *calls[RENDEZVOUS];
	guint i;
	(void)user_data;

	for (i = 0; i < RENDEZVOUS; i++)
		calls[i] = call_start(fix, "/threaded");
	drain(fix);
	g_assert_cmpuint(fix->max_together, ==, RENDEZVOUS);
	for (i = 0; i < RENDEZVOUS; i++) {
		assert_body(calls[i], "threaded");
		call_free(calls[i]);
	}
}

/* Breaks if middleware's post-processing is lost on a deferred answer,
 * or runs in a different order from the synchronous chain's (innermost
 * middleware first: "b" was added after "a", so it is inside it). */
static void
test_hooks_run_in_chain_order(Fixture *fix, gconstpointer user_data)
{
	Call *call;
	const gchar *hooks = NULL;
	(void)user_data;

	call = call_start(fix, "/threaded");
	fix->max_together = RENDEZVOUS;
	drain(fix);
	assert_body(call, "threaded");
	hooks = soup_message_headers_get_list(soup_message_get_response_headers(call->message), "X-Hooks");
	g_assert_cmpstr(hooks, ==, "b, a");
	call_free(call);
}

/* Breaks if a placeholder completed inside its own handler is dropped. */
static void
test_completed_before_return(Fixture *fix, gconstpointer user_data)
{
	Call *call;
	(void)user_data;

	call = call_start(fix, "/now");
	drain(fix);
	assert_body(call, "now");
	call_free(call);
}

/* Breaks if answering a request whose client has gone hurts the server:
 * the placeholder is completed after the client aborted, and the server
 * must go on answering the next request. */
static void
test_abandoned_is_harmless(Fixture *fix, gconstpointer user_data)
{
	Call *call;
	Call *after;
	GCancellable *cancel = g_cancellable_new();
	gint64 deadline = g_get_monotonic_time() + 5 * G_TIME_SPAN_SECOND;
	g_autofree gchar *uri = g_strconcat(fix->base, "/slow", NULL);
	g_autoptr(SoupMessage) message = soup_message_new("GET", uri);
	(void)user_data;

	call = g_new0(Call, 1);
	call->fix = fix;
	call->path = g_strdup("/slow");
	fix->outstanding++;
	soup_session_send_and_read_async(fix->session, message, G_PRIORITY_DEFAULT, cancel, call_done, call);
	while (fix->pending == NULL && g_get_monotonic_time() < deadline)
		g_main_context_iteration(NULL, TRUE);
	g_assert_nonnull(fix->pending);
	g_assert_nonnull(htmx_response_get_cancellable(fix->pending));

	g_cancellable_cancel(cancel);
	soup_session_abort(fix->session);
	g_timeout_add(50, complete_late, fix);
	while ((!fix->cancelled || fix->outstanding > 0) && g_get_monotonic_time() < deadline)
		g_main_context_iteration(NULL, TRUE);
	g_assert_true(fix->cancelled);

	after = call_start(fix, "/now");
	drain(fix);
	assert_body(after, "now");
	call_free(after);
	g_object_unref(cancel);
	call_free(call);
}

/*
 * Breaks if the server keeps GSocket's default backlog of ten.  The loop
 * is not iterated, so nothing is accepted: every connection has to wait
 * in the kernel's queue, and once that is full the kernel drops the SYN
 * and connect() times out.  A backlog of ten fails at the eleventh or
 * twelfth connection; the default holds all of them.
 */
static void
test_listen_backlog(Fixture *fix, gconstpointer user_data)
{
	g_autoptr(GUri) uri = g_uri_parse(fix->base, G_URI_FLAGS_NONE, NULL);
	g_autoptr(GPtrArray) sockets = g_ptr_array_new_with_free_func(g_object_unref);
	g_autoptr(GInetAddress) loopback = g_inet_address_new_loopback(G_SOCKET_FAMILY_IPV4);
	g_autoptr(GSocketAddress) address = NULL;
	guint i;
	(void)user_data;

	g_assert_nonnull(uri);
	address = g_inet_socket_address_new(loopback, (guint16)g_uri_get_port(uri));
	for (i = 0; i < 64; i++) {
		g_autoptr(GError) error = NULL;
		GSocket *socket = g_socket_new(G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_STREAM,
		                               G_SOCKET_PROTOCOL_TCP, &error);

		g_assert_no_error(error);
		g_socket_set_timeout(socket, 2);
		if (!g_socket_connect(socket, address, NULL, &error))
			g_error("connection %u was not queued: %s", i, error->message);
		g_ptr_array_add(sockets, socket);
	}
}

int
main(int argc, char **argv)
{
	g_test_init(&argc, &argv, NULL);

	g_test_add("/deferred/waiting-does-not-block", Fixture, NULL,
	           fixture_set_up, test_waiting_does_not_block, fixture_tear_down);
	g_test_add("/deferred/threads-run-together", Fixture, NULL,
	           fixture_set_up, test_threads_run_together, fixture_tear_down);
	g_test_add("/deferred/hooks-run-in-chain-order", Fixture, NULL,
	           fixture_set_up, test_hooks_run_in_chain_order, fixture_tear_down);
	g_test_add("/deferred/completed-before-return", Fixture, NULL,
	           fixture_set_up, test_completed_before_return, fixture_tear_down);
	g_test_add("/deferred/abandoned-is-harmless", Fixture, NULL,
	           fixture_set_up, test_abandoned_is_harmless, fixture_tear_down);
	g_test_add("/deferred/listen-backlog", Fixture, NULL,
	           fixture_set_up, test_listen_backlog, fixture_tear_down);

	return g_test_run();
}
