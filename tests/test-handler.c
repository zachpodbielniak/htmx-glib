/*
 * test-handler.c - Tests for HtmxCallbackHandler
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include <htmx-glib.h>

static HtmxResponse *
dummy_callback(HtmxRequest *request,
               GHashTable  *params,
               gpointer     user_data)
{
	const gchar *msg = (const gchar *)user_data;

	(void)request;
	(void)params;

	return htmx_response_new_with_content(msg);
}

static HtmxResponse *
params_callback(HtmxRequest *request,
                GHashTable  *params,
                gpointer     user_data)
{
	const gchar *id;

	(void)request;
	(void)user_data;

	id = g_hash_table_lookup(params, "id");
	if (id != NULL)
		return htmx_response_new_with_content(id);
	return htmx_response_new_with_content("no-id");
}

static gint destroy_count = 0;

static void
test_destroy_notify(gpointer data)
{
	(void)data;
	destroy_count++;
}

static void
test_callback_handler_new(void)
{
	g_autoptr(HtmxCallbackHandler) handler = NULL;

	handler = htmx_callback_handler_new(dummy_callback, NULL, NULL);
	g_assert_nonnull(handler);
	g_assert_true(HTMX_IS_CALLBACK_HANDLER(handler));
}

static void
test_callback_handler_implements_interface(void)
{
	g_autoptr(HtmxCallbackHandler) handler = NULL;

	handler = htmx_callback_handler_new(dummy_callback, NULL, NULL);
	g_assert_true(HTMX_IS_HANDLER(handler));
}

static void
test_callback_handler_handle(void)
{
	g_autoptr(HtmxCallbackHandler) handler = NULL;
	g_autoptr(HtmxResponse) response = NULL;
	g_autoptr(HtmxRequest) request = NULL;
	GHashTable *params;

	handler = htmx_callback_handler_new(dummy_callback, (gpointer)"Hello", NULL);
	request = g_object_new(HTMX_TYPE_REQUEST, NULL);
	params = g_hash_table_new(g_str_hash, g_str_equal);

	response = htmx_handler_handle(HTMX_HANDLER(handler), request, params);

	g_assert_nonnull(response);
	g_assert_cmpstr(htmx_response_get_content(response), ==, "Hello");

	g_hash_table_unref(params);
}

static void
test_callback_handler_with_params(void)
{
	g_autoptr(HtmxCallbackHandler) handler = NULL;
	g_autoptr(HtmxResponse) response = NULL;
	g_autoptr(HtmxRequest) request = NULL;
	GHashTable *params;

	handler = htmx_callback_handler_new(params_callback, NULL, NULL);
	request = g_object_new(HTMX_TYPE_REQUEST, NULL);
	params = g_hash_table_new(g_str_hash, g_str_equal);
	g_hash_table_insert(params, (gpointer)"id", (gpointer)"42");

	response = htmx_handler_handle(HTMX_HANDLER(handler), request, params);

	g_assert_nonnull(response);
	g_assert_cmpstr(htmx_response_get_content(response), ==, "42");

	g_hash_table_unref(params);
}

static void
test_callback_handler_destroy_notify(void)
{
	HtmxCallbackHandler *handler;
	gchar *data;

	destroy_count = 0;
	data = g_strdup("test data");

	handler = htmx_callback_handler_new(dummy_callback, data, test_destroy_notify);
	g_assert_cmpint(destroy_count, ==, 0);

	g_object_unref(handler);
	g_assert_cmpint(destroy_count, ==, 1);
}

static void
test_callback_handler_no_destroy(void)
{
	HtmxCallbackHandler *handler;

	destroy_count = 0;

	handler = htmx_callback_handler_new(dummy_callback, (gpointer)"static", NULL);
	g_object_unref(handler);

	/* No crash, no destroy called */
	g_assert_cmpint(destroy_count, ==, 0);
}

int
main(int argc, char *argv[])
{
	g_test_init(&argc, &argv, NULL);

	g_test_add_func("/handler/callback/new", test_callback_handler_new);
	g_test_add_func("/handler/callback/implements_interface", test_callback_handler_implements_interface);
	g_test_add_func("/handler/callback/handle", test_callback_handler_handle);
	g_test_add_func("/handler/callback/with_params", test_callback_handler_with_params);
	g_test_add_func("/handler/callback/destroy_notify", test_callback_handler_destroy_notify);
	g_test_add_func("/handler/callback/no_destroy", test_callback_handler_no_destroy);

	return g_test_run();
}
