/*
 * test-validation-group.c - Tests for HtmxValidationGroup
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include <htmx-glib.h>
#include <string.h>

static void
test_validation_group_new(void)
{
	g_autoptr(HtmxValidationGroup) group = htmx_validation_group_new();

	g_assert_nonnull(group);
	g_assert_true(HTMX_IS_VALIDATION_GROUP(group));
}

static void
test_validation_group_empty_is_valid(void)
{
	g_autoptr(HtmxValidationGroup) group = htmx_validation_group_new();
	GHashTable *form_data;
	gboolean valid;

	form_data = g_hash_table_new(g_str_hash, g_str_equal);
	g_hash_table_insert(form_data, (gpointer)"name", (gpointer)"John");

	valid = htmx_validation_group_is_valid(group, form_data);
	g_assert_true(valid);

	g_hash_table_unref(form_data);
}

static void
test_validation_group_per_field_pass(void)
{
	g_autoptr(HtmxValidationGroup) group = htmx_validation_group_new();
	g_autoptr(HtmxValidator) required = htmx_validator_required(NULL);
	GHashTable *form_data;
	GHashTable *errors;

	htmx_validation_group_add_validator(group, "name", required);

	form_data = g_hash_table_new(g_str_hash, g_str_equal);
	g_hash_table_insert(form_data, (gpointer)"name", (gpointer)"Alice");

	errors = htmx_validation_group_validate(group, form_data);
	g_assert_nonnull(errors);
	g_assert_cmpuint(g_hash_table_size(errors), ==, 0);

	g_hash_table_unref(errors);
	g_hash_table_unref(form_data);
}

static void
test_validation_group_per_field_fail(void)
{
	g_autoptr(HtmxValidationGroup) group = htmx_validation_group_new();
	g_autoptr(HtmxValidator) required = htmx_validator_required(NULL);
	GHashTable *form_data;
	GHashTable *errors;

	htmx_validation_group_add_validator(group, "name", required);

	form_data = g_hash_table_new(g_str_hash, g_str_equal);
	g_hash_table_insert(form_data, (gpointer)"name", (gpointer)"");

	errors = htmx_validation_group_validate(group, form_data);
	g_assert_nonnull(errors);
	g_assert_cmpuint(g_hash_table_size(errors), ==, 1);
	g_assert_nonnull(g_hash_table_lookup(errors, "name"));

	g_hash_table_unref(errors);
	g_hash_table_unref(form_data);
}

static void
test_validation_group_missing_field(void)
{
	g_autoptr(HtmxValidationGroup) group = htmx_validation_group_new();
	g_autoptr(HtmxValidator) required = htmx_validator_required(NULL);
	GHashTable *form_data;
	GHashTable *errors;

	htmx_validation_group_add_validator(group, "email", required);

	/* form_data has no "email" key */
	form_data = g_hash_table_new(g_str_hash, g_str_equal);

	errors = htmx_validation_group_validate(group, form_data);
	g_assert_cmpuint(g_hash_table_size(errors), ==, 1);
	g_assert_nonnull(g_hash_table_lookup(errors, "email"));

	g_hash_table_unref(errors);
	g_hash_table_unref(form_data);
}

static void
test_validation_group_multiple_validators(void)
{
	g_autoptr(HtmxValidationGroup) group = htmx_validation_group_new();
	g_autoptr(HtmxValidator) required = htmx_validator_required(NULL);
	g_autoptr(HtmxValidator) email_v = htmx_validator_email(NULL);
	g_autoptr(HtmxValidator) min_len = htmx_validator_min_length(3, NULL);
	GHashTable *form_data;
	GHashTable *errors;

	htmx_validation_group_add_validator(group, "email", required);
	htmx_validation_group_add_validator(group, "email", email_v);
	htmx_validation_group_add_validator(group, "name", min_len);

	/* Valid data */
	form_data = g_hash_table_new(g_str_hash, g_str_equal);
	g_hash_table_insert(form_data, (gpointer)"email", (gpointer)"test@example.com");
	g_hash_table_insert(form_data, (gpointer)"name", (gpointer)"Alice");

	errors = htmx_validation_group_validate(group, form_data);
	g_assert_cmpuint(g_hash_table_size(errors), ==, 0);

	g_hash_table_unref(errors);
	g_hash_table_unref(form_data);

	/* Invalid: empty email fails required (first validator), short name fails min_length */
	form_data = g_hash_table_new(g_str_hash, g_str_equal);
	g_hash_table_insert(form_data, (gpointer)"email", (gpointer)"");
	g_hash_table_insert(form_data, (gpointer)"name", (gpointer)"AB");

	errors = htmx_validation_group_validate(group, form_data);
	g_assert_cmpuint(g_hash_table_size(errors), ==, 2);
	g_assert_nonnull(g_hash_table_lookup(errors, "email"));
	g_assert_nonnull(g_hash_table_lookup(errors, "name"));

	g_hash_table_unref(errors);
	g_hash_table_unref(form_data);
}

/* Cross-field rule: password must match confirm_password */
static gboolean
passwords_match(GHashTable *form_data,
                gpointer    user_data,
                gchar     **error)
{
	const gchar *pw;
	const gchar *confirm;

	(void)user_data;

	pw = g_hash_table_lookup(form_data, "password");
	confirm = g_hash_table_lookup(form_data, "confirm_password");

	if (g_strcmp0(pw, confirm) != 0) {
		if (error != NULL)
			*error = g_strdup("Passwords do not match");
		return FALSE;
	}
	return TRUE;
}

