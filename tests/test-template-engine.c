/*
 * test-template-engine.c - What a template name is allowed to name
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * load_template() turned a name into a filename with g_build_filename()
 * and read it, with no containment check of any kind -- while
 * htmx_router_serve_static(), in this same library, already had
 * path_is_inside() doing exactly that on the canonical path.  A template
 * name reaches this function from two places a request can influence:
 * htmx_template_engine_render(self, name, ...) and a {{> name}} partial
 * inside a template.  Both read /etc/passwd with a name of "../".
 *
 * Separately, the partial branch did `} else if (*error != NULL)` while
 * the header annotates @error as (out) (optional), so a caller that
 * passed NULL for a template with a missing partial segfaulted rather
 * than getting a NULL return.
 */

#include <htmx-glib.h>
#include <glib/gstdio.h>
#include <string.h>

typedef struct {
	gchar *root;      /* search path handed to the engine */
	gchar *outside;   /* a file next to it, which nothing may read */
} Fixture;

static void
fixture_set_up(Fixture *fix, gconstpointer user_data)
{
	g_autoptr(GError) error = NULL;
	g_autofree gchar *base = NULL;

	(void)user_data;

	base = g_dir_make_tmp("htmx-tmpl-XXXXXX", &error);
	g_assert_no_error(error);

	fix->root = g_build_filename(base, "templates", NULL);
	g_assert_cmpint(g_mkdir_with_parents(fix->root, 0700), ==, 0);

	fix->outside = g_build_filename(base, "secret.txt", NULL);
	g_assert_true(g_file_set_contents(fix->outside, "TOPSECRET", -1, &error));
	g_assert_no_error(error);
}

static void
remove_tree(const gchar *path)
{
	g_autoptr(GDir) dir = g_dir_open(path, 0, NULL);
	const gchar *name;

	if (dir != NULL) {
		while ((name = g_dir_read_name(dir)) != NULL) {
			g_autofree gchar *child =
				g_build_filename(path, name, NULL);

			if (g_file_test(child, G_FILE_TEST_IS_DIR)) {
				remove_tree(child);
			} else {
				g_unlink(child);
			}
		}
	}

	g_rmdir(path);
}

static void
fixture_tear_down(Fixture *fix, gconstpointer user_data)
{
	g_autofree gchar *base = g_path_get_dirname(fix->root);

	(void)user_data;

	remove_tree(base);
	g_free(fix->root);
	g_free(fix->outside);
}

/*
 * The name given to render() is not allowed to leave the search path.
 *
 * "../secret.txt" came back as TOPSECRET: g_build_filename() joins it
 * happily and g_file_get_contents() does not care where the file is.
 */
static void
test_render_name_cannot_escape(Fixture *fix, gconstpointer user_data)
{
	g_autoptr(HtmxTemplateEngine) engine = htmx_template_engine_new();
	g_autoptr(GError) error = NULL;
	g_autofree gchar *out = NULL;

	(void)user_data;

	htmx_template_engine_add_path(engine, fix->root);

	g_test_expect_message(NULL, G_LOG_LEVEL_WARNING, "htmx: refusing*");
	out = htmx_template_engine_render(engine, "../secret.txt", NULL, &error);
	g_test_assert_expected_messages();

	g_assert_null(out);
	g_assert_nonnull(error);
}

/*
 * And neither is the name inside a {{> partial}}, which is the reachable
 * half: the template itself can be one an application ships, while the
 * partial name comes out of it.
 */
static void
test_partial_name_cannot_escape(Fixture *fix, gconstpointer user_data)
{
	g_autoptr(HtmxTemplateEngine) engine = htmx_template_engine_new();
	g_autoptr(GError) error = NULL;
	g_autofree gchar *page = NULL;
	g_autofree gchar *out = NULL;

	(void)user_data;

	page = g_build_filename(fix->root, "page.html", NULL);
	g_assert_true(g_file_set_contents(page,
	                                  "A{{> ../secret.txt}}B", -1, NULL));

	htmx_template_engine_add_path(engine, fix->root);

	g_test_expect_message(NULL, G_LOG_LEVEL_WARNING, "htmx: refusing*");
	out = htmx_template_engine_render(engine, "page.html", NULL, &error);
	g_test_assert_expected_messages();

	/*
	 * Whether this is an error or an empty include is a judgement; that
	 * the bytes of a file outside the search path are in the answer is
	 * not.
	 */
	if (out != NULL) {
		g_assert_null(strstr(out, "TOPSECRET"));
	}
}

/*
 * A name that stays inside still works, including one in a subdirectory
 * -- a containment check that forbids nesting has broken the feature to
 * fix the hole.
 */
static void
test_a_name_that_stays_inside_still_loads(Fixture *fix,
                                          gconstpointer user_data)
{
	g_autoptr(HtmxTemplateEngine) engine = htmx_template_engine_new();
	g_autoptr(GError) error = NULL;
	g_autofree gchar *sub = NULL;
	g_autofree gchar *partial = NULL;
	g_autofree gchar *page = NULL;
	g_autofree gchar *out = NULL;

	(void)user_data;

	sub = g_build_filename(fix->root, "parts", NULL);
	g_assert_cmpint(g_mkdir_with_parents(sub, 0700), ==, 0);

	partial = g_build_filename(sub, "head.html", NULL);
	g_assert_true(g_file_set_contents(partial, "<head>", -1, NULL));

	page = g_build_filename(fix->root, "page.html", NULL);
	g_assert_true(g_file_set_contents(page,
	                                  "{{> parts/head.html}}{{name}}", -1,
	                                  NULL));

	htmx_template_engine_add_path(engine, fix->root);

	{
		g_autoptr(GHashTable) context =
			g_hash_table_new(g_str_hash, g_str_equal);

		g_hash_table_insert(context, (gpointer)"name", (gpointer)"kudu");
		out = htmx_template_engine_render(engine, "page.html", context,
		                                  &error);
	}

	g_assert_no_error(error);
	g_assert_cmpstr(out, ==, "<head>kudu");
}

/*
 * @error is annotated (out) (optional), so NULL is a legal thing to pass.
 * The partial branch read *error unconditionally, so a template naming a
 * partial that does not exist crashed the server rather than returning
 * NULL.
 */
static void
test_a_missing_partial_with_no_error_pointer(Fixture *fix,
                                             gconstpointer user_data)
{
	g_autoptr(HtmxTemplateEngine) engine = htmx_template_engine_new();
	g_autofree gchar *out = NULL;

	(void)user_data;

	htmx_template_engine_add_path(engine, fix->root);

	/* The whole assertion is that this returns at all. */
	out = htmx_template_engine_render_string(engine, "x{{> nope.html}}y",
	                                         NULL, NULL);
	g_assert_null(out);
}

int
main(int argc, char *argv[])
{
	g_test_init(&argc, &argv, NULL);

	g_test_add("/template-engine/render-name-cannot-escape",
	           Fixture, NULL, fixture_set_up,
	           test_render_name_cannot_escape, fixture_tear_down);
	g_test_add("/template-engine/partial-name-cannot-escape",
	           Fixture, NULL, fixture_set_up,
	           test_partial_name_cannot_escape, fixture_tear_down);
	g_test_add("/template-engine/a-name-that-stays-inside-still-loads",
	           Fixture, NULL, fixture_set_up,
	           test_a_name_that_stays_inside_still_loads, fixture_tear_down);
	g_test_add("/template-engine/missing-partial-with-no-error-pointer",
	           Fixture, NULL, fixture_set_up,
	           test_a_missing_partial_with_no_error_pointer,
	           fixture_tear_down);

	return g_test_run();
}
