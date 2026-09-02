/*
 * test-enums.c - Tests for HTMX enumerations
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include <htmx-glib.h>

static void
test_method_to_string(void)
{
	g_assert_cmpstr(htmx_method_to_string(HTMX_METHOD_GET), ==, "GET");
	g_assert_cmpstr(htmx_method_to_string(HTMX_METHOD_POST), ==, "POST");
	g_assert_cmpstr(htmx_method_to_string(HTMX_METHOD_PUT), ==, "PUT");
	g_assert_cmpstr(htmx_method_to_string(HTMX_METHOD_PATCH), ==, "PATCH");
	g_assert_cmpstr(htmx_method_to_string(HTMX_METHOD_DELETE), ==, "DELETE");
	g_assert_cmpstr(htmx_method_to_string(HTMX_METHOD_HEAD), ==, "HEAD");

	/*
	 * The table is indexed by the enum value, so a member without an
	 * entry is a read off the end of it and htmx_metrics's label is
	 * whatever was next in .rodata.  This is the assertion that says
	 * both grew together.
	 */
	g_assert_cmpstr(htmx_method_to_string(HTMX_METHOD_UNKNOWN), ==,
	                "UNKNOWN");
}

static void
test_method_from_string(void)
{
	g_assert_cmpint(htmx_method_from_string("GET"), ==, HTMX_METHOD_GET);
	g_assert_cmpint(htmx_method_from_string("POST"), ==, HTMX_METHOD_POST);
	g_assert_cmpint(htmx_method_from_string("PUT"), ==, HTMX_METHOD_PUT);
	g_assert_cmpint(htmx_method_from_string("PATCH"), ==, HTMX_METHOD_PATCH);
	g_assert_cmpint(htmx_method_from_string("DELETE"), ==, HTMX_METHOD_DELETE);
	g_assert_cmpint(htmx_method_from_string("HEAD"), ==, HTMX_METHOD_HEAD);

	/*
	 * HTMX_METHOD_UNKNOWN, not -1.  -1 is not a member of HtmxMethod, so
	 * a caller switching over the type was never warned that it had a
	 * case to handle, and a caller comparing against HTMX_METHOD_GET saw
	 * nothing at all.
	 *
	 * The second assertion is the one that matters and the first cannot
	 * make on its own: whatever this answers for a name nobody knows, it
	 * must not be the value a real GET produces.  It was.
	 */
	g_assert_cmpint(htmx_method_from_string("FROB"), ==,
	                HTMX_METHOD_UNKNOWN);
	g_assert_cmpint(htmx_method_from_string("FROB"), !=, HTMX_METHOD_GET);
	g_assert_cmpint(htmx_method_from_string("INVALID"), ==,
	                HTMX_METHOD_UNKNOWN);
}

/*
 * Every member of the registered GEnum has a string, and every string
 * comes back as the member it was written for.  Walked from the type
 * rather than listed here, because a hand-written list of an enum's
 * values is the thing that drifts: HTMX_METHOD_HEAD and
 * HTMX_METHOD_UNKNOWN were both added to the enum, to method_strings and
 * to the GEnumValue table, and nothing but this would notice one of the
 * three being missed.
 */
static void
test_method_round_trips_every_member(void)
{
	g_autoptr(GEnumClass) klass = g_type_class_ref(HTMX_TYPE_METHOD);
	guint i;

	g_assert_cmpuint(klass->n_values, >, 0);

	for (i = 0; i < klass->n_values; i++) {
		HtmxMethod method = (HtmxMethod)klass->values[i].value;
		const gchar *string = htmx_method_to_string(method);

		g_assert_nonnull(string);
		g_assert_cmpint(htmx_method_from_string(string), ==, method);
	}
}

static void
test_swap_style_to_string(void)
{
	g_assert_cmpstr(htmx_swap_style_to_string(HTMX_SWAP_INNER_HTML), ==, "innerHTML");
	g_assert_cmpstr(htmx_swap_style_to_string(HTMX_SWAP_OUTER_HTML), ==, "outerHTML");
	g_assert_cmpstr(htmx_swap_style_to_string(HTMX_SWAP_BEFORE_BEGIN), ==, "beforebegin");
	g_assert_cmpstr(htmx_swap_style_to_string(HTMX_SWAP_AFTER_BEGIN), ==, "afterbegin");
	g_assert_cmpstr(htmx_swap_style_to_string(HTMX_SWAP_BEFORE_END), ==, "beforeend");
	g_assert_cmpstr(htmx_swap_style_to_string(HTMX_SWAP_AFTER_END), ==, "afterend");
	g_assert_cmpstr(htmx_swap_style_to_string(HTMX_SWAP_DELETE), ==, "delete");
	g_assert_cmpstr(htmx_swap_style_to_string(HTMX_SWAP_NONE), ==, "none");
}

static void
test_swap_style_from_string(void)
{
	g_assert_cmpint(htmx_swap_style_from_string("innerHTML"), ==, HTMX_SWAP_INNER_HTML);
	g_assert_cmpint(htmx_swap_style_from_string("outerHTML"), ==, HTMX_SWAP_OUTER_HTML);
	g_assert_cmpint(htmx_swap_style_from_string("beforebegin"), ==, HTMX_SWAP_BEFORE_BEGIN);
	g_assert_cmpint((gint)htmx_swap_style_from_string("invalid"), ==, (gint)-1);
}

static void
test_input_type_to_string(void)
{
	g_assert_cmpstr(htmx_input_type_to_string(HTMX_INPUT_TEXT), ==, "text");
	g_assert_cmpstr(htmx_input_type_to_string(HTMX_INPUT_PASSWORD), ==, "password");
	g_assert_cmpstr(htmx_input_type_to_string(HTMX_INPUT_EMAIL), ==, "email");
	g_assert_cmpstr(htmx_input_type_to_string(HTMX_INPUT_CHECKBOX), ==, "checkbox");
	g_assert_cmpstr(htmx_input_type_to_string(HTMX_INPUT_SUBMIT), ==, "submit");
}

static void
test_method_get_type(void)
{
	GType type = htmx_method_get_type();
	g_assert_true(g_type_is_a(type, G_TYPE_ENUM));
}

static void
test_swap_style_get_type(void)
{
	GType type = htmx_swap_style_get_type();
	g_assert_true(g_type_is_a(type, G_TYPE_ENUM));
}

int
main(int argc, char *argv[])
{
	g_test_init(&argc, &argv, NULL);

	g_test_add_func("/enums/method/to_string", test_method_to_string);
	g_test_add_func("/enums/method/from_string", test_method_from_string);
	g_test_add_func("/enums/method/round_trips_every_member",
	                test_method_round_trips_every_member);
	g_test_add_func("/enums/method/get_type", test_method_get_type);
	g_test_add_func("/enums/swap_style/to_string", test_swap_style_to_string);
	g_test_add_func("/enums/swap_style/from_string", test_swap_style_from_string);
	g_test_add_func("/enums/swap_style/get_type", test_swap_style_get_type);
	g_test_add_func("/enums/input_type/to_string", test_input_type_to_string);

	return g_test_run();
}
