/*
 * test-elements.c - Tests for explicit HTML element types
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include <htmx-glib.h>
#include <unistd.h>

/*
 * Div tests
 */
static void
test_div_new(void)
{
	g_autoptr(HtmxDiv) div = htmx_div_new();

	g_assert_nonnull(div);
	g_assert_cmpstr(htmx_node_get_tag_name(HTMX_NODE(div)), ==, "div");
}

static void
test_div_render(void)
{
	g_autoptr(HtmxDiv) div = htmx_div_new();
	g_autofree gchar *html = NULL;

	htmx_node_set_text_content(HTMX_NODE(div), "Hello");
	html = htmx_element_render(HTMX_ELEMENT(div));

	g_assert_cmpstr(html, ==, "<div>Hello</div>");
}

/*
 * Span tests
 */
static void
test_span_new(void)
{
	g_autoptr(HtmxSpan) span = htmx_span_new();

	g_assert_nonnull(span);
	g_assert_cmpstr(htmx_node_get_tag_name(HTMX_NODE(span)), ==, "span");
}

/*
 * Paragraph tests
 */
static void
test_p_new(void)
{
	g_autoptr(HtmxP) p = htmx_p_new();

	g_assert_nonnull(p);
	g_assert_cmpstr(htmx_node_get_tag_name(HTMX_NODE(p)), ==, "p");
}

/*
 * Heading tests
 */
static void
test_heading_new(void)
{
	g_autoptr(HtmxHeading) h1 = htmx_heading_new(1);
	g_autoptr(HtmxHeading) h2 = htmx_heading_new(2);
	g_autoptr(HtmxHeading) h6 = htmx_heading_new(6);

	g_assert_nonnull(h1);
	g_assert_nonnull(h2);
	g_assert_nonnull(h6);

	g_assert_cmpstr(htmx_node_get_tag_name(HTMX_NODE(h1)), ==, "h1");
	g_assert_cmpstr(htmx_node_get_tag_name(HTMX_NODE(h2)), ==, "h2");
	g_assert_cmpstr(htmx_node_get_tag_name(HTMX_NODE(h6)), ==, "h6");

	g_assert_cmpuint(htmx_heading_get_level(h1), ==, 1);
	g_assert_cmpuint(htmx_heading_get_level(h2), ==, 2);
	g_assert_cmpuint(htmx_heading_get_level(h6), ==, 6);
}

static void
test_heading_convenience(void)
{
	g_autoptr(HtmxHeading) h1 = htmx_h1_new();
	g_autoptr(HtmxHeading) h2 = htmx_h2_new();
	g_autoptr(HtmxHeading) h3 = htmx_h3_new();
	g_autoptr(HtmxHeading) h4 = htmx_h4_new();
	g_autoptr(HtmxHeading) h5 = htmx_h5_new();
	g_autoptr(HtmxHeading) h6 = htmx_h6_new();

	g_assert_cmpuint(htmx_heading_get_level(h1), ==, 1);
	g_assert_cmpuint(htmx_heading_get_level(h2), ==, 2);
	g_assert_cmpuint(htmx_heading_get_level(h3), ==, 3);
	g_assert_cmpuint(htmx_heading_get_level(h4), ==, 4);
	g_assert_cmpuint(htmx_heading_get_level(h5), ==, 5);
	g_assert_cmpuint(htmx_heading_get_level(h6), ==, 6);
}

static void
test_heading_render(void)
{
	g_autoptr(HtmxHeading) h1 = htmx_h1_new();
	g_autofree gchar *html = NULL;

	htmx_node_set_text_content(HTMX_NODE(h1), "Title");
	html = htmx_element_render(HTMX_ELEMENT(h1));

	g_assert_cmpstr(html, ==, "<h1>Title</h1>");
}

/*
 * Anchor tests
 */
static void
test_a_new(void)
{
	g_autoptr(HtmxA) a = htmx_a_new();

	g_assert_nonnull(a);
	g_assert_cmpstr(htmx_node_get_tag_name(HTMX_NODE(a)), ==, "a");
}

