/*
 * htmx-input.h - HTML input element
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef HTMX_INPUT_H
#define HTMX_INPUT_H

#include <glib-object.h>
#include "htmx-node.h"
#include "core/htmx-enums.h"

G_BEGIN_DECLS

#define HTMX_TYPE_INPUT (htmx_input_get_type())
G_DECLARE_FINAL_TYPE(HtmxInput, htmx_input, HTMX, INPUT, HtmxNode)

HtmxInput *htmx_input_new(HtmxInputType type);
HtmxInput *htmx_input_new_text(const gchar *name);
HtmxInput *htmx_input_new_hidden(const gchar *name, const gchar *value);

void htmx_input_set_name(HtmxInput *self, const gchar *name);
void htmx_input_set_value(HtmxInput *self, const gchar *value);
void htmx_input_set_placeholder(HtmxInput *self, const gchar *placeholder);
void htmx_input_set_required(HtmxInput *self, gboolean required);

G_END_DECLS

#endif /* HTMX_INPUT_H */
