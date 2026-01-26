/*
 * htmx-media.h - HTML media elements
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * This file defines media elements: audio, video, source, track, picture,
 * canvas, iframe, embed, and object.
 */

#ifndef HTMX_MEDIA_H
#define HTMX_MEDIA_H

#include <glib-object.h>
#include "htmx-node.h"

G_BEGIN_DECLS

/*
 * HtmxAudio - Audio element
 */

#define HTMX_TYPE_AUDIO (htmx_audio_get_type())
G_DECLARE_FINAL_TYPE(HtmxAudio, htmx_audio, HTMX, AUDIO, HtmxNode)

/**
 * htmx_audio_new:
 *
 * Creates a new #HtmxAudio element.
 *
 * Returns: (transfer full): a new #HtmxAudio
 */
HtmxAudio *
htmx_audio_new(void);

/**
 * htmx_audio_new_with_src:
 * @src: the audio source URL
 *
 * Creates a new #HtmxAudio with a source.
 *
 * Returns: (transfer full): a new #HtmxAudio
 */
HtmxAudio *
htmx_audio_new_with_src(const gchar *src);

void htmx_audio_set_src(HtmxAudio *self, const gchar *src);
void htmx_audio_set_controls(HtmxAudio *self, gboolean controls);
void htmx_audio_set_autoplay(HtmxAudio *self, gboolean autoplay);
void htmx_audio_set_loop(HtmxAudio *self, gboolean loop);
void htmx_audio_set_muted(HtmxAudio *self, gboolean muted);
void htmx_audio_set_preload(HtmxAudio *self, const gchar *preload);

/*
 * HtmxVideo - Video element
 */

#define HTMX_TYPE_VIDEO (htmx_video_get_type())
G_DECLARE_FINAL_TYPE(HtmxVideo, htmx_video, HTMX, VIDEO, HtmxNode)

/**
 * htmx_video_new:
 *
 * Creates a new #HtmxVideo element.
 *
 * Returns: (transfer full): a new #HtmxVideo
 */
HtmxVideo *
htmx_video_new(void);

/**
 * htmx_video_new_with_src:
 * @src: the video source URL
 *
 * Creates a new #HtmxVideo with a source.
 *
 * Returns: (transfer full): a new #HtmxVideo
 */
HtmxVideo *
htmx_video_new_with_src(const gchar *src);

void htmx_video_set_src(HtmxVideo *self, const gchar *src);
void htmx_video_set_poster(HtmxVideo *self, const gchar *poster);
void htmx_video_set_controls(HtmxVideo *self, gboolean controls);
void htmx_video_set_autoplay(HtmxVideo *self, gboolean autoplay);
void htmx_video_set_loop(HtmxVideo *self, gboolean loop);
void htmx_video_set_muted(HtmxVideo *self, gboolean muted);
void htmx_video_set_width(HtmxVideo *self, guint width);
void htmx_video_set_height(HtmxVideo *self, guint height);

/*
 * HtmxSource - Media source element (void element)
 */

#define HTMX_TYPE_SOURCE (htmx_source_get_type())
G_DECLARE_FINAL_TYPE(HtmxSource, htmx_source, HTMX, SOURCE, HtmxNode)

/**
 * htmx_source_new:
 *
 * Creates a new #HtmxSource element.
 *
 * Returns: (transfer full): a new #HtmxSource
 */
HtmxSource *
htmx_source_new(void);

/**
 * htmx_source_new_with_src:
 * @src: the source URL
 * @type: the MIME type (nullable)
 *
 * Creates a new #HtmxSource with src and type.
 *
 * Returns: (transfer full): a new #HtmxSource
 */
HtmxSource *
htmx_source_new_with_src(
	const gchar *src,
	const gchar *type
);

void htmx_source_set_src(HtmxSource *self, const gchar *src);
void htmx_source_set_type(HtmxSource *self, const gchar *type);
void htmx_source_set_srcset(HtmxSource *self, const gchar *srcset);
void htmx_source_set_sizes(HtmxSource *self, const gchar *sizes);
void htmx_source_set_media(HtmxSource *self, const gchar *media);

/*
 * HtmxTrack - Text track element (void element)
 */

#define HTMX_TYPE_TRACK (htmx_track_get_type())
G_DECLARE_FINAL_TYPE(HtmxTrack, htmx_track, HTMX, TRACK, HtmxNode)

/**
 * htmx_track_new:
 *
 * Creates a new #HtmxTrack element.
 *
 * Returns: (transfer full): a new #HtmxTrack
 */
HtmxTrack *
htmx_track_new(void);

void htmx_track_set_src(HtmxTrack *self, const gchar *src);
void htmx_track_set_kind(HtmxTrack *self, const gchar *kind);
void htmx_track_set_srclang(HtmxTrack *self, const gchar *srclang);
void htmx_track_set_label(HtmxTrack *self, const gchar *label);
void htmx_track_set_default(HtmxTrack *self, gboolean is_default);

/*
 * HtmxPicture - Picture element
 */

#define HTMX_TYPE_PICTURE (htmx_picture_get_type())
G_DECLARE_FINAL_TYPE(HtmxPicture, htmx_picture, HTMX, PICTURE, HtmxNode)

/**
 * htmx_picture_new:
 *
 * Creates a new #HtmxPicture element.
 *
 * Returns: (transfer full): a new #HtmxPicture
 */
