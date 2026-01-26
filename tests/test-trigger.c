/*
 * test-trigger.c - Tests for HtmxTrigger boxed type
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include <htmx-glib.h>

static void
test_trigger_new(void)
{
	g_autoptr(HtmxTrigger) trigger = htmx_trigger_new("click");

	g_assert_nonnull(trigger);
	g_assert_cmpstr(htmx_trigger_get_event_name(trigger), ==, "click");
}

static void
test_trigger_copy(void)
{
	g_autoptr(HtmxTrigger) trigger = htmx_trigger_new("submit");
	g_autoptr(HtmxTrigger) copy = htmx_trigger_copy(trigger);

	g_assert_nonnull(copy);
	g_assert_cmpstr(htmx_trigger_get_event_name(copy), ==, "submit");
	g_assert_true(trigger != copy);
}

static void
test_trigger_render_simple(void)
{
	g_autoptr(HtmxTrigger) trigger = htmx_trigger_new("click");
	g_autofree gchar *rendered = htmx_trigger_render(trigger);

	g_assert_cmpstr(rendered, ==, "click");
}

static void
test_trigger_get_type(void)
{
	GType type = htmx_trigger_get_type();
	g_assert_true(g_type_is_a(type, G_TYPE_BOXED));
}

int
main(int argc, char *argv[])
{
	g_test_init(&argc, &argv, NULL);

	g_test_add_func("/trigger/new", test_trigger_new);
	g_test_add_func("/trigger/copy", test_trigger_copy);
	g_test_add_func("/trigger/render_simple", test_trigger_render_simple);
	g_test_add_func("/trigger/get_type", test_trigger_get_type);

	return g_test_run();
}
