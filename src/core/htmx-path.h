/*
 * htmx-path.h - Filesystem containment, for the places that build a path
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * Internal to the library: not in <htmx-glib.h> and not installed.
 *
 * Two places turn a name that came from outside into a filename and read
 * it -- htmx_router_serve_static() and the template engine's
 * load_template().  The router had this check and the template engine did
 * not, which is what a rule enforced at one call site looks like.  It
 * lives here so there is one of it.
 */

#ifndef HTMX_PATH_H
#define HTMX_PATH_H

#include <glib.h>

G_BEGIN_DECLS

/**
 * htmx_path_is_inside:
 * @root: the directory a path is not allowed to leave
 * @path: the candidate path, which need not exist
 *
 * Answers whether @path is @root itself or lies beneath it, comparing
 * the two after g_canonicalize_filename() has resolved "." and ".."
 * lexically.  The separator after @root is part of the comparison:
 * without it "/srv/static-secrets" counts as inside "/srv/static", which
 * is a prefix and not a parent.
 *
 * What this cannot see: g_canonicalize_filename() is lexical, so a
 * symlink inside @root pointing out of it is inside as far as this
 * function is concerned.  Resolving those needs the file to exist, which
 * is not true of every path a caller wants to check, so the guarantee
 * offered here is exactly "the name does not name its way out".
 *
 * Returns: %TRUE if @path is contained by @root
 */
gboolean
htmx_path_is_inside(
	const gchar *root,
	const gchar *path
);

G_END_DECLS

#endif /* HTMX_PATH_H */
