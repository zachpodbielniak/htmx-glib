/*
 * media-gallery.c - Media gallery with video, audio, and images
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * Demonstrates htmx-glib media elements:
 * - HtmxVideo, HtmxAudio, HtmxSource, HtmxTrack
 * - HtmxPicture, HtmxImg
 * - HtmxIframe, HtmxCanvas
 * - HtmxEmbed, HtmxObject
 * - HtmxFigure, HtmxFigcaption
 */

#include <htmx-glib.h>
#include <stdio.h>

/* Media item types */
typedef enum {
	MEDIA_VIDEO,
	MEDIA_AUDIO,
	MEDIA_IMAGE,
	MEDIA_EMBED
} MediaType;

/* Media item data */
typedef struct {
	guint        id;
	MediaType    type;
	const gchar *title;
	const gchar *description;
	const gchar *url;
	const gchar *poster;     /* For video */
	const gchar *thumbnail;  /* For gallery view */
} MediaItem;

static MediaItem media_items[] = {
	{
		1, MEDIA_VIDEO,
		"Introduction to htmx-glib",
		"A comprehensive introduction to building web applications with htmx-glib.",
		"https://example.com/videos/htmx-intro.mp4",
		"https://example.com/posters/htmx-intro.jpg",
		"https://example.com/thumbs/htmx-intro.jpg"
	},
	{
		2, MEDIA_VIDEO,
		"Building Real-time Apps",
		"Learn how to use SSE and WebSockets for real-time features.",
		"https://example.com/videos/realtime.mp4",
		"https://example.com/posters/realtime.jpg",
		"https://example.com/thumbs/realtime.jpg"
	},
	{
		3, MEDIA_AUDIO,
		"htmx-glib Podcast Episode 1",
		"Discussion about the library architecture and design decisions.",
		"https://example.com/audio/podcast-01.mp3",
		NULL,
		"https://example.com/thumbs/podcast.jpg"
	},
	{
		4, MEDIA_IMAGE,
		"Architecture Diagram",
		"High-level overview of the htmx-glib component architecture.",
		"https://example.com/images/architecture.png",
		NULL,
		"https://example.com/thumbs/architecture.jpg"
	},
	{
		5, MEDIA_EMBED,
		"Live Demo",
		"Interactive demo running on our servers.",
		"https://demo.htmx-glib.example.com/",
		NULL,
		"https://example.com/thumbs/demo.jpg"
	},
};

static const guint num_items = G_N_ELEMENTS(media_items);

/*
 * Render a video player with multiple sources and captions
 */
static HtmxFigure *
render_video_player(MediaItem *item)
{
	g_autoptr(HtmxFigure) figure = htmx_figure_new();
	g_autoptr(HtmxVideo) video = htmx_video_new();
	g_autoptr(HtmxFigcaption) caption = htmx_figcaption_new();

	htmx_element_add_class(HTMX_ELEMENT(figure), "media-item video-item");

	/* Video element with attributes */
	htmx_video_set_controls(video, TRUE);
	htmx_video_set_width(video, 640);
	htmx_video_set_height(video, 360);

	if (item->poster != NULL) {
		htmx_video_set_poster(video, item->poster);
	}

	/* Add multiple source formats for compatibility */
	{
		g_autoptr(HtmxSource) source_mp4 = htmx_source_new_with_src(item->url, "video/mp4");
		htmx_node_add_child(HTMX_NODE(video), HTMX_NODE(source_mp4));

		/* WebM alternative */
		g_autofree gchar *webm_url = g_strdup(item->url);
		gchar *ext = strstr(webm_url, ".mp4");
		if (ext != NULL) {
			strcpy(ext, ".webm");
			g_autoptr(HtmxSource) source_webm = htmx_source_new_with_src(webm_url, "video/webm");
			htmx_node_add_child(HTMX_NODE(video), HTMX_NODE(source_webm));
		}
	}

	/* Add subtitle track */
	{
		g_autoptr(HtmxTrack) subtitles = htmx_track_new();
		htmx_track_set_kind(subtitles, "subtitles");
		htmx_track_set_src(subtitles, g_strdup_printf("%s.vtt", item->url));
		htmx_track_set_srclang(subtitles, "en");
		htmx_track_set_label(subtitles, "English");
		htmx_track_set_default(subtitles, TRUE);
		htmx_node_add_child(HTMX_NODE(video), HTMX_NODE(subtitles));
	}

	/* Fallback text */
	{
		g_autoptr(HtmxP) fallback = htmx_p_new();
		htmx_node_set_text_content(HTMX_NODE(fallback),
			"Your browser doesn't support HTML5 video.");
		htmx_node_add_child(HTMX_NODE(video), HTMX_NODE(fallback));
	}

	/* Caption with title and description */
	{
		g_autoptr(HtmxStrong) title = htmx_strong_new_with_text(item->title);
		g_autoptr(HtmxBr) br = htmx_br_new();
		g_autoptr(HtmxSpan) desc = htmx_span_new();

		htmx_node_set_text_content(HTMX_NODE(desc), item->description);
		htmx_element_add_class(HTMX_ELEMENT(desc), "description");

		htmx_node_add_child(HTMX_NODE(caption), HTMX_NODE(title));
		htmx_node_add_child(HTMX_NODE(caption), HTMX_NODE(br));
		htmx_node_add_child(HTMX_NODE(caption), HTMX_NODE(desc));
	}

	htmx_node_add_child(HTMX_NODE(figure), HTMX_NODE(video));
	htmx_node_add_child(HTMX_NODE(figure), HTMX_NODE(caption));

	return (HtmxFigure *)g_steal_pointer(&figure);
}

