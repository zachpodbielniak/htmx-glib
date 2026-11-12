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

	/* The bytes themselves do not stay on the heap behind the string.  A
	 * plain memset() here was a dead store the optimiser was free to
	 * delete, since nothing reads raw again. */
	htmx_secret_wipe(raw, bytes);

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

/*
 * The store has to survive dead-store elimination.  explicit_bzero() is
 * exactly that where libc has it; elsewhere memset() is called through a
 * volatile function pointer, which the compiler must load at run time and
 * so cannot prove is memset().
 */
#if defined(__GLIBC__) || defined(__FreeBSD__) || defined(__OpenBSD__) || \
    defined(__NetBSD__) || defined(__DragonFly__)
# define HTMX_HAVE_EXPLICIT_BZERO 1
#else
static void *(*const volatile htmx_secret_memset)(void *, int, size_t) = memset;
#endif

void
htmx_secret_wipe(
	gpointer data,
	gsize    length
){
	if (data == NULL || length == 0) {
		return;
	}

#if defined(HTMX_HAVE_EXPLICIT_BZERO)
	explicit_bzero(data, length);
#else
	htmx_secret_memset(data, 0, length);
#endif
}

void
htmx_secret_free(gpointer str)
{
	if (str == NULL) {
		return;
	}

	htmx_secret_wipe(str, strlen((const gchar *)str));
	g_free(str);
}

/*
 * What a secret #GBytes carries to its free function: the length is not
 * recoverable from the pointer, and the data may contain NULs, so strlen()
 * would stop short of the end.
 */
typedef struct {
	gsize   size;
	guint8 *data;
} HtmxSecretBuffer;

static void
secret_buffer_free(gpointer user_data)
{
	HtmxSecretBuffer *buffer = (HtmxSecretBuffer *)user_data;

	/* The trailing NUL is ours too, but it is already zero. */
	htmx_secret_wipe(buffer->data, buffer->size);
	g_free(buffer->data);
	g_free(buffer);
}

GBytes *
htmx_secret_bytes_new(
	gconstpointer data,
	gsize         size
){
	HtmxSecretBuffer *buffer;

	g_return_val_if_fail(data != NULL || size == 0, NULL);
	g_return_val_if_fail(size < G_MAXSIZE, NULL);

	/*
	 * Allocated by hand rather than with g_bytes_new(), whose storage is
	 * freed by GLib with no chance to clear it, and one byte longer so
	 * htmx_request_get_body() can be the same memory as the bytes instead
	 * of a second plaintext copy.
	 */
	buffer = g_new0(HtmxSecretBuffer, 1);
	buffer->size = size;
	buffer->data = (guint8 *)g_malloc(size + 1);
	if (size > 0) {
		memcpy(buffer->data, data, size);
	}
	buffer->data[size] = '\0';

	return g_bytes_new_with_free_func(buffer->data, size,
	                                  secret_buffer_free, buffer);
}

/*
 * Clear one #GBytes in place.  The storage is not ours to write in
 * general -- a #GBytes is immutable by contract -- which is why the only
 * caller is the request body, whose chunks libsoup allocated off the
 * socket and nobody reads once the request is finished.
 */
static void
wipe_bytes_in_place(GBytes *bytes)
{
	gconstpointer data;
	gsize size = 0;

	if (bytes == NULL) {
		return;
	}

	data = g_bytes_get_data(bytes, &size);
	htmx_secret_wipe((gpointer)data, size);
}

void
htmx_secret_wipe_message_body(SoupMessageBody *body)
{
	goffset offset = 0;

	if (body == NULL) {
		return;
	}

	/*
	 * soup_message_body_get_chunk() hands back each stored chunk's own
	 * memory (a slice, not a copy), so clearing what it returns clears
	 * the chunk.  A body that did not accumulate holds no chunks and the
	 * loop ends at once.
	 */
	while (offset < body->length) {
		g_autoptr(GBytes) chunk = soup_message_body_get_chunk(body, offset);
		gsize size;

		if (chunk == NULL) {
			break;
		}

		size = g_bytes_get_size(chunk);
		if (size == 0) {
			break;
		}

		wipe_bytes_in_place(chunk);
		offset += (goffset)size;
	}

	/*
	 * soup_message_body_flatten() concatenated the chunks into a buffer
	 * of its own (body->data points there), which the walk above does not
	 * see.  libsoup keeps body->data NULL until a flatten and drops it
	 * again on the next append, so when it is set it is that copy and it
	 * is exactly body->length long -- never one chunk of a longer body.
	 */
	if (body->length > 0 && body->data != NULL) {
		htmx_secret_wipe((gpointer)body->data, (gsize)body->length);
	}

	soup_message_body_truncate(body);
}
