/*
 * test-serve-static.c - What a static route actually hands back
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * The router names image/png, image/jpeg and image/svg+xml in its MIME
 * table and then built the response with
 * htmx_response_new_with_content(), which g_strdup()s and is written out
 * with strlen() -- so every binary file it served was truncated at its
 * first NUL, which for a PNG is eight bytes in.  The length came back
 * from g_file_get_contents() and was thrown away.
 *
 * htmx-response.c's own comment states this hazard in as many words and
 * htmx_response_set_bytes() exists for it; the router just did not use
 * it.  The one existing test served a text file, so the binary case had
 * never run.
 */

#include <htmx-glib.h>
#include <glib/gstdio.h>
#include <string.h>

/* A PNG signature.  Byte 3 is zero, so strlen() says 2. */
static const guchar PNG[] = {
	0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a,
	0x00, 0x00, 0x00, 0x0d, 'I', 'H', 'D', 'R'
};

static void
test_a_binary_file_is_served_whole(void)
{
	g_autofree gchar *dir = g_dir_make_tmp("htmx-static-XXXXXX", NULL);
	g_autofree gchar *file = NULL;
	g_autoptr(HtmxRouter) router = htmx_router_new();
	g_autoptr(HtmxRequest) request = NULL;
	HtmxResponse *response;
	g_autoptr(GHashTable) params = NULL;
	GBytes *bytes;
	gsize size = 0;

	g_assert_nonnull(dir);

	file = g_build_filename(dir, "logo.png", NULL);
	g_assert_true(g_file_set_contents(file, (const gchar *)PNG,
	                                  sizeof(PNG), NULL));

	htmx_router_serve_static(router, "/static", dir);

	request = htmx_request_new_for_path(HTMX_METHOD_GET,
	                                    "/static/logo.png");
	response = htmx_router_match(router, request, &params);

	g_assert_nonnull(response);

	bytes = htmx_response_get_bytes(response);
	g_assert_nonnull(bytes);

	g_bytes_get_data(bytes, &size);

	/* strlen() would say 2. */
	g_assert_cmpuint(size, ==, sizeof(PNG));
	g_assert_cmpint(memcmp(g_bytes_get_data(bytes, NULL), PNG,
	                       sizeof(PNG)), ==, 0);

	g_object_unref(response);

	g_unlink(file);
	g_rmdir(dir);
}

/* And a text file still arrives, unchanged. */
static void
test_a_text_file_is_still_served(void)
{
	g_autofree gchar *dir = g_dir_make_tmp("htmx-static2-XXXXXX", NULL);
	g_autofree gchar *file = NULL;
	g_autoptr(HtmxRouter) router = htmx_router_new();
	g_autoptr(HtmxRequest) request = NULL;
	HtmxResponse *response;
	g_autoptr(GHashTable) params = NULL;
	GBytes *bytes;
	gsize size = 0;

	file = g_build_filename(dir, "app.js", NULL);
	g_assert_true(g_file_set_contents(file, "console.log(1);", -1, NULL));

	htmx_router_serve_static(router, "/static", dir);

	request = htmx_request_new_for_path(HTMX_METHOD_GET, "/static/app.js");
	response = htmx_router_match(router, request, &params);

	g_assert_nonnull(response);

	bytes = htmx_response_get_bytes(response);
	g_assert_nonnull(bytes);
	g_bytes_get_data(bytes, &size);
	g_assert_cmpuint(size, ==, strlen("console.log(1);"));

	g_object_unref(response);

	g_unlink(file);
	g_rmdir(dir);
}

int
main(int argc, char *argv[])
{
	g_test_init(&argc, &argv, NULL);

	g_test_add_func("/serve-static/binary-file-whole",
	                test_a_binary_file_is_served_whole);
	g_test_add_func("/serve-static/text-file-still-served",
	                test_a_text_file_is_still_served);

	return g_test_run();
}
