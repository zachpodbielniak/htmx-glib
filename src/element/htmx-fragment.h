/*
 * htmx-fragment.h - HTML fragment container
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef HTMX_FRAGMENT_H
#define HTMX_FRAGMENT_H

#include <glib-object.h>
#include "htmx-node.h"

G_BEGIN_DECLS

#define HTMX_TYPE_FRAGMENT (htmx_fragment_get_type())
G_DECLARE_FINAL_TYPE(HtmxFragment, htmx_fragment, HTMX, FRAGMENT, HtmxNode)

HtmxFragment *htmx_fragment_new(void);

G_END_DECLS

#endif /* HTMX_FRAGMENT_H */
