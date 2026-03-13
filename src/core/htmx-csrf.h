/*
 * htmx-csrf.h - CSRF protection
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * HtmxCsrf provides Cross-Site Request Forgery protection via
 * token-based validation. Tokens are stored in the session and
 * validated against a request header or form field on unsafe
 * HTTP methods (POST, PUT, PATCH, DELETE).
 *
 * Requires session middleware to be registered BEFORE CSRF middleware.
 *
 * Usage:
 * |[<!-- language="C" -->
 * HtmxSession *session = htmx_session_new();
 * HtmxCsrf *csrf = htmx_csrf_new(session);
 *
 * htmx_server_use(server, htmx_session_middleware,
 *                 g_object_ref(session), g_object_unref);
 * htmx_server_use(server, htmx_csrf_middleware,
 *                 g_object_ref(csrf), g_object_unref);
 * ]|
 *
 * The CSRF token is available in handlers via:
 *   htmx_context_get_data(ctx, "htmx_csrf_token")
 */

#ifndef HTMX_CSRF_H
#define HTMX_CSRF_H

#include <glib-object.h>
#include "htmx-context.h"
#include "htmx-session.h"
#include "../htmx-types.h"

G_BEGIN_DECLS

#define HTMX_TYPE_CSRF (htmx_csrf_get_type())
G_DECLARE_FINAL_TYPE(HtmxCsrf, htmx_csrf, HTMX, CSRF, GObject)

/**
 * htmx_csrf_new:
 * @session: the session store for token storage
 *
 * Creates a new #HtmxCsrf protector.
 * Default header name: "X-CSRF-Token"
 * Default form field: "_csrf"
 * Default session key: "_csrf_token"
 *
 * Returns: (transfer full): a new #HtmxCsrf
 */
HtmxCsrf *
htmx_csrf_new(HtmxSession *session);

/**
 * htmx_csrf_set_header_name:
 * @self: an #HtmxCsrf
 * @name: the header name to check for tokens
 *
 * Sets the HTTP header name that carries the CSRF token.
 */
void
htmx_csrf_set_header_name(HtmxCsrf *self, const gchar *name);

/**
 * htmx_csrf_set_form_field:
 * @self: an #HtmxCsrf
 * @field: the form field name that carries the CSRF token
 *
 * Sets the form field name to check for the CSRF token.
 */
void
htmx_csrf_set_form_field(HtmxCsrf *self, const gchar *field);

/**
 * htmx_csrf_generate_token:
 * @self: an #HtmxCsrf
 * @session_id: the session to store the token in
 *
 * Generates a new CSRF token and stores it in the session.
 *
 * Returns: (transfer none): the generated token
 */
const gchar *
htmx_csrf_generate_token(HtmxCsrf *self, const gchar *session_id);

/**
 * htmx_csrf_validate_token:
 * @self: an #HtmxCsrf
 * @session_id: the session to validate against
 * @token: the submitted token to validate
 *
 * Validates a submitted CSRF token against the session's stored token.
 *
 * Returns: %TRUE if the token is valid
 */
gboolean
htmx_csrf_validate_token(
	HtmxCsrf   *self,
	const gchar *session_id,
	const gchar *token
);

/**
 * htmx_csrf_middleware:
 * @context: the request context
 * @next: continuation function
 * @next_data: continuation data
 * @user_data: (type HtmxCsrf): the CSRF protector
 *
 * Middleware function for CSRF protection.
 * For safe methods (GET): generates a token and stores it on the context.
 * For unsafe methods (POST, PUT, PATCH, DELETE): validates the token
 * from the X-CSRF-Token header or _csrf form field.
 *
 * On validation failure, sets a 403 Forbidden response.
 *
 * Register AFTER session middleware:
 *   htmx_server_use(server, htmx_csrf_middleware,
 *                   g_object_ref(csrf), g_object_unref)
 */
void
htmx_csrf_middleware(
	HtmxContext        *context,
	HtmxMiddlewareNext  next,
	gpointer            next_data,
	gpointer            user_data
);

G_END_DECLS

#endif /* HTMX_CSRF_H */
