/*
 * htmx-img.c - HTML image element implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-img.h"

struct _HtmxImg {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxImg, htmx_img, HTMX_TYPE_NODE)

static void
htmx_img_class_init(HtmxImgClass *klass)
{
}

static void
htmx_img_init(HtmxImg *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "img");
	htmx_node_set_void_element(HTMX_NODE(self), TRUE);
}

HtmxImg *
htmx_img_new(void)
{
	return g_object_new(HTMX_TYPE_IMG, NULL);
}

HtmxImg *
htmx_img_new_with_src(
	const gchar *src,
	const gchar *alt
){
	HtmxImg *self;

	self = htmx_img_new();
	if (src != NULL) {
		htmx_img_set_src(self, src);
	}
	if (alt != NULL) {
		htmx_img_set_alt(self, alt);
	}

	return self;
}

void
htmx_img_set_src(
	HtmxImg     *self,
	const gchar *src
){
	g_return_if_fail(HTMX_IS_IMG(self));

	htmx_element_set_attribute(HTMX_ELEMENT(self), "src", src);
}

void
htmx_img_set_alt(
	HtmxImg     *self,
	const gchar *alt
){
	g_return_if_fail(HTMX_IS_IMG(self));

	htmx_element_set_attribute(HTMX_ELEMENT(self), "alt", alt);
}

void
htmx_img_set_width(
	HtmxImg *self,
	guint    width
){
	g_autofree gchar *width_str = NULL;

	g_return_if_fail(HTMX_IS_IMG(self));

	width_str = g_strdup_printf("%u", width);
	htmx_element_set_attribute(HTMX_ELEMENT(self), "width", width_str);
}

void
htmx_img_set_height(
	HtmxImg *self,
	guint    height
){
	g_autofree gchar *height_str = NULL;

	g_return_if_fail(HTMX_IS_IMG(self));

	height_str = g_strdup_printf("%u", height);
	htmx_element_set_attribute(HTMX_ELEMENT(self), "height", height_str);
}

void
htmx_img_set_loading(
	HtmxImg     *self,
	const gchar *loading
){
	g_return_if_fail(HTMX_IS_IMG(self));

	htmx_element_set_attribute(HTMX_ELEMENT(self), "loading", loading);
}

/*
 * HtmxMap - Image map element
 */

struct _HtmxMap {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxMap, htmx_map, HTMX_TYPE_NODE)

static void
htmx_map_class_init(HtmxMapClass *klass)
{
}

static void
htmx_map_init(HtmxMap *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "map");
}

HtmxMap *
htmx_map_new(void)
{
	return g_object_new(HTMX_TYPE_MAP, NULL);
}

HtmxMap *
htmx_map_new_with_name(const gchar *name)
{
	HtmxMap *self;

	self = htmx_map_new();
	if (name != NULL) {
		htmx_map_set_name(self, name);
	}

	return self;
}

void
htmx_map_set_name(
	HtmxMap     *self,
	const gchar *name
){
	g_return_if_fail(HTMX_IS_MAP(self));

	htmx_element_set_attribute(HTMX_ELEMENT(self), "name", name);
}

/*
 * HtmxArea - Image map area element (void element)
 */

struct _HtmxArea {
	HtmxNode parent_instance;
};

G_DEFINE_FINAL_TYPE(HtmxArea, htmx_area, HTMX_TYPE_NODE)

static void
htmx_area_class_init(HtmxAreaClass *klass)
{
}

static void
htmx_area_init(HtmxArea *self)
{
	htmx_node_set_tag_name(HTMX_NODE(self), "area");
	htmx_node_set_void_element(HTMX_NODE(self), TRUE);
}

HtmxArea *
htmx_area_new(void)
{
	return g_object_new(HTMX_TYPE_AREA, NULL);
}

void
htmx_area_set_shape(
	HtmxArea    *self,
	const gchar *shape
){
	g_return_if_fail(HTMX_IS_AREA(self));

	htmx_element_set_attribute(HTMX_ELEMENT(self), "shape", shape);
}

void
htmx_area_set_coords(
	HtmxArea    *self,
	const gchar *coords
){
	g_return_if_fail(HTMX_IS_AREA(self));

	htmx_element_set_attribute(HTMX_ELEMENT(self), "coords", coords);
}

void
htmx_area_set_href(
	HtmxArea    *self,
	const gchar *href
){
	g_return_if_fail(HTMX_IS_AREA(self));

	htmx_element_set_attribute(HTMX_ELEMENT(self), "href", href);
}

void
htmx_area_set_alt(
	HtmxArea    *self,
	const gchar *alt
){
	g_return_if_fail(HTMX_IS_AREA(self));

	htmx_element_set_attribute(HTMX_ELEMENT(self), "alt", alt);
}

void
htmx_area_set_target(
	HtmxArea    *self,
	const gchar *target
){
	g_return_if_fail(HTMX_IS_AREA(self));

	htmx_element_set_attribute(HTMX_ELEMENT(self), "target", target);
}
