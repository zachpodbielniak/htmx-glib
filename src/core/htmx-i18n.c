/*
 * htmx-i18n.c - Internationalization implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-i18n.h"
#include "htmx-request.h"
#include <libsoup/soup.h>
#include <string.h>

struct _HtmxI18n {
	GObject     parent_instance;

	GHashTable *catalogs;        /* locale (gchar*) -> messages (GHashTable*) */
	gchar      *default_locale;
	gchar      *active_locale;
};

G_DEFINE_FINAL_TYPE(HtmxI18n, htmx_i18n, G_TYPE_OBJECT)

static void
catalog_free(gpointer data)
{
	g_hash_table_unref((GHashTable *)data);
}

static GHashTable *
catalog_new(void)
{
	return g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
}

static void
htmx_i18n_finalize(GObject *object)
{
	HtmxI18n *self = HTMX_I18N(object);

	g_hash_table_unref(self->catalogs);
	g_free(self->default_locale);
	g_free(self->active_locale);

	G_OBJECT_CLASS(htmx_i18n_parent_class)->finalize(object);
}

static void
htmx_i18n_class_init(HtmxI18nClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);

	object_class->finalize = htmx_i18n_finalize;
}

static void
htmx_i18n_init(HtmxI18n *self)
{
	self->catalogs = g_hash_table_new_full(g_str_hash, g_str_equal,
	                                      g_free, catalog_free);
	self->default_locale = g_strdup("en");
	self->active_locale = g_strdup("en");
}

HtmxI18n *
htmx_i18n_new(const gchar *default_locale)
{
	HtmxI18n *self;

	g_return_val_if_fail(default_locale != NULL, NULL);

	self = g_object_new(HTMX_TYPE_I18N, NULL);

	g_free(self->default_locale);
	self->default_locale = g_strdup(default_locale);
	g_free(self->active_locale);
	self->active_locale = g_strdup(default_locale);

	return self;
}

static GHashTable *
ensure_catalog(HtmxI18n *self, const gchar *locale)
{
	GHashTable *messages;

	messages = (GHashTable *)g_hash_table_lookup(self->catalogs, locale);
	if (messages == NULL) {
		messages = catalog_new();
		g_hash_table_insert(self->catalogs, g_strdup(locale), messages);
	}

	return messages;
}

void
htmx_i18n_add_messages(HtmxI18n    *self,
                       const gchar *locale,
                       GHashTable  *messages)
{
	GHashTable *catalog;
	GHashTableIter iter;
	gpointer key;
	gpointer val;

	g_return_if_fail(HTMX_IS_I18N(self));
	g_return_if_fail(locale != NULL);
	g_return_if_fail(messages != NULL);

	catalog = ensure_catalog(self, locale);

	g_hash_table_iter_init(&iter, messages);
	while (g_hash_table_iter_next(&iter, &key, &val)) {
		g_hash_table_insert(catalog,
		                    g_strdup((const gchar *)key),
		                    g_strdup((const gchar *)val));
	}
}

void
htmx_i18n_add_message(HtmxI18n    *self,
                      const gchar *locale,
                      const gchar *key,
                      const gchar *value)
{
	GHashTable *catalog;

	g_return_if_fail(HTMX_IS_I18N(self));
	g_return_if_fail(locale != NULL);
	g_return_if_fail(key != NULL);
	g_return_if_fail(value != NULL);

	catalog = ensure_catalog(self, locale);
	g_hash_table_insert(catalog, g_strdup(key), g_strdup(value));
}

void
htmx_i18n_set_locale(HtmxI18n *self, const gchar *locale)
{
	g_return_if_fail(HTMX_IS_I18N(self));
	g_return_if_fail(locale != NULL);

	g_free(self->active_locale);
	self->active_locale = g_strdup(locale);
}

const gchar *
htmx_i18n_get_locale(HtmxI18n *self)
{
	g_return_val_if_fail(HTMX_IS_I18N(self), NULL);

	return self->active_locale;
}

static const gchar *
lookup_message(HtmxI18n *self, const gchar *locale, const gchar *key)
{
	GHashTable *catalog;

	catalog = (GHashTable *)g_hash_table_lookup(self->catalogs, locale);
	if (catalog == NULL) {
		return NULL;
	}

	return (const gchar *)g_hash_table_lookup(catalog, key);
}

const gchar *
htmx_i18n_gettext(HtmxI18n *self, const gchar *key)
{
	const gchar *result;

	g_return_val_if_fail(HTMX_IS_I18N(self), key);
	g_return_val_if_fail(key != NULL, key);

	/* Try active locale first */
	result = lookup_message(self, self->active_locale, key);
	if (result != NULL) {
		return result;
	}

	/* Fall back to default locale */
	if (g_strcmp0(self->active_locale, self->default_locale) != 0) {
		result = lookup_message(self, self->default_locale, key);
		if (result != NULL) {
			return result;
		}
	}

	/* Return the key itself as last resort */
	return key;
}

const gchar *
htmx_i18n_ngettext(HtmxI18n    *self,
                   const gchar *singular_key,
                   const gchar *plural_key,
                   gulong       count)
{
	g_return_val_if_fail(HTMX_IS_I18N(self), singular_key);

	if (count == 1) {
		return htmx_i18n_gettext(self, singular_key);
	}

	return htmx_i18n_gettext(self, plural_key);
}

/*
 * Parse the first preferred locale from Accept-Language header.
 * Returns a newly allocated string with the language tag (e.g. "en", "fr").
 * Only extracts the primary language subtag for simplicity.
 */
static gchar *
parse_accept_language(const gchar *header)
{
	const gchar *end;
	gchar *lang;
	const gchar *dash;

	if (header == NULL || *header == '\0') {
		return NULL;
	}

	/* Take the first language before any comma */
	end = strchr(header, ',');
	if (end == NULL) {
		end = header + strlen(header);
	}

	/* Strip quality value (;q=...) */
	{
		const gchar *semi = memchr(header, ';', (gsize)(end - header));
		if (semi != NULL) {
			end = semi;
		}
	}

	/* Strip whitespace */
	while (end > header && *(end - 1) == ' ') {
		end--;
	}

	lang = g_strndup(header, (gsize)(end - header));

	/* Extract primary subtag (before first dash) */
	dash = strchr(lang, '-');
	if (dash != NULL) {
		gchar *primary = g_strndup(lang, (gsize)(dash - lang));
		g_free(lang);
		lang = primary;
	}

	return lang;
}

void
htmx_i18n_middleware(
	HtmxContext        *context,
	HtmxMiddlewareNext  next,
	gpointer            next_data,
	gpointer            user_data
){
	HtmxI18n *self = HTMX_I18N(user_data);
	HtmxRequest *request;
	SoupServerMessage *msg;
	SoupMessageHeaders *headers;
	const gchar *accept_lang;
	g_autofree gchar *locale = NULL;

	request = htmx_context_get_request(context);
	msg = htmx_request_get_message(request);
	headers = soup_server_message_get_request_headers(msg);
	accept_lang = soup_message_headers_get_one(headers, "Accept-Language");

	locale = parse_accept_language(accept_lang);

	if (locale != NULL &&
	    g_hash_table_contains(self->catalogs, locale)) {
		htmx_i18n_set_locale(self, locale);
	} else {
		htmx_i18n_set_locale(self, self->default_locale);
	}

	/* Store locale and i18n instance on context */
	htmx_context_set_data(context, "htmx_locale",
	                      g_strdup(self->active_locale), g_free);
	htmx_context_set_data(context, "htmx_i18n",
	                      g_object_ref(self), g_object_unref);

	next(context, next_data);
}
