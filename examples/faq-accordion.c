/*
 * faq-accordion.c - FAQ accordion with interactive elements
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * Demonstrates htmx-glib interactive elements:
 * - HtmxDetails, HtmxSummary - Disclosure widgets
 * - HtmxDialog - Modal dialogs
 * - HtmxMenu - Context menus
 * - Definition lists for FAQ structure
 * - HtmxDl, HtmxDt, HtmxDd
 */

#include <htmx-glib.h>
#include <stdio.h>

/* FAQ data structure */
typedef struct {
	const gchar *question;
	const gchar *answer;
	const gchar *category;
} FaqItem;

static FaqItem faqs[] = {
	{
		"What is htmx-glib?",
		"htmx-glib is a GObject-based library for building HTMX-compatible HTTP servers in C. "
		"It provides tools for parsing HTMX request headers, generating HTML elements with HTMX "
		"attributes, and building response headers for controlling client behavior.",
		"General"
	},
	{
		"Do I need to write JavaScript?",
		"No! htmx-glib lets you build dynamic, interactive web applications without writing "
		"JavaScript. HTMX handles all the client-side interactivity, and you write your "
		"application logic entirely in C.",
		"General"
	},
	{
		"How do I install htmx-glib?",
		"Clone the repository, ensure you have the dependencies (glib2-devel, libsoup3-devel, "
		"json-glib-devel, gobject-introspection-devel), then run 'make && make install'. "
		"The library will be installed to /usr/local by default.",
		"Installation"
	},
	{
		"What dependencies are required?",
		"htmx-glib requires GLib 2.0, GObject 2.0, Gio 2.0, libsoup 3.0, and json-glib 1.0. "
		"On Fedora, install: glib2-devel libsoup3-devel json-glib-devel gobject-introspection-devel",
		"Installation"
	},
	{
		"Can I use htmx-glib with Python?",
		"Yes! htmx-glib is GObject-Introspection compatible, so you can use it from Python "
		"via PyGObject. After installing, import it with: gi.require_version('HtmxGlib', '1.0') "
		"and from gi.repository import HtmxGlib",
		"Usage"
	},
	{
		"How do I handle forms?",
		"Use htmx_request_get_form_data() to retrieve parsed form data from POST requests. "
		"The function returns a GHashTable with field names as keys and values as strings.",
		"Usage"
	},
	{
		"Does htmx-glib support WebSockets?",
		"Yes! htmx-glib includes WebSocket support via HtmxWebSocketConnection. You can use "
		"it for real-time bidirectional communication between server and client.",
		"Features"
	},
	{
		"What about Server-Sent Events?",
		"htmx-glib has built-in SSE support through HtmxSseConnection. This is perfect for "
		"pushing updates from server to client, like live notifications or progress updates.",
		"Features"
	},
};

static const guint num_faqs = G_N_ELEMENTS(faqs);

/*
 * Render a single FAQ item using HtmxDetails/HtmxSummary
 */
static HtmxDetails *
render_faq_item(FaqItem *faq, guint index)
{
	g_autoptr(HtmxDetails) details = htmx_details_new();
	g_autoptr(HtmxSummary) summary = htmx_summary_new();
	g_autoptr(HtmxDiv) content = htmx_div_new();
	g_autoptr(HtmxP) answer = htmx_p_new();
	g_autoptr(HtmxSpan) category_badge = htmx_span_new();

	htmx_element_add_class(HTMX_ELEMENT(details), "faq-item");
	htmx_element_set_id(HTMX_ELEMENT(details), g_strdup_printf("faq-%u", index));

	/* Summary with question and category badge */
	{
		g_autoptr(HtmxStrong) question = htmx_strong_new_with_text(faq->question);
		htmx_node_add_child(HTMX_NODE(summary), HTMX_NODE(question));

		htmx_node_set_text_content(HTMX_NODE(category_badge), faq->category);
		htmx_element_add_class(HTMX_ELEMENT(category_badge), "category-badge");
		htmx_node_add_child(HTMX_NODE(summary), HTMX_NODE(category_badge));
	}

	/* Answer content */
	htmx_element_add_class(HTMX_ELEMENT(content), "faq-content");
	htmx_node_set_text_content(HTMX_NODE(answer), faq->answer);
	htmx_node_add_child(HTMX_NODE(content), HTMX_NODE(answer));

	/* Assemble */
	htmx_node_add_child(HTMX_NODE(details), HTMX_NODE(summary));
	htmx_node_add_child(HTMX_NODE(details), HTMX_NODE(content));

	return (HtmxDetails *)g_steal_pointer(&details);
}

/*
 * Render all FAQs grouped by category using definition lists
 */
