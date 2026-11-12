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
#include <libsoup/soup.h>

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

/*
 * Wiping.
 *
 * A request body is where a password arrives: a login form, a password
 * change, an API call creating an account.  Every copy of it this library
 * made -- the body string, the body bytes, the decoded form values, the
 * multipart fields -- used to go back to the allocator with g_free(), which
 * leaves the bytes where they were until the allocator happens to hand the
 * block to somebody else.  A core dump, a heap read past some other bug, or
 * a swapped-out page then has the plaintext in it.
 *
 * A plain memset() before the free does not fix that: the compiler may see
 * that nothing reads the buffer again and delete the store.  The functions
 * below are the one place the store is made unremovable, and everything in
 * this library that held request input frees through them.
 *
 * What they cannot reach is said once, here: libsoup's socket read buffers
 * and its header parser, the TLS library's records, the kernel's socket
 * buffers, and anything a caller copied out of a value (a JsonParser built
 * on the body copies every string it reads, for instance).
 */

/**
 * htmx_secret_wipe:
 * @data: (nullable) (array length=length) (element-type guint8): memory to clear
 * @length: how many bytes to clear
 *
 * Overwrites @length bytes at @data with zeros in a way the compiler may
 * not remove as a dead store.  A %NULL @data or a zero @length does
 * nothing.
 */
void
htmx_secret_wipe(
	gpointer data,
	gsize    length
);

/**
 * htmx_secret_free:
 * @str: (nullable) (transfer full): a string that may hold a secret
 *
 * Wipes @str up to its terminating NUL and frees it with g_free().  It is
 * a #GDestroyNotify for hash tables whose keys or values came from a
 * request, and it is safe on %NULL.
 */
void
htmx_secret_free(gpointer str);

/**
 * htmx_secret_bytes_new:
 * @data: (nullable) (array length=size) (element-type guint8): bytes to copy
 * @size: how many bytes
 *
 * Copies @data into a new #GBytes whose storage is wiped when its last
 * reference is dropped.  One byte past @size is always a NUL that is not
 * part of the #GBytes, so the data may be read as a C string up to its
 * first NUL without a second copy.
 *
 * Returns: (transfer full): the new #GBytes
 */
GBytes *
htmx_secret_bytes_new(
	gconstpointer data,
	gsize         size
);

/**
 * htmx_secret_wipe_message_body:
 * @body: (nullable): a #SoupMessageBody to clear
 *
 * Wipes every chunk @body holds, and the flattened copy libsoup makes of
 * them, then truncates it.  soup_message_body_truncate() alone releases
 * the chunks without clearing them.
 *
 * Use it only on a request body, and only once nothing will read it
 * again.  It writes into the chunks' storage, which for a body libsoup
 * read off a socket is ordinary heap; a body built from static or mapped
 * memory must not be passed here.
 */
void
htmx_secret_wipe_message_body(SoupMessageBody *body);

G_END_DECLS

#endif /* HTMX_SECRET_H */
