/*
 * htmx-glib.h - Umbrella header for htmx-glib
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * htmx-glib is a GObject-based library for building HTMX-compatible
 * HTTP servers in C. It provides:
 *
 * - Parsing of HTMX request headers
 * - Generation of HTML elements with HTMX attributes
 * - Building HTMX response headers
 * - Integration with libsoup for HTTP server functionality
 * - SSE and WebSocket support for real-time updates
 * - GObject-Introspection compatibility
 */

#ifndef HTMX_GLIB_H
#define HTMX_GLIB_H

#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>
#include <libsoup/soup.h>
#include <json-glib/json-glib.h>

G_BEGIN_DECLS

/* Version information */
#include "htmx-version.h"

/* Forward declarations */
#include "htmx-types.h"

/* Core infrastructure */
#include "core/htmx-enums.h"
#include "core/htmx-error.h"
#include "core/htmx-config.h"
#include "core/htmx-request.h"
#include "core/htmx-response.h"
#include "core/htmx-context.h"
#include "core/htmx-middleware.h"
#include "core/htmx-cookie.h"
#include "core/htmx-session.h"
#include "core/htmx-csrf.h"
#include "core/htmx-validator.h"
#include "core/htmx-cache.h"
#include "core/htmx-memory-cache.h"
#include "core/htmx-rate-limiter.h"
#include "core/htmx-i18n.h"
#include "core/htmx-template-engine.h"
#include "core/htmx-handler.h"
#include "core/htmx-router.h"
#include "core/htmx-server.h"

/* Data model types (boxed) */
#include "model/htmx-attribute.h"
#include "model/htmx-trigger.h"
#include "model/htmx-swap.h"
#include "model/htmx-headers.h"

/* HTML element building */
#include "element/htmx-element.h"
#include "element/htmx-node.h"
#include "element/htmx-builder.h"
#include "element/htmx-fragment.h"
#include "element/htmx-form.h"
#include "element/htmx-input.h"
#include "element/htmx-template.h"

/* Explicit HTML element types */
#include "element/htmx-div.h"
#include "element/htmx-span.h"
#include "element/htmx-p.h"
#include "element/htmx-heading.h"
#include "element/htmx-a.h"
#include "element/htmx-button.h"
#include "element/htmx-label.h"
#include "element/htmx-list.h"
#include "element/htmx-img.h"
#include "element/htmx-textarea.h"
#include "element/htmx-select.h"

/* Table elements */
#include "element/htmx-table.h"

/* Semantic container elements */
#include "element/htmx-semantic.h"

/* Form grouping elements */
#include "element/htmx-fieldset.h"

/* Text formatting elements */
#include "element/htmx-text.h"

/* Definition list elements */
#include "element/htmx-deflist.h"

/* Interactive elements */
#include "element/htmx-interactive.h"

/* Media elements */
#include "element/htmx-media.h"

/* Void/empty elements */
#include "element/htmx-void.h"

/* Metadata and scripting elements */
#include "element/htmx-meta.h"

/* Document structure elements */
#include "element/htmx-document.h"

/* Extensions */
#include "extensions/htmx-extension.h"
#include "extensions/htmx-extension-registry.h"
#include "extensions/htmx-sse.h"
#include "extensions/htmx-websocket.h"

G_END_DECLS

#endif /* HTMX_GLIB_H */
