/*
 * htmx-types.h - Forward type declarations for htmx-glib
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef HTMX_TYPES_H
#define HTMX_TYPES_H

#include <glib-object.h>

G_BEGIN_DECLS

/*
 * GObject types (final classes)
 */

typedef struct _HtmxConfig              HtmxConfig;
typedef struct _HtmxRequest             HtmxRequest;
typedef struct _HtmxResponse            HtmxResponse;
typedef struct _HtmxRouter              HtmxRouter;
typedef struct _HtmxServer              HtmxServer;
typedef struct _HtmxFragment            HtmxFragment;
typedef struct _HtmxForm                HtmxForm;
typedef struct _HtmxInput               HtmxInput;
typedef struct _HtmxTemplate            HtmxTemplate;
typedef struct _HtmxSseConnection       HtmxSseConnection;
typedef struct _HtmxWebSocketConnection HtmxWebSocketConnection;

/*
 * GObject types (derivable classes)
 */

typedef struct _HtmxNode                HtmxNode;
typedef struct _HtmxNodeClass           HtmxNodeClass;

/*
 * GBoxed types
 */

typedef struct _HtmxAttribute           HtmxAttribute;
typedef struct _HtmxTrigger             HtmxTrigger;
typedef struct _HtmxSwap                HtmxSwap;
typedef struct _HtmxRequestHeaders      HtmxRequestHeaders;
typedef struct _HtmxResponseHeaders     HtmxResponseHeaders;

/*
 * GInterface types
 */

typedef struct _HtmxElement             HtmxElement;
typedef struct _HtmxElementInterface    HtmxElementInterface;

typedef struct _HtmxHandler             HtmxHandler;
typedef struct _HtmxHandlerInterface    HtmxHandlerInterface;

typedef struct _HtmxExtension           HtmxExtension;
typedef struct _HtmxExtensionInterface  HtmxExtensionInterface;

/*
 * Callback types
 */

/**
 * HtmxRouteCallback:
 * @request: the incoming #HtmxRequest
 * @params: (element-type utf8 utf8): path parameters extracted from the URL
 * @user_data: user data passed to the route registration
 *
 * Callback function for handling HTMX requests.
 *
 * Returns: (transfer full): a new #HtmxResponse to send to the client
 */
typedef HtmxResponse *(*HtmxRouteCallback)(HtmxRequest  *request,
                                           GHashTable   *params,
                                           gpointer      user_data);

/**
 * HtmxSseCallback:
 * @connection: the #HtmxSseConnection
 * @user_data: user data passed to the SSE registration
 *
 * Callback function for SSE events.
 */
typedef void (*HtmxSseCallback)(HtmxSseConnection *connection,
                                gpointer           user_data);

/**
 * HtmxWebSocketCallback:
 * @connection: the #HtmxWebSocketConnection
 * @message: the received message
 * @user_data: user data passed to the WebSocket registration
 *
 * Callback function for WebSocket messages.
 */
typedef void (*HtmxWebSocketCallback)(HtmxWebSocketConnection *connection,
                                      const gchar             *message,
                                      gpointer                 user_data);

G_END_DECLS

#endif /* HTMX_TYPES_H */
