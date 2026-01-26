/*
 * blog-layout.c - Blog layout example with semantic elements
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * Demonstrates htmx-glib semantic container elements:
 * - HtmxArticle, HtmxSection, HtmxNav
 * - HtmxHeader, HtmxFooter, HtmxAside
 * - HtmxMain, HtmxFigure, HtmxFigcaption
 * - HtmxAddress
 * - Proper HTML5 document structure
 */

#include <htmx-glib.h>
#include <stdio.h>

/* Blog post data */
typedef struct {
	guint        id;
	const gchar *title;
	const gchar *author;
	const gchar *date;
	const gchar *summary;
	const gchar *content;
	const gchar *image_url;
	const gchar *image_caption;
} BlogPost;

static BlogPost posts[] = {
	{
		1,
		"Getting Started with htmx-glib",
		"Alice Johnson",
		"2026-01-20",
		"Learn how to build modern web applications using htmx-glib and C.",
		"htmx-glib is a powerful library that brings HTMX capabilities to C applications. "
		"With GObject-based design, it provides clean APIs for building dynamic web interfaces "
		"without writing JavaScript. This post covers the basics of setting up your first project.",
		"/images/htmx-intro.jpg",
		"The htmx-glib architecture diagram"
	},
	{
		2,
		"Building Real-time Features with SSE",
		"Bob Smith",
		"2026-01-18",
		"Implement live updates using Server-Sent Events in htmx-glib.",
		"Server-Sent Events provide an efficient way to push updates from server to client. "
		"htmx-glib's SSE extension makes it easy to add real-time features like notifications, "
		"live feeds, and progress indicators to your applications.",
		"/images/sse-demo.jpg",
		"Real-time data flowing to the browser"
	},
	{
		3,
		"Advanced Routing Patterns",
		"Carol Williams",
		"2026-01-15",
		"Master URL routing and parameter handling in htmx-glib.",
		"Effective routing is crucial for any web application. This post explores htmx-glib's "
		"routing capabilities, including path parameters, query strings, and middleware patterns "
		"that help you build well-organized applications.",
		"/images/routing.jpg",
		"Route matching visualization"
	},
};

static const guint num_posts = G_N_ELEMENTS(posts);

/*
 * Render site navigation using HtmxNav
 */
static HtmxNav *
render_navigation(void)
{
	g_autoptr(HtmxNav) nav = htmx_nav_new();
	g_autoptr(HtmxUl) nav_list = htmx_ul_new();

	htmx_element_set_attribute(HTMX_ELEMENT(nav), "aria-label", "Main navigation");

	/* Navigation links */
	{
		const gchar *nav_items[][2] = {
			{ "/", "Home" },
			{ "/posts", "Articles" },
			{ "/about", "About" },
			{ "/contact", "Contact" },
		};
		guint i;

		for (i = 0; i < G_N_ELEMENTS(nav_items); i++) {
			g_autoptr(HtmxLi) item = htmx_li_new();
			g_autoptr(HtmxA) link = htmx_a_new_with_href(nav_items[i][0]);
			htmx_node_set_text_content(HTMX_NODE(link), nav_items[i][1]);

			/* HTMX-powered navigation */
			htmx_element_set_attribute(HTMX_ELEMENT(link), "hx-get", nav_items[i][0]);
			htmx_element_set_attribute(HTMX_ELEMENT(link), "hx-target", "#main-content");
			htmx_element_set_attribute(HTMX_ELEMENT(link), "hx-push-url", "true");

			htmx_node_add_child(HTMX_NODE(item), HTMX_NODE(link));
			htmx_node_add_child(HTMX_NODE(nav_list), HTMX_NODE(item));
		}
	}

	htmx_node_add_child(HTMX_NODE(nav), HTMX_NODE(nav_list));

	return (HtmxNav *)g_steal_pointer(&nav);
}

/*
 * Render site header using HtmxHeader
 */
