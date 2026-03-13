/*
 * htmx-cookie.c - HTTP cookie management implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-cookie.h"
#include <string.h>

G_DEFINE_BOXED_TYPE(HtmxCookie, htmx_cookie,
                    htmx_cookie_copy, htmx_cookie_free)

HtmxCookie *
htmx_cookie_new(const gchar *name, const gchar *value)
{
	HtmxCookie *self;

	g_return_val_if_fail(name != NULL, NULL);

	self = g_slice_new0(HtmxCookie);
	self->name = g_strdup(name);
	self->value = g_strdup(value != NULL ? value : "");
	self->domain = NULL;
	self->path = g_strdup("/");
	self->max_age = -1;
	self->secure = FALSE;
	self->http_only = TRUE;
	self->same_site = HTMX_COOKIE_SAME_SITE_LAX;

	return self;
}

HtmxCookie *
htmx_cookie_copy(const HtmxCookie *self)
{
	HtmxCookie *copy;

	if (self == NULL) {
		return NULL;
	}

	copy = g_slice_new0(HtmxCookie);
	copy->name = g_strdup(self->name);
	copy->value = g_strdup(self->value);
	copy->domain = g_strdup(self->domain);
	copy->path = g_strdup(self->path);
	copy->max_age = self->max_age;
	copy->secure = self->secure;
	copy->http_only = self->http_only;
	copy->same_site = self->same_site;

	return copy;
}

void
htmx_cookie_free(HtmxCookie *self)
{
	if (self == NULL) {
		return;
	}

	g_free(self->name);
	g_free(self->value);
	g_free(self->domain);
	g_free(self->path);
	g_slice_free(HtmxCookie, self);
}

void
htmx_cookie_set_path(HtmxCookie *self, const gchar *path)
{
	g_return_if_fail(self != NULL);

	g_free(self->path);
	self->path = g_strdup(path);
}

void
htmx_cookie_set_domain(HtmxCookie *self, const gchar *domain)
{
	g_return_if_fail(self != NULL);

	g_free(self->domain);
	self->domain = g_strdup(domain);
}

void
htmx_cookie_set_max_age(HtmxCookie *self, gint64 max_age)
{
	g_return_if_fail(self != NULL);

	self->max_age = max_age;
}

void
htmx_cookie_set_secure(HtmxCookie *self, gboolean secure)
{
	g_return_if_fail(self != NULL);

	self->secure = secure;
}

void
htmx_cookie_set_http_only(HtmxCookie *self, gboolean http_only)
{
	g_return_if_fail(self != NULL);

	self->http_only = http_only;
}

void
htmx_cookie_set_same_site(HtmxCookie *self, HtmxCookieSameSite same_site)
{
	g_return_if_fail(self != NULL);

	self->same_site = same_site;
}

gchar *
htmx_cookie_to_set_cookie(const HtmxCookie *self)
{
	GString *header;

	g_return_val_if_fail(self != NULL, NULL);

	header = g_string_new(NULL);
	g_string_append_printf(header, "%s=%s", self->name, self->value);

	if (self->path != NULL) {
		g_string_append_printf(header, "; Path=%s", self->path);
	}

	if (self->domain != NULL) {
		g_string_append_printf(header, "; Domain=%s", self->domain);
	}

	if (self->max_age >= 0) {
		g_string_append_printf(header, "; Max-Age=%" G_GINT64_FORMAT,
		                       self->max_age);
	}

	if (self->secure) {
		g_string_append(header, "; Secure");
	}

	if (self->http_only) {
		g_string_append(header, "; HttpOnly");
	}

	g_string_append_printf(header, "; SameSite=%s",
	                       htmx_cookie_same_site_to_string(self->same_site));

	return g_string_free(header, FALSE);
}

GHashTable *
htmx_cookie_parse_request(const gchar *cookie_header)
{
	GHashTable *cookies;
	gchar **pairs;
	gint i;

	cookies = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);

	if (cookie_header == NULL || *cookie_header == '\0') {
		return cookies;
	}

	pairs = g_strsplit(cookie_header, ";", -1);
	for (i = 0; pairs[i] != NULL; i++) {
		gchar *pair;
		gchar *eq;

		pair = g_strstrip(pairs[i]);
		if (*pair == '\0') {
			continue;
		}

		eq = strchr(pair, '=');
		if (eq != NULL) {
			gchar *name;
			gchar *value;

			name = g_strndup(pair, (gsize)(eq - pair));
			value = g_strdup(eq + 1);
			g_hash_table_insert(cookies, g_strstrip(name), g_strstrip(value));
		}
	}

	g_strfreev(pairs);
	return cookies;
}
