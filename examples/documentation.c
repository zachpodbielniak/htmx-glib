/*
 * documentation.c - Documentation page with text formatting elements
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * Demonstrates htmx-glib text formatting elements:
 * - HtmxStrong, HtmxEm, HtmxB, HtmxI, HtmxU, HtmxS
 * - HtmxMark, HtmxSmall, HtmxSub, HtmxSup
 * - HtmxCode, HtmxPre, HtmxKbd, HtmxSamp, HtmxVar
 * - HtmxAbbr, HtmxCite, HtmxQ, HtmxBlockquote
 * - HtmxTime
 */

#include <htmx-glib.h>
#include <stdio.h>

/*
 * Render a section about text emphasis
 */
static gchar *
render_emphasis_section(void)
{
	g_autoptr(HtmxSection) section = htmx_section_new();
	g_autoptr(HtmxHeading) heading = htmx_h2_new();
	g_autoptr(HtmxP) intro = htmx_p_new();

	htmx_node_set_text_content(HTMX_NODE(heading), "Text Emphasis & Styling");
	htmx_element_add_class(HTMX_ELEMENT(section), "doc-section");

	/* Introduction paragraph with various emphasis */
	{
		g_autoptr(HtmxSpan) text1 = htmx_span_new();
		g_autoptr(HtmxStrong) strong = htmx_strong_new_with_text("important text");
		g_autoptr(HtmxSpan) text2 = htmx_span_new();
		g_autoptr(HtmxEm) em = htmx_em_new_with_text("emphasized text");
		g_autoptr(HtmxSpan) text3 = htmx_span_new();
		g_autoptr(HtmxMark) mark = htmx_mark_new_with_text("highlighted text");
		g_autoptr(HtmxSpan) text4 = htmx_span_new();

		htmx_node_set_text_content(HTMX_NODE(text1), "Use ");
		htmx_node_set_text_content(HTMX_NODE(text2), " for semantic importance, ");
		htmx_node_set_text_content(HTMX_NODE(text3), " for stress emphasis, and ");
		htmx_node_set_text_content(HTMX_NODE(text4), " for relevance.");

		htmx_node_add_child(HTMX_NODE(intro), HTMX_NODE(text1));
		htmx_node_add_child(HTMX_NODE(intro), HTMX_NODE(strong));
		htmx_node_add_child(HTMX_NODE(intro), HTMX_NODE(text2));
		htmx_node_add_child(HTMX_NODE(intro), HTMX_NODE(em));
		htmx_node_add_child(HTMX_NODE(intro), HTMX_NODE(text3));
		htmx_node_add_child(HTMX_NODE(intro), HTMX_NODE(mark));
		htmx_node_add_child(HTMX_NODE(intro), HTMX_NODE(text4));
	}

	/* Stylistic variants */
	{
		g_autoptr(HtmxP) stylistic = htmx_p_new();
		g_autoptr(HtmxSpan) text1 = htmx_span_new();
		g_autoptr(HtmxB) bold = htmx_b_new_with_text("bold");
		g_autoptr(HtmxSpan) text2 = htmx_span_new();
		g_autoptr(HtmxI) italic = htmx_i_new_with_text("italic");
		g_autoptr(HtmxSpan) text3 = htmx_span_new();
		g_autoptr(HtmxU) underline = htmx_u_new_with_text("underlined");
		g_autoptr(HtmxSpan) text4 = htmx_span_new();
		g_autoptr(HtmxS) strikethrough = htmx_s_new_with_text("strikethrough");
		g_autoptr(HtmxSpan) text5 = htmx_span_new();
		g_autoptr(HtmxSmall) small = htmx_small_new_with_text("smaller");
		g_autoptr(HtmxSpan) text6 = htmx_span_new();

		htmx_node_set_text_content(HTMX_NODE(text1), "Stylistic variants: ");
		htmx_node_set_text_content(HTMX_NODE(text2), ", ");
		htmx_node_set_text_content(HTMX_NODE(text3), ", ");
		htmx_node_set_text_content(HTMX_NODE(text4), ", ");
		htmx_node_set_text_content(HTMX_NODE(text5), ", and ");
		htmx_node_set_text_content(HTMX_NODE(text6), " text.");

		htmx_node_add_child(HTMX_NODE(stylistic), HTMX_NODE(text1));
		htmx_node_add_child(HTMX_NODE(stylistic), HTMX_NODE(bold));
		htmx_node_add_child(HTMX_NODE(stylistic), HTMX_NODE(text2));
		htmx_node_add_child(HTMX_NODE(stylistic), HTMX_NODE(italic));
		htmx_node_add_child(HTMX_NODE(stylistic), HTMX_NODE(text3));
		htmx_node_add_child(HTMX_NODE(stylistic), HTMX_NODE(underline));
		htmx_node_add_child(HTMX_NODE(stylistic), HTMX_NODE(text4));
		htmx_node_add_child(HTMX_NODE(stylistic), HTMX_NODE(strikethrough));
		htmx_node_add_child(HTMX_NODE(stylistic), HTMX_NODE(text5));
		htmx_node_add_child(HTMX_NODE(stylistic), HTMX_NODE(small));
		htmx_node_add_child(HTMX_NODE(stylistic), HTMX_NODE(text6));

		htmx_node_add_child(HTMX_NODE(section), HTMX_NODE(heading));
		htmx_node_add_child(HTMX_NODE(section), HTMX_NODE(intro));
		htmx_node_add_child(HTMX_NODE(section), HTMX_NODE(stylistic));
	}

	/* Subscript and superscript example */
	{
		g_autoptr(HtmxP) math_example = htmx_p_new();
		g_autoptr(HtmxSpan) text1 = htmx_span_new();
		g_autoptr(HtmxVar) var_e = htmx_var_new_with_text("E");
		g_autoptr(HtmxSpan) text2 = htmx_span_new();
		g_autoptr(HtmxVar) var_m = htmx_var_new_with_text("mc");
		g_autoptr(HtmxSup) sup = htmx_sup_new_with_text("2");
		g_autoptr(HtmxSpan) text3 = htmx_span_new();
		g_autoptr(HtmxSpan) chem = htmx_span_new();
		g_autoptr(HtmxSub) sub = htmx_sub_new_with_text("2");

		htmx_node_set_text_content(HTMX_NODE(text1), "Mathematical: ");
		htmx_node_set_text_content(HTMX_NODE(text2), " = ");
		htmx_node_set_text_content(HTMX_NODE(text3), ". Chemical: H");
		htmx_node_set_text_content(HTMX_NODE(chem), "O");

		htmx_node_add_child(HTMX_NODE(math_example), HTMX_NODE(text1));
		htmx_node_add_child(HTMX_NODE(math_example), HTMX_NODE(var_e));
		htmx_node_add_child(HTMX_NODE(math_example), HTMX_NODE(text2));
		htmx_node_add_child(HTMX_NODE(math_example), HTMX_NODE(var_m));
		htmx_node_add_child(HTMX_NODE(math_example), HTMX_NODE(sup));
		htmx_node_add_child(HTMX_NODE(math_example), HTMX_NODE(text3));
		htmx_node_add_child(HTMX_NODE(math_example), HTMX_NODE(sub));
		htmx_node_add_child(HTMX_NODE(math_example), HTMX_NODE(chem));

		htmx_node_add_child(HTMX_NODE(section), HTMX_NODE(math_example));
	}

	return htmx_element_render(HTMX_ELEMENT(section));
}