static HtmxHeader *
render_site_header(void)
{
	g_autoptr(HtmxHeader) header = htmx_header_new();
	g_autoptr(HtmxDiv) container = htmx_div_new();
	g_autoptr(HtmxHeading) title = htmx_h1_new();
	g_autoptr(HtmxP) tagline = htmx_p_new();

	htmx_element_add_class(HTMX_ELEMENT(header), "site-header");
	htmx_element_add_class(HTMX_ELEMENT(container), "header-content");

	htmx_node_set_text_content(HTMX_NODE(title), "htmx-glib Blog");
	htmx_element_add_class(HTMX_ELEMENT(title), "site-title");

	htmx_node_set_text_content(HTMX_NODE(tagline), "Building modern web apps with C");
	htmx_element_add_class(HTMX_ELEMENT(tagline), "tagline");

	htmx_node_add_child(HTMX_NODE(container), HTMX_NODE(title));
	htmx_node_add_child(HTMX_NODE(container), HTMX_NODE(tagline));

	/* Add navigation */
	{
		g_autoptr(HtmxNav) nav = render_navigation();
		htmx_node_add_child(HTMX_NODE(container), HTMX_NODE(nav));
	}

	htmx_node_add_child(HTMX_NODE(header), HTMX_NODE(container));

	return (HtmxHeader *)g_steal_pointer(&header);
}

/*
 * Render a blog article using HtmxArticle
 */
static HtmxArticle *
render_article(BlogPost *post, gboolean full_content)
{
	g_autoptr(HtmxArticle) article = htmx_article_new();
	g_autoptr(HtmxHeader) article_header = htmx_header_new();
	g_autoptr(HtmxHeading) title = htmx_h2_new();
	g_autoptr(HtmxP) meta = htmx_p_new();
	g_autoptr(HtmxTime) pub_time = htmx_time_new_with_datetime(post->date, post->date);

	htmx_element_add_class(HTMX_ELEMENT(article), "blog-post");
	{
		g_autofree gchar *id = g_strdup_printf("post-%u", post->id);
		htmx_element_set_id(HTMX_ELEMENT(article), id);
	}

	/* Article header with title and metadata */
	htmx_node_set_text_content(HTMX_NODE(title), post->title);
	htmx_element_add_class(HTMX_ELEMENT(title), "post-title");

	htmx_element_add_class(HTMX_ELEMENT(meta), "post-meta");

	/* Build meta line: "By Author on Date" */
	{
		g_autoptr(HtmxSpan) by_text = htmx_span_new();
		g_autoptr(HtmxStrong) author = htmx_strong_new_with_text(post->author);
		g_autoptr(HtmxSpan) on_text = htmx_span_new();

		htmx_node_set_text_content(HTMX_NODE(by_text), "By ");
		htmx_node_set_text_content(HTMX_NODE(on_text), " on ");

		htmx_node_add_child(HTMX_NODE(meta), HTMX_NODE(by_text));
		htmx_node_add_child(HTMX_NODE(meta), HTMX_NODE(author));
		htmx_node_add_child(HTMX_NODE(meta), HTMX_NODE(on_text));
		htmx_node_add_child(HTMX_NODE(meta), HTMX_NODE(pub_time));
	}

	htmx_node_add_child(HTMX_NODE(article_header), HTMX_NODE(title));
	htmx_node_add_child(HTMX_NODE(article_header), HTMX_NODE(meta));
	htmx_node_add_child(HTMX_NODE(article), HTMX_NODE(article_header));

	/* Figure with image */
	if (post->image_url != NULL) {
		g_autoptr(HtmxFigure) figure = htmx_figure_new();
		g_autoptr(HtmxImg) img = htmx_img_new_with_src(post->image_url, post->title);
		g_autoptr(HtmxFigcaption) caption = htmx_figcaption_new_with_text(post->image_caption);

		htmx_element_add_class(HTMX_ELEMENT(figure), "post-image");
		htmx_img_set_loading(img, "lazy");

		htmx_node_add_child(HTMX_NODE(figure), HTMX_NODE(img));
		htmx_node_add_child(HTMX_NODE(figure), HTMX_NODE(caption));
		htmx_node_add_child(HTMX_NODE(article), HTMX_NODE(figure));
	}

	/* Article content */
	if (full_content) {
		g_autoptr(HtmxSection) content_section = htmx_section_new();
		g_autoptr(HtmxP) content = htmx_p_new();

		htmx_element_add_class(HTMX_ELEMENT(content_section), "post-content");
		htmx_node_set_text_content(HTMX_NODE(content), post->content);

		htmx_node_add_child(HTMX_NODE(content_section), HTMX_NODE(content));
		htmx_node_add_child(HTMX_NODE(article), HTMX_NODE(content_section));
	} else {
		/* Summary with read more link */
		g_autoptr(HtmxP) summary = htmx_p_new();
		g_autofree gchar *post_url = g_strdup_printf("/posts/%u", post->id);
		g_autoptr(HtmxA) read_more = htmx_a_new_with_href(post_url);
		htmx_node_set_text_content(HTMX_NODE(read_more), "Read more...");

		htmx_element_add_class(HTMX_ELEMENT(summary), "post-summary");
		htmx_node_set_text_content(HTMX_NODE(summary), post->summary);

		htmx_element_add_class(HTMX_ELEMENT(read_more), "read-more");
		htmx_element_set_attribute(HTMX_ELEMENT(read_more), "hx-get", post_url);
		htmx_element_set_attribute(HTMX_ELEMENT(read_more), "hx-target", "#main-content");
		htmx_element_set_attribute(HTMX_ELEMENT(read_more), "hx-push-url", "true");

		htmx_node_add_child(HTMX_NODE(article), HTMX_NODE(summary));
		htmx_node_add_child(HTMX_NODE(article), HTMX_NODE(read_more));
	}

	return (HtmxArticle *)g_steal_pointer(&article);
}

