/*
 * htmx-validator.h - Form validation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * HtmxValidator is a derivable type for validating form field values.
 * Built-in validators are provided for common patterns (required,
 * length, email, URL, regex). Custom validators can be created by
 * subclassing or using htmx_validator_new_custom().
 */

#ifndef HTMX_VALIDATOR_H
#define HTMX_VALIDATOR_H

#include <glib-object.h>

G_BEGIN_DECLS

#define HTMX_TYPE_VALIDATOR (htmx_validator_get_type())
G_DECLARE_DERIVABLE_TYPE(HtmxValidator, htmx_validator, HTMX, VALIDATOR, GObject)

/**
 * HtmxValidateFunc:
 * @value: the value to validate
 * @user_data: (closure): user data
 * @error: (out) (optional) (transfer full): error message on failure
 *
 * Custom validation function.
 *
 * Returns: %TRUE if the value is valid
 */
typedef gboolean (*HtmxValidateFunc)(const gchar *value,
                                     gpointer     user_data,
                                     gchar      **error);

/**
 * HtmxValidatorClass:
 * @parent_class: the parent class
 * @validate: virtual method to validate a value
 *
 * Class structure for #HtmxValidator.
 * Override @validate to implement custom validation logic.
 */
struct _HtmxValidatorClass {
	GObjectClass parent_class;

	gboolean (*validate)(HtmxValidator  *self,
	                     const gchar    *value,
	                     gchar         **error);

	/* Padding for future expansion */
	gpointer padding[8];
};

/**
 * htmx_validator_validate:
 * @self: an #HtmxValidator
 * @value: (nullable): the value to validate
 * @error: (out) (optional) (transfer full): error message on failure
 *
 * Validates a value. Sets @error to a human-readable message
 * on validation failure.
 *
 * Returns: %TRUE if the value is valid
 */
gboolean
htmx_validator_validate(
	HtmxValidator  *self,
	const gchar    *value,
	gchar         **error
);

/**
 * htmx_validator_validate_form:
 * @validators: (element-type utf8 HtmxValidator): field name -> validator
 * @form_data: (element-type utf8 utf8): field name -> value
 *
 * Validates all fields in @form_data against the corresponding
 * validators. Returns a table of field names that failed validation
 * mapped to their error messages.
 *
 * Returns: (transfer full) (element-type utf8 utf8): errors, or empty if valid
 */
GHashTable *
htmx_validator_validate_form(
	GHashTable *validators,
	GHashTable *form_data
);

/*
 * Built-in validator constructors
 */

/**
 * htmx_validator_new_custom:
 * @func: the validation function
 * @user_data: (nullable): user data for @func
 * @destroy: (nullable): destroy function for @user_data
 * @error_message: default error message
 *
 * Creates a validator from a custom function.
 *
 * Returns: (transfer full): a new #HtmxValidator
 */
HtmxValidator *
htmx_validator_new_custom(
	HtmxValidateFunc  func,
	gpointer          user_data,
	GDestroyNotify    destroy,
	const gchar      *error_message
);

/**
 * htmx_validator_required:
 * @error_message: (nullable): custom error (default: "This field is required")
 *
 * Creates a validator that requires a non-empty value.
 *
 * Returns: (transfer full): a new #HtmxValidator
 */
HtmxValidator *
htmx_validator_required(const gchar *error_message);

/**
 * htmx_validator_min_length:
 * @min: minimum length
 * @error_message: (nullable): custom error message
 *
 * Creates a validator that requires a minimum string length.
 *
 * Returns: (transfer full): a new #HtmxValidator
 */
HtmxValidator *
htmx_validator_min_length(guint min, const gchar *error_message);

/**
 * htmx_validator_max_length:
 * @max: maximum length
 * @error_message: (nullable): custom error message
 *
 * Creates a validator that enforces a maximum string length.
 *
 * Returns: (transfer full): a new #HtmxValidator
 */
HtmxValidator *
htmx_validator_max_length(guint max, const gchar *error_message);

/**
 * htmx_validator_email:
 * @error_message: (nullable): custom error message
 *
 * Creates a validator that checks for a valid email format.
 *
 * Returns: (transfer full): a new #HtmxValidator
 */
HtmxValidator *
htmx_validator_email(const gchar *error_message);

/**
 * htmx_validator_url:
 * @error_message: (nullable): custom error message
 *
 * Creates a validator that checks for a valid URL format.
 *
 * Returns: (transfer full): a new #HtmxValidator
 */
HtmxValidator *
htmx_validator_url(const gchar *error_message);

/**
 * htmx_validator_regex:
 * @pattern: the regex pattern
 * @error_message: (nullable): custom error message
 *
 * Creates a validator that matches values against a regex pattern.
 *
 * Returns: (transfer full): a new #HtmxValidator
 */
HtmxValidator *
htmx_validator_regex(const gchar *pattern, const gchar *error_message);

G_END_DECLS

#endif /* HTMX_VALIDATOR_H */
