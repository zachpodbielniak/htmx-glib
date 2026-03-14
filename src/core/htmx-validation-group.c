/*
 * htmx-validation-group.c - Cross-field form validation implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-validation-group.h"

typedef struct {
	gchar              *name;
	HtmxCrossFieldFunc  func;
	gpointer            user_data;
	GDestroyNotify      destroy;
} CrossFieldRule;

static void
cross_field_rule_free(CrossFieldRule *rule)
{
	if (rule == NULL)
		return;

	g_free(rule->name);
	if (rule->destroy != NULL && rule->user_data != NULL)
		rule->destroy(rule->user_data);
	g_slice_free(CrossFieldRule, rule);
}

struct _HtmxValidationGroup {
	GObject     parent_instance;
	GHashTable *validators;       /* gchar* -> GPtrArray of HtmxValidator* */
	GList      *cross_field_rules; /* list of CrossFieldRule* */
};

G_DEFINE_FINAL_TYPE(HtmxValidationGroup, htmx_validation_group, G_TYPE_OBJECT)

static void
htmx_validation_group_finalize(GObject *object)
{
	HtmxValidationGroup *self = HTMX_VALIDATION_GROUP(object);

	g_hash_table_unref(self->validators);
	g_list_free_full(self->cross_field_rules, (GDestroyNotify)cross_field_rule_free);

	G_OBJECT_CLASS(htmx_validation_group_parent_class)->finalize(object);
}

static void
htmx_validation_group_class_init(HtmxValidationGroupClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);
	object_class->finalize = htmx_validation_group_finalize;
}

static void
htmx_validation_group_init(HtmxValidationGroup *self)
{
	self->validators = g_hash_table_new_full(
		g_str_hash, g_str_equal,
		g_free,
		(GDestroyNotify)g_ptr_array_unref);
	self->cross_field_rules = NULL;
}

HtmxValidationGroup *
htmx_validation_group_new(void)
{
	return g_object_new(HTMX_TYPE_VALIDATION_GROUP, NULL);
}

void
htmx_validation_group_add_validator(HtmxValidationGroup *self,
                                     const gchar         *field,
                                     HtmxValidator       *validator)
{
	GPtrArray *field_validators;

	g_return_if_fail(HTMX_IS_VALIDATION_GROUP(self));
	g_return_if_fail(field != NULL);
	g_return_if_fail(HTMX_IS_VALIDATOR(validator));

	field_validators = g_hash_table_lookup(self->validators, field);

	if (field_validators == NULL) {
		field_validators = g_ptr_array_new_with_free_func(g_object_unref);
		g_hash_table_insert(self->validators, g_strdup(field), field_validators);
	}

	g_ptr_array_add(field_validators, g_object_ref(validator));
}

void
htmx_validation_group_add_cross_field_rule(HtmxValidationGroup *self,
                                            const gchar         *name,
                                            HtmxCrossFieldFunc   func,
                                            gpointer             user_data,
                                            GDestroyNotify       destroy)
{
	CrossFieldRule *rule;

	g_return_if_fail(HTMX_IS_VALIDATION_GROUP(self));
	g_return_if_fail(name != NULL);
	g_return_if_fail(func != NULL);

	rule = g_slice_new0(CrossFieldRule);
	rule->name = g_strdup(name);
	rule->func = func;
	rule->user_data = user_data;
	rule->destroy = destroy;

	self->cross_field_rules = g_list_append(self->cross_field_rules, rule);
}

GHashTable *
htmx_validation_group_validate(HtmxValidationGroup *self,
                                GHashTable          *form_data)
{
	GHashTable *errors;
	GHashTableIter iter;
	gpointer key;
	gpointer value;
	GList *l;

	g_return_val_if_fail(HTMX_IS_VALIDATION_GROUP(self), NULL);
	g_return_val_if_fail(form_data != NULL, NULL);

	errors = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);

	/* Run per-field validators */
	g_hash_table_iter_init(&iter, self->validators);
	while (g_hash_table_iter_next(&iter, &key, &value)) {
		const gchar *field = (const gchar *)key;
		GPtrArray *field_validators = (GPtrArray *)value;
		const gchar *field_value;
		guint i;

		field_value = g_hash_table_lookup(form_data, field);

		for (i = 0; i < field_validators->len; i++) {
			HtmxValidator *validator = g_ptr_array_index(field_validators, i);
			gchar *error_msg = NULL;

			if (!htmx_validator_validate(validator, field_value, &error_msg)) {
				/* Only keep the first error per field */
				if (g_hash_table_lookup(errors, field) == NULL) {
					g_hash_table_insert(errors, g_strdup(field),
					                    error_msg != NULL ? error_msg : g_strdup("Validation failed"));
				} else {
					g_free(error_msg);
				}
				break;
			}
			g_free(error_msg);
		}
	}

	/* Run cross-field rules */
	for (l = self->cross_field_rules; l != NULL; l = l->next) {
		CrossFieldRule *rule = (CrossFieldRule *)l->data;
		gchar *error_msg = NULL;

		if (!rule->func(form_data, rule->user_data, &error_msg)) {
			g_hash_table_insert(errors, g_strdup(rule->name),
			                    error_msg != NULL ? error_msg : g_strdup("Validation failed"));
		} else {
			g_free(error_msg);
		}
	}

	return errors;
}

gboolean
htmx_validation_group_is_valid(HtmxValidationGroup *self,
                                GHashTable          *form_data)
{
	GHashTable *errors;
	gboolean valid;

	g_return_val_if_fail(HTMX_IS_VALIDATION_GROUP(self), FALSE);
	g_return_val_if_fail(form_data != NULL, FALSE);

	errors = htmx_validation_group_validate(self, form_data);
	valid = (g_hash_table_size(errors) == 0);
	g_hash_table_unref(errors);

	return valid;
}