/*
 * Render the sidebar using HtmxAside
 */
static HtmxAside *
render_sidebar(void)
{
	g_autoptr(HtmxAside) aside = htmx_aside_new();
	g_autoptr(HtmxSection) about_section = htmx_section_new();
	g_autoptr(HtmxSection) tags_section = htmx_section_new();

	htmx_element_add_class(HTMX_ELEMENT(aside), "sidebar");

	/* About section */
	{
		g_autoptr(HtmxHeading) heading = htmx_h3_new();
		g_autoptr(HtmxP) about_text = htmx_p_new();

		htmx_node_set_text_content(HTMX_NODE(heading), "About");
		htmx_node_set_text_content(HTMX_NODE(about_text),
			"This blog explores htmx-glib development, sharing tips, tutorials, "
			"and best practices for building web applications with C.");

		htmx_element_add_class(HTMX_ELEMENT(about_section), "sidebar-section");
		htmx_node_add_child(HTMX_NODE(about_section), HTMX_NODE(heading));
		htmx_node_add_child(HTMX_NODE(about_section), HTMX_NODE(about_text));
	}

	/* Tags section */
	{
		g_autoptr(HtmxHeading) heading = htmx_h3_new();
		g_autoptr(HtmxUl) tags_list = htmx_ul_new();
		const gchar *tags[] = { "htmx", "c-programming", "gobject", "web-development", "sse" };
		guint i;

		htmx_node_set_text_content(HTMX_NODE(heading), "Popular Tags");
		htmx_element_add_class(HTMX_ELEMENT(tags_list), "tag-list");

		for (i = 0; i < G_N_ELEMENTS(tags); i++) {
			g_autoptr(HtmxLi) item = htmx_li_new();
			g_autofree gchar *tag_url = g_strdup_printf("/tags/%s", tags[i]);
			g_autoptr(HtmxA) link = htmx_a_new_with_href(tag_url);
			htmx_node_set_text_content(HTMX_NODE(link), tags[i]);
			htmx_element_add_class(HTMX_ELEMENT(link), "tag");
			htmx_node_add_child(HTMX_NODE(item), HTMX_NODE(link));
			htmx_node_add_child(HTMX_NODE(tags_list), HTMX_NODE(item));
		}

		htmx_element_add_class(HTMX_ELEMENT(tags_section), "sidebar-section");
		htmx_node_add_child(HTMX_NODE(tags_section), HTMX_NODE(heading));
		htmx_node_add_child(HTMX_NODE(tags_section), HTMX_NODE(tags_list));
	}

	htmx_node_add_child(HTMX_NODE(aside), HTMX_NODE(about_section));
	htmx_node_add_child(HTMX_NODE(aside), HTMX_NODE(tags_section));

	return (HtmxAside *)g_steal_pointer(&aside);
}

/*
 * Render the site footer using HtmxFooter
 */
