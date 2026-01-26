/*
 * htmx-media.c - HTML media elements implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-media.h"

/*
 * HtmxAudio - Audio element
 */

struct _HtmxAudio {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxAudio, htmx_audio, HTMX_TYPE_NODE)

static void
htmx_audio_class_init(HtmxAudioClass *klass)
{
}

static void
htmx_audio_init(HtmxAudio *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "audio");
}

HtmxAudio *
htmx_audio_new(void)
{
	return g_object_new(HTMX_TYPE_AUDIO, NULL);
}

HtmxAudio *
htmx_audio_new_with_src(const gchar *src)
{
	HtmxAudio *self;

	self = htmx_audio_new();
	if (src != NULL) {
		htmx_audio_set_src(self, src);
	}

	return self;
}

void
htmx_audio_set_src(HtmxAudio *self, const gchar *src)
{
	g_return_if_fail(HTMX_IS_AUDIO(self));
	htmx_element_set_attribute(HTMX_ELEMENT(self), "src", src);
}

void
htmx_audio_set_controls(HtmxAudio *self, gboolean controls)
{
	g_return_if_fail(HTMX_IS_AUDIO(self));
	if (controls) {
		htmx_element_set_attribute(HTMX_ELEMENT(self), "controls", "controls");
	}
}

void
htmx_audio_set_autoplay(HtmxAudio *self, gboolean autoplay)
{
	g_return_if_fail(HTMX_IS_AUDIO(self));
	if (autoplay) {
		htmx_element_set_attribute(HTMX_ELEMENT(self), "autoplay", "autoplay");
	}
}

void
htmx_audio_set_loop(HtmxAudio *self, gboolean loop)
{
	g_return_if_fail(HTMX_IS_AUDIO(self));
	if (loop) {
		htmx_element_set_attribute(HTMX_ELEMENT(self), "loop", "loop");
	}
}

void
htmx_audio_set_muted(HtmxAudio *self, gboolean muted)
{
	g_return_if_fail(HTMX_IS_AUDIO(self));
	if (muted) {
		htmx_element_set_attribute(HTMX_ELEMENT(self), "muted", "muted");
	}
}

void
htmx_audio_set_preload(HtmxAudio *self, const gchar *preload)
{
	g_return_if_fail(HTMX_IS_AUDIO(self));
	htmx_element_set_attribute(HTMX_ELEMENT(self), "preload", preload);
}

/*
 * HtmxVideo - Video element
 */

struct _HtmxVideo {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxVideo, htmx_video, HTMX_TYPE_NODE)

static void
htmx_video_class_init(HtmxVideoClass *klass)
{
}

static void
htmx_video_init(HtmxVideo *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "video");
}

HtmxVideo *
htmx_video_new(void)
{
	return g_object_new(HTMX_TYPE_VIDEO, NULL);
}

HtmxVideo *
htmx_video_new_with_src(const gchar *src)
{
	HtmxVideo *self;

	self = htmx_video_new();
	if (src != NULL) {
		htmx_video_set_src(self, src);
	}

	return self;
}

void
htmx_video_set_src(HtmxVideo *self, const gchar *src)
{
	g_return_if_fail(HTMX_IS_VIDEO(self));
	htmx_element_set_attribute(HTMX_ELEMENT(self), "src", src);
}

void
htmx_video_set_poster(HtmxVideo *self, const gchar *poster)
{
	g_return_if_fail(HTMX_IS_VIDEO(self));
	htmx_element_set_attribute(HTMX_ELEMENT(self), "poster", poster);
}

void
htmx_video_set_controls(HtmxVideo *self, gboolean controls)
{
	g_return_if_fail(HTMX_IS_VIDEO(self));
	if (controls) {
		htmx_element_set_attribute(HTMX_ELEMENT(self), "controls", "controls");
	}
}

void
htmx_video_set_autoplay(HtmxVideo *self, gboolean autoplay)
{
	g_return_if_fail(HTMX_IS_VIDEO(self));
	if (autoplay) {
		htmx_element_set_attribute(HTMX_ELEMENT(self), "autoplay", "autoplay");
	}
}

void
htmx_video_set_loop(HtmxVideo *self, gboolean loop)
{
	g_return_if_fail(HTMX_IS_VIDEO(self));
	if (loop) {
		htmx_element_set_attribute(HTMX_ELEMENT(self), "loop", "loop");
	}
}

void
htmx_video_set_muted(HtmxVideo *self, gboolean muted)
{
	g_return_if_fail(HTMX_IS_VIDEO(self));
	if (muted) {
		htmx_element_set_attribute(HTMX_ELEMENT(self), "muted", "muted");
	}
}

