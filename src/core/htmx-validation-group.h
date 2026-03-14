/*
 * htmx-validation-group.h - Cross-field form validation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * HtmxValidationGroup validates entire forms by running per-field
 * validators plus cross-field rules. Per-field validators use the
 * existing #HtmxValidator type; cross-field rules use a callback
 * that receives the complete form data.
 */

#ifndef HTMX_VALIDATION_GROUP_H
#define HTMX_VALIDATION_GROUP_H

#include <glib-object.h>
#include "htmx-validator.h"

G_BEGIN_DECLS

#define HTMX_TYPE_VALIDATION_GROUP (htmx_validation_group_get_type())
G_DECLARE_FINAL_TYPE(HtmxValidationGroup, htmx_validation_group, HTMX, VALIDATION_GROUP, GObject)

/**
 * HtmxCrossFieldFunc:
 * @form_data: (element-type utf8 utf8): the complete form data
 * @user_data: (closure): user data
 * @error: (out) (optional) (transfer full): error message on failure
 *
 * Cross-field validation function. Receives the entire form and
 * returns %FALSE with an error message if validation fails.
 *
 * Returns: %TRUE if the cross-field rule passes
 */
typedef gboolean (*HtmxCrossFieldFunc)(GHashTable   *form_data,
                                        gpointer      user_data,
                                        gchar       **error);

/**
 * htmx_validation_group_new:
 *
 * Creates a new empty validation group.
 *
 * Returns: (transfer full): a new #HtmxValidationGroup
 */
HtmxValidationGroup *
htmx_validation_group_new(void);

/**
 * htmx_validation_group_add_validator:
 * @self: an #HtmxValidationGroup
 * @field: the form field name
 * @validator: (transfer none): a validator for this field
 *
 * Adds a per-field validator. Multiple validators can be added
 * for the same field — they all run, and all errors are collected.
 */
void
htmx_validation_group_add_validator(HtmxValidationGroup *self,
                                     const gchar         *field,
                                     HtmxValidator       *validator);

/**
 * htmx_validation_group_add_cross_field_rule:
 * @self: an #HtmxValidationGroup
 * @name: a descriptive name for this rule (used as error key)
 * @func: the validation function
 * @user_data: (nullable) (closure func): user data
 * @destroy: (nullable): destroy function for @user_data
 *
 * Adds a cross-field validation rule that receives the complete
 * form data and can validate relationships between fields.
 */
void
htmx_validation_group_add_cross_field_rule(HtmxValidationGroup *self,
                                            const gchar         *name,
                                            HtmxCrossFieldFunc   func,
                                            gpointer             user_data,
                                            GDestroyNotify       destroy);

/**
 * htmx_validation_group_validate:
 * @self: an #HtmxValidationGroup
 * @form_data: (element-type utf8 utf8): the form data to validate
 *
 * Validates all fields and cross-field rules. Returns a hash table
 * of field/rule names mapped to error messages for all failures.
 * Returns an empty table if validation passes.
 *
 * Returns: (transfer full) (element-type utf8 utf8): error map
 */
GHashTable *
htmx_validation_group_validate(HtmxValidationGroup *self,
                                GHashTable          *form_data);

/**
 * htmx_validation_group_is_valid:
 * @self: an #HtmxValidationGroup
 * @form_data: (element-type utf8 utf8): the form data to validate
 *
 * Convenience method — returns %TRUE if validate() returns no errors.
 *
 * Returns: %TRUE if the form is valid
 */
gboolean
htmx_validation_group_is_valid(HtmxValidationGroup *self,
                                GHashTable          *form_data);

G_END_DECLS

#endif /* HTMX_VALIDATION_GROUP_H */