/*
 * Render an audio player
 */
static HtmxFigure *
render_audio_player(MediaItem *item)
{
	g_autoptr(HtmxFigure) figure = htmx_figure_new();
	g_autoptr(HtmxDiv) player_container = htmx_div_new();
	g_autoptr(HtmxAudio) audio = htmx_audio_new();
	g_autoptr(HtmxFigcaption) caption = htmx_figcaption_new();

	htmx_element_add_class(HTMX_ELEMENT(figure), "media-item audio-item");
	htmx_element_add_class(HTMX_ELEMENT(player_container), "audio-player");

	/* Thumbnail image */
	if (item->thumbnail != NULL) {
		g_autoptr(HtmxImg) thumb = htmx_img_new_with_src(item->thumbnail, item->title);
		htmx_element_add_class(HTMX_ELEMENT(thumb), "audio-thumbnail");
		htmx_node_add_child(HTMX_NODE(player_container), HTMX_NODE(thumb));
	}

	/* Audio element */
	htmx_audio_set_controls(audio, TRUE);
	htmx_audio_set_preload(audio, "metadata");

	{
		g_autoptr(HtmxSource) source_mp3 = htmx_source_new_with_src(item->url, "audio/mpeg");
		htmx_node_add_child(HTMX_NODE(audio), HTMX_NODE(source_mp3));

		/* OGG alternative */
		g_autofree gchar *ogg_url = g_strdup(item->url);
		gchar *ext = strstr(ogg_url, ".mp3");
		if (ext != NULL) {
			strcpy(ext, ".ogg");
			g_autoptr(HtmxSource) source_ogg = htmx_source_new_with_src(ogg_url, "audio/ogg");
			htmx_node_add_child(HTMX_NODE(audio), HTMX_NODE(source_ogg));
		}
	}

	htmx_node_add_child(HTMX_NODE(player_container), HTMX_NODE(audio));

	/* Caption */
	{
		g_autoptr(HtmxStrong) title = htmx_strong_new_with_text(item->title);
		g_autoptr(HtmxP) desc = htmx_p_new();

		htmx_node_set_text_content(HTMX_NODE(desc), item->description);

		htmx_node_add_child(HTMX_NODE(caption), HTMX_NODE(title));
		htmx_node_add_child(HTMX_NODE(caption), HTMX_NODE(desc));
	}

	htmx_node_add_child(HTMX_NODE(figure), HTMX_NODE(player_container));
	htmx_node_add_child(HTMX_NODE(figure), HTMX_NODE(caption));

	return (HtmxFigure *)g_steal_pointer(&figure);
}

/*
 * Render a responsive image using picture element
 */
