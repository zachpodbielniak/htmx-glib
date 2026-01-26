/*
 * test-document.c - Tests for HTML document structure elements
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include <htmx-glib.h>

/*
 * Html tests
 */
static void
test_html_new(void)
{
	g_autoptr(HtmxHtml) html = htmx_html_new();

	g_assert_nonnull(html);
	g_assert_cmpstr(htmx_node_get_tag_name(HTMX_NODE(html)), ==, "html");
}

static void
test_html_set_lang(void)
{
	g_autoptr(HtmxHtml) html = htmx_html_new();
	g_autofree gchar *output = NULL;

	htmx_html_set_lang(html, "en");
	output = htmx_element_render(HTMX_ELEMENT(html));

	g_assert_true(g_str_has_prefix(output, "<html"));
	g_assert_nonnull(g_strstr_len(output, -1, "lang=\"en\""));
}

/*
 * Head tests
 */
static void
test_head_new(void)
{
	g_autoptr(HtmxHead) head = htmx_head_new();

	g_assert_nonnull(head);
	g_assert_cmpstr(htmx_node_get_tag_name(HTMX_NODE(head)), ==, "head");
}

static void
test_head_render(void)
{
	g_autoptr(HtmxHead) head = htmx_head_new();
	g_autofree gchar *html = NULL;

	html = htmx_element_render(HTMX_ELEMENT(head));

	g_assert_cmpstr(html, ==, "<head></head>");
}

/*
 * Body tests
 */
static void
test_body_new(void)
{
	g_autoptr(HtmxBody) body = htmx_body_new();

	g_assert_nonnull(body);
	g_assert_cmpstr(htmx_node_get_tag_name(HTMX_NODE(body)), ==, "body");
}

static void
test_body_render(void)
{
	g_autoptr(HtmxBody) body = htmx_body_new();
	g_autofree gchar *html = NULL;

	htmx_node_set_text_content(HTMX_NODE(body), "Hello World");
	html = htmx_element_render(HTMX_ELEMENT(body));

	g_assert_cmpstr(html, ==, "<body>Hello World</body>");
}

/*
 * Title tests
 */
static void
test_title_new(void)
{
	g_autoptr(HtmxTitle) title = htmx_title_new();

	g_assert_nonnull(title);
	g_assert_cmpstr(htmx_node_get_tag_name(HTMX_NODE(title)), ==, "title");
}

static void
test_title_new_with_text(void)
{
	g_autoptr(HtmxTitle) title = htmx_title_new_with_text("My Page");
	g_autofree gchar *html = NULL;

	html = htmx_element_render(HTMX_ELEMENT(title));

	g_assert_cmpstr(html, ==, "<title>My Page</title>");
}

/*
 * Base tests
 */
static void
test_base_new(void)
{
	g_autoptr(HtmxBase) base = htmx_base_new();

	g_assert_nonnull(base);
	g_assert_cmpstr(htmx_node_get_tag_name(HTMX_NODE(base)), ==, "base");
	g_assert_true(htmx_node_is_void_element(HTMX_NODE(base)));
}

static void
test_base_with_href(void)
{
	g_autoptr(HtmxBase) base = htmx_base_new_with_href("https://example.com/");
	g_autofree gchar *html = NULL;

	html = htmx_element_render(HTMX_ELEMENT(base));

	g_assert_nonnull(g_strstr_len(html, -1, "href=\"https://example.com/\""));
	g_assert_nonnull(g_strstr_len(html, -1, "/>"));
}

static void
test_base_set_target(void)
{
	g_autoptr(HtmxBase) base = htmx_base_new();
	g_autofree gchar *html = NULL;

	htmx_base_set_href(base, "/");
	htmx_base_set_target(base, "_blank");
	html = htmx_element_render(HTMX_ELEMENT(base));

	g_assert_nonnull(g_strstr_len(html, -1, "target=\"_blank\""));
}

/*
 * Document structure integration test
 */
static void
test_document_structure(void)
{
	g_autoptr(HtmxHtml) html = htmx_html_new();
	g_autoptr(HtmxHead) head = htmx_head_new();
	g_autoptr(HtmxTitle) title = htmx_title_new_with_text("Test");
	g_autoptr(HtmxBody) body = htmx_body_new();
	g_autofree gchar *output = NULL;

	htmx_html_set_lang(html, "en");

	/* Build structure */
	htmx_node_add_child(HTMX_NODE(head), HTMX_NODE(g_object_ref(title)));
	htmx_node_add_child(HTMX_NODE(html), HTMX_NODE(g_object_ref(head)));
	htmx_node_add_child(HTMX_NODE(html), HTMX_NODE(g_object_ref(body)));

	output = htmx_element_render(HTMX_ELEMENT(html));

	g_assert_nonnull(g_strstr_len(output, -1, "lang=\"en\""));
	g_assert_nonnull(g_strstr_len(output, -1, "<head>"));
	g_assert_nonnull(g_strstr_len(output, -1, "<title>Test</title>"));
	g_assert_nonnull(g_strstr_len(output, -1, "<body></body>"));
}

int
main(int argc, char *argv[])
{
	g_test_init(&argc, &argv, NULL);

	/* Html tests */
	g_test_add_func("/document/html/new", test_html_new);
	g_test_add_func("/document/html/set_lang", test_html_set_lang);

	/* Head tests */
	g_test_add_func("/document/head/new", test_head_new);
	g_test_add_func("/document/head/render", test_head_render);

	/* Body tests */
	g_test_add_func("/document/body/new", test_body_new);
	g_test_add_func("/document/body/render", test_body_render);

	/* Title tests */
	g_test_add_func("/document/title/new", test_title_new);
	g_test_add_func("/document/title/with_text", test_title_new_with_text);

	/* Base tests */
	g_test_add_func("/document/base/new", test_base_new);
	g_test_add_func("/document/base/with_href", test_base_with_href);
	g_test_add_func("/document/base/set_target", test_base_set_target);

	/* Integration test */
	g_test_add_func("/document/structure", test_document_structure);

	return g_test_run();
}