static void
test_a_with_href(void)
{
	g_autoptr(HtmxA) a = htmx_a_new_with_href("https://example.com");
	g_autofree gchar *html = NULL;

	htmx_node_set_text_content(HTMX_NODE(a), "Link");
	html = htmx_element_render(HTMX_ELEMENT(a));

	g_assert_true(g_strstr_len(html, -1, "href=\"https://example.com\"") != NULL);
	g_assert_true(g_strstr_len(html, -1, ">Link</a>") != NULL);
}

/*
 * Button tests
 */
static void
test_button_new(void)
{
	g_autoptr(HtmxButton) btn = htmx_button_new();

	g_assert_nonnull(btn);
	g_assert_cmpstr(htmx_node_get_tag_name(HTMX_NODE(btn)), ==, "button");
}

static void
test_button_with_label(void)
{
	g_autoptr(HtmxButton) btn = htmx_button_new_with_label("Click me");
	g_autofree gchar *html = NULL;

	html = htmx_element_render(HTMX_ELEMENT(btn));

	g_assert_true(g_strstr_len(html, -1, "type=\"button\"") != NULL);
	g_assert_true(g_strstr_len(html, -1, ">Click me</button>") != NULL);
}

static void
test_button_submit(void)
{
	g_autoptr(HtmxButton) btn = htmx_button_new_submit();
	g_autofree gchar *html = NULL;

	html = htmx_element_render(HTMX_ELEMENT(btn));

	g_assert_true(g_strstr_len(html, -1, "type=\"submit\"") != NULL);
}

/*
 * Label tests
 */
static void
test_label_new(void)
{
	g_autoptr(HtmxLabel) label = htmx_label_new();

	g_assert_nonnull(label);
	g_assert_cmpstr(htmx_node_get_tag_name(HTMX_NODE(label)), ==, "label");
}

static void
test_label_for(void)
{
	g_autoptr(HtmxLabel) label = htmx_label_new_for("username");
	g_autofree gchar *html = NULL;

	htmx_node_set_text_content(HTMX_NODE(label), "Username:");
	html = htmx_element_render(HTMX_ELEMENT(label));

	g_assert_true(g_strstr_len(html, -1, "for=\"username\"") != NULL);
	g_assert_true(g_strstr_len(html, -1, ">Username:</label>") != NULL);
}

/*
 * List tests
 */
static void
test_ul_new(void)
{
	g_autoptr(HtmxUl) ul = htmx_ul_new();

	g_assert_nonnull(ul);
	g_assert_cmpstr(htmx_node_get_tag_name(HTMX_NODE(ul)), ==, "ul");
}

static void
test_ol_new(void)
{
	g_autoptr(HtmxOl) ol = htmx_ol_new();

	g_assert_nonnull(ol);
	g_assert_cmpstr(htmx_node_get_tag_name(HTMX_NODE(ol)), ==, "ol");
}

static void
test_li_new(void)
{
	g_autoptr(HtmxLi) li = htmx_li_new();

	g_assert_nonnull(li);
	g_assert_cmpstr(htmx_node_get_tag_name(HTMX_NODE(li)), ==, "li");
}

static void
test_list_render(void)
{
	g_autoptr(HtmxUl) ul = htmx_ul_new();
	g_autoptr(HtmxLi) li1 = htmx_li_new_with_text("Item 1");
	g_autoptr(HtmxLi) li2 = htmx_li_new_with_text("Item 2");
	g_autofree gchar *html = NULL;

	/* Add children - note: add_child takes ownership so we ref first */
	htmx_node_add_child(HTMX_NODE(ul), HTMX_NODE(g_object_ref(li1)));
	htmx_node_add_child(HTMX_NODE(ul), HTMX_NODE(g_object_ref(li2)));

	html = htmx_element_render(HTMX_ELEMENT(ul));

	g_assert_cmpstr(html, ==, "<ul><li>Item 1</li><li>Item 2</li></ul>");
}

/*
 * Img tests
 */
static void
test_img_new(void)
{
	g_autoptr(HtmxImg) img = htmx_img_new();

	g_assert_nonnull(img);
	g_assert_cmpstr(htmx_node_get_tag_name(HTMX_NODE(img)), ==, "img");
	g_assert_true(htmx_node_is_void_element(HTMX_NODE(img)));
}

