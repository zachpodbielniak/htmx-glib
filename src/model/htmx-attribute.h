/*
 * htmx-attribute.h - HTMX attribute boxed type
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * This file defines the HtmxAttribute boxed type which represents a single
 * HTMX attribute with a name and value.
 */

#ifndef HTMX_ATTRIBUTE_H
#define HTMX_ATTRIBUTE_H

#include <glib-object.h>

G_BEGIN_DECLS

#define HTMX_TYPE_ATTRIBUTE (htmx_attribute_get_type())

typedef struct _HtmxAttribute HtmxAttribute;

/**
 * HtmxAttribute:
 *
 * A boxed type representing a single HTMX attribute.
 *
 * An attribute consists of a name (e.g., "hx-get") and a value (e.g., "/api/data").
 * Attributes are used to build HTML elements with HTMX functionality.
 */
struct _HtmxAttribute {
	gchar *name;    /* Attribute name (e.g., "hx-get", "hx-target") */
	gchar *value;   /* Attribute value */
};

GType htmx_attribute_get_type(void) G_GNUC_CONST;

/**
 * htmx_attribute_new:
 * @name: the attribute name
 * @value: the attribute value
 *
 * Creates a new #HtmxAttribute with the given name and value.
 *
 * Returns: (transfer full): a new #HtmxAttribute. Free with htmx_attribute_free().
 */
HtmxAttribute *
htmx_attribute_new(
	const gchar *name,
	const gchar *value
);

/**
 * htmx_attribute_copy:
 * @self: (nullable): an #HtmxAttribute
 *
 * Creates a copy of an #HtmxAttribute.
 *
 * Returns: (transfer full) (nullable): a copy of @self, or %NULL if @self is %NULL.
 *          Free with htmx_attribute_free().
 */
HtmxAttribute *
htmx_attribute_copy(const HtmxAttribute *self);

/**
 * htmx_attribute_free:
 * @self: (nullable): an #HtmxAttribute
 *
 * Frees an #HtmxAttribute and its contents.
 */
void
htmx_attribute_free(HtmxAttribute *self);

/**
 * htmx_attribute_get_name:
 * @self: an #HtmxAttribute
 *
 * Gets the attribute name.
 *
 * Returns: (transfer none): the attribute name
 */
const gchar *
htmx_attribute_get_name(const HtmxAttribute *self);

/**
 * htmx_attribute_get_value:
 * @self: an #HtmxAttribute
 *
 * Gets the attribute value.
 *
 * Returns: (transfer none): the attribute value
 */
const gchar *
htmx_attribute_get_value(const HtmxAttribute *self);

/**
 * htmx_attribute_set_value:
 * @self: an #HtmxAttribute
 * @value: the new value
 *
 * Sets the attribute value.
 */
void
htmx_attribute_set_value(
	HtmxAttribute *self,
	const gchar   *value
);

/**
 * htmx_attribute_render:
 * @self: an #HtmxAttribute
 *
 * Renders the attribute as an HTML attribute string (e.g., 'hx-get="/api"').
 *
 * Returns: (transfer full): the rendered attribute string. Free with g_free().
 */
gchar *
htmx_attribute_render(const HtmxAttribute *self);

/**
 * htmx_attribute_is_htmx:
 * @self: an #HtmxAttribute
 *
 * Checks if this is an HTMX attribute (starts with "hx-").
 *
 * Returns: %TRUE if this is an HTMX attribute
 */
gboolean
htmx_attribute_is_htmx(const HtmxAttribute *self);

G_DEFINE_AUTOPTR_CLEANUP_FUNC(HtmxAttribute, htmx_attribute_free)

G_END_DECLS

#endif /* HTMX_ATTRIBUTE_H */