void
htmx_video_set_width(HtmxVideo *self, guint width)
{
	g_autofree gchar *width_str = NULL;

	g_return_if_fail(HTMX_IS_VIDEO(self));

	width_str = g_strdup_printf("%u", width);
	htmx_element_set_attribute(HTMX_ELEMENT(self), "width", width_str);
}

void
htmx_video_set_height(HtmxVideo *self, guint height)
{
	g_autofree gchar *height_str = NULL;

	g_return_if_fail(HTMX_IS_VIDEO(self));

	height_str = g_strdup_printf("%u", height);
	htmx_element_set_attribute(HTMX_ELEMENT(self), "height", height_str);
}

/*
 * HtmxSource - Media source element (void element)
 */

struct _HtmxSource {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxSource, htmx_source, HTMX_TYPE_NODE)

static void
htmx_source_class_init(HtmxSourceClass *klass)
{
}

static void
htmx_source_init(HtmxSource *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "source");
	htmx_node_set_void_element(HTMX_NODE(self), TRUE);
}

HtmxSource *
htmx_source_new(void)
{
	return g_object_new(HTMX_TYPE_SOURCE, NULL);
}

HtmxSource *
htmx_source_new_with_src(
	const gchar *src,
	const gchar *type
){
	HtmxSource *self;

	self = htmx_source_new();
	if (src != NULL) {
		htmx_source_set_src(self, src);
	}
	if (type != NULL) {
		htmx_source_set_type(self, type);
	}

	return self;
}

void
htmx_source_set_src(HtmxSource *self, const gchar *src)
{
	g_return_if_fail(HTMX_IS_SOURCE(self));
	htmx_element_set_attribute(HTMX_ELEMENT(self), "src", src);
}

void
htmx_source_set_type(HtmxSource *self, const gchar *type)
{
	g_return_if_fail(HTMX_IS_SOURCE(self));
	htmx_element_set_attribute(HTMX_ELEMENT(self), "type", type);
}

void
htmx_source_set_srcset(HtmxSource *self, const gchar *srcset)
{
	g_return_if_fail(HTMX_IS_SOURCE(self));
	htmx_element_set_attribute(HTMX_ELEMENT(self), "srcset", srcset);
}

void
htmx_source_set_sizes(HtmxSource *self, const gchar *sizes)
{
	g_return_if_fail(HTMX_IS_SOURCE(self));
	htmx_element_set_attribute(HTMX_ELEMENT(self), "sizes", sizes);
}

void
htmx_source_set_media(HtmxSource *self, const gchar *media)
{
	g_return_if_fail(HTMX_IS_SOURCE(self));
	htmx_element_set_attribute(HTMX_ELEMENT(self), "media", media);
}

/*
 * HtmxTrack - Text track element (void element)
 */

struct _HtmxTrack {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxTrack, htmx_track, HTMX_TYPE_NODE)

static void
htmx_track_class_init(HtmxTrackClass *klass)
{
}

static void
htmx_track_init(HtmxTrack *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "track");
	htmx_node_set_void_element(HTMX_NODE(self), TRUE);
}

HtmxTrack *
htmx_track_new(void)
{
	return g_object_new(HTMX_TYPE_TRACK, NULL);
}

void
htmx_track_set_src(HtmxTrack *self, const gchar *src)
{
	g_return_if_fail(HTMX_IS_TRACK(self));
	htmx_element_set_attribute(HTMX_ELEMENT(self), "src", src);
}

void
htmx_track_set_kind(HtmxTrack *self, const gchar *kind)
{
	g_return_if_fail(HTMX_IS_TRACK(self));
	htmx_element_set_attribute(HTMX_ELEMENT(self), "kind", kind);
}

void
htmx_track_set_srclang(HtmxTrack *self, const gchar *srclang)
{
	g_return_if_fail(HTMX_IS_TRACK(self));
	htmx_element_set_attribute(HTMX_ELEMENT(self), "srclang", srclang);
}

void
htmx_track_set_label(HtmxTrack *self, const gchar *label)
{
	g_return_if_fail(HTMX_IS_TRACK(self));
	htmx_element_set_attribute(HTMX_ELEMENT(self), "label", label);
}

void
htmx_track_set_default(HtmxTrack *self, gboolean is_default)
{
	g_return_if_fail(HTMX_IS_TRACK(self));
	if (is_default) {
		htmx_element_set_attribute(HTMX_ELEMENT(self), "default", "default");
	}
}

/*
 * HtmxPicture - Picture element
 */