static HtmxFooter *
render_site_footer(void)
{
	g_autoptr(HtmxFooter) footer = htmx_footer_new();
	g_autoptr(HtmxDiv) container = htmx_div_new();
	g_autoptr(HtmxP) copyright = htmx_p_new();
	g_autoptr(HtmxAddress) contact = htmx_address_new();

	htmx_element_add_class(HTMX_ELEMENT(footer), "site-footer");
	htmx_element_add_class(HTMX_ELEMENT(container), "footer-content");

	htmx_node_set_text_content(HTMX_NODE(copyright),
		"© 2026 htmx-glib Blog. Licensed under AGPL-3.0-or-later.");
	htmx_element_add_class(HTMX_ELEMENT(copyright), "copyright");

	/* Contact info in address element */
	{
		g_autoptr(HtmxA) email = htmx_a_new_with_href("mailto:blog@example.com");
		g_autoptr(HtmxSpan) separator = htmx_span_new();
		g_autoptr(HtmxA) github = htmx_a_new_with_href("https://gitlab.com/htmx-glib/htmx-glib");

		htmx_node_set_text_content(HTMX_NODE(email), "blog@example.com");
		htmx_node_set_text_content(HTMX_NODE(github), "GitLab");
		htmx_node_set_text_content(HTMX_NODE(separator), " | ");

		htmx_node_add_child(HTMX_NODE(contact), HTMX_NODE(email));
		htmx_node_add_child(HTMX_NODE(contact), HTMX_NODE(separator));
		htmx_node_add_child(HTMX_NODE(contact), HTMX_NODE(github));
	}

	htmx_node_add_child(HTMX_NODE(container), HTMX_NODE(copyright));
	htmx_node_add_child(HTMX_NODE(container), HTMX_NODE(contact));
	htmx_node_add_child(HTMX_NODE(footer), HTMX_NODE(container));

	return (HtmxFooter *)g_steal_pointer(&footer);
}

/*
 * Render the main content area
 */
static gchar *
render_main_content(void)
{
	g_autoptr(HtmxMain) main_content = htmx_main_new();
	g_autoptr(HtmxDiv) layout = htmx_div_new();
	g_autoptr(HtmxSection) posts_section = htmx_section_new();

	htmx_element_set_id(HTMX_ELEMENT(main_content), "main-content");
	htmx_element_add_class(HTMX_ELEMENT(layout), "main-layout");
	htmx_element_add_class(HTMX_ELEMENT(posts_section), "posts");

	/* Add blog posts */
	{
		guint i;
		for (i = 0; i < num_posts; i++) {
			g_autoptr(HtmxArticle) article = render_article(&posts[i], FALSE);
			htmx_node_add_child(HTMX_NODE(posts_section), HTMX_NODE(article));
		}
	}

	/* Add sidebar */
	{
		g_autoptr(HtmxAside) sidebar = render_sidebar();
		htmx_node_add_child(HTMX_NODE(layout), HTMX_NODE(posts_section));
		htmx_node_add_child(HTMX_NODE(layout), HTMX_NODE(sidebar));
	}

	htmx_node_add_child(HTMX_NODE(main_content), HTMX_NODE(layout));

	return htmx_element_render(HTMX_ELEMENT(main_content));
}

/*
 * Render the complete page
 */
