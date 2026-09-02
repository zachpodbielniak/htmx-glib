/*
 * htmx-path.c - Filesystem containment, for the places that build a path
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-path.h"
#include <string.h>

gboolean
htmx_path_is_inside(const gchar *root, const gchar *path)
{
	g_autofree gchar *real_root = NULL;
	g_autofree gchar *real_path = NULL;
	gsize root_length;

	g_return_val_if_fail(root != NULL, FALSE);
	g_return_val_if_fail(path != NULL, FALSE);

	real_root = g_canonicalize_filename(root, NULL);
	real_path = g_canonicalize_filename(path, NULL);

	if (real_root == NULL || real_path == NULL) {
		return FALSE;
	}

	if (g_strcmp0(real_root, real_path) == 0) {
		return TRUE;
	}

	root_length = strlen(real_root);

	/*
	 * The separator matters. Without it "/srv/static-secrets" counts as
	 * inside "/srv/static", which is a prefix and not a parent.
	 */
	if (strncmp(real_path, real_root, root_length) != 0) {
		return FALSE;
	}

	return real_path[root_length] == G_DIR_SEPARATOR;
}
