/*
 * htmx-cookie.h - HTTP cookie management
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * HtmxCookie is a boxed type representing an HTTP cookie with
 * standard attributes (Secure, HttpOnly, SameSite, Max-Age, etc.).
 */

#ifndef HTMX_COOKIE_H
#define HTMX_COOKIE_H

#include <glib-object.h>
#include "htmx-enums.h"

G_BEGIN_DECLS

#define HTMX_TYPE_COOKIE (htmx_cookie_get_type())

typedef struct _HtmxCookie HtmxCookie;

/**
 * HtmxCookie:
 *
 * A boxed type representing an HTTP cookie.
 */
struct _HtmxCookie {
	gchar              *name;
	gchar              *value;
	gchar              *domain;
	gchar              *path;
	gint64              max_age;    /* -1 = session cookie, 0 = delete, >0 = seconds */
	gboolean            secure;
	gboolean            http_only;
	HtmxCookieSameSite  same_site;
};

GType htmx_cookie_get_type(void) G_GNUC_CONST;

/**
 * htmx_cookie_new:
 * @name: the cookie name
 * @value: the cookie value
 *
 * Creates a new #HtmxCookie with default attributes
 * (Path="/", HttpOnly=TRUE, SameSite=Lax, session cookie).
 *
 * Returns: (transfer full): a new #HtmxCookie
 */
HtmxCookie *
htmx_cookie_new(const gchar *name, const gchar *value);

/**
 * htmx_cookie_copy:
 * @self: an #HtmxCookie
 *
 * Creates a deep copy.
 *
 * Returns: (transfer full): a new #HtmxCookie
 */
HtmxCookie *
htmx_cookie_copy(const HtmxCookie *self);

/**
 * htmx_cookie_free:
 * @self: an #HtmxCookie
 *
 * Frees the cookie and its contents.
 */
void
htmx_cookie_free(HtmxCookie *self);

/**
 * htmx_cookie_set_path:
 * @self: an #HtmxCookie
 * @path: the cookie path
 *
 * Sets the Path attribute.
 */
void
htmx_cookie_set_path(HtmxCookie *self, const gchar *path);

/**
 * htmx_cookie_set_domain:
 * @self: an #HtmxCookie
 * @domain: the cookie domain
 *
 * Sets the Domain attribute.
 */
void
htmx_cookie_set_domain(HtmxCookie *self, const gchar *domain);

/**
 * htmx_cookie_set_max_age:
 * @self: an #HtmxCookie
 * @max_age: max age in seconds (-1 for session, 0 to delete)
 *
 * Sets the Max-Age attribute.
 */
void
htmx_cookie_set_max_age(HtmxCookie *self, gint64 max_age);

/**
 * htmx_cookie_set_secure:
 * @self: an #HtmxCookie
 * @secure: whether to set the Secure flag
 *
 * Sets the Secure attribute.
 */
void
htmx_cookie_set_secure(HtmxCookie *self, gboolean secure);

/**
 * htmx_cookie_set_http_only:
 * @self: an #HtmxCookie
 * @http_only: whether to set the HttpOnly flag
 *
 * Sets the HttpOnly attribute.
 */
void
htmx_cookie_set_http_only(HtmxCookie *self, gboolean http_only);

/**
 * htmx_cookie_set_same_site:
 * @self: an #HtmxCookie
 * @same_site: the SameSite attribute
 *
 * Sets the SameSite attribute.
 */
void
htmx_cookie_set_same_site(HtmxCookie *self, HtmxCookieSameSite same_site);

/**
 * htmx_cookie_to_set_cookie:
 * @self: an #HtmxCookie
 *
 * Formats the cookie as a Set-Cookie header value.
 *
 * Returns: (transfer full): the Set-Cookie header string
 */
gchar *
htmx_cookie_to_set_cookie(const HtmxCookie *self);

/**
 * htmx_cookie_parse_request:
 * @cookie_header: the Cookie header value
 *
 * Parses a Cookie request header into a hash table of name=value pairs.
 *
 * Returns: (transfer full) (element-type utf8 utf8): parsed cookies
 */
GHashTable *
htmx_cookie_parse_request(const gchar *cookie_header);

G_DEFINE_AUTOPTR_CLEANUP_FUNC(HtmxCookie, htmx_cookie_free)

G_END_DECLS

#endif /* HTMX_COOKIE_H */