static HtmxFigure *
render_responsive_image(MediaItem *item)
{
	g_autoptr(HtmxFigure) figure = htmx_figure_new();
	g_autoptr(HtmxPicture) picture = htmx_picture_new();
	g_autoptr(HtmxFigcaption) caption = htmx_figcaption_new_with_text(item->description);

	htmx_element_add_class(HTMX_ELEMENT(figure), "media-item image-item");

	/* Add responsive source variants */
	{
		/* WebP for modern browsers */
		g_autoptr(HtmxSource) source_webp = htmx_source_new();
		htmx_source_set_srcset(source_webp, g_strdup_printf("%s.webp", item->url));
		htmx_source_set_type(source_webp, "image/webp");
		htmx_node_add_child(HTMX_NODE(picture), HTMX_NODE(source_webp));

		/* Different sizes for different viewports */
		g_autoptr(HtmxSource) source_large = htmx_source_new();
		htmx_source_set_srcset(source_large,
			g_strdup_printf("%s?w=1200 1200w, %s?w=800 800w", item->url, item->url));
		htmx_source_set_sizes(source_large, "(min-width: 1024px) 800px, 100vw");
		htmx_source_set_media(source_large, "(min-width: 768px)");
		htmx_node_add_child(HTMX_NODE(picture), HTMX_NODE(source_large));
	}

	/* Fallback img */
	{
		g_autoptr(HtmxImg) img = htmx_img_new_with_src(item->url, item->title);
		htmx_img_set_loading(img, "lazy");
		htmx_element_add_class(HTMX_ELEMENT(img), "gallery-image");
		htmx_node_add_child(HTMX_NODE(picture), HTMX_NODE(img));
	}

	htmx_node_add_child(HTMX_NODE(figure), HTMX_NODE(picture));
	htmx_node_add_child(HTMX_NODE(figure), HTMX_NODE(caption));

	return (HtmxFigure *)g_steal_pointer(&figure);
}

/*
 * Render an embedded iframe
 */
static HtmxFigure *
render_embed(MediaItem *item)
{
	g_autoptr(HtmxFigure) figure = htmx_figure_new();
	g_autoptr(HtmxIframe) iframe = htmx_iframe_new_with_src(item->url);
	g_autoptr(HtmxFigcaption) caption = htmx_figcaption_new();

	htmx_element_add_class(HTMX_ELEMENT(figure), "media-item embed-item");

	/* Configure iframe */
	htmx_iframe_set_width(iframe, 640);
	htmx_iframe_set_height(iframe, 400);
	htmx_iframe_set_sandbox(iframe, "allow-scripts allow-same-origin");
	htmx_iframe_set_allow(iframe, "fullscreen");
	htmx_element_set_attribute(HTMX_ELEMENT(iframe), "loading", "lazy");

	/* Caption */
	{
		g_autoptr(HtmxStrong) title = htmx_strong_new_with_text(item->title);
		g_autoptr(HtmxP) desc = htmx_p_new();

		htmx_node_set_text_content(HTMX_NODE(desc), item->description);

		htmx_node_add_child(HTMX_NODE(caption), HTMX_NODE(title));
		htmx_node_add_child(HTMX_NODE(caption), HTMX_NODE(desc));
	}

	htmx_node_add_child(HTMX_NODE(figure), HTMX_NODE(iframe));
	htmx_node_add_child(HTMX_NODE(figure), HTMX_NODE(caption));

	return (HtmxFigure *)g_steal_pointer(&figure);
}

/*
 * Render a media item based on its type
 */
static HtmxFigure *
render_media_item(MediaItem *item)
{
	switch (item->type) {
	case MEDIA_VIDEO:
		return render_video_player(item);
	case MEDIA_AUDIO:
		return render_audio_player(item);
	case MEDIA_IMAGE:
		return render_responsive_image(item);
	case MEDIA_EMBED:
		return render_embed(item);
	default:
		return NULL;
	}
}

/*
 * Render the thumbnail gallery
 */