static void
test_img_with_src(void)
{
	g_autoptr(HtmxImg) img = htmx_img_new_with_src("/image.png", "An image");
	g_autofree gchar *html = NULL;

	html = htmx_element_render(HTMX_ELEMENT(img));

	g_assert_true(g_strstr_len(html, -1, "src=\"/image.png\"") != NULL);
	g_assert_true(g_strstr_len(html, -1, "alt=\"An image\"") != NULL);
	g_assert_true(g_strstr_len(html, -1, "/>") != NULL);
}

/*
 * Textarea tests
 */
static void
test_textarea_new(void)
{
	g_autoptr(HtmxTextarea) textarea = htmx_textarea_new();

	g_assert_nonnull(textarea);
	g_assert_cmpstr(htmx_node_get_tag_name(HTMX_NODE(textarea)), ==, "textarea");
}

static void
test_textarea_with_name(void)
{
	g_autoptr(HtmxTextarea) textarea = htmx_textarea_new_with_name("comment");
	g_autofree gchar *html = NULL;

	htmx_textarea_set_rows(textarea, 5);
	htmx_textarea_set_cols(textarea, 40);
	html = htmx_element_render(HTMX_ELEMENT(textarea));

	g_assert_true(g_strstr_len(html, -1, "name=\"comment\"") != NULL);
	g_assert_true(g_strstr_len(html, -1, "rows=\"5\"") != NULL);
	g_assert_true(g_strstr_len(html, -1, "cols=\"40\"") != NULL);
}

/*
 * Select tests
 */
static void
test_select_new(void)
{
	g_autoptr(HtmxSelect) select = htmx_select_new();

	g_assert_nonnull(select);
	g_assert_cmpstr(htmx_node_get_tag_name(HTMX_NODE(select)), ==, "select");
}

static void
test_option_new(void)
{
	g_autoptr(HtmxOption) option = htmx_option_new();

	g_assert_nonnull(option);
	g_assert_cmpstr(htmx_node_get_tag_name(HTMX_NODE(option)), ==, "option");
}

static void
test_select_with_options(void)
{
	g_autoptr(HtmxSelect) select = htmx_select_new_with_name("color");
	g_autoptr(HtmxOption) opt1 = htmx_option_new_with_value("red", "Red");
	g_autoptr(HtmxOption) opt2 = htmx_option_new_with_value("blue", "Blue");
	g_autofree gchar *html = NULL;

	htmx_option_set_selected(opt1, TRUE);

	htmx_node_add_child(HTMX_NODE(select), HTMX_NODE(g_object_ref(opt1)));
	htmx_node_add_child(HTMX_NODE(select), HTMX_NODE(g_object_ref(opt2)));

	html = htmx_element_render(HTMX_ELEMENT(select));

	g_assert_true(g_strstr_len(html, -1, "name=\"color\"") != NULL);
	g_assert_true(g_strstr_len(html, -1, "value=\"red\"") != NULL);
	g_assert_true(g_strstr_len(html, -1, "selected=\"selected\"") != NULL);
	g_assert_true(g_strstr_len(html, -1, ">Red</option>") != NULL);
	g_assert_true(g_strstr_len(html, -1, ">Blue</option>") != NULL);
}

/*
 * Form and Input tag name tests
 */
static void
test_form_tag_name(void)
{
	g_autoptr(HtmxForm) form = htmx_form_new();

	g_assert_nonnull(form);
	g_assert_cmpstr(htmx_node_get_tag_name(HTMX_NODE(form)), ==, "form");
}

static void
test_input_tag_name(void)
{
	g_autoptr(HtmxInput) input = htmx_input_new(HTMX_INPUT_TEXT);

	g_assert_nonnull(input);
	g_assert_cmpstr(htmx_node_get_tag_name(HTMX_NODE(input)), ==, "input");
}

/*
 * Button type enum tests
 */