static gchar *
render_faq_list(void)
{
	g_autoptr(HtmxSection) section = htmx_section_new();
	g_autoptr(HtmxHeading) heading = htmx_h2_new();

	htmx_element_add_class(HTMX_ELEMENT(section), "faq-section");
	htmx_node_set_text_content(HTMX_NODE(heading), "Frequently Asked Questions");
	htmx_node_add_child(HTMX_NODE(section), HTMX_NODE(heading));

	/* Create FAQ items */
	{
		guint i;
		for (i = 0; i < num_faqs; i++) {
			g_autoptr(HtmxDetails) item = render_faq_item(&faqs[i], i);
			htmx_node_add_child(HTMX_NODE(section), HTMX_NODE(item));
		}
	}

	return htmx_element_render(HTMX_ELEMENT(section));
}

/*
 * Render a glossary using definition lists
 */
static gchar *
render_glossary(void)
{
	g_autoptr(HtmxSection) section = htmx_section_new();
	g_autoptr(HtmxHeading) heading = htmx_h2_new();
	g_autoptr(HtmxDl) def_list = htmx_dl_new();

	typedef struct {
		const gchar *term;
		const gchar *definition;
	} GlossaryItem;

	GlossaryItem glossary[] = {
		{ "HTMX", "A JavaScript library that allows you to access modern browser features directly from HTML, without writing JavaScript." },
		{ "GObject", "A type system and object model used by GLib and GTK, providing object-oriented features for C." },
		{ "SSE", "Server-Sent Events - a server push technology enabling a client to receive automatic updates from a server." },
		{ "WebSocket", "A communication protocol providing full-duplex communication channels over a single TCP connection." },
		{ "GIR", "GObject Introspection Repository - metadata format describing GObject-based libraries for use from other languages." },
	};

	htmx_element_add_class(HTMX_ELEMENT(section), "glossary-section");
	htmx_node_set_text_content(HTMX_NODE(heading), "Glossary");
	htmx_element_add_class(HTMX_ELEMENT(def_list), "glossary");

	{
		guint i;
		for (i = 0; i < G_N_ELEMENTS(glossary); i++) {
			g_autoptr(HtmxDt) term = htmx_dt_new();
			g_autoptr(HtmxDd) def = htmx_dd_new_with_text(glossary[i].definition);

			/* Term with abbr for technical terms */
			{
				g_autoptr(HtmxAbbr) abbr = htmx_abbr_new_with_title(
					glossary[i].term,
					glossary[i].definition
				);
				htmx_node_add_child(HTMX_NODE(term), HTMX_NODE(abbr));
			}

			htmx_node_add_child(HTMX_NODE(def_list), HTMX_NODE(term));
			htmx_node_add_child(HTMX_NODE(def_list), HTMX_NODE(def));
		}
	}

	htmx_node_add_child(HTMX_NODE(section), HTMX_NODE(heading));
	htmx_node_add_child(HTMX_NODE(section), HTMX_NODE(def_list));

	return htmx_element_render(HTMX_ELEMENT(section));
}

/*
 * Render a contact dialog
 */
