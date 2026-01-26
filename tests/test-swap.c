/*
 * test-swap.c - Tests for HtmxSwap boxed type
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include <htmx-glib.h>

static void
test_swap_new(void)
{
	g_autoptr(HtmxSwap) swap = htmx_swap_new(HTMX_SWAP_INNER_HTML);

	g_assert_nonnull(swap);
	g_assert_cmpint(htmx_swap_get_style(swap), ==, HTMX_SWAP_INNER_HTML);
}

static void
test_swap_copy(void)
{
	g_autoptr(HtmxSwap) swap = htmx_swap_new(HTMX_SWAP_OUTER_HTML);
	g_autoptr(HtmxSwap) copy = htmx_swap_copy(swap);

	g_assert_nonnull(copy);
	g_assert_cmpint(htmx_swap_get_style(copy), ==, HTMX_SWAP_OUTER_HTML);
	g_assert_true(swap != copy);
}

static void
test_swap_render_simple(void)
{
	g_autoptr(HtmxSwap) swap = htmx_swap_new(HTMX_SWAP_INNER_HTML);
	g_autofree gchar *rendered = htmx_swap_render(swap);

	g_assert_cmpstr(rendered, ==, "innerHTML");
}

static void
test_swap_render_outer_html(void)
{
	g_autoptr(HtmxSwap) swap = htmx_swap_new(HTMX_SWAP_OUTER_HTML);
	g_autofree gchar *rendered = htmx_swap_render(swap);

	g_assert_cmpstr(rendered, ==, "outerHTML");
}

static void
test_swap_get_type(void)
{
	GType type = htmx_swap_get_type();
	g_assert_true(g_type_is_a(type, G_TYPE_BOXED));
}

int
main(int argc, char *argv[])
{
	g_test_init(&argc, &argv, NULL);

	g_test_add_func("/swap/new", test_swap_new);
	g_test_add_func("/swap/copy", test_swap_copy);
	g_test_add_func("/swap/render_simple", test_swap_render_simple);
	g_test_add_func("/swap/render_outer_html", test_swap_render_outer_html);
	g_test_add_func("/swap/get_type", test_swap_get_type);

	return g_test_run();
}
