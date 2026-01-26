/*
 * htmx-img.h - HTML image element
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * This file defines the HtmxImg class, a typed wrapper for <img> elements.
 * Note: <img> is a void element (self-closing, no content).
 */

#ifndef HTMX_IMG_H
#define HTMX_IMG_H

#include <glib-object.h>
#include "htmx-node.h"

G_BEGIN_DECLS

#define HTMX_TYPE_IMG (htmx_img_get_type())
G_DECLARE_FINAL_TYPE(HtmxImg, htmx_img, HTMX, IMG, HtmxNode)

/**
 * htmx_img_new:
 *
 * Creates a new #HtmxImg element.
 *
 * Returns: (transfer full): a new #HtmxImg
 */
HtmxImg *
htmx_img_new(void);

/**
 * htmx_img_new_with_src:
 * @src: the image source URL
 * @alt: the alt text
 *
 * Creates a new #HtmxImg element with src and alt attributes.
 *
 * Returns: (transfer full): a new #HtmxImg
 */
HtmxImg *
htmx_img_new_with_src(
	const gchar *src,
	const gchar *alt
);

/**
 * htmx_img_set_src:
 * @self: an #HtmxImg
 * @src: the image source URL
 *
 * Sets the src attribute.
 */
void
htmx_img_set_src(
	HtmxImg     *self,
	const gchar *src
);

/**
 * htmx_img_set_alt:
 * @self: an #HtmxImg
 * @alt: the alt text
 *
 * Sets the alt attribute.
 */
void
htmx_img_set_alt(
	HtmxImg     *self,
	const gchar *alt
);

/**
 * htmx_img_set_width:
 * @self: an #HtmxImg
 * @width: the width in pixels
 *
 * Sets the width attribute.
 */
void
htmx_img_set_width(
	HtmxImg *self,
	guint    width
);

/**
 * htmx_img_set_height:
 * @self: an #HtmxImg
 * @height: the height in pixels
 *
 * Sets the height attribute.
 */
void
htmx_img_set_height(
	HtmxImg *self,
	guint    height
);

/**
 * htmx_img_set_loading:
 * @self: an #HtmxImg
 * @loading: the loading strategy ("lazy" or "eager")
 *
 * Sets the loading attribute.
 */
void
htmx_img_set_loading(
	HtmxImg     *self,
	const gchar *loading
);

/*
 * HtmxMap - Image map element
 */

#define HTMX_TYPE_MAP (htmx_map_get_type())
G_DECLARE_FINAL_TYPE(HtmxMap, htmx_map, HTMX, MAP, HtmxNode)

/**
 * htmx_map_new:
 *
 * Creates a new #HtmxMap element.
 *
 * Returns: (transfer full): a new #HtmxMap
 */
HtmxMap *
htmx_map_new(void);

/**
 * htmx_map_new_with_name:
 * @name: the map name
 *
 * Creates a new #HtmxMap with a name.
 *
 * Returns: (transfer full): a new #HtmxMap
 */
HtmxMap *
htmx_map_new_with_name(const gchar *name);

/**
 * htmx_map_set_name:
 * @self: an #HtmxMap
 * @name: the map name
 *
 * Sets the name attribute.
 */
void
htmx_map_set_name(
	HtmxMap     *self,
	const gchar *name
);

/*
 * HtmxArea - Image map area element (void element)
 */

#define HTMX_TYPE_AREA (htmx_area_get_type())
G_DECLARE_FINAL_TYPE(HtmxArea, htmx_area, HTMX, AREA, HtmxNode)

/**
 * htmx_area_new:
 *
 * Creates a new #HtmxArea element.
 *
 * Returns: (transfer full): a new #HtmxArea
 */
HtmxArea *
htmx_area_new(void);

/**
 * htmx_area_set_shape:
 * @self: an #HtmxArea
 * @shape: the area shape ("rect", "circle", "poly", "default")
 *
 * Sets the shape attribute.
 */
void
htmx_area_set_shape(
	HtmxArea    *self,
	const gchar *shape
);

/**
 * htmx_area_set_coords:
 * @self: an #HtmxArea
 * @coords: the area coordinates
 *
 * Sets the coords attribute.
 */
void
htmx_area_set_coords(
	HtmxArea    *self,
	const gchar *coords
);

/**
 * htmx_area_set_href:
 * @self: an #HtmxArea
 * @href: the link URL
 *
 * Sets the href attribute.
 */
void
htmx_area_set_href(
	HtmxArea    *self,
	const gchar *href
);

/**
 * htmx_area_set_alt:
 * @self: an #HtmxArea
 * @alt: the alt text
 *
 * Sets the alt attribute.
 */
void
htmx_area_set_alt(
	HtmxArea    *self,
	const gchar *alt
);

/**
 * htmx_area_set_target:
 * @self: an #HtmxArea
 * @target: the target context (e.g., "_blank")
 *
 * Sets the target attribute.
 */
void
htmx_area_set_target(
	HtmxArea    *self,
	const gchar *target
);

G_END_DECLS

#endif /* HTMX_IMG_H */