static void
test_button_type_to_string(void)
{
	g_assert_cmpstr(htmx_button_type_to_string(HTMX_BUTTON_BUTTON), ==, "button");
	g_assert_cmpstr(htmx_button_type_to_string(HTMX_BUTTON_SUBMIT), ==, "submit");
	g_assert_cmpstr(htmx_button_type_to_string(HTMX_BUTTON_RESET), ==, "reset");
}

/*
 * Progress tests
 */
static void
test_progress_new(void)
{
	g_autoptr(HtmxProgress) progress = htmx_progress_new();

	g_assert_nonnull(progress);
	g_assert_cmpstr(htmx_node_get_tag_name(HTMX_NODE(progress)), ==, "progress");
}

static void
test_progress_with_value(void)
{
	g_autoptr(HtmxProgress) progress = htmx_progress_new_with_value(70, 100);
	g_autofree gchar *html = NULL;

	html = htmx_element_render(HTMX_ELEMENT(progress));

	g_assert_nonnull(g_strstr_len(html, -1, "value=\"70\""));
	g_assert_nonnull(g_strstr_len(html, -1, "max=\"100\""));
}

/*
 * Meter tests
 */
static void
test_meter_new(void)
{
	g_autoptr(HtmxMeter) meter = htmx_meter_new();

	g_assert_nonnull(meter);
	g_assert_cmpstr(htmx_node_get_tag_name(HTMX_NODE(meter)), ==, "meter");
}

static void
test_meter_attributes(void)
{
	g_autoptr(HtmxMeter) meter = htmx_meter_new();
	g_autofree gchar *html = NULL;

	htmx_meter_set_value(meter, 0.6);
	htmx_meter_set_min(meter, 0);
	htmx_meter_set_max(meter, 1);
	htmx_meter_set_low(meter, 0.25);
	htmx_meter_set_high(meter, 0.75);
	htmx_meter_set_optimum(meter, 0.5);

	html = htmx_element_render(HTMX_ELEMENT(meter));

	g_assert_nonnull(g_strstr_len(html, -1, "value=\"0.6\""));
	g_assert_nonnull(g_strstr_len(html, -1, "min=\"0\""));
	g_assert_nonnull(g_strstr_len(html, -1, "max=\"1\""));
	g_assert_nonnull(g_strstr_len(html, -1, "low=\"0.25\""));
	g_assert_nonnull(g_strstr_len(html, -1, "high=\"0.75\""));
	g_assert_nonnull(g_strstr_len(html, -1, "optimum=\"0.5\""));
}

/*
 * Del tests
 */
static void
test_del_new(void)
{
	g_autoptr(HtmxDel) del = htmx_del_new();

	g_assert_nonnull(del);
	g_assert_cmpstr(htmx_node_get_tag_name(HTMX_NODE(del)), ==, "del");
}

static void
test_del_with_attributes(void)
{
	g_autoptr(HtmxDel) del = htmx_del_new_with_text("old text");
	g_autofree gchar *html = NULL;

	htmx_del_set_cite(del, "https://example.com/change");
	htmx_del_set_datetime(del, "2026-01-01");

	html = htmx_element_render(HTMX_ELEMENT(del));

	g_assert_nonnull(g_strstr_len(html, -1, ">old text</del>"));
	g_assert_nonnull(g_strstr_len(html, -1, "cite=\"https://example.com/change\""));
	g_assert_nonnull(g_strstr_len(html, -1, "datetime=\"2026-01-01\""));
}

/*
 * Ins tests
 */
static void
test_ins_new(void)
{
	g_autoptr(HtmxIns) ins = htmx_ins_new();

	g_assert_nonnull(ins);
	g_assert_cmpstr(htmx_node_get_tag_name(HTMX_NODE(ins)), ==, "ins");
}

static void
test_ins_with_attributes(void)
{
	g_autoptr(HtmxIns) ins = htmx_ins_new_with_text("new text");
	g_autofree gchar *html = NULL;

	htmx_ins_set_cite(ins, "https://example.com/change");
	htmx_ins_set_datetime(ins, "2026-01-01");

	html = htmx_element_render(HTMX_ELEMENT(ins));

	g_assert_nonnull(g_strstr_len(html, -1, ">new text</ins>"));
	g_assert_nonnull(g_strstr_len(html, -1, "cite=\"https://example.com/change\""));
	g_assert_nonnull(g_strstr_len(html, -1, "datetime=\"2026-01-01\""));
}

