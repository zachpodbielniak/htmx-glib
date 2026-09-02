/*
 * htmx-secret.h - Values that have to be unguessable
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * Session ids and CSRF tokens are handed to whoever asks for a page --
 * one in a Set-Cookie, the other in the form -- so their generator's
 * output is available to an attacker by design.  Both used to come from
 * g_uuid_string_random(), which is a pure function of GLib's global
 * GRand: MT19937, whose 624-word state is recoverable from a few hundred
 * consecutive outputs, after which every value it will ever produce is
 * known.  Nothing about the values looks wrong; the generator is the
 * whole defect.
 *
 * htmx_secret_new_hex() is the one place in this library that bytes
 * nobody may predict come from, and they come from the kernel.  A UUID
 * is still the right thing for a filename or a correlation id; the
 * question is whether guessing it gets anybody anything.
 */

#ifndef HTMX_SECRET_H
#define HTMX_SECRET_H

#include <glib.h>

G_BEGIN_DECLS

/**
 * HTMX_SECRET_DEFAULT_BYTES:
 *
 * The entropy, in bytes, behind a secret this library mints for itself.
 * 32 bytes is 256 bits; the floor anybody should go to is 16.
 */
#define HTMX_SECRET_DEFAULT_BYTES (32)

/**
 * htmx_secret_new_hex:
 * @bytes: how many bytes of entropy to draw; must not be zero
 *
 * Draws @bytes from the kernel -- getrandom(2) where it exists,
 * /dev/urandom otherwise -- and returns them lowercase hex-encoded, so
 * the string is twice @bytes characters long.
 *
 * There is deliberately no fallback to a userspace generator.  A
 * predictable session id is not a degraded session id, it is somebody
 * else's session, so a machine that can supply neither source aborts
 * rather than issuing one.  On Linux getrandom(2) is a syscall and works
 * inside a chroot with no /dev, which is the case that would otherwise
 * have reached the fallback.
 *
 * Returns: (transfer full): the hex-encoded secret
 */
gchar *
htmx_secret_new_hex(gsize bytes);

/**
 * htmx_secret_equals:
 * @a: (nullable): one value
 * @b: (nullable): the other
 *
 * Compares two secrets without stopping at the first differing byte.
 * g_str_equal() does stop, and how long it takes is a measurement of how
 * much of the secret the caller already has, which is enough to build
 * the rest one byte at a time.
 *
 * The lengths are compared first and that comparison is not constant
 * time, so this hides the contents of @a and not its length.  For the
 * fixed-width values htmx_secret_new_hex() produces there is nothing in
 * the length to learn.
 *
 * A %NULL on either side is not equal to anything, including another
 * %NULL: the callers ask this question about a credential that was
 * supplied, and "neither was supplied" is not a match.
 *
 * Returns: %TRUE if @a and @b are the same string
 */
gboolean
htmx_secret_equals(
	const gchar *a,
	const gchar *b
);

G_END_DECLS

#endif /* HTMX_SECRET_H */
