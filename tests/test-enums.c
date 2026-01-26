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
}

static void
test_method_from_string(void)
{
	g_assert_cmpint(htmx_method_from_string("GET"), ==, HTMX_METHOD_GET);
	g_assert_cmpint(htmx_method_from_string("POST"), ==, HTMX_METHOD_POST);
	g_assert_cmpint(htmx_method_from_string("PUT"), ==, HTMX_METHOD_PUT);
	g_assert_cmpint(htmx_method_from_string("PATCH"), ==, HTMX_METHOD_PATCH);
	g_assert_cmpint(htmx_method_from_string("DELETE"), ==, HTMX_METHOD_DELETE);
	g_assert_cmpint((gint)htmx_method_from_string("INVALID"), ==, (gint)-1);
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
	g_test_add_func("/enums/method/get_type", test_method_get_type);
	g_test_add_func("/enums/swap_style/to_string", test_swap_style_to_string);
	g_test_add_func("/enums/swap_style/from_string", test_swap_style_from_string);
	g_test_add_func("/enums/swap_style/get_type", test_swap_style_get_type);
	g_test_add_func("/enums/input_type/to_string", test_input_type_to_string);

	return g_test_run();
}