/*
 * Render a section about code and technical text
 */
static gchar *
render_code_section(void)
{
	g_autoptr(HtmxSection) section = htmx_section_new();
	g_autoptr(HtmxHeading) heading = htmx_h2_new();

	htmx_node_set_text_content(HTMX_NODE(heading), "Code & Technical Text");
	htmx_element_add_class(HTMX_ELEMENT(section), "doc-section");

	/* Inline code example */
	{
		g_autoptr(HtmxP) inline_example = htmx_p_new();
		g_autoptr(HtmxSpan) text1 = htmx_span_new();
		g_autoptr(HtmxCode) code = htmx_code_new_with_text("htmx_response_new()");
		g_autoptr(HtmxSpan) text2 = htmx_span_new();

		htmx_node_set_text_content(HTMX_NODE(text1), "Call ");
		htmx_node_set_text_content(HTMX_NODE(text2), " to create a new response object.");

		htmx_node_add_child(HTMX_NODE(inline_example), HTMX_NODE(text1));
		htmx_node_add_child(HTMX_NODE(inline_example), HTMX_NODE(code));
		htmx_node_add_child(HTMX_NODE(inline_example), HTMX_NODE(text2));

		htmx_node_add_child(HTMX_NODE(section), HTMX_NODE(heading));
		htmx_node_add_child(HTMX_NODE(section), HTMX_NODE(inline_example));
	}

	/* Preformatted code block */
	{
		g_autoptr(HtmxPre) pre = htmx_pre_new();
		g_autoptr(HtmxCode) code_block = htmx_code_new();

		htmx_node_set_text_content(HTMX_NODE(code_block),
			"static HtmxResponse *\n"
			"handle_request(HtmxRequest *request, GHashTable *params, gpointer user_data)\n"
			"{\n"
			"    g_autoptr(HtmxResponse) response = htmx_response_new();\n"
			"    \n"
			"    htmx_response_set_content(response, \"Hello, World!\");\n"
			"    \n"
			"    return (HtmxResponse *)g_steal_pointer(&response);\n"
			"}"
		);

		htmx_element_add_class(HTMX_ELEMENT(pre), "code-block");
		htmx_node_add_child(HTMX_NODE(pre), HTMX_NODE(code_block));
		htmx_node_add_child(HTMX_NODE(section), HTMX_NODE(pre));
	}

	/* Keyboard shortcuts */
	{
		g_autoptr(HtmxP) kbd_example = htmx_p_new();
		g_autoptr(HtmxSpan) text1 = htmx_span_new();
		g_autoptr(HtmxKbd) ctrl = htmx_kbd_new_with_text("Ctrl");
		g_autoptr(HtmxSpan) plus1 = htmx_span_new();
		g_autoptr(HtmxKbd) shift = htmx_kbd_new_with_text("Shift");
		g_autoptr(HtmxSpan) plus2 = htmx_span_new();
		g_autoptr(HtmxKbd) key_p = htmx_kbd_new_with_text("P");
		g_autoptr(HtmxSpan) text2 = htmx_span_new();

		htmx_node_set_text_content(HTMX_NODE(text1), "Press ");
		htmx_node_set_text_content(HTMX_NODE(plus1), " + ");
		htmx_node_set_text_content(HTMX_NODE(plus2), " + ");
		htmx_node_set_text_content(HTMX_NODE(text2), " to open the command palette.");

		htmx_node_add_child(HTMX_NODE(kbd_example), HTMX_NODE(text1));
		htmx_node_add_child(HTMX_NODE(kbd_example), HTMX_NODE(ctrl));
		htmx_node_add_child(HTMX_NODE(kbd_example), HTMX_NODE(plus1));
		htmx_node_add_child(HTMX_NODE(kbd_example), HTMX_NODE(shift));
		htmx_node_add_child(HTMX_NODE(kbd_example), HTMX_NODE(plus2));
		htmx_node_add_child(HTMX_NODE(kbd_example), HTMX_NODE(key_p));
		htmx_node_add_child(HTMX_NODE(kbd_example), HTMX_NODE(text2));

		htmx_node_add_child(HTMX_NODE(section), HTMX_NODE(kbd_example));
	}

	/* Sample output */
	{
		g_autoptr(HtmxP) samp_example = htmx_p_new();
		g_autoptr(HtmxSpan) text1 = htmx_span_new();
		g_autoptr(HtmxSamp) samp = htmx_samp_new_with_text("Server running at http://localhost:8080");
		g_autoptr(HtmxSpan) text2 = htmx_span_new();

		htmx_node_set_text_content(HTMX_NODE(text1), "When successful, you'll see: ");
		htmx_node_set_text_content(HTMX_NODE(text2), "");

		htmx_node_add_child(HTMX_NODE(samp_example), HTMX_NODE(text1));
		htmx_node_add_child(HTMX_NODE(samp_example), HTMX_NODE(samp));

		htmx_node_add_child(HTMX_NODE(section), HTMX_NODE(samp_example));
	}

	return htmx_element_render(HTMX_ELEMENT(section));
}

