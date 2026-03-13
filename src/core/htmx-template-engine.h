/*
 * htmx-template-engine.h - Template engine with caching and layouts
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * HtmxTemplateEngine manages template loading, caching, and rendering.
 * It supports:
 * - Loading templates from a search path
 * - Caching compiled templates for reuse
 * - Layout/base templates via {{> partial}} includes
 *
 * Templates use simple mustache-style substitution:
 * - {{key}} — variable substitution (HTML-escaped)
 * - {{{key}}} — raw/unescaped substitution
 * - {{> name}} — partial/include another template
 */

#ifndef HTMX_TEMPLATE_ENGINE_H
#define HTMX_TEMPLATE_ENGINE_H

#include <glib-object.h>

G_BEGIN_DECLS

#define HTMX_TYPE_TEMPLATE_ENGINE (htmx_template_engine_get_type())
G_DECLARE_FINAL_TYPE(HtmxTemplateEngine, htmx_template_engine, HTMX, TEMPLATE_ENGINE, GObject)

/**
 * htmx_template_engine_new:
 *
 * Creates a new template engine.
 *
 * Returns: (transfer full): a new #HtmxTemplateEngine
 */
HtmxTemplateEngine *
htmx_template_engine_new(void);

/**
 * htmx_template_engine_add_path:
 * @self: an #HtmxTemplateEngine
 * @path: directory path to search for templates
 *
 * Adds a directory to the template search path.
 * Paths are searched in the order they are added.
 */
void
htmx_template_engine_add_path(HtmxTemplateEngine *self,
                              const gchar        *path);

/**
 * htmx_template_engine_set_cache_enabled:
 * @self: an #HtmxTemplateEngine
 * @enabled: whether to cache loaded templates
 *
 * Enables or disables template caching. When enabled, templates
 * are loaded from disk only once. Disable during development
 * for live-reload behavior.
 */
void
htmx_template_engine_set_cache_enabled(HtmxTemplateEngine *self,
                                       gboolean            enabled);

/**
 * htmx_template_engine_add_partial:
 * @self: an #HtmxTemplateEngine
 * @name: the partial name (used in {{> name}})
 * @content: the partial template content
 *
 * Registers a named partial template. Partials registered this
 * way take precedence over file-based partials.
 */
void
htmx_template_engine_add_partial(HtmxTemplateEngine *self,
                                 const gchar        *name,
                                 const gchar        *content);

/**
 * htmx_template_engine_render:
 * @self: an #HtmxTemplateEngine
 * @template_name: the template file name (e.g. "index.html")
 * @context: (element-type utf8 utf8): template variables
 * @error: (out) (optional): return location for error
 *
 * Loads and renders a template with the given context variables.
 * Template files are searched in the configured paths.
 *
 * Returns: (transfer full) (nullable): the rendered content, or
 *   %NULL on error
 */
gchar *
htmx_template_engine_render(HtmxTemplateEngine  *self,
                            const gchar         *template_name,
                            GHashTable          *context,
                            GError             **error);

/**
 * htmx_template_engine_render_string:
 * @self: an #HtmxTemplateEngine
 * @template_str: the template string
 * @context: (element-type utf8 utf8): template variables
 * @error: (out) (optional): return location for error
 *
 * Renders a template string directly (no file loading).
 *
 * Returns: (transfer full) (nullable): the rendered content, or
 *   %NULL on error
 */
gchar *
htmx_template_engine_render_string(HtmxTemplateEngine  *self,
                                   const gchar         *template_str,
                                   GHashTable          *context,
                                   GError             **error);

/**
 * htmx_template_engine_clear_cache:
 * @self: an #HtmxTemplateEngine
 *
 * Clears all cached templates.
 */
void
htmx_template_engine_clear_cache(HtmxTemplateEngine *self);

G_END_DECLS

#endif /* HTMX_TEMPLATE_ENGINE_H */