static gchar *
render_gallery(void)
{
	g_autoptr(HtmxSection) gallery = htmx_section_new();
	g_autoptr(HtmxHeading) heading = htmx_h2_new();
	g_autoptr(HtmxDiv) grid = htmx_div_new();

	htmx_element_add_class(HTMX_ELEMENT(gallery), "gallery-section");
	htmx_node_set_text_content(HTMX_NODE(heading), "Media Gallery");
	htmx_element_add_class(HTMX_ELEMENT(grid), "thumbnail-grid");
	htmx_element_set_id(HTMX_ELEMENT(grid), "gallery-grid");

	{
		guint i;
		for (i = 0; i < num_items; i++) {
			MediaItem *item = &media_items[i];
			g_autoptr(HtmxDiv) thumb_card = htmx_div_new();
			g_autoptr(HtmxA) link = htmx_a_new();

			htmx_element_add_class(HTMX_ELEMENT(thumb_card), "thumbnail-card");

			/* Thumbnail image */
			if (item->thumbnail != NULL) {
				g_autoptr(HtmxImg) thumb = htmx_img_new_with_src(item->thumbnail, item->title);
				htmx_img_set_loading(thumb, "lazy");
				htmx_node_add_child(HTMX_NODE(link), HTMX_NODE(thumb));
			}

			/* Title overlay */
			{
				g_autoptr(HtmxSpan) title = htmx_span_new();
				g_autoptr(HtmxSpan) type_badge = htmx_span_new();
				const gchar *type_names[] = { "Video", "Audio", "Image", "Embed" };

				htmx_node_set_text_content(HTMX_NODE(title), item->title);
				htmx_element_add_class(HTMX_ELEMENT(title), "thumb-title");

				htmx_node_set_text_content(HTMX_NODE(type_badge), type_names[item->type]);
				htmx_element_add_class(HTMX_ELEMENT(type_badge), "type-badge");

				htmx_node_add_child(HTMX_NODE(link), HTMX_NODE(title));
				htmx_node_add_child(HTMX_NODE(link), HTMX_NODE(type_badge));
			}

			/* HTMX-powered loading */
			htmx_element_set_attribute(HTMX_ELEMENT(link), "href",
				g_strdup_printf("/media/%u", item->id));
			htmx_element_set_attribute(HTMX_ELEMENT(link), "hx-get",
				g_strdup_printf("/media/%u", item->id));
			htmx_element_set_attribute(HTMX_ELEMENT(link), "hx-target", "#media-viewer");
			htmx_element_set_attribute(HTMX_ELEMENT(link), "hx-swap", "innerHTML");

			htmx_node_add_child(HTMX_NODE(thumb_card), HTMX_NODE(link));
			htmx_node_add_child(HTMX_NODE(grid), HTMX_NODE(thumb_card));
		}
	}

	htmx_node_add_child(HTMX_NODE(gallery), HTMX_NODE(heading));
	htmx_node_add_child(HTMX_NODE(gallery), HTMX_NODE(grid));

	return htmx_element_render(HTMX_ELEMENT(gallery));
}

/*
 * Render the media viewer area
 */
static gchar *
render_viewer(guint item_id)
{
	g_autoptr(HtmxDiv) viewer = htmx_div_new();

	htmx_element_set_id(HTMX_ELEMENT(viewer), "media-viewer");
	htmx_element_add_class(HTMX_ELEMENT(viewer), "media-viewer");

	if (item_id > 0 && item_id <= num_items) {
		MediaItem *item = &media_items[item_id - 1];
		g_autoptr(HtmxFigure) media = render_media_item(item);
		if (media != NULL) {
			htmx_node_add_child(HTMX_NODE(viewer), HTMX_NODE(media));
		}
	} else {
		g_autoptr(HtmxP) placeholder = htmx_p_new();
		htmx_node_set_text_content(HTMX_NODE(placeholder),
			"Select a media item from the gallery above.");
		htmx_element_add_class(HTMX_ELEMENT(placeholder), "placeholder");
		htmx_node_add_child(HTMX_NODE(viewer), HTMX_NODE(placeholder));
	}

	return htmx_element_render(HTMX_ELEMENT(viewer));
}

/*
 * Render the complete page
 */
