/*
 * test-response-bytes.c - A response body that is not a string
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * htmx_response_set_content() takes a NUL-terminated string, which is
 * right for HTML and cannot carry a PNG: the first zero byte ends the
 * body, so an image served that way arrives truncated at a length that
 * depends on the picture.  These assert on the *length* rather than on
 * the pointer, because that is the thing that was wrong.
 */

#include <htmx-glib.h>
#include <string.h>

/* A PNG signature: the second byte of a real one is 0x50, but the eighth
 * is 0x0a and byte three is 'N' -- and a zero appears in the very first
 * chunk length.  This has one at index 2 on purpose. */
static const guchar SAMPLE[] = { 0x89, 'P', 0x00, 'G', 0x0d, 0x0a, 0x1a, 0x0a };

static void
test_bytes_survive_an_embedded_nul(void)
{
	HtmxResponse *response = htmx_response_new();
	g_autoptr(GBytes) bytes = g_bytes_new(SAMPLE, sizeof(SAMPLE));
	GBytes *back;
	gsize size = 0;

	htmx_response_set_bytes(response, bytes);

	back = htmx_response_get_bytes(response);
	g_assert_nonnull(back);
	g_bytes_get_data(back, &size);

	/* strlen() would say 2. */
	g_assert_cmpuint(size, ==, sizeof(SAMPLE));
	g_assert_cmpint(memcmp(g_bytes_get_data(back, NULL), SAMPLE,
	                       sizeof(SAMPLE)), ==, 0);

	g_object_unref(response);
}

/*
 * Bytes win over content, so a handler that set both does not serve the
 * string by accident.
 */
static void
test_bytes_win_over_content(void)
{
	HtmxResponse *response = htmx_response_new_with_content("<p>hi</p>");
	g_autoptr(GBytes) bytes = g_bytes_new(SAMPLE, sizeof(SAMPLE));
	gsize size = 0;

	htmx_response_set_bytes(response, bytes);

	g_assert_cmpstr(htmx_response_get_content(response), ==, "<p>hi</p>");
	g_bytes_get_data(htmx_response_get_bytes(response), &size);
	g_assert_cmpuint(size, ==, sizeof(SAMPLE));

	g_object_unref(response);
}

/* Clearing them puts the string back in charge. */
static void
test_bytes_can_be_cleared(void)
{
	HtmxResponse *response = htmx_response_new_with_content("<p>hi</p>");
	g_autoptr(GBytes) bytes = g_bytes_new(SAMPLE, sizeof(SAMPLE));

	htmx_response_set_bytes(response, bytes);
	htmx_response_set_bytes(response, NULL);

	g_assert_null(htmx_response_get_bytes(response));
	g_assert_cmpstr(htmx_response_get_content(response), ==, "<p>hi</p>");

	g_object_unref(response);
}

int
main(int argc, char *argv[])
{
	g_test_init(&argc, &argv, NULL);

	g_test_add_func("/response/bytes/embedded-nul",
	                test_bytes_survive_an_embedded_nul);
	g_test_add_func("/response/bytes/win-over-content",
	                test_bytes_win_over_content);
	g_test_add_func("/response/bytes/cleared", test_bytes_can_be_cleared);

	return g_test_run();
}
