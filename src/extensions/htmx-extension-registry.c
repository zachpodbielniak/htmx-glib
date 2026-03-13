/*
 * htmx-extension-registry.c - Extension registry implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-extension-registry.h"
#include <string.h>

struct _HtmxExtensionRegistry {
	GObject     parent_instance;

	GHashTable *by_name;     /* name (gchar*) -> HtmxExtension* (owned) */
	GPtrArray  *extensions;  /* HtmxExtension* (borrowed from by_name) */
};

G_DEFINE_FINAL_TYPE(HtmxExtensionRegistry, htmx_extension_registry,
                    G_TYPE_OBJECT)

static void
htmx_extension_registry_finalize(GObject *object)
{
	HtmxExtensionRegistry *self = HTMX_EXTENSION_REGISTRY(object);

	g_ptr_array_unref(self->extensions);
	g_hash_table_unref(self->by_name);

	G_OBJECT_CLASS(htmx_extension_registry_parent_class)->finalize(object);
}

static void
htmx_extension_registry_class_init(HtmxExtensionRegistryClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);

	object_class->finalize = htmx_extension_registry_finalize;
}

static void
htmx_extension_registry_init(HtmxExtensionRegistry *self)
{
	self->by_name = g_hash_table_new_full(g_str_hash, g_str_equal,
	                                     g_free, g_object_unref);
	self->extensions = g_ptr_array_new();
}

/*
 * Rebuild the extensions array from the hash table values.
 */
static void
rebuild_array(HtmxExtensionRegistry *self)
{
	GHashTableIter iter;
	gpointer val;

	g_ptr_array_set_size(self->extensions, 0);

	g_hash_table_iter_init(&iter, self->by_name);
	while (g_hash_table_iter_next(&iter, NULL, &val)) {
		g_ptr_array_add(self->extensions, val);
	}
}

HtmxExtensionRegistry *
htmx_extension_registry_new(void)
{
	return g_object_new(HTMX_TYPE_EXTENSION_REGISTRY, NULL);
}

void
htmx_extension_registry_register(HtmxExtensionRegistry *self,
                                 HtmxExtension         *extension)
{
	const gchar *name;

	g_return_if_fail(HTMX_IS_EXTENSION_REGISTRY(self));
	g_return_if_fail(HTMX_IS_EXTENSION(extension));

	name = htmx_extension_get_name(extension);
	g_return_if_fail(name != NULL);

	g_hash_table_insert(self->by_name, g_strdup(name),
	                    g_object_ref(extension));
	rebuild_array(self);

	/* Call init hook */
	htmx_extension_init_ext(extension);
}

gboolean
htmx_extension_registry_unregister(HtmxExtensionRegistry *self,
                                   const gchar           *name)
{
	gboolean removed;

	g_return_val_if_fail(HTMX_IS_EXTENSION_REGISTRY(self), FALSE);
	g_return_val_if_fail(name != NULL, FALSE);

	removed = g_hash_table_remove(self->by_name, name);
	if (removed) {
		rebuild_array(self);
	}

	return removed;
}

HtmxExtension *
htmx_extension_registry_get(HtmxExtensionRegistry *self,
                            const gchar           *name)
{
	g_return_val_if_fail(HTMX_IS_EXTENSION_REGISTRY(self), NULL);
	g_return_val_if_fail(name != NULL, NULL);

	return (HtmxExtension *)g_hash_table_lookup(self->by_name, name);
}

GPtrArray *
htmx_extension_registry_get_all(HtmxExtensionRegistry *self)
{
	g_return_val_if_fail(HTMX_IS_EXTENSION_REGISTRY(self), NULL);

	return self->extensions;
}

guint
htmx_extension_registry_get_count(HtmxExtensionRegistry *self)
{
	g_return_val_if_fail(HTMX_IS_EXTENSION_REGISTRY(self), 0);

	return self->extensions->len;
}

gboolean
htmx_extension_registry_fire_event(HtmxExtensionRegistry *self,
                                   const gchar           *event_name,
                                   GHashTable            *detail)
{
	guint i;

	g_return_val_if_fail(HTMX_IS_EXTENSION_REGISTRY(self), TRUE);
	g_return_val_if_fail(event_name != NULL, TRUE);

	for (i = 0; i < self->extensions->len; i++) {
		HtmxExtension *ext;

		ext = (HtmxExtension *)g_ptr_array_index(self->extensions, i);
		if (!htmx_extension_on_event(ext, event_name, detail)) {
			return FALSE;
		}
	}

	return TRUE;
}

gchar *
htmx_extension_registry_transform_response(HtmxExtensionRegistry *self,
                                           const gchar           *content,
                                           const gchar           *content_type)
{
	gchar *current;
	guint i;

	g_return_val_if_fail(HTMX_IS_EXTENSION_REGISTRY(self), NULL);

	if (content == NULL) {
		return NULL;
	}

	current = g_strdup(content);

	for (i = 0; i < self->extensions->len; i++) {
		HtmxExtension *ext;
		gchar *transformed;

		ext = (HtmxExtension *)g_ptr_array_index(self->extensions, i);
		transformed = htmx_extension_transform_response(
			ext, current, content_type);

		if (transformed != NULL) {
			g_free(current);
			current = transformed;
		}
	}

	return current;
}

void
htmx_extension_registry_filter_headers(HtmxExtensionRegistry *self,
                                       GHashTable            *headers,
                                       gboolean               is_request)
{
	guint i;

	g_return_if_fail(HTMX_IS_EXTENSION_REGISTRY(self));
	g_return_if_fail(headers != NULL);

	for (i = 0; i < self->extensions->len; i++) {
		HtmxExtension *ext;

		ext = (HtmxExtension *)g_ptr_array_index(self->extensions, i);
		htmx_extension_header_filter(ext, headers, is_request);
	}
}