static gchar *
render_page(void)
{
	g_autoptr(HtmxBuilder) builder = htmx_builder_new();
	g_autofree gchar *gallery_html = render_gallery();
	g_autofree gchar *viewer_html = render_viewer(0);

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
	htmx_builder_text(builder, "Media Gallery - htmx-glib");
	htmx_builder_end(builder);

	htmx_builder_begin(builder, "script");
	htmx_builder_attr(builder, "src", "https://unpkg.com/htmx.org@1.9.10");
	htmx_builder_end(builder);

	htmx_builder_begin(builder, "style");
	htmx_builder_text(builder,
		"* { box-sizing: border-box; }\n"
		"body { font-family: system-ui, sans-serif; margin: 0; padding: 1em; background: #1a1a2e; color: #eee; }\n"
		"h1, h2 { color: #fff; }\n"
		"h1 { text-align: center; }\n"
		".gallery-section { margin-bottom: 2em; }\n"
		".thumbnail-grid { display: grid; grid-template-columns: repeat(auto-fill, minmax(200px, 1fr)); gap: 1em; }\n"
		".thumbnail-card { position: relative; border-radius: 8px; overflow: hidden; background: #16213e; }\n"
		".thumbnail-card a { display: block; text-decoration: none; color: inherit; }\n"
		".thumbnail-card img { width: 100%; height: 150px; object-fit: cover; display: block; }\n"
		".thumb-title { display: block; padding: 0.5em; font-weight: 500; }\n"
		".type-badge { position: absolute; top: 0.5em; right: 0.5em; background: rgba(0,0,0,0.7); padding: 0.25em 0.5em; border-radius: 3px; font-size: 0.8em; }\n"
		".media-viewer { background: #0f0f23; border-radius: 8px; padding: 1em; min-height: 400px; }\n"
		".media-viewer .placeholder { text-align: center; padding: 3em; color: #888; }\n"
		".media-item { margin: 0; }\n"
		".media-item figcaption { padding: 1em 0; }\n"
		".media-item .description { color: #aaa; }\n"
		".video-item video, .embed-item iframe { max-width: 100%; border-radius: 4px; }\n"
		".audio-player { display: flex; align-items: center; gap: 1em; background: #16213e; padding: 1em; border-radius: 8px; }\n"
		".audio-thumbnail { width: 80px; height: 80px; object-fit: cover; border-radius: 4px; }\n"
		".audio-player audio { flex: 1; }\n"
		".image-item img { max-width: 100%; height: auto; border-radius: 4px; }\n"
		".htmx-request { opacity: 0.5; }\n"
	);
	htmx_builder_end(builder);

	htmx_builder_end(builder); /* head */

	/* Body */
	htmx_builder_begin(builder, "body");

	htmx_builder_begin(builder, "h1");
	htmx_builder_text(builder, "htmx-glib Media Gallery");
	htmx_builder_end(builder);

	/* Gallery */
	htmx_builder_raw_html(builder, gallery_html);

	/* Viewer */
	htmx_builder_begin(builder, "h2");
	htmx_builder_text(builder, "Media Viewer");
	htmx_builder_end(builder);

	htmx_builder_raw_html(builder, viewer_html);

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

/*
 * Handle GET /media/:id - Get a specific media item
 */
static HtmxResponse *
handle_media(HtmxRequest *request, GHashTable *params, gpointer user_data)
{
	g_autoptr(HtmxResponse) response = htmx_response_new();
	const gchar *id_str;
	guint id;
	g_autofree gchar *html = NULL;

	id_str = g_hash_table_lookup(params, "id");
	if (id_str == NULL) {
		htmx_response_set_status(response, 400);
		htmx_response_set_content(response, "Missing media ID");
		return (HtmxResponse *)g_steal_pointer(&response);
	}

	id = (guint)g_ascii_strtoull(id_str, NULL, 10);
	if (id == 0 || id > num_items) {
		htmx_response_set_status(response, 404);
		htmx_response_set_content(response, "Media not found");
		return (HtmxResponse *)g_steal_pointer(&response);
	}

	{
		MediaItem *item = &media_items[id - 1];
		g_autoptr(HtmxFigure) media = render_media_item(item);
		if (media != NULL) {
			html = htmx_element_render(HTMX_ELEMENT(media));
		} else {
			html = g_strdup("<p>Unable to render media</p>");
		}
	}

	htmx_response_set_content(response, html);

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
	htmx_router_get(router, "/media/:id", handle_media, NULL);

	g_print("Media Gallery Example\n");
	g_print("Server running at http://localhost:%u\n", port);
	g_print("Press Ctrl+C to stop\n\n");

	htmx_server_run(server);

	return 0;
}
