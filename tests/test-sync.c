/*
 * test-sync.c - Tests for HtmxSync boxed type
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include <htmx-glib.h>

static void
test_sync_new_with_selector(void)
{
	g_autoptr(HtmxSync) sync = htmx_sync_new("closest form", HTMX_SYNC_ABORT);

	g_assert_nonnull(sync);
	g_assert_cmpstr(htmx_sync_get_selector(sync), ==, "closest form");
	g_assert_cmpint(htmx_sync_get_strategy(sync), ==, HTMX_SYNC_ABORT);
}

static void
test_sync_new_null_selector(void)
{
	g_autoptr(HtmxSync) sync = htmx_sync_new(NULL, HTMX_SYNC_DROP);

	g_assert_nonnull(sync);
	g_assert_null(htmx_sync_get_selector(sync));
	g_assert_cmpint(htmx_sync_get_strategy(sync), ==, HTMX_SYNC_DROP);
}

static void
test_sync_copy(void)
{
	g_autoptr(HtmxSync) sync = htmx_sync_new("this", HTMX_SYNC_REPLACE);
	g_autoptr(HtmxSync) copy = htmx_sync_copy(sync);

	g_assert_nonnull(copy);
	g_assert_true(sync != copy);
	g_assert_cmpstr(htmx_sync_get_selector(copy), ==, "this");
	g_assert_cmpint(htmx_sync_get_strategy(copy), ==, HTMX_SYNC_REPLACE);
}

static void
test_sync_get_type(void)
{
	GType type = htmx_sync_get_type();
	g_assert_true(g_type_is_a(type, G_TYPE_BOXED));
}

static void
test_sync_render_with_selector(void)
{
	g_autoptr(HtmxSync) sync = htmx_sync_new("closest form", HTMX_SYNC_ABORT);
	g_autofree gchar *rendered = htmx_sync_render(sync);

	g_assert_cmpstr(rendered, ==, "closest form:abort");
}

static void
test_sync_render_no_selector(void)
{
	g_autoptr(HtmxSync) sync = htmx_sync_new(NULL, HTMX_SYNC_DROP);
	g_autofree gchar *rendered = htmx_sync_render(sync);

	g_assert_cmpstr(rendered, ==, "drop");
}

static void
test_sync_render_empty_selector(void)
{
	g_autoptr(HtmxSync) sync = htmx_sync_new("", HTMX_SYNC_REPLACE);
	g_autofree gchar *rendered = htmx_sync_render(sync);

	g_assert_cmpstr(rendered, ==, "replace");
}

static void
test_sync_render_all_strategies(void)
{
	struct {
		HtmxSyncStrategy strategy;
		const gchar *expected;
	} cases[] = {
		{ HTMX_SYNC_DROP,        "drop" },
		{ HTMX_SYNC_ABORT,       "abort" },
		{ HTMX_SYNC_REPLACE,     "replace" },
		{ HTMX_SYNC_QUEUE_FIRST, "queue first" },
		{ HTMX_SYNC_QUEUE_LAST,  "queue last" },
		{ HTMX_SYNC_QUEUE_ALL,   "queue all" }
	};
	guint i;

	for (i = 0; i < G_N_ELEMENTS(cases); i++) {
		g_autoptr(HtmxSync) sync = htmx_sync_new(NULL, cases[i].strategy);
		g_autofree gchar *rendered = htmx_sync_render(sync);

		g_assert_cmpstr(rendered, ==, cases[i].expected);
	}
}

static void
test_sync_parse_with_selector(void)
{
	GError *error = NULL;
	g_autoptr(HtmxSync) sync = htmx_sync_parse("closest form:abort", &error);

	g_assert_no_error(error);
	g_assert_nonnull(sync);
	g_assert_cmpstr(htmx_sync_get_selector(sync), ==, "closest form");
	g_assert_cmpint(htmx_sync_get_strategy(sync), ==, HTMX_SYNC_ABORT);
}

static void
test_sync_parse_no_selector(void)
{
	GError *error = NULL;
	g_autoptr(HtmxSync) sync = htmx_sync_parse("drop", &error);

	g_assert_no_error(error);
	g_assert_nonnull(sync);
	g_assert_null(htmx_sync_get_selector(sync));
	g_assert_cmpint(htmx_sync_get_strategy(sync), ==, HTMX_SYNC_DROP);
}

static void
test_sync_parse_queue_strategies(void)
{
	GError *error = NULL;
	g_autoptr(HtmxSync) sync = NULL;

	sync = htmx_sync_parse("this:queue first", &error);
	g_assert_no_error(error);
	g_assert_nonnull(sync);
	g_assert_cmpint(htmx_sync_get_strategy(sync), ==, HTMX_SYNC_QUEUE_FIRST);
	htmx_sync_free(g_steal_pointer(&sync));

	sync = htmx_sync_parse("this:queue last", &error);
	g_assert_no_error(error);
	g_assert_nonnull(sync);
	g_assert_cmpint(htmx_sync_get_strategy(sync), ==, HTMX_SYNC_QUEUE_LAST);
	htmx_sync_free(g_steal_pointer(&sync));

	sync = htmx_sync_parse("this:queue all", &error);
	g_assert_no_error(error);
	g_assert_nonnull(sync);
	g_assert_cmpint(htmx_sync_get_strategy(sync), ==, HTMX_SYNC_QUEUE_ALL);
}

static void
test_sync_parse_invalid_strategy(void)
{
	GError *error = NULL;
	HtmxSync *sync = htmx_sync_parse("closest form:bogus", &error);

	g_assert_null(sync);
	g_assert_error(error, HTMX_ERROR, HTMX_ERROR_INVALID_REQUEST);
	g_error_free(error);
}

static void
test_sync_parse_whitespace(void)
{
	GError *error = NULL;
	g_autoptr(HtmxSync) sync = htmx_sync_parse("  closest form : abort ", &error);

	g_assert_no_error(error);
	g_assert_nonnull(sync);
	g_assert_cmpstr(htmx_sync_get_selector(sync), ==, "closest form");
	g_assert_cmpint(htmx_sync_get_strategy(sync), ==, HTMX_SYNC_ABORT);
}

static void
test_sync_roundtrip(void)
{
	GError *error = NULL;
	g_autoptr(HtmxSync) original = htmx_sync_new("closest form", HTMX_SYNC_QUEUE_LAST);
	g_autofree gchar *rendered = htmx_sync_render(original);
	g_autoptr(HtmxSync) parsed = htmx_sync_parse(rendered, &error);

	g_assert_no_error(error);
	g_assert_nonnull(parsed);
	g_assert_cmpstr(htmx_sync_get_selector(parsed), ==, "closest form");
	g_assert_cmpint(htmx_sync_get_strategy(parsed), ==, HTMX_SYNC_QUEUE_LAST);
}

int
main(int argc, char *argv[])
{
	g_test_init(&argc, &argv, NULL);

	g_test_add_func("/sync/new_with_selector", test_sync_new_with_selector);
	g_test_add_func("/sync/new_null_selector", test_sync_new_null_selector);
	g_test_add_func("/sync/copy", test_sync_copy);
	g_test_add_func("/sync/get_type", test_sync_get_type);
	g_test_add_func("/sync/render_with_selector", test_sync_render_with_selector);
	g_test_add_func("/sync/render_no_selector", test_sync_render_no_selector);
	g_test_add_func("/sync/render_empty_selector", test_sync_render_empty_selector);
	g_test_add_func("/sync/render_all_strategies", test_sync_render_all_strategies);
	g_test_add_func("/sync/parse_with_selector", test_sync_parse_with_selector);
	g_test_add_func("/sync/parse_no_selector", test_sync_parse_no_selector);
	g_test_add_func("/sync/parse_queue_strategies", test_sync_parse_queue_strategies);
	g_test_add_func("/sync/parse_invalid_strategy", test_sync_parse_invalid_strategy);
	g_test_add_func("/sync/parse_whitespace", test_sync_parse_whitespace);
	g_test_add_func("/sync/roundtrip", test_sync_roundtrip);

	return g_test_run();
}