HtmxPicture *
htmx_picture_new(void);

/*
 * HtmxCanvas - Canvas element
 */

#define HTMX_TYPE_CANVAS (htmx_canvas_get_type())
G_DECLARE_FINAL_TYPE(HtmxCanvas, htmx_canvas, HTMX, CANVAS, HtmxNode)

/**
 * htmx_canvas_new:
 *
 * Creates a new #HtmxCanvas element.
 *
 * Returns: (transfer full): a new #HtmxCanvas
 */
HtmxCanvas *
htmx_canvas_new(void);

/**
 * htmx_canvas_new_with_size:
 * @width: the canvas width
 * @height: the canvas height
 *
 * Creates a new #HtmxCanvas with dimensions.
 *
 * Returns: (transfer full): a new #HtmxCanvas
 */
HtmxCanvas *
htmx_canvas_new_with_size(
	guint width,
	guint height
);

void htmx_canvas_set_width(HtmxCanvas *self, guint width);
void htmx_canvas_set_height(HtmxCanvas *self, guint height);

/*
 * HtmxIframe - Inline frame element
 */

#define HTMX_TYPE_IFRAME (htmx_iframe_get_type())
G_DECLARE_FINAL_TYPE(HtmxIframe, htmx_iframe, HTMX, IFRAME, HtmxNode)

/**
 * htmx_iframe_new:
 *
 * Creates a new #HtmxIframe element.
 *
 * Returns: (transfer full): a new #HtmxIframe
 */
HtmxIframe *
htmx_iframe_new(void);

/**
 * htmx_iframe_new_with_src:
 * @src: the iframe source URL
 *
 * Creates a new #HtmxIframe with a source.
 *
 * Returns: (transfer full): a new #HtmxIframe
 */
HtmxIframe *
htmx_iframe_new_with_src(const gchar *src);

void htmx_iframe_set_src(HtmxIframe *self, const gchar *src);
void htmx_iframe_set_srcdoc(HtmxIframe *self, const gchar *srcdoc);
void htmx_iframe_set_name(HtmxIframe *self, const gchar *name);
void htmx_iframe_set_width(HtmxIframe *self, guint width);
void htmx_iframe_set_height(HtmxIframe *self, guint height);
void htmx_iframe_set_sandbox(HtmxIframe *self, const gchar *sandbox);
void htmx_iframe_set_allow(HtmxIframe *self, const gchar *allow);

/*
 * HtmxEmbed - Embed element (void element)
 */

#define HTMX_TYPE_EMBED (htmx_embed_get_type())
G_DECLARE_FINAL_TYPE(HtmxEmbed, htmx_embed, HTMX, EMBED, HtmxNode)

/**
 * htmx_embed_new:
 *
 * Creates a new #HtmxEmbed element.
 *
 * Returns: (transfer full): a new #HtmxEmbed
 */
HtmxEmbed *
htmx_embed_new(void);

void htmx_embed_set_src(HtmxEmbed *self, const gchar *src);
void htmx_embed_set_type(HtmxEmbed *self, const gchar *type);
void htmx_embed_set_width(HtmxEmbed *self, guint width);
void htmx_embed_set_height(HtmxEmbed *self, guint height);

/*
 * HtmxObject - Object element
 */

#define HTMX_TYPE_OBJECT (htmx_object_get_type())
G_DECLARE_FINAL_TYPE(HtmxObject, htmx_object, HTMX, OBJECT, HtmxNode)

/**
 * htmx_object_new:
 *
 * Creates a new #HtmxObject element.
 *
 * Returns: (transfer full): a new #HtmxObject
 */
HtmxObject *
htmx_object_new(void);

void htmx_object_set_data(HtmxObject *self, const gchar *data);
void htmx_object_set_type(HtmxObject *self, const gchar *type);
void htmx_object_set_width(HtmxObject *self, guint width);
void htmx_object_set_height(HtmxObject *self, guint height);
void htmx_object_set_name(HtmxObject *self, const gchar *name);

/*
 * HtmxParam - Object parameter element (void element)
 */

#define HTMX_TYPE_PARAM (htmx_param_get_type())
G_DECLARE_FINAL_TYPE(HtmxParam, htmx_param, HTMX, PARAM, HtmxNode)

/**
 * htmx_param_new:
 *
 * Creates a new #HtmxParam element.
 *
 * Returns: (transfer full): a new #HtmxParam
 */
HtmxParam *
htmx_param_new(void);

/**
 * htmx_param_new_with_name_value:
 * @name: the parameter name
 * @value: the parameter value
 *
 * Creates a new #HtmxParam with name and value.
 *
 * Returns: (transfer full): a new #HtmxParam
 */
HtmxParam *
htmx_param_new_with_name_value(
	const gchar *name,
	const gchar *value
);

/**
 * htmx_param_set_name:
 * @self: an #HtmxParam
 * @name: the parameter name
 *
 * Sets the name attribute.
 */
void
htmx_param_set_name(
	HtmxParam   *self,
	const gchar *name
);

/**
 * htmx_param_set_value:
 * @self: an #HtmxParam
 * @value: the parameter value
 *
 * Sets the value attribute.
 */
void
htmx_param_set_value(
	HtmxParam   *self,
	const gchar *value
);

G_END_DECLS

#endif /* HTMX_MEDIA_H */