/*
 * Ruby tests
 */
static void
test_ruby_new(void)
{
	g_autoptr(HtmxRuby) ruby = htmx_ruby_new();

	g_assert_nonnull(ruby);
	g_assert_cmpstr(htmx_node_get_tag_name(HTMX_NODE(ruby)), ==, "ruby");
}

static void
test_rt_new(void)
{
	g_autoptr(HtmxRt) rt = htmx_rt_new();

	g_assert_nonnull(rt);
	g_assert_cmpstr(htmx_node_get_tag_name(HTMX_NODE(rt)), ==, "rt");
}

static void
test_rp_new(void)
{
	g_autoptr(HtmxRp) rp = htmx_rp_new();

	g_assert_nonnull(rp);
	g_assert_cmpstr(htmx_node_get_tag_name(HTMX_NODE(rp)), ==, "rp");
}

static void
test_ruby_structure(void)
{
	g_autoptr(HtmxRuby) ruby = htmx_ruby_new();
	g_autoptr(HtmxRp) rp1 = htmx_rp_new_with_text("(");
	g_autoptr(HtmxRt) rt = htmx_rt_new_with_text("kan");
	g_autoptr(HtmxRp) rp2 = htmx_rp_new_with_text(")");
	g_autofree gchar *html = NULL;

	htmx_node_set_text_content(HTMX_NODE(ruby), "");
	htmx_node_add_child(HTMX_NODE(ruby), HTMX_NODE(g_object_ref(rp1)));
	htmx_node_add_child(HTMX_NODE(ruby), HTMX_NODE(g_object_ref(rt)));
	htmx_node_add_child(HTMX_NODE(ruby), HTMX_NODE(g_object_ref(rp2)));

	html = htmx_element_render(HTMX_ELEMENT(ruby));

	g_assert_nonnull(g_strstr_len(html, -1, "<ruby>"));
	g_assert_nonnull(g_strstr_len(html, -1, "<rp>(</rp>"));
	g_assert_nonnull(g_strstr_len(html, -1, "<rt>kan</rt>"));
	g_assert_nonnull(g_strstr_len(html, -1, "<rp>)</rp>"));
}

/*
 * Dfn tests
 */
static void
test_dfn_new(void)
{
	g_autoptr(HtmxDfn) dfn = htmx_dfn_new();

	g_assert_nonnull(dfn);
	g_assert_cmpstr(htmx_node_get_tag_name(HTMX_NODE(dfn)), ==, "dfn");
}

static void
test_dfn_with_text(void)
{
	g_autoptr(HtmxDfn) dfn = htmx_dfn_new_with_text("HTML");
	g_autofree gchar *html = NULL;

	html = htmx_element_render(HTMX_ELEMENT(dfn));

	g_assert_cmpstr(html, ==, "<dfn>HTML</dfn>");
}

/*
 * Data tests
 */
static void
test_data_new(void)
{
	g_autoptr(HtmxData) data = htmx_data_new();

	g_assert_nonnull(data);
	g_assert_cmpstr(htmx_node_get_tag_name(HTMX_NODE(data)), ==, "data");
}

static void
test_data_with_value(void)
{
	g_autoptr(HtmxData) data = htmx_data_new_with_value("Mini Ketchup", "00000000");
	g_autofree gchar *html = NULL;

	html = htmx_element_render(HTMX_ELEMENT(data));

	g_assert_nonnull(g_strstr_len(html, -1, "value=\"00000000\""));
	g_assert_nonnull(g_strstr_len(html, -1, ">Mini Ketchup</data>"));
}

/*
 * Bdi tests
 */
static void
test_bdi_new(void)
{
	g_autoptr(HtmxBdi) bdi = htmx_bdi_new();

	g_assert_nonnull(bdi);
	g_assert_cmpstr(htmx_node_get_tag_name(HTMX_NODE(bdi)), ==, "bdi");
}

