/*
 * test-attribute.c - Tests for HtmxAttribute boxed type
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include <htmx-glib.h>

static void
test_attribute_new(void)
{
	g_autoptr(HtmxAttribute) attr = htmx_attribute_new("hx-get", "/api/data");

	g_assert_nonnull(attr);
	g_assert_cmpstr(htmx_attribute_get_name(attr), ==, "hx-get");
	g_assert_cmpstr(htmx_attribute_get_value(attr), ==, "/api/data");
}

static void
test_attribute_copy(void)
{
	g_autoptr(HtmxAttribute) attr = htmx_attribute_new("hx-target", "#result");
	g_autoptr(HtmxAttribute) copy = htmx_attribute_copy(attr);

	g_assert_nonnull(copy);
	g_assert_cmpstr(htmx_attribute_get_name(copy), ==, "hx-target");
	g_assert_cmpstr(htmx_attribute_get_value(copy), ==, "#result");
	g_assert_true(attr != copy);
}

static void
test_attribute_set_value(void)
{
	g_autoptr(HtmxAttribute) attr = htmx_attribute_new("class", "old-value");

	htmx_attribute_set_value(attr, "new-value");
	g_assert_cmpstr(htmx_attribute_get_value(attr), ==, "new-value");
}

static void
test_attribute_render(void)
{
	g_autoptr(HtmxAttribute) attr = htmx_attribute_new("hx-swap", "innerHTML");
	g_autofree gchar *rendered = htmx_attribute_render(attr);

	g_assert_cmpstr(rendered, ==, "hx-swap=\"innerHTML\"");
}

static void
test_attribute_render_escaping(void)
{
	g_autoptr(HtmxAttribute) attr = htmx_attribute_new("data-text", "Hello \"World\" & <Test>");
	g_autofree gchar *rendered = htmx_attribute_render(attr);

	g_assert_cmpstr(rendered, ==, "data-text=\"Hello &quot;World&quot; &amp; &lt;Test&gt;\"");
}

static void
test_attribute_is_htmx(void)
{
	g_autoptr(HtmxAttribute) htmx_attr = htmx_attribute_new("hx-get", "/api");
	g_autoptr(HtmxAttribute) normal_attr = htmx_attribute_new("class", "btn");

	g_assert_true(htmx_attribute_is_htmx(htmx_attr));
	g_assert_false(htmx_attribute_is_htmx(normal_attr));
}

static void
test_attribute_get_type(void)
{
	GType type = htmx_attribute_get_type();
	g_assert_true(g_type_is_a(type, G_TYPE_BOXED));
}

int
main(int argc, char *argv[])
{
	g_test_init(&argc, &argv, NULL);

	g_test_add_func("/attribute/new", test_attribute_new);
	g_test_add_func("/attribute/copy", test_attribute_copy);
	g_test_add_func("/attribute/set_value", test_attribute_set_value);
	g_test_add_func("/attribute/render", test_attribute_render);
	g_test_add_func("/attribute/render_escaping", test_attribute_render_escaping);
	g_test_add_func("/attribute/is_htmx", test_attribute_is_htmx);
	g_test_add_func("/attribute/get_type", test_attribute_get_type);

	return g_test_run();
}
