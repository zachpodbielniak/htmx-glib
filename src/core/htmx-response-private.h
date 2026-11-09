/*
 * htmx-response-private.h - What the server needs of a deferred response
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * Not installed.  Only htmx-server.c and htmx-response.c include it.
 */

#ifndef HTMX_RESPONSE_PRIVATE_H
#define HTMX_RESPONSE_PRIVATE_H

#include "htmx-response.h"

G_BEGIN_DECLS

/*
 * htmx_response_attach_message:
 * @self: a deferred #HtmxResponse
 * @message: the request's message
 *
 * Hands the message to the placeholder.  If the answer is already there
 * it is written at once; otherwise the message is paused until
 * htmx_response_complete(), and abandoned if the client disconnects
 * first.
 */
void
htmx_response_attach_message(
	HtmxResponse      *self,
	SoupServerMessage *message
);

G_END_DECLS

#endif /* HTMX_RESPONSE_PRIVATE_H */
