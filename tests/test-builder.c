/*
 * test-builder.c - Tests for HtmxBuilder
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include <htmx-glib.h>

static void
test_builder_new(void)
{
	g_autoptr(HtmxBuilder) builder = htmx_builder_new();

	g_assert_nonnull(builder);
}

static void
test_builder_simple_element(void)
{
	g_autoptr(HtmxBuilder) builder = htmx_builder_new();
	g_autofree gchar *html = NULL;

	htmx_builder_begin(builder, "div");
	htmx_builder_text(builder, "Hello");
	htmx_builder_end(builder);

	html = htmx_builder_render(builder);
	g_assert_cmpstr(html, ==, "<div>Hello</div>");
}

static void
test_builder_nested_elements(void)
{
	g_autoptr(HtmxBuilder) builder = htmx_builder_new();
	g_autofree gchar *html = NULL;

	htmx_builder_begin(builder, "div");
	htmx_builder_begin(builder, "span");
	htmx_builder_text(builder, "Nested");
	htmx_builder_end(builder);
	htmx_builder_end(builder);

	html = htmx_builder_render(builder);
	g_assert_cmpstr(html, ==, "<div><span>Nested</span></div>");
}

static void
test_builder_with_attribute(void)
{
	g_autoptr(HtmxBuilder) builder = htmx_builder_new();
	g_autofree gchar *html = NULL;

	htmx_builder_begin(builder, "div");
	htmx_builder_attr(builder, "class", "container");
	htmx_builder_end(builder);

	html = htmx_builder_render(builder);
	g_assert_cmpstr(html, ==, "<div class=\"container\"></div>");
}

static void
test_builder_with_id(void)
{
	g_autoptr(HtmxBuilder) builder = htmx_builder_new();
	g_autofree gchar *html = NULL;

	htmx_builder_begin(builder, "div");
	htmx_builder_id(builder, "main");
	htmx_builder_end(builder);

	html = htmx_builder_render(builder);
	g_assert_cmpstr(html, ==, "<div id=\"main\"></div>");
}

static void
test_builder_void_element(void)
{
	g_autoptr(HtmxBuilder) builder = htmx_builder_new();
	g_autofree gchar *html = NULL;

	htmx_builder_begin_void(builder, "input");
	htmx_builder_attr(builder, "type", "text");
	htmx_builder_end_void(builder);

	html = htmx_builder_render(builder);
	g_assert_cmpstr(html, ==, "<input type=\"text\" />");
}

static void
test_builder_htmx_get(void)
{
	g_autoptr(HtmxBuilder) builder = htmx_builder_new();
	g_autofree gchar *html = NULL;

	htmx_builder_begin(builder, "button");
	htmx_builder_hx_get(builder, "/api/data");
	htmx_builder_text(builder, "Load");
	htmx_builder_end(builder);

	html = htmx_builder_render(builder);
	g_assert_cmpstr(html, ==, "<button hx-get=\"/api/data\">Load</button>");
}

static void
test_builder_htmx_post(void)
{
	g_autoptr(HtmxBuilder) builder = htmx_builder_new();
	g_autofree gchar *html = NULL;

	htmx_builder_begin(builder, "form");
	htmx_builder_hx_post(builder, "/api/submit");
	htmx_builder_end(builder);

	html = htmx_builder_render(builder);
	g_assert_cmpstr(html, ==, "<form hx-post=\"/api/submit\"></form>");
}

static void
test_builder_htmx_target(void)
{
	g_autoptr(HtmxBuilder) builder = htmx_builder_new();
	g_autofree gchar *html = NULL;

	htmx_builder_begin(builder, "button");
	htmx_builder_hx_get(builder, "/api/data");
	htmx_builder_hx_target(builder, "#result");
	htmx_builder_text(builder, "Load");
	htmx_builder_end(builder);

	html = htmx_builder_render(builder);
	g_assert_cmpstr(html, ==, "<button hx-target=\"#result\" hx-get=\"/api/data\">Load</button>");
}

static void
test_builder_doctype(void)
{
	g_autoptr(HtmxBuilder) builder = htmx_builder_new();
	g_autofree gchar *html = NULL;

	htmx_builder_doctype(builder);
	htmx_builder_begin(builder, "html");
	htmx_builder_end(builder);

	html = htmx_builder_render(builder);
	g_assert_cmpstr(html, ==, "<!DOCTYPE html><html></html>");
}

int
main(int argc, char *argv[])
{
	g_test_init(&argc, &argv, NULL);

	g_test_add_func("/builder/new", test_builder_new);
	g_test_add_func("/builder/simple_element", test_builder_simple_element);
	g_test_add_func("/builder/nested_elements", test_builder_nested_elements);
	g_test_add_func("/builder/with_attribute", test_builder_with_attribute);
	g_test_add_func("/builder/with_id", test_builder_with_id);
	g_test_add_func("/builder/void_element", test_builder_void_element);
	g_test_add_func("/builder/htmx_get", test_builder_htmx_get);
	g_test_add_func("/builder/htmx_post", test_builder_htmx_post);
	g_test_add_func("/builder/htmx_target", test_builder_htmx_target);
	g_test_add_func("/builder/doctype", test_builder_doctype);

	return g_test_run();
}
