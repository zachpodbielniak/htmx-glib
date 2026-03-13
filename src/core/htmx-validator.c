/*
 * htmx-validator.c - Form validation implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-validator.h"
#include <string.h>

typedef struct {
	gchar            *error_message;
	HtmxValidateFunc  func;
	gpointer          func_data;
	GDestroyNotify    func_destroy;
} HtmxValidatorPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(HtmxValidator, htmx_validator, G_TYPE_OBJECT)

static gboolean
htmx_validator_real_validate(
	HtmxValidator  *self,
	const gchar    *value,
	gchar         **error
){
	HtmxValidatorPrivate *priv = htmx_validator_get_instance_private(self);

	if (priv->func != NULL) {
		return priv->func(value, priv->func_data, error);
	}

	/* No function set — always passes */
	return TRUE;
}

static void
htmx_validator_finalize(GObject *object)
{
	HtmxValidator *self = HTMX_VALIDATOR(object);
	HtmxValidatorPrivate *priv = htmx_validator_get_instance_private(self);

	g_free(priv->error_message);
	if (priv->func_destroy != NULL && priv->func_data != NULL) {
		priv->func_destroy(priv->func_data);
	}

	G_OBJECT_CLASS(htmx_validator_parent_class)->finalize(object);
}

static void
htmx_validator_class_init(HtmxValidatorClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);

	object_class->finalize = htmx_validator_finalize;
	klass->validate = htmx_validator_real_validate;
}

static void
htmx_validator_init(HtmxValidator *self)
{
	HtmxValidatorPrivate *priv = htmx_validator_get_instance_private(self);

	priv->error_message = NULL;
	priv->func = NULL;
	priv->func_data = NULL;
	priv->func_destroy = NULL;
}

gboolean
htmx_validator_validate(
	HtmxValidator  *self,
	const gchar    *value,
	gchar         **error
){
	HtmxValidatorClass *klass;

	g_return_val_if_fail(HTMX_IS_VALIDATOR(self), FALSE);

	klass = HTMX_VALIDATOR_GET_CLASS(self);
	g_return_val_if_fail(klass->validate != NULL, FALSE);

	return klass->validate(self, value, error);
}

GHashTable *
htmx_validator_validate_form(
	GHashTable *validators,
	GHashTable *form_data
){
	GHashTable *errors;
	GHashTableIter iter;
	gpointer key;
	gpointer val;

	g_return_val_if_fail(validators != NULL, NULL);
	g_return_val_if_fail(form_data != NULL, NULL);

	errors = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);

	g_hash_table_iter_init(&iter, validators);
	while (g_hash_table_iter_next(&iter, &key, &val)) {
		const gchar *field_name = (const gchar *)key;
		HtmxValidator *validator = HTMX_VALIDATOR(val);
		const gchar *value;
		gchar *error = NULL;

		value = (const gchar *)g_hash_table_lookup(form_data, field_name);

		if (!htmx_validator_validate(validator, value, &error)) {
			if (error != NULL) {
				g_hash_table_insert(errors, g_strdup(field_name), error);
			} else {
				HtmxValidatorPrivate *priv;

				priv = htmx_validator_get_instance_private(validator);
				g_hash_table_insert(errors, g_strdup(field_name),
				                    g_strdup(priv->error_message != NULL
				                             ? priv->error_message
				                             : "Validation failed"));
			}
		}
	}

	return errors;
}

HtmxValidator *
htmx_validator_new_custom(
	HtmxValidateFunc  func,
	gpointer          user_data,
	GDestroyNotify    destroy,
	const gchar      *error_message
){
	HtmxValidator *self;
	HtmxValidatorPrivate *priv;

	g_return_val_if_fail(func != NULL, NULL);

	self = g_object_new(HTMX_TYPE_VALIDATOR, NULL);
	priv = htmx_validator_get_instance_private(self);

	priv->func = func;
	priv->func_data = user_data;
	priv->func_destroy = destroy;
	priv->error_message = g_strdup(error_message);

	return self;
}

/*
 * Built-in validation functions
 */

static gboolean
validate_required(const gchar *value, gpointer user_data, gchar **error)
{
	(void)user_data;

	if (value == NULL || *value == '\0') {
		if (error != NULL) {
			*error = g_strdup("This field is required");
		}
		return FALSE;
	}
	return TRUE;
}

typedef struct {
	guint threshold;
	gchar *message;
} LengthData;

static void
length_data_free(gpointer data)
{
	LengthData *ld = (LengthData *)data;

	g_free(ld->message);
	g_slice_free(LengthData, ld);
}

static gboolean
validate_min_length(const gchar *value, gpointer user_data, gchar **error)
{
	LengthData *ld = (LengthData *)user_data;

	if (value == NULL || g_utf8_strlen(value, -1) < (glong)ld->threshold) {
		if (error != NULL) {
			*error = g_strdup_printf(
				ld->message != NULL ? ld->message :
				"Must be at least %u characters", ld->threshold);
		}
		return FALSE;
	}
	return TRUE;
}