static void
test_bdi_with_text(void)
{
	g_autoptr(HtmxBdi) bdi = htmx_bdi_new_with_text("username");
	g_autofree gchar *html = NULL;

	html = htmx_element_render(HTMX_ELEMENT(bdi));

	g_assert_cmpstr(html, ==, "<bdi>username</bdi>");
}

/*
 * Bdo tests
 */
static void
test_bdo_new(void)
{
	g_autoptr(HtmxBdo) bdo = htmx_bdo_new();

	g_assert_nonnull(bdo);
	g_assert_cmpstr(htmx_node_get_tag_name(HTMX_NODE(bdo)), ==, "bdo");
}

static void
test_bdo_with_dir(void)
{
	g_autoptr(HtmxBdo) bdo = htmx_bdo_new_with_dir("right to left", "rtl");
	g_autofree gchar *html = NULL;

	html = htmx_element_render(HTMX_ELEMENT(bdo));

	g_assert_nonnull(g_strstr_len(html, -1, "dir=\"rtl\""));
	g_assert_nonnull(g_strstr_len(html, -1, ">right to left</bdo>"));
}

/*
 * Hgroup tests
 */
static void
test_hgroup_new(void)
{
	g_autoptr(HtmxHgroup) hgroup = htmx_hgroup_new();

	g_assert_nonnull(hgroup);
	g_assert_cmpstr(htmx_node_get_tag_name(HTMX_NODE(hgroup)), ==, "hgroup");
}

/*
 * Search tests
 */
static void
test_search_new(void)
{
	g_autoptr(HtmxSearch) search = htmx_search_new();

	g_assert_nonnull(search);
	g_assert_cmpstr(htmx_node_get_tag_name(HTMX_NODE(search)), ==, "search");
}

/*
 * Map tests
 */
static void
test_map_new(void)
{
	g_autoptr(HtmxMap) map = htmx_map_new();

	g_assert_nonnull(map);
	g_assert_cmpstr(htmx_node_get_tag_name(HTMX_NODE(map)), ==, "map");
}

static void
test_map_with_name(void)
{
	g_autoptr(HtmxMap) map = htmx_map_new_with_name("primary");
	g_autofree gchar *html = NULL;

	html = htmx_element_render(HTMX_ELEMENT(map));

	g_assert_nonnull(g_strstr_len(html, -1, "name=\"primary\""));
}

/*
 * Area tests
 */
static void
test_area_new(void)
{
	g_autoptr(HtmxArea) area = htmx_area_new();

	g_assert_nonnull(area);
	g_assert_cmpstr(htmx_node_get_tag_name(HTMX_NODE(area)), ==, "area");
	g_assert_true(htmx_node_is_void_element(HTMX_NODE(area)));
}

static void
test_area_attributes(void)
{
	g_autoptr(HtmxArea) area = htmx_area_new();
	g_autofree gchar *html = NULL;

	htmx_area_set_shape(area, "rect");
	htmx_area_set_coords(area, "0,0,100,100");
	htmx_area_set_href(area, "/region1");
	htmx_area_set_alt(area, "Region 1");
	htmx_area_set_target(area, "_blank");

	html = htmx_element_render(HTMX_ELEMENT(area));

	g_assert_nonnull(g_strstr_len(html, -1, "shape=\"rect\""));
	g_assert_nonnull(g_strstr_len(html, -1, "coords=\"0,0,100,100\""));
	g_assert_nonnull(g_strstr_len(html, -1, "href=\"/region1\""));
	g_assert_nonnull(g_strstr_len(html, -1, "alt=\"Region 1\""));
	g_assert_nonnull(g_strstr_len(html, -1, "target=\"_blank\""));
	g_assert_nonnull(g_strstr_len(html, -1, "/>"));
}

/*
 * Param tests
 */
static void
test_param_new(void)
{
	g_autoptr(HtmxParam) param = htmx_param_new();

	g_assert_nonnull(param);
	g_assert_cmpstr(htmx_node_get_tag_name(HTMX_NODE(param)), ==, "param");
	g_assert_true(htmx_node_is_void_element(HTMX_NODE(param)));
}