static void
test_validation_group_cross_field_pass(void)
{
	g_autoptr(HtmxValidationGroup) group = htmx_validation_group_new();
	GHashTable *form_data;
	GHashTable *errors;

	htmx_validation_group_add_cross_field_rule(
		group, "password_match", passwords_match, NULL, NULL);

	form_data = g_hash_table_new(g_str_hash, g_str_equal);
	g_hash_table_insert(form_data, (gpointer)"password", (gpointer)"secret123");
	g_hash_table_insert(form_data, (gpointer)"confirm_password", (gpointer)"secret123");

	errors = htmx_validation_group_validate(group, form_data);
	g_assert_cmpuint(g_hash_table_size(errors), ==, 0);

	g_hash_table_unref(errors);
	g_hash_table_unref(form_data);
}

static void
test_validation_group_cross_field_fail(void)
{
	g_autoptr(HtmxValidationGroup) group = htmx_validation_group_new();
	GHashTable *form_data;
	GHashTable *errors;

	htmx_validation_group_add_cross_field_rule(
		group, "password_match", passwords_match, NULL, NULL);

	form_data = g_hash_table_new(g_str_hash, g_str_equal);
	g_hash_table_insert(form_data, (gpointer)"password", (gpointer)"secret123");
	g_hash_table_insert(form_data, (gpointer)"confirm_password", (gpointer)"wrong");

	errors = htmx_validation_group_validate(group, form_data);
	g_assert_cmpuint(g_hash_table_size(errors), ==, 1);
	g_assert_cmpstr(g_hash_table_lookup(errors, "password_match"), ==,
	                "Passwords do not match");

	g_hash_table_unref(errors);
	g_hash_table_unref(form_data);
}

static void
test_validation_group_combined(void)
{
	g_autoptr(HtmxValidationGroup) group = htmx_validation_group_new();
	g_autoptr(HtmxValidator) req_pw = htmx_validator_required(NULL);
	g_autoptr(HtmxValidator) req_conf = htmx_validator_required(NULL);
	GHashTable *form_data;
	gboolean valid;

	htmx_validation_group_add_validator(group, "password", req_pw);
	htmx_validation_group_add_validator(group, "confirm_password", req_conf);
	htmx_validation_group_add_cross_field_rule(
		group, "password_match", passwords_match, NULL, NULL);

	/* All valid */
	form_data = g_hash_table_new(g_str_hash, g_str_equal);
	g_hash_table_insert(form_data, (gpointer)"password", (gpointer)"abc");
	g_hash_table_insert(form_data, (gpointer)"confirm_password", (gpointer)"abc");

	valid = htmx_validation_group_is_valid(group, form_data);
	g_assert_true(valid);

	g_hash_table_unref(form_data);

	/* Both empty — fails required AND cross-field (both NULL, so match) */
	form_data = g_hash_table_new(g_str_hash, g_str_equal);
	g_hash_table_insert(form_data, (gpointer)"password", (gpointer)"");
	g_hash_table_insert(form_data, (gpointer)"confirm_password", (gpointer)"");

	valid = htmx_validation_group_is_valid(group, form_data);
	g_assert_false(valid);

	g_hash_table_unref(form_data);
}

static gint cross_destroy_count = 0;

static void
cross_destroy_notify(gpointer data)
{
	(void)data;
	cross_destroy_count++;
}

static gboolean
always_pass(GHashTable *form_data, gpointer user_data, gchar **error)
{
	(void)form_data;
	(void)user_data;
	(void)error;
	return TRUE;
}

static void
test_validation_group_cross_field_destroy(void)
{
	HtmxValidationGroup *group;
	gchar *data;

	cross_destroy_count = 0;
	data = g_strdup("test");

	group = htmx_validation_group_new();
	htmx_validation_group_add_cross_field_rule(
		group, "rule", always_pass, data, cross_destroy_notify);

	g_assert_cmpint(cross_destroy_count, ==, 0);
	g_object_unref(group);
	g_assert_cmpint(cross_destroy_count, ==, 1);
}

int
main(int argc, char *argv[])
{
	g_test_init(&argc, &argv, NULL);

	g_test_add_func("/validation-group/new", test_validation_group_new);
	g_test_add_func("/validation-group/empty_is_valid", test_validation_group_empty_is_valid);
	g_test_add_func("/validation-group/per_field_pass", test_validation_group_per_field_pass);
	g_test_add_func("/validation-group/per_field_fail", test_validation_group_per_field_fail);
	g_test_add_func("/validation-group/missing_field", test_validation_group_missing_field);
	g_test_add_func("/validation-group/multiple_validators", test_validation_group_multiple_validators);
	g_test_add_func("/validation-group/cross_field_pass", test_validation_group_cross_field_pass);
	g_test_add_func("/validation-group/cross_field_fail", test_validation_group_cross_field_fail);
	g_test_add_func("/validation-group/combined", test_validation_group_combined);
	g_test_add_func("/validation-group/cross_field_destroy", test_validation_group_cross_field_destroy);

	return g_test_run();
}
