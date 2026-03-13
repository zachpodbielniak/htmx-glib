/*
 * htmx-template-engine.c - Template engine implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-template-engine.h"
#include "htmx-error.h"
#include <string.h>

struct _HtmxTemplateEngine {
	GObject     parent_instance;

	GPtrArray  *search_paths;   /* gchar* directories */
	GHashTable *cache;          /* name (gchar*) -> content (gchar*) */
	GHashTable *partials;       /* name (gchar*) -> content (gchar*) */
	gboolean    cache_enabled;
};

G_DEFINE_FINAL_TYPE(HtmxTemplateEngine, htmx_template_engine, G_TYPE_OBJECT)

static void
htmx_template_engine_finalize(GObject *object)
{
	HtmxTemplateEngine *self = HTMX_TEMPLATE_ENGINE(object);

	g_ptr_array_unref(self->search_paths);
	g_hash_table_unref(self->cache);
	g_hash_table_unref(self->partials);

	G_OBJECT_CLASS(htmx_template_engine_parent_class)->finalize(object);
}

static void
htmx_template_engine_class_init(HtmxTemplateEngineClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);

	object_class->finalize = htmx_template_engine_finalize;
}

static void
htmx_template_engine_init(HtmxTemplateEngine *self)
{
	self->search_paths = g_ptr_array_new_with_free_func(g_free);
	self->cache = g_hash_table_new_full(g_str_hash, g_str_equal,
	                                   g_free, g_free);
	self->partials = g_hash_table_new_full(g_str_hash, g_str_equal,
	                                      g_free, g_free);
	self->cache_enabled = TRUE;
}

HtmxTemplateEngine *
htmx_template_engine_new(void)
{
	return g_object_new(HTMX_TYPE_TEMPLATE_ENGINE, NULL);
}

void
htmx_template_engine_add_path(HtmxTemplateEngine *self,
                              const gchar        *path)
{
	g_return_if_fail(HTMX_IS_TEMPLATE_ENGINE(self));
	g_return_if_fail(path != NULL);

	g_ptr_array_add(self->search_paths, g_strdup(path));
}

void
htmx_template_engine_set_cache_enabled(HtmxTemplateEngine *self,
                                       gboolean            enabled)
{
	g_return_if_fail(HTMX_IS_TEMPLATE_ENGINE(self));

	self->cache_enabled = enabled;
	if (!enabled) {
		g_hash_table_remove_all(self->cache);
	}
}

void
htmx_template_engine_add_partial(HtmxTemplateEngine *self,
                                 const gchar        *name,
                                 const gchar        *content)
{
	g_return_if_fail(HTMX_IS_TEMPLATE_ENGINE(self));
	g_return_if_fail(name != NULL);
	g_return_if_fail(content != NULL);

	g_hash_table_insert(self->partials, g_strdup(name), g_strdup(content));
}

/*
 * Load a template file from the search paths.
 */
static gchar *
load_template(HtmxTemplateEngine *self, const gchar *name, GError **error)
{
	guint i;
	gchar *content = NULL;

	/* Check cache first */
	if (self->cache_enabled) {
		content = (gchar *)g_hash_table_lookup(self->cache, name);
		if (content != NULL) {
			return g_strdup(content);
		}
	}

	/* Search paths */
	for (i = 0; i < self->search_paths->len; i++) {
		const gchar *dir = (const gchar *)g_ptr_array_index(
			self->search_paths, i);
		g_autofree gchar *path = g_build_filename(dir, name, NULL);

		if (g_file_get_contents(path, &content, NULL, NULL)) {
			if (self->cache_enabled) {
				g_hash_table_insert(self->cache,
				                    g_strdup(name),
				                    g_strdup(content));
			}
			return content;
		}
	}

	g_set_error(error, HTMX_ERROR, HTMX_ERROR_TEMPLATE_ERROR,
	            "Template '%s' not found in search paths", name);
	return NULL;
}

/*
 * HTML-escape a string. Caller must free result.
 */
static gchar *
html_escape(const gchar *str)
{
	GString *escaped;
	const gchar *p;

	if (str == NULL) {
		return g_strdup("");
	}

	escaped = g_string_sized_new(strlen(str));

	for (p = str; *p != '\0'; p++) {
		switch (*p) {
		case '&':
			g_string_append(escaped, "&amp;");
			break;
		case '<':
			g_string_append(escaped, "&lt;");
			break;
		case '>':
			g_string_append(escaped, "&gt;");
			break;
		case '"':
			g_string_append(escaped, "&quot;");
			break;
		case '\'':
			g_string_append(escaped, "&#39;");
			break;
		default:
			g_string_append_c(escaped, *p);
			break;
		}
	}

	return g_string_free(escaped, FALSE);
}

