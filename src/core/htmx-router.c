/*
 * htmx-router.c - URL router implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-router.h"
#include <string.h>
#include <gio/gio.h>

typedef struct {
	HtmxMethod        method;
	gchar            *pattern;
	GRegex           *regex;
	gchar           **param_names;
	gint              param_count;
	HtmxRouteCallback callback;
	gpointer          user_data;
} Route;

typedef struct {
	gchar *url_prefix;
	gchar *directory;
} StaticRoute;

struct _HtmxRouter {
	GObject parent_instance;

	GList *routes;        /* List of Route* */
	GList *static_routes; /* List of StaticRoute* */
};

G_DEFINE_FINAL_TYPE(HtmxRouter, htmx_router, G_TYPE_OBJECT)

static void
route_free(Route *route)
{
	if (route == NULL) {
		return;
	}

	g_free(route->pattern);
	g_regex_unref(route->regex);
	g_strfreev(route->param_names);
	g_slice_free(Route, route);
}

static void
static_route_free(StaticRoute *route)
{
	if (route == NULL) {
		return;
	}

	g_free(route->url_prefix);
	g_free(route->directory);
	g_slice_free(StaticRoute, route);
}

/*
 * Convert a pattern like "/users/:id/posts/:post_id" to a regex
 */
static Route *
route_new(
	HtmxMethod        method,
	const gchar      *pattern,
	HtmxRouteCallback callback,
	gpointer          user_data
){
	Route *route;
	GString *regex_str;
	GPtrArray *params;
	const gchar *p;
	GError *error = NULL;

	route = g_slice_new0(Route);
	route->method = method;
	route->pattern = g_strdup(pattern);
	route->callback = callback;
	route->user_data = user_data;

	regex_str = g_string_new("^");
	params = g_ptr_array_new();

	p = pattern;
	while (*p != '\0') {
		if (*p == ':') {
			/* Parameter placeholder */
			const gchar *start = p + 1;
			const gchar *end = start;

			while (*end != '\0' && *end != '/') {
				end++;
			}

			g_ptr_array_add(params, g_strndup(start, end - start));
			g_string_append(regex_str, "([^/]+)");
			p = end;
		} else if (*p == '*') {
			/* Wildcard */
			g_string_append(regex_str, ".*");
			p++;
		} else {
			/* Literal character (escape regex special chars) */
			if (*p == '.' || *p == '?' || *p == '+' || *p == '[' || *p == ']' ||
			    *p == '(' || *p == ')' || *p == '{' || *p == '}' || *p == '^' ||
			    *p == '$' || *p == '|' || *p == '\\') {
				g_string_append_c(regex_str, '\\');
			}
			g_string_append_c(regex_str, *p);
			p++;
		}
	}

	g_string_append_c(regex_str, '$');

	route->regex = g_regex_new(regex_str->str, 0, 0, &error);
	if (route->regex == NULL) {
		g_warning("Failed to compile route regex: %s", error->message);
		g_error_free(error);
		route_free(route);
		g_string_free(regex_str, TRUE);
		g_ptr_array_free(params, TRUE);
		return NULL;
	}

	g_ptr_array_add(params, NULL);
	route->param_names = (gchar **)g_ptr_array_free(params, FALSE);
	route->param_count = g_strv_length(route->param_names);

	g_string_free(regex_str, TRUE);

	return route;
}

static void
htmx_router_finalize(GObject *object)
{
	HtmxRouter *self = HTMX_ROUTER(object);

	g_list_free_full(self->routes, (GDestroyNotify)route_free);
	g_list_free_full(self->static_routes, (GDestroyNotify)static_route_free);

	G_OBJECT_CLASS(htmx_router_parent_class)->finalize(object);
}

static void
htmx_router_class_init(HtmxRouterClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);

	object_class->finalize = htmx_router_finalize;
}

static void
htmx_router_init(HtmxRouter *self)
{
	self->routes = NULL;
	self->static_routes = NULL;
}

HtmxRouter *
htmx_router_new(void)
{
	return g_object_new(HTMX_TYPE_ROUTER, NULL);
}

void
htmx_router_add_route(
	HtmxRouter       *self,
	HtmxMethod        method,
	const gchar      *pattern,
	HtmxRouteCallback callback,
	gpointer          user_data
){
	Route *route;

	g_return_if_fail(HTMX_IS_ROUTER(self));
	g_return_if_fail(pattern != NULL);
	g_return_if_fail(callback != NULL);

	route = route_new(method, pattern, callback, user_data);
	if (route != NULL) {
		self->routes = g_list_append(self->routes, route);
	}
}

void
htmx_router_get(
	HtmxRouter       *self,
	const gchar      *pattern,
	HtmxRouteCallback callback,
	gpointer          user_data
){
	htmx_router_add_route(self, HTMX_METHOD_GET, pattern, callback, user_data);
}