static void
test_param_with_name_value(void)
{
	g_autoptr(HtmxParam) param = htmx_param_new_with_name_value("movie", "video.mp4");
	g_autofree gchar *html = NULL;

	html = htmx_element_render(HTMX_ELEMENT(param));

	g_assert_nonnull(g_strstr_len(html, -1, "name=\"movie\""));
	g_assert_nonnull(g_strstr_len(html, -1, "value=\"video.mp4\""));
	g_assert_nonnull(g_strstr_len(html, -1, "/>"));
}

/*
 * Script from file tests
 */
static void
test_script_from_file(void)
{
	g_autoptr(HtmxScript) script = NULL;
	g_autoptr(GError) error = NULL;
	g_autofree gchar *html = NULL;
	g_autofree gchar *temp_path = NULL;
	const gchar *js_content = "console.log('test');";

	/* Create a temporary file with JS content */
	gint fd = g_file_open_tmp("test-XXXXXX.js", &temp_path, &error);
	g_assert_no_error(error);
	g_assert_cmpint(fd, >=, 0);

	/* Write JS content to the file */
	g_assert_true(g_file_set_contents(temp_path, js_content, -1, &error));
	g_assert_no_error(error);
	close(fd);

	/* Load script from file */
	script = htmx_script_new_from_file(temp_path, &error);
	g_assert_no_error(error);
	g_assert_nonnull(script);

	html = htmx_element_render(HTMX_ELEMENT(script));
	g_assert_nonnull(g_strstr_len(html, -1, "console.log('test');"));

	/* Clean up */
	g_unlink(temp_path);
}

static void
test_script_from_file_not_found(void)
{
	g_autoptr(HtmxScript) script = NULL;
	g_autoptr(GError) error = NULL;

	script = htmx_script_new_from_file("/nonexistent/path/to/file.js", &error);

	g_assert_null(script);
	g_assert_error(error, G_FILE_ERROR, G_FILE_ERROR_NOENT);
}

/*
 * Style from file tests
 */
static void
test_style_from_file(void)
{
	g_autoptr(HtmxStyle) style = NULL;
	g_autoptr(GError) error = NULL;
	g_autofree gchar *html = NULL;
	g_autofree gchar *temp_path = NULL;
	const gchar *css_content = "body { color: red; }";

	/* Create a temporary file with CSS content */
	gint fd = g_file_open_tmp("test-XXXXXX.css", &temp_path, &error);
	g_assert_no_error(error);
	g_assert_cmpint(fd, >=, 0);

	/* Write CSS content to the file */
	g_assert_true(g_file_set_contents(temp_path, css_content, -1, &error));
	g_assert_no_error(error);
	close(fd);

	/* Load style from file */
	style = htmx_style_new_from_file(temp_path, &error);
	g_assert_no_error(error);
	g_assert_nonnull(style);

	html = htmx_element_render(HTMX_ELEMENT(style));
	g_assert_nonnull(g_strstr_len(html, -1, "body { color: red; }"));

	/* Clean up */
	g_unlink(temp_path);
}

static void
test_style_from_file_not_found(void)
{
	g_autoptr(HtmxStyle) style = NULL;
	g_autoptr(GError) error = NULL;

	style = htmx_style_new_from_file("/nonexistent/path/to/file.css", &error);

	g_assert_null(style);
	g_assert_error(error, G_FILE_ERROR, G_FILE_ERROR_NOENT);
}

