/*
 * htmx-attribute.c - HTMX attribute boxed type implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-attribute.h"
#include <string.h>

/*
 * GType registration for HtmxAttribute
 */
G_DEFINE_BOXED_TYPE(HtmxAttribute, htmx_attribute,
                    htmx_attribute_copy, htmx_attribute_free)

/*
 * Helper function to escape HTML attribute values.
 * Escapes quotes and ampersands for safe embedding in HTML attributes.
 */
static gchar *
escape_attribute_value(const gchar *value)
{
	GString *escaped;
	const gchar *p;

	if (value == NULL) {
		return g_strdup("");
	}

	escaped = g_string_sized_new(strlen(value));

	for (p = value; *p != '\0'; p++) {
		switch (*p) {
		case '"':
			g_string_append(escaped, "&quot;");
			break;
		case '&':
			g_string_append(escaped, "&amp;");
			break;
		case '<':
			g_string_append(escaped, "&lt;");
			break;
		case '>':
			g_string_append(escaped, "&gt;");
			break;
		default:
			g_string_append_c(escaped, *p);
			break;
		}
	}

	return g_string_free(escaped, FALSE);
}

/*
 * Create a new attribute with name and value
 */
HtmxAttribute *
htmx_attribute_new(
	const gchar *name,
	const gchar *value
){
	HtmxAttribute *self;

	g_return_val_if_fail(name != NULL, NULL);

	self = g_slice_new0(HtmxAttribute);
	self->name = g_strdup(name);
	self->value = g_strdup(value != NULL ? value : "");

	return self;
}

/*
 * Copy an attribute
 */
HtmxAttribute *
htmx_attribute_copy(const HtmxAttribute *self)
{
	if (self == NULL) {
		return NULL;
	}

	return htmx_attribute_new(self->name, self->value);
}

/*
 * Free an attribute
 */
void
htmx_attribute_free(HtmxAttribute *self)
{
	if (self == NULL) {
		return;
	}

	g_free(self->name);
	g_free(self->value);
	g_slice_free(HtmxAttribute, self);
}

/*
 * Get the attribute name
 */
const gchar *
htmx_attribute_get_name(const HtmxAttribute *self)
{
	g_return_val_if_fail(self != NULL, NULL);

	return self->name;
}

/*
 * Get the attribute value
 */
const gchar *
htmx_attribute_get_value(const HtmxAttribute *self)
{
	g_return_val_if_fail(self != NULL, NULL);

	return self->value;
}

/*
 * Set the attribute value
 */
void
htmx_attribute_set_value(
	HtmxAttribute *self,
	const gchar   *value
){
	g_return_if_fail(self != NULL);

	g_free(self->value);
	self->value = g_strdup(value != NULL ? value : "");
}

/*
 * Render the attribute as HTML
 */
gchar *
htmx_attribute_render(const HtmxAttribute *self)
{
	g_autofree gchar *escaped_value = NULL;

	g_return_val_if_fail(self != NULL, NULL);

	escaped_value = escape_attribute_value(self->value);

	return g_strdup_printf("%s=\"%s\"", self->name, escaped_value);
}

/*
 * Check if this is an HTMX attribute
 */
gboolean
htmx_attribute_is_htmx(const HtmxAttribute *self)
{
	g_return_val_if_fail(self != NULL, FALSE);

	return g_str_has_prefix(self->name, "hx-");
}