/*
 * Render a section about citations and quotations
 */
static gchar *
render_quotation_section(void)
{
	g_autoptr(HtmxSection) section = htmx_section_new();
	g_autoptr(HtmxHeading) heading = htmx_h2_new();

	htmx_node_set_text_content(HTMX_NODE(heading), "Citations & Quotations");
	htmx_element_add_class(HTMX_ELEMENT(section), "doc-section");
	htmx_node_add_child(HTMX_NODE(section), HTMX_NODE(heading));

	/* Inline quotation */
	{
		g_autoptr(HtmxP) quote_example = htmx_p_new();
		g_autoptr(HtmxSpan) text1 = htmx_span_new();
		g_autoptr(HtmxQ) quote = htmx_q_new_with_text(
			"The best way to predict the future is to invent it."
		);
		g_autoptr(HtmxSpan) text2 = htmx_span_new();
		g_autoptr(HtmxCite) cite = htmx_cite_new_with_text("Alan Kay");

		htmx_q_set_cite(quote, "https://example.com/alan-kay-quotes");
		htmx_node_set_text_content(HTMX_NODE(text1), "As ");
		htmx_node_set_text_content(HTMX_NODE(text2), " once said: ");

		htmx_node_add_child(HTMX_NODE(quote_example), HTMX_NODE(text1));
		htmx_node_add_child(HTMX_NODE(quote_example), HTMX_NODE(cite));
		htmx_node_add_child(HTMX_NODE(quote_example), HTMX_NODE(text2));
		htmx_node_add_child(HTMX_NODE(quote_example), HTMX_NODE(quote));

		htmx_node_add_child(HTMX_NODE(section), HTMX_NODE(quote_example));
	}

	/* Block quotation */
	{
		g_autoptr(HtmxBlockquote) blockquote = htmx_blockquote_new();
		g_autoptr(HtmxP) bq_text = htmx_p_new();
		g_autoptr(HtmxFooter) bq_footer = htmx_footer_new();
		g_autoptr(HtmxCite) bq_cite = htmx_cite_new_with_text("— Linus Torvalds");

		htmx_blockquote_set_cite(blockquote, "https://example.com/linus-quotes");
		htmx_node_set_text_content(HTMX_NODE(bq_text),
			"Talk is cheap. Show me the code. In open source, "
			"we feel strongly that to really do something well, you have to get "
			"a lot of people involved."
		);

		htmx_node_add_child(HTMX_NODE(bq_footer), HTMX_NODE(bq_cite));
		htmx_node_add_child(HTMX_NODE(blockquote), HTMX_NODE(bq_text));
		htmx_node_add_child(HTMX_NODE(blockquote), HTMX_NODE(bq_footer));

		htmx_node_add_child(HTMX_NODE(section), HTMX_NODE(blockquote));
	}

	/* Abbreviations */
	{
		g_autoptr(HtmxP) abbr_example = htmx_p_new();
		g_autoptr(HtmxAbbr) abbr_html = htmx_abbr_new_with_title("HTML", "HyperText Markup Language");
		g_autoptr(HtmxSpan) text1 = htmx_span_new();
		g_autoptr(HtmxAbbr) abbr_css = htmx_abbr_new_with_title("CSS", "Cascading Style Sheets");
		g_autoptr(HtmxSpan) text2 = htmx_span_new();
		g_autoptr(HtmxAbbr) abbr_api = htmx_abbr_new_with_title("API", "Application Programming Interface");
		g_autoptr(HtmxSpan) text3 = htmx_span_new();

		htmx_node_set_text_content(HTMX_NODE(text1), " and ");
		htmx_node_set_text_content(HTMX_NODE(text2), " are enhanced via the htmx-glib ");
		htmx_node_set_text_content(HTMX_NODE(text3), ".");

		htmx_node_add_child(HTMX_NODE(abbr_example), HTMX_NODE(abbr_html));
		htmx_node_add_child(HTMX_NODE(abbr_example), HTMX_NODE(text1));
		htmx_node_add_child(HTMX_NODE(abbr_example), HTMX_NODE(abbr_css));
		htmx_node_add_child(HTMX_NODE(abbr_example), HTMX_NODE(text2));
		htmx_node_add_child(HTMX_NODE(abbr_example), HTMX_NODE(abbr_api));
		htmx_node_add_child(HTMX_NODE(abbr_example), HTMX_NODE(text3));

		htmx_node_add_child(HTMX_NODE(section), HTMX_NODE(abbr_example));
	}

	return htmx_element_render(HTMX_ELEMENT(section));
}

