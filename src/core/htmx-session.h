/*
 * htmx-session.h - Server-side session management
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * HtmxSession provides in-memory server-side session management.
 * It stores per-user state across requests using cookie-based
 * session IDs.
 *
 * Use htmx_session_middleware() with htmx_server_use() to
 * automatically load/save sessions for each request. Session
 * data is available via htmx_context_get_data(ctx, "htmx_session").
 */

#ifndef HTMX_SESSION_H
#define HTMX_SESSION_H

#include <glib-object.h>
#include "htmx-context.h"
#include "../htmx-types.h"

G_BEGIN_DECLS

#define HTMX_TYPE_SESSION (htmx_session_get_type())
G_DECLARE_FINAL_TYPE(HtmxSession, htmx_session, HTMX, SESSION, GObject)

/**
 * htmx_session_new:
 *
 * Creates a new in-memory session store with default settings.
 * Default cookie name is "htmx_sid", HttpOnly, SameSite=Lax.
 *
 * Returns: (transfer full): a new #HtmxSession
 */
HtmxSession *
htmx_session_new(void);

/**
 * htmx_session_set_cookie_name:
 * @self: an #HtmxSession
 * @name: the cookie name for session IDs
 *
 * Sets the cookie name used for session tracking.
 */
void
htmx_session_set_cookie_name(HtmxSession *self, const gchar *name);

/**
 * htmx_session_get_cookie_name:
 * @self: an #HtmxSession
 *
 * Gets the cookie name used for session tracking.
 *
 * Returns: (transfer none): the cookie name
 */
const gchar *
htmx_session_get_cookie_name(HtmxSession *self);

/**
 * htmx_session_set_max_age:
 * @self: an #HtmxSession
 * @max_age: cookie max age in seconds (-1 for session cookie)
 *
 * Sets the Max-Age attribute for session cookies.
 */
void
htmx_session_set_max_age(HtmxSession *self, gint64 max_age);

/**
 * htmx_session_set_secure:
 * @self: an #HtmxSession
 * @secure: whether to set the Secure flag on session cookies
 *
 * Sets the Secure attribute for session cookies.
 */
void
htmx_session_set_secure(HtmxSession *self, gboolean secure);

/**
 * htmx_session_create:
 * @self: an #HtmxSession
 *
 * Creates a new session and returns its ID.
 *
 * Returns: (transfer full): the new session ID
 */
gchar *
htmx_session_create(HtmxSession *self);

/**
 * htmx_session_destroy:
 * @self: an #HtmxSession
 * @session_id: the session ID to destroy
 *
 * Destroys a session, removing all its data.
 */
void
htmx_session_destroy(HtmxSession *self, const gchar *session_id);

/**
 * htmx_session_exists:
 * @self: an #HtmxSession
 * @session_id: the session ID to check
 *
 * Checks if a session exists.
 *
 * Returns: %TRUE if the session exists
 */
gboolean
htmx_session_exists(HtmxSession *self, const gchar *session_id);

/**
 * htmx_session_set_value:
 * @self: an #HtmxSession
 * @session_id: the session ID
 * @key: the data key
 * @value: (nullable): the data value
 *
 * Sets a value in a session. Creates the session if it doesn't exist.
 */
void
htmx_session_set_value(
	HtmxSession *self,
	const gchar *session_id,
	const gchar *key,
	const gchar *value
);

/**
 * htmx_session_get_value:
 * @self: an #HtmxSession
 * @session_id: the session ID
 * @key: the data key
 *
 * Gets a value from a session.
 *
 * Returns: (transfer none) (nullable): the value, or %NULL
 */
const gchar *
htmx_session_get_value(
	HtmxSession *self,
	const gchar *session_id,
	const gchar *key
);

/**
 * htmx_session_remove_value:
 * @self: an #HtmxSession
 * @session_id: the session ID
 * @key: the data key to remove
 *
 * Removes a value from a session.
 */
void
htmx_session_remove_value(
	HtmxSession *self,
	const gchar *session_id,
	const gchar *key
);

/**
 * htmx_session_middleware:
 * @context: the request context
 * @next: continuation function
 * @next_data: continuation data
 * @user_data: (type HtmxSession): the session store
 *
 * Middleware function for session management.
 * Loads session data from cookies and stores it on the context.
 * After the handler runs, sets the session cookie on the response.
 *
 * Session data is accessible via:
 * - htmx_context_get_data(ctx, "htmx_session_id") — session ID string
 * - htmx_context_get_data(ctx, "htmx_session") — the HtmxSession store
 *
 * Register with: htmx_server_use(server, htmx_session_middleware,
 *                                g_object_ref(session), g_object_unref)
 */
void
htmx_session_middleware(
	HtmxContext        *context,
	HtmxMiddlewareNext  next,
	gpointer            next_data,
	gpointer            user_data
);

G_END_DECLS

#endif /* HTMX_SESSION_H */
