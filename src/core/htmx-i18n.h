/*
 * htmx-i18n.h - Internationalization support
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * HtmxI18n provides message-catalog-based internationalization.
 * Each locale has a GHashTable mapping message keys to translated
 * strings. The active locale can be set globally or per-request
 * via middleware.
 *
 * Pluralization is supported via htmx_i18n_ngettext() which selects
 * between singular and plural forms based on count.
 */

#ifndef HTMX_I18N_H
#define HTMX_I18N_H

#include <glib-object.h>
#include "htmx-context.h"
#include "../htmx-types.h"

G_BEGIN_DECLS

#define HTMX_TYPE_I18N (htmx_i18n_get_type())
G_DECLARE_FINAL_TYPE(HtmxI18n, htmx_i18n, HTMX, I18N, GObject)

/**
 * htmx_i18n_new:
 * @default_locale: the default locale (e.g. "en")
 *
 * Creates a new i18n instance with the given default locale.
 *
 * Returns: (transfer full): a new #HtmxI18n
 */
HtmxI18n *
htmx_i18n_new(const gchar *default_locale);

/**
 * htmx_i18n_add_messages:
 * @self: an #HtmxI18n
 * @locale: the locale (e.g. "en", "fr", "de")
 * @messages: (element-type utf8 utf8): key -> translated string
 *
 * Adds a message catalog for a locale. If messages already exist
 * for the locale, new entries are merged (overwriting duplicates).
 */
void
htmx_i18n_add_messages(HtmxI18n   *self,
                       const gchar *locale,
                       GHashTable  *messages);

/**
 * htmx_i18n_add_message:
 * @self: an #HtmxI18n
 * @locale: the locale
 * @key: the message key
 * @value: the translated string
 *
 * Adds a single message to a locale's catalog.
 */
void
htmx_i18n_add_message(HtmxI18n    *self,
                      const gchar *locale,
                      const gchar *key,
                      const gchar *value);

/**
 * htmx_i18n_set_locale:
 * @self: an #HtmxI18n
 * @locale: the locale to set as active
 *
 * Sets the active locale for translations.
 */
void
htmx_i18n_set_locale(HtmxI18n *self, const gchar *locale);

/**
 * htmx_i18n_get_locale:
 * @self: an #HtmxI18n
 *
 * Gets the active locale.
 *
 * Returns: (transfer none): the active locale
 */
const gchar *
htmx_i18n_get_locale(HtmxI18n *self);

/**
 * htmx_i18n_gettext:
 * @self: an #HtmxI18n
 * @key: the message key
 *
 * Looks up a translated string for the current locale.
 * Falls back to the default locale, then returns the key itself.
 *
 * Returns: (transfer none): the translated string or the key
 */
const gchar *
htmx_i18n_gettext(HtmxI18n *self, const gchar *key);

/**
 * htmx_i18n_ngettext:
 * @self: an #HtmxI18n
 * @singular_key: key for singular form
 * @plural_key: key for plural form
 * @count: the count for pluralization
 *
 * Returns the singular or plural translation based on @count.
 * Uses the simple rule: count == 1 → singular, otherwise plural.
 *
 * Returns: (transfer none): the translated string
 */
const gchar *
htmx_i18n_ngettext(HtmxI18n    *self,
                   const gchar *singular_key,
                   const gchar *plural_key,
                   gulong       count);

/**
 * htmx_i18n_middleware:
 * @context: the request context
 * @next: continuation function
 * @next_data: continuation data
 * @user_data: (type HtmxI18n): the i18n instance
 *
 * Middleware that detects the client's preferred locale from the
 * Accept-Language header and sets it on the i18n instance and context.
 *
 * The locale is stored on the context as "htmx_locale".
 *
 * Register with: htmx_server_use(server, htmx_i18n_middleware,
 *                                g_object_ref(i18n), g_object_unref)
 */
void
htmx_i18n_middleware(
	HtmxContext        *context,
	HtmxMiddlewareNext  next,
	gpointer            next_data,
	gpointer            user_data
);

G_END_DECLS

#endif /* HTMX_I18N_H */