static gchar *
render_contact_dialog(void)
{
	g_autoptr(HtmxDialog) dialog = htmx_dialog_new();
	g_autoptr(HtmxForm) form = htmx_form_new();
	g_autoptr(HtmxHeading) heading = htmx_h3_new();
	g_autoptr(HtmxDiv) form_group1 = htmx_div_new();
	g_autoptr(HtmxDiv) form_group2 = htmx_div_new();
	g_autoptr(HtmxDiv) buttons = htmx_div_new();

	htmx_element_set_id(HTMX_ELEMENT(dialog), "contact-dialog");
	htmx_element_add_class(HTMX_ELEMENT(dialog), "modal");

	htmx_node_set_text_content(HTMX_NODE(heading), "Contact Us");

	/* Name input */
	{
		g_autoptr(HtmxLabel) label = htmx_label_new_for("name");
		g_autoptr(HtmxInput) input = htmx_input_new_text("name");

		htmx_node_set_text_content(HTMX_NODE(label), "Your Name:");
		htmx_input_set_required(input, TRUE);
		htmx_element_add_class(HTMX_ELEMENT(form_group1), "form-group");

		htmx_node_add_child(HTMX_NODE(form_group1), HTMX_NODE(label));
		htmx_node_add_child(HTMX_NODE(form_group1), HTMX_NODE(input));
	}

	/* Message textarea */
	{
		g_autoptr(HtmxLabel) label = htmx_label_new_for("message");
		g_autoptr(HtmxTextarea) textarea = htmx_textarea_new_with_name("message");

		htmx_node_set_text_content(HTMX_NODE(label), "Your Message:");
		htmx_textarea_set_rows(textarea, 4);
		htmx_textarea_set_required(textarea, TRUE);
		htmx_element_add_class(HTMX_ELEMENT(form_group2), "form-group");

		htmx_node_add_child(HTMX_NODE(form_group2), HTMX_NODE(label));
		htmx_node_add_child(HTMX_NODE(form_group2), HTMX_NODE(textarea));
	}

	/* Buttons */
	{
		g_autoptr(HtmxButton) submit = htmx_button_new_submit();
		g_autoptr(HtmxButton) cancel = htmx_button_new_with_label("Cancel");

		htmx_node_set_text_content(HTMX_NODE(submit), "Send Message");
		htmx_element_add_class(HTMX_ELEMENT(submit), "btn-primary");

		htmx_button_set_button_type(cancel, HTMX_BUTTON_BUTTON);
		htmx_element_set_attribute(HTMX_ELEMENT(cancel), "onclick",
			"document.getElementById('contact-dialog').close()");
		htmx_element_add_class(HTMX_ELEMENT(cancel), "btn-secondary");

		htmx_element_add_class(HTMX_ELEMENT(buttons), "dialog-buttons");
		htmx_node_add_child(HTMX_NODE(buttons), HTMX_NODE(cancel));
		htmx_node_add_child(HTMX_NODE(buttons), HTMX_NODE(submit));
	}

	/* Assemble form */
	htmx_element_set_attribute(HTMX_ELEMENT(form), "method", "dialog");
	htmx_node_add_child(HTMX_NODE(form), HTMX_NODE(heading));
	htmx_node_add_child(HTMX_NODE(form), HTMX_NODE(form_group1));
	htmx_node_add_child(HTMX_NODE(form), HTMX_NODE(form_group2));
	htmx_node_add_child(HTMX_NODE(form), HTMX_NODE(buttons));

	htmx_node_add_child(HTMX_NODE(dialog), HTMX_NODE(form));

	return htmx_element_render(HTMX_ELEMENT(dialog));
}

/*
 * Render a context menu using HtmxMenu
 */
static gchar *
render_actions_menu(void)
{
	g_autoptr(HtmxMenu) menu = htmx_menu_new();
	g_autoptr(HtmxLi) item1 = htmx_li_new();
	g_autoptr(HtmxLi) item2 = htmx_li_new();
	g_autoptr(HtmxLi) item3 = htmx_li_new();

	htmx_element_set_id(HTMX_ELEMENT(menu), "actions-menu");
	htmx_element_add_class(HTMX_ELEMENT(menu), "dropdown-menu");

	{
		g_autoptr(HtmxButton) btn = htmx_button_new_with_label("Expand All");
		htmx_element_set_attribute(HTMX_ELEMENT(btn), "onclick",
			"document.querySelectorAll('details').forEach(d => d.open = true)");
		htmx_node_add_child(HTMX_NODE(item1), HTMX_NODE(btn));
	}

	{
		g_autoptr(HtmxButton) btn = htmx_button_new_with_label("Collapse All");
		htmx_element_set_attribute(HTMX_ELEMENT(btn), "onclick",
			"document.querySelectorAll('details').forEach(d => d.open = false)");
		htmx_node_add_child(HTMX_NODE(item2), HTMX_NODE(btn));
	}

	{
		g_autoptr(HtmxButton) btn = htmx_button_new_with_label("Contact Support");
		htmx_element_set_attribute(HTMX_ELEMENT(btn), "onclick",
			"document.getElementById('contact-dialog').showModal()");
		htmx_node_add_child(HTMX_NODE(item3), HTMX_NODE(btn));
	}

	htmx_node_add_child(HTMX_NODE(menu), HTMX_NODE(item1));
	htmx_node_add_child(HTMX_NODE(menu), HTMX_NODE(item2));
	htmx_node_add_child(HTMX_NODE(menu), HTMX_NODE(item3));

	return htmx_element_render(HTMX_ELEMENT(menu));
}

/*
 * Render the complete page
 */