/*
 * Render a section about dates and times
 */
static gchar *
render_time_section(void)
{
	g_autoptr(HtmxSection) section = htmx_section_new();
	g_autoptr(HtmxHeading) heading = htmx_h2_new();
	g_autoptr(HtmxP) intro = htmx_p_new();

	htmx_node_set_text_content(HTMX_NODE(heading), "Dates & Times");
	htmx_element_add_class(HTMX_ELEMENT(section), "doc-section");

	/* Time examples */
	{
		g_autoptr(HtmxSpan) text1 = htmx_span_new();
		g_autoptr(HtmxTime) release_date = htmx_time_new_with_datetime("January 15, 2026", "2026-01-15");
		g_autoptr(HtmxSpan) text2 = htmx_span_new();
		g_autoptr(HtmxTime) meeting = htmx_time_new_with_datetime("3:00 PM", "15:00");
		g_autoptr(HtmxSpan) text3 = htmx_span_new();

		htmx_node_set_text_content(HTMX_NODE(text1), "The library was released on ");
		htmx_node_set_text_content(HTMX_NODE(text2), ". The next community meeting is at ");
		htmx_node_set_text_content(HTMX_NODE(text3), " UTC.");

		htmx_node_add_child(HTMX_NODE(intro), HTMX_NODE(text1));
		htmx_node_add_child(HTMX_NODE(intro), HTMX_NODE(release_date));
		htmx_node_add_child(HTMX_NODE(intro), HTMX_NODE(text2));
		htmx_node_add_child(HTMX_NODE(intro), HTMX_NODE(meeting));
		htmx_node_add_child(HTMX_NODE(intro), HTMX_NODE(text3));
	}

	htmx_node_add_child(HTMX_NODE(section), HTMX_NODE(heading));
	htmx_node_add_child(HTMX_NODE(section), HTMX_NODE(intro));

	return htmx_element_render(HTMX_ELEMENT(section));
}