static gchar *
render_page(void)
{
	g_autoptr(HtmxBuilder) builder = htmx_builder_new();

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
	htmx_builder_text(builder, "htmx-glib Blog");
	htmx_builder_end(builder);

	htmx_builder_begin(builder, "script");
	htmx_builder_attr(builder, "src", "https://unpkg.com/htmx.org@1.9.10");
	htmx_builder_end(builder);

	htmx_builder_begin(builder, "style");
	htmx_builder_text(builder,
		"* { box-sizing: border-box; }\n"
		"body { font-family: Georgia, serif; line-height: 1.6; margin: 0; color: #333; }\n"
		".site-header { background: #2c3e50; color: white; padding: 2em 0; }\n"
		".header-content { max-width: 1100px; margin: 0 auto; padding: 0 1em; }\n"
		".site-title { margin: 0; font-size: 2em; }\n"
		".tagline { margin: 0.5em 0; opacity: 0.8; font-style: italic; }\n"
		"nav ul { list-style: none; padding: 0; margin: 1em 0 0; display: flex; gap: 1.5em; }\n"
		"nav a { color: white; text-decoration: none; }\n"
		"nav a:hover { text-decoration: underline; }\n"
		".main-layout { max-width: 1100px; margin: 2em auto; padding: 0 1em; display: grid; grid-template-columns: 1fr 300px; gap: 2em; }\n"
		".blog-post { margin-bottom: 2em; padding-bottom: 2em; border-bottom: 1px solid #eee; }\n"
		".post-title { font-size: 1.5em; margin: 0; }\n"
		".post-meta { color: #666; font-size: 0.9em; margin: 0.5em 0; }\n"
		".post-image { margin: 1em 0; }\n"
		".post-image img { max-width: 100%; height: auto; }\n"
		".post-image figcaption { font-size: 0.9em; color: #666; font-style: italic; margin-top: 0.5em; }\n"
		".post-summary { color: #555; }\n"
		".read-more { color: #3498db; }\n"
		".back-link { color: #3498db; text-decoration: none; }\n"
		".back-link:hover { text-decoration: underline; }\n"
		".page-title { margin-bottom: 1em; }\n"
		".sidebar { background: #f9f9f9; padding: 1em; border-radius: 4px; }\n"
		".sidebar-section { margin-bottom: 1.5em; }\n"
		".sidebar-section h3 { margin: 0 0 0.5em; font-size: 1.1em; }\n"
		".tag-list { list-style: none; padding: 0; display: flex; flex-wrap: wrap; gap: 0.5em; }\n"
		".tag { background: #e9ecef; padding: 0.25em 0.5em; border-radius: 3px; font-size: 0.9em; text-decoration: none; color: #495057; }\n"
		".tag:hover { background: #dee2e6; }\n"
		".site-footer { background: #2c3e50; color: white; padding: 2em 0; margin-top: 2em; }\n"
		".footer-content { max-width: 1100px; margin: 0 auto; padding: 0 1em; text-align: center; }\n"
		".copyright { margin: 0 0 0.5em; }\n"
		"address { font-style: normal; }\n"
		"address a { color: #3498db; }\n"
		"@media (max-width: 768px) { .main-layout { grid-template-columns: 1fr; } }\n"
	);
	htmx_builder_end(builder);

	htmx_builder_end(builder); /* head */

	/* Body */
	htmx_builder_begin(builder, "body");

	/* Site header */
	{
		g_autoptr(HtmxHeader) header = render_site_header();
		g_autofree gchar *header_html = htmx_element_render(HTMX_ELEMENT(header));
		htmx_builder_raw_html(builder, header_html);
	}

	/* Main content */
	{
		g_autofree gchar *main_html = render_main_content();
		htmx_builder_raw_html(builder, main_html);
	}

	/* Site footer */
	{
		g_autoptr(HtmxFooter) footer = render_site_footer();
		g_autofree gchar *footer_html = htmx_element_render(HTMX_ELEMENT(footer));
		htmx_builder_raw_html(builder, footer_html);
	}

	htmx_builder_end(builder); /* body */
	htmx_builder_end(builder); /* html */

	return htmx_builder_render(builder);
}

/*
 * Handle GET / - Show the main page
 *
 * Returns full page for initial load, or just main content for HTMX requests
 */
static HtmxResponse *
handle_index(HtmxRequest *request, GHashTable *params, gpointer user_data)
{
	g_autoptr(HtmxResponse) response = htmx_response_new();
	g_autofree gchar *html = NULL;

	/* Check if this is an HTMX request (clicking nav link) */
	if (htmx_request_is_htmx(request)) {
		/* Return just the main content for swap into #main-content */
		g_autoptr(HtmxDiv) layout = htmx_div_new();
		g_autoptr(HtmxSection) posts_section = htmx_section_new();
		guint i;

		htmx_element_add_class(HTMX_ELEMENT(layout), "main-layout");
		htmx_element_add_class(HTMX_ELEMENT(posts_section), "posts");

		/* Add blog posts */
		for (i = 0; i < num_posts; i++) {
			g_autoptr(HtmxArticle) article = render_article(&posts[i], FALSE);
			htmx_node_add_child(HTMX_NODE(posts_section), HTMX_NODE(article));
		}

		/* Add sidebar */
		{
			g_autoptr(HtmxAside) sidebar = render_sidebar();
			htmx_node_add_child(HTMX_NODE(layout), HTMX_NODE(posts_section));
			htmx_node_add_child(HTMX_NODE(layout), HTMX_NODE(sidebar));
		}

		html = htmx_element_render(HTMX_ELEMENT(layout));
	} else {
		/* Full page load - return complete HTML document */
		html = render_page();
	}

	htmx_response_set_content(response, html);
	htmx_response_set_content_type(response, "text/html; charset=utf-8");

	return (HtmxResponse *)g_steal_pointer(&response);
}

