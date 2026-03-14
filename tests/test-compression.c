/*
 * test-compression.c - Tests for HtmxCompression middleware
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include <htmx-glib.h>

static void
test_compression_new(void)
{
	g_autoptr(HtmxCompression) comp = htmx_compression_new();

	g_assert_nonnull(comp);
	g_assert_true(HTMX_IS_COMPRESSION(comp));
}

static void
test_compression_default_min_size(void)
{
	g_autoptr(HtmxCompression) comp = htmx_compression_new();

	g_assert_cmpuint(htmx_compression_get_min_size(comp), ==, 256);
}

static void
test_compression_set_min_size(void)
{
	g_autoptr(HtmxCompression) comp = htmx_compression_new();

	htmx_compression_set_min_size(comp, 1024);
	g_assert_cmpuint(htmx_compression_get_min_size(comp), ==, 1024);

	htmx_compression_set_min_size(comp, 0);
	g_assert_cmpuint(htmx_compression_get_min_size(comp), ==, 0);
}

static void
test_compression_get_type(void)
{
	GType type = htmx_compression_get_type();
	g_assert_true(g_type_is_a(type, G_TYPE_OBJECT));
}

int
main(int argc, char *argv[])
{
	g_test_init(&argc, &argv, NULL);

	g_test_add_func("/compression/new", test_compression_new);
	g_test_add_func("/compression/default_min_size", test_compression_default_min_size);
	g_test_add_func("/compression/set_min_size", test_compression_set_min_size);
	g_test_add_func("/compression/get_type", test_compression_get_type);

	return g_test_run();
}