int
main(int argc, char *argv[])
{
	g_test_init(&argc, &argv, NULL);

	/* Div tests */
	g_test_add_func("/elements/div/new", test_div_new);
	g_test_add_func("/elements/div/render", test_div_render);

	/* Span tests */
	g_test_add_func("/elements/span/new", test_span_new);

	/* Paragraph tests */
	g_test_add_func("/elements/p/new", test_p_new);

	/* Heading tests */
	g_test_add_func("/elements/heading/new", test_heading_new);
	g_test_add_func("/elements/heading/convenience", test_heading_convenience);
	g_test_add_func("/elements/heading/render", test_heading_render);

	/* Anchor tests */
	g_test_add_func("/elements/a/new", test_a_new);
	g_test_add_func("/elements/a/with_href", test_a_with_href);

	/* Button tests */
	g_test_add_func("/elements/button/new", test_button_new);
	g_test_add_func("/elements/button/with_label", test_button_with_label);
	g_test_add_func("/elements/button/submit", test_button_submit);

	/* Label tests */
	g_test_add_func("/elements/label/new", test_label_new);
	g_test_add_func("/elements/label/for", test_label_for);

	/* List tests */
	g_test_add_func("/elements/ul/new", test_ul_new);
	g_test_add_func("/elements/ol/new", test_ol_new);
	g_test_add_func("/elements/li/new", test_li_new);
	g_test_add_func("/elements/list/render", test_list_render);

	/* Img tests */
	g_test_add_func("/elements/img/new", test_img_new);
	g_test_add_func("/elements/img/with_src", test_img_with_src);

	/* Textarea tests */
	g_test_add_func("/elements/textarea/new", test_textarea_new);
	g_test_add_func("/elements/textarea/with_name", test_textarea_with_name);

	/* Select tests */
	g_test_add_func("/elements/select/new", test_select_new);
	g_test_add_func("/elements/option/new", test_option_new);
	g_test_add_func("/elements/select/with_options", test_select_with_options);

	/* Form and Input tag name tests */
	g_test_add_func("/elements/form/tag_name", test_form_tag_name);
	g_test_add_func("/elements/input/tag_name", test_input_tag_name);

	/* Button type enum tests */
	g_test_add_func("/elements/button_type/to_string", test_button_type_to_string);

	/* Progress tests */
	g_test_add_func("/elements/progress/new", test_progress_new);
	g_test_add_func("/elements/progress/with_value", test_progress_with_value);

	/* Meter tests */
	g_test_add_func("/elements/meter/new", test_meter_new);
	g_test_add_func("/elements/meter/attributes", test_meter_attributes);

	/* Del tests */
	g_test_add_func("/elements/del/new", test_del_new);
	g_test_add_func("/elements/del/with_attributes", test_del_with_attributes);

	/* Ins tests */
	g_test_add_func("/elements/ins/new", test_ins_new);
	g_test_add_func("/elements/ins/with_attributes", test_ins_with_attributes);

	/* Ruby tests */
	g_test_add_func("/elements/ruby/new", test_ruby_new);
	g_test_add_func("/elements/rt/new", test_rt_new);
	g_test_add_func("/elements/rp/new", test_rp_new);
	g_test_add_func("/elements/ruby/structure", test_ruby_structure);

	/* Dfn tests */
	g_test_add_func("/elements/dfn/new", test_dfn_new);
	g_test_add_func("/elements/dfn/with_text", test_dfn_with_text);

	/* Data tests */
	g_test_add_func("/elements/data/new", test_data_new);
	g_test_add_func("/elements/data/with_value", test_data_with_value);

	/* Bdi tests */
	g_test_add_func("/elements/bdi/new", test_bdi_new);
	g_test_add_func("/elements/bdi/with_text", test_bdi_with_text);

	/* Bdo tests */
	g_test_add_func("/elements/bdo/new", test_bdo_new);
	g_test_add_func("/elements/bdo/with_dir", test_bdo_with_dir);

	/* Hgroup tests */
	g_test_add_func("/elements/hgroup/new", test_hgroup_new);

	/* Search tests */
	g_test_add_func("/elements/search/new", test_search_new);

	/* Map tests */
	g_test_add_func("/elements/map/new", test_map_new);
	g_test_add_func("/elements/map/with_name", test_map_with_name);

	/* Area tests */
	g_test_add_func("/elements/area/new", test_area_new);
	g_test_add_func("/elements/area/attributes", test_area_attributes);

	/* Param tests */
	g_test_add_func("/elements/param/new", test_param_new);
	g_test_add_func("/elements/param/with_name_value", test_param_with_name_value);

	/* Script from file tests */
	g_test_add_func("/elements/script/from_file", test_script_from_file);
	g_test_add_func("/elements/script/from_file_not_found", test_script_from_file_not_found);

	/* Style from file tests */
	g_test_add_func("/elements/style/from_file", test_style_from_file);
	g_test_add_func("/elements/style/from_file_not_found", test_style_from_file_not_found);

	return g_test_run();
}