void
htmx_router_post(
	HtmxRouter       *self,
	const gchar      *pattern,
	HtmxRouteCallback callback,
	gpointer          user_data
){
	htmx_router_add_route(self, HTMX_METHOD_POST, pattern, callback, user_data);
}

void
htmx_router_put(
	HtmxRouter       *self,
	const gchar      *pattern,
	HtmxRouteCallback callback,
	gpointer          user_data
){
	htmx_router_add_route(self, HTMX_METHOD_PUT, pattern, callback, user_data);
}

void
htmx_router_patch(
	HtmxRouter       *self,
	const gchar      *pattern,
	HtmxRouteCallback callback,
	gpointer          user_data
){
	htmx_router_add_route(self, HTMX_METHOD_PATCH, pattern, callback, user_data);
}

void
htmx_router_delete(
	HtmxRouter       *self,
	const gchar      *pattern,
	HtmxRouteCallback callback,
	gpointer          user_data
){
	htmx_router_add_route(self, HTMX_METHOD_DELETE, pattern, callback, user_data);
}

void
htmx_router_serve_static(
	HtmxRouter  *self,
	const gchar *url_prefix,
	const gchar *directory
){
	StaticRoute *route;

	g_return_if_fail(HTMX_IS_ROUTER(self));
	g_return_if_fail(url_prefix != NULL);
	g_return_if_fail(directory != NULL);

	route = g_slice_new0(StaticRoute);
	route->url_prefix = g_strdup(url_prefix);
	route->directory = g_strdup(directory);

	self->static_routes = g_list_append(self->static_routes, route);
}

/*
 * Try to serve a static file
 */
static HtmxResponse *
try_serve_static(HtmxRouter *self, const gchar *path)
{
	GList *l;

	for (l = self->static_routes; l != NULL; l = l->next) {
		StaticRoute *route = (StaticRoute *)l->data;

		if (g_str_has_prefix(path, route->url_prefix)) {
			g_autofree gchar *relative = NULL;
			g_autofree gchar *full_path = NULL;
			g_autofree gchar *content = NULL;
			gsize length;
			GError *error = NULL;

			relative = g_strdup(path + strlen(route->url_prefix));
			if (relative[0] == '/') {
				relative++;
			}

			full_path = g_build_filename(route->directory, relative, NULL);

			if (g_file_get_contents(full_path, &content, &length, &error)) {
				HtmxResponse *response;
				const gchar *mime_type = "application/octet-stream";

				/* Detect MIME type */
				if (g_str_has_suffix(full_path, ".html")) {
					mime_type = "text/html";
				} else if (g_str_has_suffix(full_path, ".css")) {
					mime_type = "text/css";
				} else if (g_str_has_suffix(full_path, ".js")) {
					mime_type = "application/javascript";
				} else if (g_str_has_suffix(full_path, ".json")) {
					mime_type = "application/json";
				} else if (g_str_has_suffix(full_path, ".png")) {
					mime_type = "image/png";
				} else if (g_str_has_suffix(full_path, ".jpg") ||
				           g_str_has_suffix(full_path, ".jpeg")) {
					mime_type = "image/jpeg";
				} else if (g_str_has_suffix(full_path, ".svg")) {
					mime_type = "image/svg+xml";
				}

				response = htmx_response_new_with_content(content);
				htmx_response_set_content_type(response, mime_type);
				return response;
			}

			if (error != NULL) {
				g_error_free(error);
			}
		}
	}

	return NULL;
}

HtmxResponse *
htmx_router_match(
	HtmxRouter  *self,
	HtmxRequest *request,
	GHashTable **params
){
	HtmxMethod method;
	const gchar *path;
	GList *l;
	HtmxResponse *response;

	g_return_val_if_fail(HTMX_IS_ROUTER(self), NULL);
	g_return_val_if_fail(HTMX_IS_REQUEST(request), NULL);

	method = htmx_request_get_method(request);
	path = htmx_request_get_path(request);

	*params = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);

	/* Try dynamic routes first */
	for (l = self->routes; l != NULL; l = l->next) {
		Route *route = (Route *)l->data;
		GMatchInfo *match_info;

		if (route->method != method) {
			continue;
		}

		if (g_regex_match(route->regex, path, 0, &match_info)) {
			gint i;

			/* Extract parameters */
			for (i = 0; i < route->param_count; i++) {
				gchar *value = g_match_info_fetch(match_info, i + 1);
				if (value != NULL) {
					g_hash_table_insert(*params, g_strdup(route->param_names[i]), value);
				}
			}

			g_match_info_free(match_info);

			return route->callback(request, *params, route->user_data);
		}

		g_match_info_free(match_info);
	}

	/* Try static routes */
	if (method == HTMX_METHOD_GET) {
		response = try_serve_static(self, path);
		if (response != NULL) {
			return response;
		}
	}

	/* No match found */
	g_hash_table_unref(*params);
	*params = NULL;

	return NULL;
}