/*
 * Render the complete page
 */
static gchar *
render_page(void)
{
	g_autoptr(HtmxBuilder) builder = htmx_builder_new();
	g_autofree gchar *emphasis_html = render_emphasis_section();
	g_autofree gchar *code_html = render_code_section();
	g_autofree gchar *quotation_html = render_quotation_section();
	g_autofree gchar *time_html = render_time_section();

	htmx_builder_doctype(builder);
	htmx_builder_begin(builder, "html");
	htmx_builder_attr(builder, "lang", "en");

	/* Head */
	htmx_builder_begin(builder, "head");

	htmx_builder_begin_void(builder, "meta");
	htmx_builder_attr(builder, "charset", "utf-8");
	htmx_builder_end_void(builder);

	htmx_builder_begin_void(builder, "meta");
	htmx_builder_attr(builder, "name", "viewport");
	htmx_builder_attr(builder, "content", "width=device-width, initial-scale=1");
	htmx_builder_end_void(builder);

	htmx_builder_begin(builder, "title");
	htmx_builder_text(builder, "Documentation - htmx-glib Text Elements");
	htmx_builder_end(builder);

	htmx_builder_begin(builder, "style");
	htmx_builder_text(builder,
		"* { box-sizing: border-box; }\n"
		"body { font-family: Georgia, serif; line-height: 1.8; max-width: 800px; margin: 0 auto; padding: 1em; color: #333; }\n"
		"h1 { border-bottom: 3px solid #3498db; padding-bottom: 0.5em; }\n"
		"h2 { color: #2c3e50; margin-top: 2em; }\n"
		".doc-section { margin-bottom: 2em; }\n"
		"strong { color: #c0392b; }\n"
		"em { color: #8e44ad; }\n"
		"mark { background: #f1c40f; padding: 0 0.2em; }\n"
		"code { background: #ecf0f1; padding: 0.1em 0.3em; border-radius: 3px; font-family: 'JetBrains Mono', monospace; }\n"
		".code-block { background: #2c3e50; color: #ecf0f1; padding: 1em; border-radius: 4px; overflow-x: auto; }\n"
		".code-block code { background: none; color: inherit; }\n"
		"kbd { background: #34495e; color: white; padding: 0.2em 0.4em; border-radius: 3px; font-size: 0.9em; }\n"
		"samp { background: #27ae60; color: white; padding: 0.2em 0.4em; border-radius: 3px; }\n"
		"var { font-style: italic; color: #2980b9; }\n"
		"abbr { border-bottom: 1px dotted #666; cursor: help; }\n"
		"q { font-style: italic; color: #555; }\n"
		"q::before, q::after { color: #888; }\n"
		"blockquote { margin: 1em 0; padding: 1em 1.5em; border-left: 4px solid #3498db; background: #f9f9f9; }\n"
		"blockquote p { margin: 0 0 0.5em; }\n"
		"blockquote footer { font-size: 0.9em; color: #666; }\n"
		"cite { font-style: italic; }\n"
		"time { background: #e8f4fd; padding: 0.1em 0.3em; border-radius: 3px; }\n"
		"sub, sup { font-size: 0.75em; }\n"
	);
	htmx_builder_end(builder);

	htmx_builder_end(builder); /* head */

	/* Body */
	htmx_builder_begin(builder, "body");

	htmx_builder_begin(builder, "header");
	htmx_builder_begin(builder, "h1");
	htmx_builder_text(builder, "htmx-glib Text Formatting Elements");
	htmx_builder_end(builder);
	htmx_builder_begin(builder, "p");
	htmx_builder_text(builder, "A comprehensive guide to text formatting and semantic elements.");
	htmx_builder_end(builder);
	htmx_builder_end(builder);

	htmx_builder_begin(builder, "main");
	htmx_builder_raw_html(builder, emphasis_html);
	htmx_builder_raw_html(builder, code_html);
	htmx_builder_raw_html(builder, quotation_html);
	htmx_builder_raw_html(builder, time_html);
	htmx_builder_end(builder);

	htmx_builder_end(builder); /* body */
	htmx_builder_end(builder); /* html */

	return htmx_builder_render(builder);
}

/*
 * Handle GET / - Show the documentation page
 */
static HtmxResponse *
handle_index(HtmxRequest *request, GHashTable *params, gpointer user_data)
{
	g_autoptr(HtmxResponse) response = htmx_response_new();
	g_autofree gchar *html = render_page();

	htmx_response_set_content(response, html);
	htmx_response_set_content_type(response, "text/html; charset=utf-8");

	return (HtmxResponse *)g_steal_pointer(&response);
}

int
main(int argc, char *argv[])
{
	g_autoptr(HtmxServer) server = NULL;
	HtmxRouter *router;
	guint port = 8080;

	if (argc > 1) {
		port = (guint)g_ascii_strtoull(argv[1], NULL, 10);
		if (port == 0) port = 8080;
	}

	server = htmx_server_new_with_port(port);
	if (server == NULL) {
		g_printerr("Failed to create server\n");
		return 1;
	}

	router = htmx_server_get_router(server);
	htmx_router_get(router, "/", handle_index, NULL);

	g_print("Documentation Example\n");
	g_print("Server running at http://localhost:%u\n", port);
	g_print("Press Ctrl+C to stop\n\n");

	htmx_server_run(server);

	return 0;
}