/*
 * Handle GET /posts - Articles listing
 *
 * Returns partial HTML for HTMX swap into #main-content
 */
static HtmxResponse *
handle_posts(HtmxRequest *request, GHashTable *params, gpointer user_data)
{
	g_autoptr(HtmxResponse) response = htmx_response_new();
	g_autoptr(HtmxDiv) layout = htmx_div_new();
	g_autoptr(HtmxSection) posts_section = htmx_section_new();
	g_autofree gchar *html = NULL;
	guint i;

	htmx_element_add_class(HTMX_ELEMENT(layout), "main-layout");
	htmx_element_add_class(HTMX_ELEMENT(posts_section), "posts");

	/* Add heading */
	{
		g_autoptr(HtmxHeading) heading = htmx_h1_new();
		htmx_node_set_text_content(HTMX_NODE(heading), "All Articles");
		htmx_element_add_class(HTMX_ELEMENT(heading), "page-title");
		htmx_node_add_child(HTMX_NODE(posts_section), HTMX_NODE(heading));
	}

	/* Add all blog posts */
	for (i = 0; i < num_posts; i++) {
		g_autoptr(HtmxArticle) article = render_article(&posts[i], FALSE);
		htmx_node_add_child(HTMX_NODE(posts_section), HTMX_NODE(article));
	}

	/* Add sidebar */
	{
		g_autoptr(HtmxAside) sidebar = render_sidebar();
		htmx_node_add_child(HTMX_NODE(layout), HTMX_NODE(posts_section));
		htmx_node_add_child(HTMX_NODE(layout), HTMX_NODE(sidebar));
	}

	html = htmx_element_render(HTMX_ELEMENT(layout));
	htmx_response_set_content(response, html);
	htmx_response_set_content_type(response, "text/html; charset=utf-8");

	return (HtmxResponse *)g_steal_pointer(&response);
}

/*
 * Handle GET /posts/:id - Single post with full content
 *
 * Looks up post by ID from params and renders the full article
 */
static HtmxResponse *
handle_post(HtmxRequest *request, GHashTable *params, gpointer user_data)
{
	g_autoptr(HtmxResponse) response = htmx_response_new();
	const gchar *id_str;
	guint id;
	guint i;
	BlogPost *post = NULL;

	/* Extract post ID from params */
	id_str = g_hash_table_lookup(params, "id");
	if (id_str == NULL) {
		htmx_response_set_status(response, 404);
		htmx_response_set_content(response, "<p>Post not found</p>");
		htmx_response_set_content_type(response, "text/html; charset=utf-8");
		return (HtmxResponse *)g_steal_pointer(&response);
	}

	id = (guint)g_ascii_strtoull(id_str, NULL, 10);

	/* Find the post */
	for (i = 0; i < num_posts; i++) {
		if (posts[i].id == id) {
			post = &posts[i];
			break;
		}
	}

	if (post == NULL) {
		htmx_response_set_status(response, 404);
		htmx_response_set_content(response, "<p>Post not found</p>");
		htmx_response_set_content_type(response, "text/html; charset=utf-8");
		return (HtmxResponse *)g_steal_pointer(&response);
	}

	/* Render the full article */
	{
		g_autoptr(HtmxDiv) layout = htmx_div_new();
		g_autoptr(HtmxSection) content_section = htmx_section_new();
		g_autoptr(HtmxArticle) article = render_article(post, TRUE);
		g_autoptr(HtmxAside) sidebar = render_sidebar();
		g_autofree gchar *html = NULL;

		htmx_element_add_class(HTMX_ELEMENT(layout), "main-layout");
		htmx_element_add_class(HTMX_ELEMENT(content_section), "posts");

		/* Add back link */
		{
			g_autoptr(HtmxP) back_nav = htmx_p_new();
			g_autoptr(HtmxA) back_link = htmx_a_new_with_href("/posts");

			htmx_node_set_text_content(HTMX_NODE(back_link), "← Back to Articles");
			htmx_element_set_attribute(HTMX_ELEMENT(back_link), "hx-get", "/posts");
			htmx_element_set_attribute(HTMX_ELEMENT(back_link), "hx-target", "#main-content");
			htmx_element_set_attribute(HTMX_ELEMENT(back_link), "hx-push-url", "true");
			htmx_element_add_class(HTMX_ELEMENT(back_link), "back-link");

			htmx_node_add_child(HTMX_NODE(back_nav), HTMX_NODE(back_link));
			htmx_node_add_child(HTMX_NODE(content_section), HTMX_NODE(back_nav));
		}

		htmx_node_add_child(HTMX_NODE(content_section), HTMX_NODE(article));
		htmx_node_add_child(HTMX_NODE(layout), HTMX_NODE(content_section));
		htmx_node_add_child(HTMX_NODE(layout), HTMX_NODE(sidebar));

		html = htmx_element_render(HTMX_ELEMENT(layout));
		htmx_response_set_content(response, html);
		htmx_response_set_content_type(response, "text/html; charset=utf-8");
	}

	return (HtmxResponse *)g_steal_pointer(&response);
}

