/*
 * htmx-template.c - Template rendering implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-template.h"
#include "core/htmx-error.h"

struct _HtmxTemplate {
	GObject parent_instance;

	gchar *source;
};

G_DEFINE_FINAL_TYPE(HtmxTemplate, htmx_template, G_TYPE_OBJECT)

static void
htmx_template_finalize(GObject *object)
{
	HtmxTemplate *self = HTMX_TEMPLATE(object);

	g_free(self->source);

	G_OBJECT_CLASS(htmx_template_parent_class)->finalize(object);
}

static void
htmx_template_class_init(HtmxTemplateClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);

	object_class->finalize = htmx_template_finalize;
}

static void
htmx_template_init(HtmxTemplate *self)
{
	self->source = NULL;
}

HtmxTemplate *
htmx_template_new(const gchar *source)
{
	HtmxTemplate *self;

	self = g_object_new(HTMX_TYPE_TEMPLATE, NULL);
	self->source = g_strdup(source);

	return self;
}

HtmxTemplate *
htmx_template_new_from_file(const gchar *path, GError **error)
{
	g_autofree gchar *content = NULL;

	if (!g_file_get_contents(path, &content, NULL, error)) {
		return NULL;
	}

	return htmx_template_new(content);
}

/*
 * Simple template rendering with {{variable}} substitution
 */
gchar *
htmx_template_render(HtmxTemplate *self, GHashTable *context, GError **error)
{
	GString *result;
	const gchar *p;
	const gchar *start;

	g_return_val_if_fail(HTMX_IS_TEMPLATE(self), NULL);

	if (self->source == NULL) {
		return g_strdup("");
	}

	result = g_string_new("");
	p = self->source;

	while (*p != '\0') {
		if (p[0] == '{' && p[1] == '{') {
			/* Find variable name */
			start = p + 2;
			const gchar *end = strstr(start, "}}");

			if (end != NULL) {
				g_autofree gchar *var_name = g_strndup(start, end - start);
				g_strstrip(var_name);

				if (context != NULL) {
					const gchar *value = g_hash_table_lookup(context, var_name);
					if (value != NULL) {
						g_string_append(result, value);
					}
				}

				p = end + 2;
				continue;
			}
		}

		g_string_append_c(result, *p);
		p++;
	}

	return g_string_free(result, FALSE);
}
