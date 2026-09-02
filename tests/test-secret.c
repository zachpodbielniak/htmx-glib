/*
 * test-secret.c - Where an unguessable value comes from
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * Session ids and CSRF tokens both came from g_uuid_string_random(),
 * which is a pure function of GLib's global GRand -- MT19937, whose
 * state is recoverable from a few hundred consecutive outputs.  Both
 * values are handed to whoever asks for a page: the session id in a
 * Set-Cookie, the CSRF token in the form.  So an attacker gets the
 * outputs by design and can then compute everybody else's.
 *
 * The assertion these tests make is not "the value looks random" -- a
 * Mersenne Twister passes that -- but "reseeding GLib's generator does
 * not reproduce it".  That is the difference between the two sources,
 * stated directly, and it is why the test does not need a hook into the
 * library to see which one is in use.
 */

#include <htmx-glib.h>
#include <string.h>

static void
test_a_reseed_does_not_reproduce_a_secret(void)
{
	g_autofree gchar *first = NULL;
	g_autofree gchar *second = NULL;

	g_random_set_seed(1234);
	first = htmx_secret_new_hex(HTMX_SECRET_DEFAULT_BYTES);

	g_random_set_seed(1234);
	second = htmx_secret_new_hex(HTMX_SECRET_DEFAULT_BYTES);

	g_assert_nonnull(first);
	g_assert_nonnull(second);
	g_assert_cmpstr(first, !=, second);
}

static void
test_a_secret_is_the_length_and_alphabet_it_says(void)
{
	g_autofree gchar *secret = htmx_secret_new_hex(16);
	gsize i;

	g_assert_cmpuint(strlen(secret), ==, 32);

	for (i = 0; secret[i] != '\0'; i++) {
		g_assert_true(g_ascii_isxdigit(secret[i]));
		g_assert_false(g_ascii_isupper(secret[i]));
	}

	/* 128 bits is the floor, so the default may not be under it. */
	g_assert_cmpuint(HTMX_SECRET_DEFAULT_BYTES, >=, 16);
}

/*
 * Drawn many times, every one different.  A generator stuck on one value
 * would pass the reseed test above by accident if that value were also
 * what it returned before the reseed.
 */
static void
test_secrets_do_not_repeat(void)
{
	g_autoptr(GHashTable) seen =
		g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
	guint i;

	for (i = 0; i < 256; i++) {
		gchar *secret = htmx_secret_new_hex(HTMX_SECRET_DEFAULT_BYTES);

		g_assert_false(g_hash_table_contains(seen, secret));
		g_hash_table_add(seen, secret);
	}
}

/* The reachable half: a session id is a credential in a cookie. */
static void
test_a_session_id_survives_a_reseed(void)
{
	g_autoptr(HtmxSession) one = htmx_session_new();
	g_autoptr(HtmxSession) two = htmx_session_new();
	g_autofree gchar *first = NULL;
	g_autofree gchar *second = NULL;

	g_random_set_seed(4321);
	first = htmx_session_create(one);

	g_random_set_seed(4321);
	second = htmx_session_create(two);

	g_assert_cmpstr(first, !=, second);
}

/* And so is a CSRF token, which is printed into the page. */
static void
test_a_csrf_token_survives_a_reseed(void)
{
	g_autoptr(HtmxSession) one = htmx_session_new();
	g_autoptr(HtmxSession) two = htmx_session_new();
	g_autoptr(HtmxCsrf) csrf_one = htmx_csrf_new(one);
	g_autoptr(HtmxCsrf) csrf_two = htmx_csrf_new(two);
	g_autofree gchar *first = NULL;

	g_random_set_seed(777);
	first = g_strdup(htmx_csrf_generate_token(csrf_one, "session-a"));

	g_random_set_seed(777);
	g_assert_cmpstr(first, !=,
	                htmx_csrf_generate_token(csrf_two, "session-a"));
}

static void
test_secret_equals(void)
{
	g_assert_true(htmx_secret_equals("abc", "abc"));
	g_assert_false(htmx_secret_equals("abc", "abd"));
	g_assert_false(htmx_secret_equals("abc", "abcd"));
	g_assert_false(htmx_secret_equals("", "a"));
	g_assert_true(htmx_secret_equals("", ""));
	g_assert_false(htmx_secret_equals(NULL, "abc"));
	g_assert_false(htmx_secret_equals("abc", NULL));
	g_assert_false(htmx_secret_equals(NULL, NULL));
}

int
main(int argc, char *argv[])
{
	g_test_init(&argc, &argv, NULL);

	g_test_add_func("/secret/a-reseed-does-not-reproduce-a-secret",
	                test_a_reseed_does_not_reproduce_a_secret);
	g_test_add_func("/secret/length-and-alphabet",
	                test_a_secret_is_the_length_and_alphabet_it_says);
	g_test_add_func("/secret/secrets-do-not-repeat",
	                test_secrets_do_not_repeat);
	g_test_add_func("/secret/a-session-id-survives-a-reseed",
	                test_a_session_id_survives_a_reseed);
	g_test_add_func("/secret/a-csrf-token-survives-a-reseed",
	                test_a_csrf_token_survives_a_reseed);
	g_test_add_func("/secret/equals", test_secret_equals);

	return g_test_run();
}