/*
 * Handle GET /about - About page
 *
 * Renders placeholder about page content
 */
static HtmxResponse *
handle_about(HtmxRequest *request, GHashTable *params, gpointer user_data)
{
	g_autoptr(HtmxResponse) response = htmx_response_new();
	g_autoptr(HtmxDiv) layout = htmx_div_new();
	g_autoptr(HtmxSection) content_section = htmx_section_new();
	g_autofree gchar *html = NULL;

	htmx_element_add_class(HTMX_ELEMENT(layout), "main-layout");
	htmx_element_add_class(HTMX_ELEMENT(content_section), "posts");

	/* About content */
	{
		g_autoptr(HtmxArticle) article = htmx_article_new();
		g_autoptr(HtmxHeading) heading = htmx_h1_new();
		g_autoptr(HtmxP) intro = htmx_p_new();
		g_autoptr(HtmxHeading) mission_heading = htmx_h2_new();
		g_autoptr(HtmxP) mission = htmx_p_new();
		g_autoptr(HtmxHeading) tech_heading = htmx_h2_new();
		g_autoptr(HtmxP) tech = htmx_p_new();

		htmx_element_add_class(HTMX_ELEMENT(article), "about-page");

		htmx_node_set_text_content(HTMX_NODE(heading), "About htmx-glib Blog");
		htmx_element_add_class(HTMX_ELEMENT(heading), "page-title");

		htmx_node_set_text_content(HTMX_NODE(intro),
			"Welcome to the htmx-glib blog! This is a demonstration of building "
			"modern, interactive web applications using htmx-glib and C. We believe "
			"in the power of hypermedia-driven applications and the simplicity of "
			"server-side rendering.");

		htmx_node_set_text_content(HTMX_NODE(mission_heading), "Our Mission");
		htmx_node_set_text_content(HTMX_NODE(mission),
			"To demonstrate that you can build rich, interactive web experiences "
			"without complex JavaScript frameworks. htmx-glib brings the power of "
			"HTMX to C developers, enabling rapid development of dynamic web "
			"applications with familiar GObject patterns.");

		htmx_node_set_text_content(HTMX_NODE(tech_heading), "Technology Stack");
		htmx_node_set_text_content(HTMX_NODE(tech),
			"This blog is built entirely with htmx-glib, showcasing semantic HTML5 "
			"elements, HTMX-powered navigation, and responsive design. No JavaScript "
			"frameworks were harmed in the making of this website.");

		htmx_node_add_child(HTMX_NODE(article), HTMX_NODE(heading));
		htmx_node_add_child(HTMX_NODE(article), HTMX_NODE(intro));
		htmx_node_add_child(HTMX_NODE(article), HTMX_NODE(mission_heading));
		htmx_node_add_child(HTMX_NODE(article), HTMX_NODE(mission));
		htmx_node_add_child(HTMX_NODE(article), HTMX_NODE(tech_heading));
		htmx_node_add_child(HTMX_NODE(article), HTMX_NODE(tech));

		htmx_node_add_child(HTMX_NODE(content_section), HTMX_NODE(article));
	}

	/* Add sidebar */
	{
		g_autoptr(HtmxAside) sidebar = render_sidebar();
		htmx_node_add_child(HTMX_NODE(layout), HTMX_NODE(content_section));
		htmx_node_add_child(HTMX_NODE(layout), HTMX_NODE(sidebar));
	}

	html = htmx_element_render(HTMX_ELEMENT(layout));
	htmx_response_set_content(response, html);
	htmx_response_set_content_type(response, "text/html; charset=utf-8");

	return (HtmxResponse *)g_steal_pointer(&response);
}

