/*
 * htmx-template.h - Template rendering
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef HTMX_TEMPLATE_H
#define HTMX_TEMPLATE_H

#include <glib-object.h>

G_BEGIN_DECLS

#define HTMX_TYPE_TEMPLATE (htmx_template_get_type())
G_DECLARE_FINAL_TYPE(HtmxTemplate, htmx_template, HTMX, TEMPLATE, GObject)

HtmxTemplate *htmx_template_new(const gchar *source);
HtmxTemplate *htmx_template_new_from_file(const gchar *path, GError **error);

gchar *htmx_template_render(HtmxTemplate *self, GHashTable *context, GError **error);

G_END_DECLS

#endif /* HTMX_TEMPLATE_H */