static gchar *
render_page(void)
{
	g_autoptr(HtmxBuilder) builder = htmx_builder_new();
	g_autofree gchar *faq_html = render_faq_list();
	g_autofree gchar *glossary_html = render_glossary();
	g_autofree gchar *dialog_html = render_contact_dialog();
	g_autofree gchar *menu_html = render_actions_menu();

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
	htmx_builder_text(builder, "FAQ - htmx-glib");
	htmx_builder_end(builder);

	htmx_builder_begin(builder, "script");
	htmx_builder_attr(builder, "src", "https://unpkg.com/htmx.org@1.9.10");
	htmx_builder_end(builder);

	htmx_builder_begin(builder, "style");
	htmx_builder_text(builder,
		"* { box-sizing: border-box; }\n"
		"body { font-family: system-ui, sans-serif; line-height: 1.6; max-width: 800px; margin: 0 auto; padding: 1em; }\n"
		"h1 { color: #2c3e50; }\n"
		"h2 { color: #34495e; border-bottom: 2px solid #3498db; padding-bottom: 0.5em; margin-top: 2em; }\n"
		".toolbar { margin-bottom: 1em; display: flex; gap: 0.5em; }\n"
		".toolbar button { padding: 0.5em 1em; background: #3498db; color: white; border: none; border-radius: 4px; cursor: pointer; }\n"
		".toolbar button:hover { background: #2980b9; }\n"
		".faq-item { margin-bottom: 0.5em; border: 1px solid #e0e0e0; border-radius: 4px; }\n"
		".faq-item summary { padding: 1em; cursor: pointer; display: flex; justify-content: space-between; align-items: center; }\n"
		".faq-item summary:hover { background: #f5f5f5; }\n"
		".faq-item[open] summary { border-bottom: 1px solid #e0e0e0; background: #f0f0f0; }\n"
		".category-badge { background: #e9ecef; color: #495057; padding: 0.25em 0.5em; border-radius: 3px; font-size: 0.8em; }\n"
		".faq-content { padding: 1em; background: #fafafa; }\n"
		".glossary { margin: 0; }\n"
		".glossary dt { font-weight: bold; color: #2c3e50; margin-top: 1em; }\n"
		".glossary dd { margin-left: 0; padding-left: 1em; border-left: 3px solid #3498db; }\n"
		".glossary abbr { text-decoration: none; border-bottom: 1px dotted #666; }\n"
		".modal { border: none; border-radius: 8px; padding: 0; max-width: 500px; box-shadow: 0 4px 20px rgba(0,0,0,0.15); }\n"
		".modal::backdrop { background: rgba(0,0,0,0.5); }\n"
		".modal form { padding: 1.5em; }\n"
		".modal h3 { margin: 0 0 1em; }\n"
		".form-group { margin-bottom: 1em; }\n"
		".form-group label { display: block; margin-bottom: 0.25em; font-weight: 500; }\n"
		".form-group input, .form-group textarea { width: 100%; padding: 0.5em; border: 1px solid #ccc; border-radius: 4px; }\n"
		".dialog-buttons { display: flex; justify-content: flex-end; gap: 0.5em; margin-top: 1em; }\n"
		".btn-primary { background: #3498db; color: white; border: none; padding: 0.5em 1em; border-radius: 4px; cursor: pointer; }\n"
		".btn-secondary { background: #95a5a6; color: white; border: none; padding: 0.5em 1em; border-radius: 4px; cursor: pointer; }\n"
		".dropdown-menu { list-style: none; padding: 0.5em 0; margin: 0; background: white; border: 1px solid #ddd; border-radius: 4px; box-shadow: 0 2px 8px rgba(0,0,0,0.1); display: inline-block; }\n"
		".dropdown-menu li { margin: 0; }\n"
		".dropdown-menu button { display: block; width: 100%; text-align: left; padding: 0.5em 1em; background: none; border: none; cursor: pointer; }\n"
		".dropdown-menu button:hover { background: #f0f0f0; }\n"
	);
	htmx_builder_end(builder);

	htmx_builder_end(builder); /* head */

	/* Body */
	htmx_builder_begin(builder, "body");

	htmx_builder_begin(builder, "header");
	htmx_builder_begin(builder, "h1");
	htmx_builder_text(builder, "htmx-glib FAQ & Glossary");
	htmx_builder_end(builder);
	htmx_builder_end(builder);

	/* Toolbar with actions menu */
	htmx_builder_begin(builder, "div");
	htmx_builder_class(builder, "toolbar");
	htmx_builder_raw_html(builder, menu_html);
	htmx_builder_end(builder);

	/* FAQ section */
	htmx_builder_raw_html(builder, faq_html);

	/* Glossary section */
	htmx_builder_raw_html(builder, glossary_html);

	/* Contact dialog */
	htmx_builder_raw_html(builder, dialog_html);

	htmx_builder_end(builder); /* body */
	htmx_builder_end(builder); /* html */

	return htmx_builder_render(builder);
}

/*
 * Handle GET / - Show the main page
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

	g_print("FAQ Accordion Example\n");
	g_print("Server running at http://localhost:%u\n", port);
	g_print("Press Ctrl+C to stop\n\n");

	htmx_server_run(server);

	return 0;
}