/*
 * Handle GET /contact - Contact page
 *
 * Renders placeholder contact page content
 */
static HtmxResponse *
handle_contact(HtmxRequest *request, GHashTable *params, gpointer user_data)
{
	g_autoptr(HtmxResponse) response = htmx_response_new();
	g_autoptr(HtmxDiv) layout = htmx_div_new();
	g_autoptr(HtmxSection) content_section = htmx_section_new();
	g_autofree gchar *html = NULL;

	htmx_element_add_class(HTMX_ELEMENT(layout), "main-layout");
	htmx_element_add_class(HTMX_ELEMENT(content_section), "posts");

	/* Contact content */
	{
		g_autoptr(HtmxArticle) article = htmx_article_new();
		g_autoptr(HtmxHeading) heading = htmx_h1_new();
		g_autoptr(HtmxP) intro = htmx_p_new();
		g_autoptr(HtmxHeading) email_heading = htmx_h2_new();
		g_autoptr(HtmxP) email_p = htmx_p_new();
		g_autoptr(HtmxA) email_link = htmx_a_new_with_href("mailto:blog@example.com");
		g_autoptr(HtmxHeading) social_heading = htmx_h2_new();
		g_autoptr(HtmxP) social = htmx_p_new();
		g_autoptr(HtmxA) gitlab_link = htmx_a_new_with_href("https://gitlab.com/htmx-glib/htmx-glib");

		htmx_element_add_class(HTMX_ELEMENT(article), "contact-page");

		htmx_node_set_text_content(HTMX_NODE(heading), "Contact Us");
		htmx_element_add_class(HTMX_ELEMENT(heading), "page-title");

		htmx_node_set_text_content(HTMX_NODE(intro),
			"Have questions about htmx-glib? Want to contribute or report a bug? "
			"We would love to hear from you!");

		htmx_node_set_text_content(HTMX_NODE(email_heading), "Email");
		htmx_node_set_text_content(HTMX_NODE(email_link), "blog@example.com");
		htmx_node_add_child(HTMX_NODE(email_p), HTMX_NODE(email_link));

		htmx_node_set_text_content(HTMX_NODE(social_heading), "Source Code");
		htmx_node_set_text_content(HTMX_NODE(gitlab_link), "Visit our GitLab repository");
		htmx_node_add_child(HTMX_NODE(social), HTMX_NODE(gitlab_link));

		htmx_node_add_child(HTMX_NODE(article), HTMX_NODE(heading));
		htmx_node_add_child(HTMX_NODE(article), HTMX_NODE(intro));
		htmx_node_add_child(HTMX_NODE(article), HTMX_NODE(email_heading));
		htmx_node_add_child(HTMX_NODE(article), HTMX_NODE(email_p));
		htmx_node_add_child(HTMX_NODE(article), HTMX_NODE(social_heading));
		htmx_node_add_child(HTMX_NODE(article), HTMX_NODE(social));

		htmx_node_add_child(HTMX_NODE(content_section), HTMX_NODE(article));
	}

	/* Add sidebar */
	{
		g_autoptr(HtmxAside) sidebar = render_sidebar();
		htmx_node_add_child(HTMX_NODE(layout), HTMX_NODE(content_section));
		htmx_node_add_child(HTMX_NODE(layout), HTMX_NODE(sidebar));
	}

	html = htmx_element_render(HTMX_ELEMENT(layout));
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
	htmx_router_get(router, "/posts", handle_posts, NULL);
	htmx_router_get(router, "/posts/:id", handle_post, NULL);
	htmx_router_get(router, "/about", handle_about, NULL);
	htmx_router_get(router, "/contact", handle_contact, NULL);

	g_print("Blog Layout Example\n");
	g_print("Server running at http://localhost:%u\n", port);
	g_print("Press Ctrl+C to stop\n\n");

	htmx_server_run(server);

	return 0;
}
