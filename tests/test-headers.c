/*
 * test-headers.c - Tests for HtmxRequestHeaders and HtmxResponseHeaders
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include <htmx-glib.h>

static void
test_request_headers_new(void)
{
	g_autoptr(HtmxRequestHeaders) headers = htmx_request_headers_new();

	g_assert_nonnull(headers);
	g_assert_false(htmx_request_headers_is_htmx_request(headers));
	g_assert_false(htmx_request_headers_is_boosted(headers));
}

static void
test_request_headers_copy(void)
{
	g_autoptr(HtmxRequestHeaders) headers = htmx_request_headers_new();
	g_autoptr(HtmxRequestHeaders) copy = htmx_request_headers_copy(headers);

	g_assert_nonnull(copy);
	g_assert_true(headers != copy);
}

static void
test_request_headers_get_type(void)
{
	GType type = htmx_request_headers_get_type();
	g_assert_true(g_type_is_a(type, G_TYPE_BOXED));
}

static void
test_response_headers_new(void)
{
	g_autoptr(HtmxResponseHeaders) headers = htmx_response_headers_new();

	g_assert_nonnull(headers);
}

static void
test_response_headers_copy(void)
{
	g_autoptr(HtmxResponseHeaders) headers = htmx_response_headers_new();
	htmx_response_headers_set_redirect(headers, "/new-page");

	g_autoptr(HtmxResponseHeaders) copy = htmx_response_headers_copy(headers);

	g_assert_nonnull(copy);
	g_assert_true(headers != copy);
}

static void
test_response_headers_get_type(void)
{
	GType type = htmx_response_headers_get_type();
	g_assert_true(g_type_is_a(type, G_TYPE_BOXED));
}

int
main(int argc, char *argv[])
{
	g_test_init(&argc, &argv, NULL);

	g_test_add_func("/headers/request/new", test_request_headers_new);
	g_test_add_func("/headers/request/copy", test_request_headers_copy);
	g_test_add_func("/headers/request/get_type", test_request_headers_get_type);
	g_test_add_func("/headers/response/new", test_response_headers_new);
	g_test_add_func("/headers/response/copy", test_response_headers_copy);
	g_test_add_func("/headers/response/get_type", test_response_headers_get_type);

	return g_test_run();
}
