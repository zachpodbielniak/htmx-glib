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
typedef struct _HtmxContext             HtmxContext;
typedef struct _HtmxRequest             HtmxRequest;
typedef struct _HtmxResponse            HtmxResponse;
typedef struct _HtmxRouter              HtmxRouter;
typedef struct _HtmxServer              HtmxServer;
typedef struct _HtmxMiddleware          HtmxMiddleware;
typedef struct _HtmxSession             HtmxSession;
typedef struct _HtmxCsrf                HtmxCsrf;
typedef struct _HtmxFragment            HtmxFragment;
typedef struct _HtmxForm                HtmxForm;
typedef struct _HtmxInput               HtmxInput;
typedef struct _HtmxTemplate            HtmxTemplate;
typedef struct _HtmxMemoryCache         HtmxMemoryCache;
typedef struct _HtmxRateLimiter         HtmxRateLimiter;
typedef struct _HtmxI18n                HtmxI18n;
typedef struct _HtmxTemplateEngine      HtmxTemplateEngine;
typedef struct _HtmxExtensionRegistry   HtmxExtensionRegistry;
typedef struct _HtmxSseConnection       HtmxSseConnection;
typedef struct _HtmxWebSocketConnection HtmxWebSocketConnection;

/*
 * GObject types (derivable classes)
 */

typedef struct _HtmxNode                HtmxNode;
typedef struct _HtmxNodeClass           HtmxNodeClass;

typedef struct _HtmxValidator           HtmxValidator;
typedef struct _HtmxValidatorClass      HtmxValidatorClass;

/*
 * GBoxed types
 */

typedef struct _HtmxAttribute           HtmxAttribute;
typedef struct _HtmxCookie              HtmxCookie;
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

typedef struct _HtmxCache               HtmxCache;
typedef struct _HtmxCacheInterface      HtmxCacheInterface;

typedef struct _HtmxExtension           HtmxExtension;
typedef struct _HtmxExtensionInterface  HtmxExtensionInterface;

/*
 * Callback types
 */

/**
 * HtmxMiddlewareNext:
 * @context: the request context
 * @data: (closure): opaque continuation data
 *
 * Continuation function passed to middleware. Call this to invoke
 * the next middleware in the chain.
 */
typedef void (*HtmxMiddlewareNext)(HtmxContext *context,
                                   gpointer     data);

/**
 * HtmxMiddlewareFunc:
 * @context: the request context
 * @next: continuation to invoke the next middleware
 * @next_data: (closure next): opaque data for @next
 * @user_data: (closure): user data from registration
 *
 * A middleware function. Must call @next(@context, @next_data) to
 * continue the pipeline, or set a response on @context to short-circuit.
 */
typedef void (*HtmxMiddlewareFunc)(HtmxContext        *context,
                                   HtmxMiddlewareNext  next,
                                   gpointer            next_data,
                                   gpointer            user_data);

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