/*
 * Render a template string with variable substitution and partial includes.
 * Handles:
 *   {{key}}    — escaped variable
 *   {{{key}}}  — unescaped variable
 *   {{> name}} — partial include
 */
static gchar *
render_template_string(HtmxTemplateEngine *self,
                       const gchar        *tmpl,
                       GHashTable         *context,
                       guint               depth,
                       GError            **error)
{
	GString *result;
	const gchar *p;
	const gchar *end;

	if (depth > 16) {
		g_set_error(error, HTMX_ERROR, HTMX_ERROR_TEMPLATE_ERROR,
		            "Template recursion depth exceeded (max 16)");
		return NULL;
	}

	result = g_string_sized_new(strlen(tmpl));
	p = tmpl;

	while (*p != '\0') {
		const gchar *open = strstr(p, "{{");

		if (open == NULL) {
			/* No more tags, append rest */
			g_string_append(result, p);
			break;
		}

		/* Append text before the tag */
		g_string_append_len(result, p, (gssize)(open - p));

		if (*(open + 2) == '{') {
			/* Triple braces: {{{key}}} — raw/unescaped */
			const gchar *key_start = open + 3;
			end = strstr(key_start, "}}}");

			if (end == NULL) {
				g_string_append(result, "{{{");
				p = open + 3;
				continue;
			}

			{
				g_autofree gchar *key = g_strndup(
					key_start, (gsize)(end - key_start));
				g_strstrip(key);

				if (context != NULL) {
					const gchar *val = (const gchar *)
						g_hash_table_lookup(context, key);
					if (val != NULL) {
						g_string_append(result, val);
					}
				}
			}

			p = end + 3;
		} else {
			/* Double braces: {{key}} or {{> partial}} */
			const gchar *key_start = open + 2;
			end = strstr(key_start, "}}");

			if (end == NULL) {
				g_string_append(result, "{{");
				p = open + 2;
				continue;
			}

			{
				g_autofree gchar *tag = g_strndup(
					key_start, (gsize)(end - key_start));
				g_strstrip(tag);

				if (tag[0] == '>' && tag[1] == ' ') {
					/* Partial include: {{> name}} */
					const gchar *partial_name = tag + 2;
					const gchar *partial_content;
					g_autofree gchar *rendered = NULL;

					/* Check registered partials first */
					partial_content = (const gchar *)
						g_hash_table_lookup(
							self->partials,
							partial_name);

					if (partial_content != NULL) {
						rendered = render_template_string(
							self, partial_content,
							context, depth + 1,
							error);
					} else {
						/* Try loading from file */
						g_autofree gchar *loaded = NULL;

						loaded = load_template(
							self, partial_name,
							error);
						if (loaded != NULL) {
							rendered =
								render_template_string(
									self,
									loaded,
									context,
									depth + 1,
									error);
						}
					}

					if (rendered != NULL) {
						g_string_append(result,
						                rendered);
					} else if (*error != NULL) {
						g_string_free(result, TRUE);
						return NULL;
					}
				} else {
					/* Variable substitution */
					if (context != NULL) {
						const gchar *val;

						val = (const gchar *)
							g_hash_table_lookup(
								context,
								tag);
						if (val != NULL) {
							g_autofree gchar *esc =
								html_escape(
									val);
							g_string_append(
								result, esc);
						}
					}
				}
			}

			p = end + 2;
		}
	}

	return g_string_free(result, FALSE);
}

gchar *
htmx_template_engine_render(HtmxTemplateEngine  *self,
                            const gchar         *template_name,
                            GHashTable          *context,
                            GError             **error)
{
	gchar *content;

	g_return_val_if_fail(HTMX_IS_TEMPLATE_ENGINE(self), NULL);
	g_return_val_if_fail(template_name != NULL, NULL);

	content = load_template(self, template_name, error);
	if (content == NULL) {
		return NULL;
	}

	{
		gchar *result = render_template_string(self, content,
		                                       context, 0, error);
		g_free(content);
		return result;
	}
}

gchar *
htmx_template_engine_render_string(HtmxTemplateEngine  *self,
                                   const gchar         *template_str,
                                   GHashTable          *context,
                                   GError             **error)
{
	g_return_val_if_fail(HTMX_IS_TEMPLATE_ENGINE(self), NULL);
	g_return_val_if_fail(template_str != NULL, NULL);

	return render_template_string(self, template_str, context, 0, error);
}

void
htmx_template_engine_clear_cache(HtmxTemplateEngine *self)
{
	g_return_if_fail(HTMX_IS_TEMPLATE_ENGINE(self));

	g_hash_table_remove_all(self->cache);
}
