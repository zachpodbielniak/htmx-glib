/*
 * test-validator.c - Tests for HtmxValidator
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * HtmxValidator had no test file of its own -- only the group that
 * drives it did -- which is part of why a caller's error message
 * reached printf as a format string for as long as it did.
 */

#include <htmx-glib.h>
#include <string.h>

/*
 * The message a caller gives is shown as typed.
 *
 * This is the regression test for the format-string bug: both length
 * validators handed `ld->message` to g_strdup_printf() as its *format*,
 * so a message containing `%s` read a pointer that was never passed and
 * one containing `%n` wrote through it. The message reaches the
 * validator from wherever the caller got it, which is often a form
 * definition or a config file.
 *
 * The samples are chosen to be conversions that consume an argument, so
 * that a build without the fix does not merely print something odd --
 * `%s` on a stack slot holding a guint is where it crashes or leaks.
 */
static void
test_a_custom_message_is_not_a_format(void)
{
	static const gchar *const messages[] = {
		"Must be at least %u characters",
		"100%s of the field is required",
		"Give it 100% effort",
		"%d%d%d",
		"%p and %x",
		"a literal %% stays doubled"
	};
	guint i;

	for (i = 0; i < G_N_ELEMENTS(messages); i++) {
		g_autoptr(HtmxValidator) shorter =
			htmx_validator_min_length(8, messages[i]);
		g_autoptr(HtmxValidator) longer =
			htmx_validator_max_length(4, messages[i]);
		g_autofree gchar *too_short = NULL;
		g_autofree gchar *too_long = NULL;

		g_assert_false(htmx_validator_validate(shorter, "abc",
		                                       &too_short));
		g_assert_cmpstr(too_short, ==, messages[i]);

		g_assert_false(htmx_validator_validate(longer, "abcdefgh",
		                                       &too_long));
		g_assert_cmpstr(too_long, ==, messages[i]);
	}
}

/*
 * The built-in default is the one string that is a format, and it still
 * carries the count -- which is the whole reason it is one.
 */
static void
test_the_default_message_still_names_the_count(void)
{
	g_autoptr(HtmxValidator) shorter = htmx_validator_min_length(8, NULL);
	g_autoptr(HtmxValidator) longer = htmx_validator_max_length(4, NULL);
	g_autofree gchar *too_short = NULL;
	g_autofree gchar *too_long = NULL;

	g_assert_false(htmx_validator_validate(shorter, "abc", &too_short));
	g_assert_cmpstr(too_short, ==, "Must be at least 8 characters");

	g_assert_false(htmx_validator_validate(longer, "abcdefgh", &too_long));
	g_assert_cmpstr(too_long, ==, "Must be at most 4 characters");
}

/*
 * One message, one rendering, whichever path produced it.
 *
 * htmx_validation_group_validate() renders a custom message through
 * g_strdup() and the validator rendered the same string through
 * printf -- so before the fix these two assertions disagreed about
 * what the caller had asked for, and only one of them was reachable
 * from any given call site.
 */
static void
test_the_group_and_the_validator_agree(void)
{
	static const gchar *message = "Needs %s more";
	g_autoptr(HtmxValidationGroup) group = htmx_validation_group_new();
	g_autoptr(HtmxValidator) direct =
		htmx_validator_min_length(8, message);
	HtmxValidator *in_group = htmx_validator_min_length(8, message);
	g_autofree gchar *alone = NULL;
	GHashTable *form_data;
	g_autoptr(GHashTable) errors = NULL;

	g_assert_false(htmx_validator_validate(direct, "abc", &alone));
	g_assert_cmpstr(alone, ==, message);

	htmx_validation_group_add_validator(group, "field", in_group);

	form_data = g_hash_table_new(g_str_hash, g_str_equal);
	g_hash_table_insert(form_data, (gpointer)"field", (gpointer)"abc");

	errors = htmx_validation_group_validate(group, form_data);
	g_assert_cmpstr(g_hash_table_lookup(errors, "field"), ==, message);

	g_hash_table_unref(form_data);
}

/*
 * A passing value sets no error at all, so a caller may leave the
 * out parameter uninitialised only if it stays untouched.
 */
static void
test_a_valid_value_leaves_the_error_alone(void)
{
	g_autoptr(HtmxValidator) shorter =
		htmx_validator_min_length(3, "%n%n%n");
	g_autoptr(HtmxValidator) longer =
		htmx_validator_max_length(8, "%n%n%n");
	gchar *error = NULL;

	g_assert_true(htmx_validator_validate(shorter, "abcd", &error));
	g_assert_null(error);

	g_assert_true(htmx_validator_validate(longer, "abcd", &error));
	g_assert_null(error);
}

/*
 * A NULL value is too short and is never too long.  Worth pinning
 * because the two validators disagree about NULL on purpose, and a
 * refactor that made them symmetrical would break a required-plus-max
 * pairing without any test noticing.
 */
static void
test_a_missing_value_is_short_but_not_long(void)
{
	g_autoptr(HtmxValidator) shorter = htmx_validator_min_length(1, NULL);
	g_autoptr(HtmxValidator) longer = htmx_validator_max_length(4, NULL);
	g_autofree gchar *error = NULL;

	g_assert_false(htmx_validator_validate(shorter, NULL, &error));
	g_assert_true(htmx_validator_validate(longer, NULL, NULL));
}

/*
 * Length is counted in characters rather than bytes, so a validator is
 * not stricter for somebody writing in a language that needs more of
 * them.
 */
static void
test_length_is_counted_in_characters(void)
{
	g_autoptr(HtmxValidator) shorter = htmx_validator_min_length(4, NULL);

	/* Four characters, ten bytes. */
	g_assert_true(htmx_validator_validate(shorter, "\xc3\xa9\xc3\xa8"
	                                               "\xc3\xaa\xc3\xab",
	                                      NULL));
	g_assert_false(htmx_validator_validate(shorter, "\xc3\xa9\xc3\xa8"
	                                                "\xc3\xaa", NULL));
}

int
main(int argc, char *argv[])
{
	g_test_init(&argc, &argv, NULL);

	g_test_add_func("/validator/a-custom-message-is-not-a-format",
	                test_a_custom_message_is_not_a_format);
	g_test_add_func("/validator/the-default-names-the-count",
	                test_the_default_message_still_names_the_count);
	g_test_add_func("/validator/the-group-and-the-validator-agree",
	                test_the_group_and_the_validator_agree);
	g_test_add_func("/validator/a-valid-value-leaves-the-error-alone",
	                test_a_valid_value_leaves_the_error_alone);
	g_test_add_func("/validator/a-missing-value-is-short-not-long",
	                test_a_missing_value_is_short_but_not_long);
	g_test_add_func("/validator/length-is-counted-in-characters",
	                test_length_is_counted_in_characters);

	return g_test_run();
}
