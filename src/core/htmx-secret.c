/*
 * htmx-secret.c - Values that have to be unguessable
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-secret.h"

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#if defined(__has_include)
# if __has_include(<sys/random.h>)
#  include <sys/random.h>
#  define HTMX_HAVE_GETRANDOM 1
# endif
#endif

/*
 * Fill @buffer from the kernel, or say it could not.
 *
 * Both routes loop: getrandom(2) is documented to return short for a
 * request over 256 bytes and to fail with EINTR, and a short read from
 * /dev/urandom is an ordinary read result rather than an error.  A
 * partially filled buffer is the dangerous outcome here -- the untouched
 * tail is whatever the stack held -- so nothing short counts as success.
 */
static gboolean
fill_from_kernel(guint8 *buffer, gsize length)
{
	gsize filled = 0;
	gint fd;

#ifdef HTMX_HAVE_GETRANDOM
	while (filled < length) {
		gssize n = getrandom(buffer + filled, length - filled, 0);

		if (n < 0) {
			if (errno == EINTR) {
				continue;
			}
			break;
		}

		filled += (gsize)n;
	}

	if (filled == length) {
		return TRUE;
	}

	/* Whatever getrandom() managed is not trusted on its own. */
	filled = 0;
#endif

	fd = open("/dev/urandom", O_RDONLY | O_CLOEXEC);
	if (fd < 0) {
		return FALSE;
	}

	while (filled < length) {
		gssize n = read(fd, buffer + filled, length - filled);

		if (n < 0) {
			if (errno == EINTR) {
				continue;
			}
			break;
		}

		if (n == 0) {
			break;
		}

		filled += (gsize)n;
	}

	close(fd);

	return filled == length;
}

gchar *
htmx_secret_new_hex(gsize bytes)
{
	g_autofree guint8 *raw = NULL;
	gchar *hex;
	gsize i;

	g_return_val_if_fail(bytes > 0, NULL);

	raw = g_malloc(bytes);

	if (!fill_from_kernel(raw, bytes)) {
		/*
		 * Not a warning and not a fallback.  Returning a value
		 * derived from anything else here would be a session id that
		 * somebody can compute, which is worse than no server at
		 * all -- and it would be indistinguishable, from the outside,
		 * from one that works.
		 */
		g_error("htmx: no kernel entropy source: getrandom(2) and "
		        "/dev/urandom both failed, refusing to invent a "
		        "secret");
	}

	hex = g_malloc(bytes * 2 + 1);

	for (i = 0; i < bytes; i++) {
		static const gchar digits[] = "0123456789abcdef";

		hex[i * 2] = digits[raw[i] >> 4];
		hex[i * 2 + 1] = digits[raw[i] & 0x0f];
	}

	hex[bytes * 2] = '\0';

	/* The bytes themselves do not stay on the heap behind the string. */
	memset(raw, 0, bytes);

	return hex;
}

gboolean
htmx_secret_equals(const gchar *a, const gchar *b)
{
	gsize length;
	gsize i;
	volatile guchar difference = 0;

	if (a == NULL || b == NULL) {
		return FALSE;
	}

	length = strlen(a);

	if (length != strlen(b)) {
		return FALSE;
	}

	for (i = 0; i < length; i++) {
		difference |= (guchar)a[i] ^ (guchar)b[i];
	}

	return difference == 0;
}