static gboolean
validate_max_length(const gchar *value, gpointer user_data, gchar **error)
{
	LengthData *ld = (LengthData *)user_data;

	if (value != NULL && g_utf8_strlen(value, -1) > (glong)ld->threshold) {
		if (error != NULL) {
			*error = g_strdup_printf(
				ld->message != NULL ? ld->message :
				"Must be at most %u characters", ld->threshold);
		}
		return FALSE;
	}
	return TRUE;
}

static gboolean
validate_email(const gchar *value, gpointer user_data, gchar **error)
{
	(void)user_data;

	if (value == NULL || *value == '\0') {
		return TRUE; /* empty is OK — use required() to reject that */
	}

	/* Basic email validation: contains @ with text on both sides */
	{
		const gchar *at = strchr(value, '@');

		if (at == NULL || at == value || *(at + 1) == '\0') {
			if (error != NULL) {
				*error = g_strdup("Invalid email address");
			}
			return FALSE;
		}

		/* Must have a dot after the @ */
		if (strchr(at + 1, '.') == NULL) {
			if (error != NULL) {
				*error = g_strdup("Invalid email address");
			}
			return FALSE;
		}
	}

	return TRUE;
}

static gboolean
validate_url(const gchar *value, gpointer user_data, gchar **error)
{
	(void)user_data;

	if (value == NULL || *value == '\0') {
		return TRUE;
	}

	if (!g_str_has_prefix(value, "http://") &&
	    !g_str_has_prefix(value, "https://")) {
		if (error != NULL) {
			*error = g_strdup("Invalid URL (must start with http:// or https://)");
		}
		return FALSE;
	}

	return TRUE;
}

typedef struct {
	GRegex *regex;
	gchar  *message;
} RegexData;

static void
regex_data_free(gpointer data)
{
	RegexData *rd = (RegexData *)data;

	g_regex_unref(rd->regex);
	g_free(rd->message);
	g_slice_free(RegexData, rd);
}

static gboolean
validate_regex(const gchar *value, gpointer user_data, gchar **error)
{
	RegexData *rd = (RegexData *)user_data;

	if (value == NULL || *value == '\0') {
		return TRUE;
	}

	if (!g_regex_match(rd->regex, value, 0, NULL)) {
		if (error != NULL) {
			*error = g_strdup(rd->message != NULL ? rd->message :
			                  "Value does not match the required pattern");
		}
		return FALSE;
	}

	return TRUE;
}

/*
 * Factory functions
 */

HtmxValidator *
htmx_validator_required(const gchar *error_message)
{
	return htmx_validator_new_custom(
		validate_required, NULL, NULL,
		error_message != NULL ? error_message : "This field is required");
}

HtmxValidator *
htmx_validator_min_length(guint min, const gchar *error_message)
{
	LengthData *ld;

	ld = g_slice_new0(LengthData);
	ld->threshold = min;
	ld->message = g_strdup(error_message);

	return htmx_validator_new_custom(
		validate_min_length, ld, length_data_free,
		error_message != NULL ? error_message : "Value is too short");
}

HtmxValidator *
htmx_validator_max_length(guint max, const gchar *error_message)
{
	LengthData *ld;

	ld = g_slice_new0(LengthData);
	ld->threshold = max;
	ld->message = g_strdup(error_message);

	return htmx_validator_new_custom(
		validate_max_length, ld, length_data_free,
		error_message != NULL ? error_message : "Value is too long");
}

HtmxValidator *
htmx_validator_email(const gchar *error_message)
{
	return htmx_validator_new_custom(
		validate_email, NULL, NULL,
		error_message != NULL ? error_message : "Invalid email address");
}

HtmxValidator *
htmx_validator_url(const gchar *error_message)
{
	return htmx_validator_new_custom(
		validate_url, NULL, NULL,
		error_message != NULL ? error_message : "Invalid URL");
}

HtmxValidator *
htmx_validator_regex(const gchar *pattern, const gchar *error_message)
{
	RegexData *rd;
	GError *err = NULL;

	g_return_val_if_fail(pattern != NULL, NULL);

	rd = g_slice_new0(RegexData);
	rd->regex = g_regex_new(pattern, 0, 0, &err);
	if (rd->regex == NULL) {
		g_warning("Invalid regex pattern '%s': %s", pattern, err->message);
		g_error_free(err);
		g_slice_free(RegexData, rd);
		return NULL;
	}
	rd->message = g_strdup(error_message);

	return htmx_validator_new_custom(
		validate_regex, rd, regex_data_free,
		error_message != NULL ? error_message :
		"Value does not match the required pattern");
}