struct _HtmxPicture {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxPicture, htmx_picture, HTMX_TYPE_NODE)

static void
htmx_picture_class_init(HtmxPictureClass *klass)
{
}

static void
htmx_picture_init(HtmxPicture *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "picture");
}

HtmxPicture *
htmx_picture_new(void)
{
	return g_object_new(HTMX_TYPE_PICTURE, NULL);
}

/*
 * HtmxCanvas - Canvas element
 */

struct _HtmxCanvas {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxCanvas, htmx_canvas, HTMX_TYPE_NODE)

static void
htmx_canvas_class_init(HtmxCanvasClass *klass)
{
}

static void
htmx_canvas_init(HtmxCanvas *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "canvas");
}

HtmxCanvas *
htmx_canvas_new(void)
{
	return g_object_new(HTMX_TYPE_CANVAS, NULL);
}

HtmxCanvas *
htmx_canvas_new_with_size(
	guint width,
	guint height
){
	HtmxCanvas *self;

	self = htmx_canvas_new();
	htmx_canvas_set_width(self, width);
	htmx_canvas_set_height(self, height);

	return self;
}

void
htmx_canvas_set_width(HtmxCanvas *self, guint width)
{
	g_autofree gchar *width_str = NULL;

	g_return_if_fail(HTMX_IS_CANVAS(self));

	width_str = g_strdup_printf("%u", width);
	htmx_element_set_attribute(HTMX_ELEMENT(self), "width", width_str);
}

void
htmx_canvas_set_height(HtmxCanvas *self, guint height)
{
	g_autofree gchar *height_str = NULL;

	g_return_if_fail(HTMX_IS_CANVAS(self));

	height_str = g_strdup_printf("%u", height);
	htmx_element_set_attribute(HTMX_ELEMENT(self), "height", height_str);
}

/*
 * HtmxIframe - Inline frame element
 */

struct _HtmxIframe {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxIframe, htmx_iframe, HTMX_TYPE_NODE)

static void
htmx_iframe_class_init(HtmxIframeClass *klass)
{
}

static void
htmx_iframe_init(HtmxIframe *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "iframe");
}

HtmxIframe *
htmx_iframe_new(void)
{
	return g_object_new(HTMX_TYPE_IFRAME, NULL);
}

HtmxIframe *
htmx_iframe_new_with_src(const gchar *src)
{
	HtmxIframe *self;

	self = htmx_iframe_new();
	if (src != NULL) {
		htmx_iframe_set_src(self, src);
	}

	return self;
}

void
htmx_iframe_set_src(HtmxIframe *self, const gchar *src)
{
	g_return_if_fail(HTMX_IS_IFRAME(self));
	htmx_element_set_attribute(HTMX_ELEMENT(self), "src", src);
}

void
htmx_iframe_set_srcdoc(HtmxIframe *self, const gchar *srcdoc)
{
	g_return_if_fail(HTMX_IS_IFRAME(self));
	htmx_element_set_attribute(HTMX_ELEMENT(self), "srcdoc", srcdoc);
}

void
htmx_iframe_set_name(HtmxIframe *self, const gchar *name)
{
	g_return_if_fail(HTMX_IS_IFRAME(self));
	htmx_element_set_attribute(HTMX_ELEMENT(self), "name", name);
}

void
htmx_iframe_set_width(HtmxIframe *self, guint width)
{
	g_autofree gchar *width_str = NULL;

	g_return_if_fail(HTMX_IS_IFRAME(self));

	width_str = g_strdup_printf("%u", width);
	htmx_element_set_attribute(HTMX_ELEMENT(self), "width", width_str);
}

void
htmx_iframe_set_height(HtmxIframe *self, guint height)
{
	g_autofree gchar *height_str = NULL;

	g_return_if_fail(HTMX_IS_IFRAME(self));

	height_str = g_strdup_printf("%u", height);
	htmx_element_set_attribute(HTMX_ELEMENT(self), "height", height_str);
}

void
htmx_iframe_set_sandbox(HtmxIframe *self, const gchar *sandbox)
{
	g_return_if_fail(HTMX_IS_IFRAME(self));
	htmx_element_set_attribute(HTMX_ELEMENT(self), "sandbox", sandbox);
}

void
htmx_iframe_set_allow(HtmxIframe *self, const gchar *allow)
{
	g_return_if_fail(HTMX_IS_IFRAME(self));
	htmx_element_set_attribute(HTMX_ELEMENT(self), "allow", allow);
}

/*
 * HtmxEmbed - Embed element (void element)
 */

struct _HtmxEmbed {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxEmbed, htmx_embed, HTMX_TYPE_NODE)

static void
htmx_embed_class_init(HtmxEmbedClass *klass)
{
}

static void
htmx_embed_init(HtmxEmbed *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "embed");
	htmx_node_set_void_element(HTMX_NODE(self), TRUE);
}

HtmxEmbed *
htmx_embed_new(void)
{
	return g_object_new(HTMX_TYPE_EMBED, NULL);
}

void
htmx_embed_set_src(HtmxEmbed *self, const gchar *src)
{
	g_return_if_fail(HTMX_IS_EMBED(self));
	htmx_element_set_attribute(HTMX_ELEMENT(self), "src", src);
}

void
htmx_embed_set_type(HtmxEmbed *self, const gchar *type)
{
	g_return_if_fail(HTMX_IS_EMBED(self));
	htmx_element_set_attribute(HTMX_ELEMENT(self), "type", type);
}

void
htmx_embed_set_width(HtmxEmbed *self, guint width)
{
	g_autofree gchar *width_str = NULL;

	g_return_if_fail(HTMX_IS_EMBED(self));

	width_str = g_strdup_printf("%u", width);
	htmx_element_set_attribute(HTMX_ELEMENT(self), "width", width_str);
}

void
htmx_embed_set_height(HtmxEmbed *self, guint height)
{
	g_autofree gchar *height_str = NULL;

	g_return_if_fail(HTMX_IS_EMBED(self));

	height_str = g_strdup_printf("%u", height);
	htmx_element_set_attribute(HTMX_ELEMENT(self), "height", height_str);
}

/*
 * HtmxObject - Object element
 */

struct _HtmxObject {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxObject, htmx_object, HTMX_TYPE_NODE)

static void
htmx_object_class_init(HtmxObjectClass *klass)
{
}

static void
htmx_object_init(HtmxObject *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "object");
}

HtmxObject *
htmx_object_new(void)
{
	return g_object_new(HTMX_TYPE_OBJECT, NULL);
}

void
htmx_object_set_data(HtmxObject *self, const gchar *data)
{
	g_return_if_fail(HTMX_IS_OBJECT(self));
	htmx_element_set_attribute(HTMX_ELEMENT(self), "data", data);
}

void
htmx_object_set_type(HtmxObject *self, const gchar *type)
{
	g_return_if_fail(HTMX_IS_OBJECT(self));
	htmx_element_set_attribute(HTMX_ELEMENT(self), "type", type);
}

void
htmx_object_set_width(HtmxObject *self, guint width)
{
	g_autofree gchar *width_str = NULL;

	g_return_if_fail(HTMX_IS_OBJECT(self));

	width_str = g_strdup_printf("%u", width);
	htmx_element_set_attribute(HTMX_ELEMENT(self), "width", width_str);
}

void
htmx_object_set_height(HtmxObject *self, guint height)
{
	g_autofree gchar *height_str = NULL;

	g_return_if_fail(HTMX_IS_OBJECT(self));

	height_str = g_strdup_printf("%u", height);
	htmx_element_set_attribute(HTMX_ELEMENT(self), "height", height_str);
}

void
htmx_object_set_name(HtmxObject *self, const gchar *name)
{
	g_return_if_fail(HTMX_IS_OBJECT(self));
	htmx_element_set_attribute(HTMX_ELEMENT(self), "name", name);
}

/*
 * HtmxParam - Object parameter element (void element)
 */

struct _HtmxParam {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxParam, htmx_param, HTMX_TYPE_NODE)

static void
htmx_param_class_init(HtmxParamClass *klass)
{
}

static void
htmx_param_init(HtmxParam *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "param");
	htmx_node_set_void_element(HTMX_NODE(self), TRUE);
}

HtmxParam *
htmx_param_new(void)
{
	return g_object_new(HTMX_TYPE_PARAM, NULL);
}

HtmxParam *
htmx_param_new_with_name_value(
	const gchar *name,
	const gchar *value
){
	HtmxParam *self;

	self = htmx_param_new();
	if (name != NULL) {
		htmx_param_set_name(self, name);
	}
	if (value != NULL) {
		htmx_param_set_value(self, value);
	}

	return self;
}

void
htmx_param_set_name(
	HtmxParam   *self,
	const gchar *name
){
	g_return_if_fail(HTMX_IS_PARAM(self));

	htmx_element_set_attribute(HTMX_ELEMENT(self), "name", name);
}

void
htmx_param_set_value(
	HtmxParam   *self,
	const gchar *value
){
	g_return_if_fail(HTMX_IS_PARAM(self));

	htmx_element_set_attribute(HTMX_ELEMENT(self), "value", value);
}
