/*
 * todo-app.c - Example htmx-glib todo application
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * A simple todo application demonstrating htmx-glib features:
 * - HTML element building with HtmxBuilder
 * - Request/Response handling
 * - Routing with URL parameters
 * - HTMX attributes (hx-get, hx-post, hx-delete, hx-swap, hx-target)
 */

#include <htmx-glib.h>
#include <stdio.h>
#include <stdlib.h>

/* Simple in-memory todo storage */
typedef struct {
	guint   id;
	gchar  *text;
	gboolean completed;
} TodoItem;

static GList *todos = NULL;
static guint next_id = 1;

/*
 * Find a todo by ID
 */
static TodoItem *
find_todo(guint id)
{
	GList *l;

	for (l = todos; l != NULL; l = l->next) {
		TodoItem *item = l->data;
		if (item->id == id) {
			return item;
		}
	}

	return NULL;
}

/*
 * Free a todo item
 */
static void
free_todo(TodoItem *item)
{
	if (item != NULL) {
		g_free(item->text);
		g_free(item);
	}
}

/*
 * Render a single todo item as HTML
 */
static gchar *
render_todo_item(TodoItem *item)
{
	g_autoptr(HtmxBuilder) builder = htmx_builder_new();
	g_autofree gchar *delete_url = NULL;
	g_autofree gchar *toggle_url = NULL;

	delete_url = g_strdup_printf("/todos/%u", item->id);
	toggle_url = g_strdup_printf("/todos/%u/toggle", item->id);

	htmx_builder_begin(builder, "li");
	htmx_builder_id(builder, g_strdup_printf("todo-%u", item->id));
	htmx_builder_class(builder, item->completed ? "completed" : "pending");

	/* Checkbox for toggling completion */
	htmx_builder_begin_void(builder, "input");
	htmx_builder_attr(builder, "type", "checkbox");
	if (item->completed) {
		htmx_builder_attr(builder, "checked", "checked");
	}
	htmx_builder_hx_post(builder, toggle_url);
	htmx_builder_hx_target(builder, g_strdup_printf("#todo-%u", item->id));
	htmx_builder_hx_swap(builder, HTMX_SWAP_OUTER_HTML);
	htmx_builder_end_void(builder);

	/* Todo text */
	htmx_builder_begin(builder, "span");
	htmx_builder_class(builder, "todo-text");
	htmx_builder_text(builder, item->text);
	htmx_builder_end(builder);

	/* Delete button */
	htmx_builder_begin(builder, "button");
	htmx_builder_class(builder, "delete-btn");
	htmx_builder_hx_delete(builder, delete_url);
	htmx_builder_hx_target(builder, g_strdup_printf("#todo-%u", item->id));
	htmx_builder_hx_swap(builder, HTMX_SWAP_OUTER_HTML);
	htmx_builder_hx_confirm(builder, "Delete this todo?");
	htmx_builder_text(builder, "Delete");
	htmx_builder_end(builder);

	htmx_builder_end(builder);

	return htmx_builder_render(builder);
}

/*
 * Render the todo list
 */
static gchar *
render_todo_list(void)
{
	g_autoptr(HtmxBuilder) builder = htmx_builder_new();
	GList *l;

	htmx_builder_begin(builder, "ul");
	htmx_builder_id(builder, "todo-list");
	htmx_builder_class(builder, "todos");

	for (l = todos; l != NULL; l = l->next) {
		TodoItem *item = l->data;
		g_autofree gchar *item_html = render_todo_item(item);
		htmx_builder_raw_html(builder, item_html);
	}

	htmx_builder_end(builder);

	return htmx_builder_render(builder);
}

/*
 * Render the complete page
 */
static gchar *
render_page(void)
{
	g_autoptr(HtmxBuilder) builder = htmx_builder_new();
	g_autofree gchar *todo_list = NULL;

	todo_list = render_todo_list();

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
	htmx_builder_text(builder, "htmx-glib Todo App");
	htmx_builder_end(builder);

	/* HTMX script */
	htmx_builder_begin(builder, "script");
	htmx_builder_attr(builder, "src", "https://unpkg.com/htmx.org@1.9.10");
	htmx_builder_end(builder);

	/* Inline styles */
	htmx_builder_begin(builder, "style");
	htmx_builder_text(builder,
		"body { font-family: system-ui, sans-serif; max-width: 600px; margin: 2em auto; padding: 0 1em; }\n"
		"h1 { color: #333; }\n"
		".todos { list-style: none; padding: 0; }\n"
		".todos li { display: flex; align-items: center; padding: 0.5em; border-bottom: 1px solid #eee; }\n"
		".todos li.completed .todo-text { text-decoration: line-through; color: #888; }\n"
		".todo-text { flex: 1; margin: 0 1em; }\n"
		".delete-btn { background: #dc3545; color: white; border: none; padding: 0.25em 0.5em; cursor: pointer; border-radius: 3px; }\n"
		".delete-btn:hover { background: #c82333; }\n"
		"#add-form { display: flex; gap: 0.5em; margin-bottom: 1em; }\n"
		"#add-form input[type=text] { flex: 1; padding: 0.5em; border: 1px solid #ccc; border-radius: 3px; }\n"
		"#add-form button { background: #28a745; color: white; border: none; padding: 0.5em 1em; cursor: pointer; border-radius: 3px; }\n"
		"#add-form button:hover { background: #218838; }\n"
		".htmx-request { opacity: 0.5; }\n"
	);
	htmx_builder_end(builder);

	htmx_builder_end(builder); /* head */

	/* Body */
	htmx_builder_begin(builder, "body");

	htmx_builder_begin(builder, "h1");
	htmx_builder_text(builder, "Todo List");
	htmx_builder_end(builder);

	/* Add todo form */
	htmx_builder_begin(builder, "form");
	htmx_builder_id(builder, "add-form");
	htmx_builder_hx_post(builder, "/todos");
	htmx_builder_hx_target(builder, "#todo-list");
	htmx_builder_hx_swap(builder, HTMX_SWAP_BEFORE_END);
	htmx_builder_attr(builder, "hx-on::after-request", "this.reset()");

	htmx_builder_begin_void(builder, "input");
	htmx_builder_attr(builder, "type", "text");
	htmx_builder_attr(builder, "name", "text");
	htmx_builder_attr(builder, "placeholder", "Add a new todo...");
	htmx_builder_attr(builder, "required", "required");
	htmx_builder_end_void(builder);

	htmx_builder_begin(builder, "button");
	htmx_builder_attr(builder, "type", "submit");
	htmx_builder_text(builder, "Add");
	htmx_builder_end(builder);

	htmx_builder_end(builder); /* form */

	/* Todo list */
	htmx_builder_raw_html(builder, todo_list);

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
	g_autofree gchar *html = NULL;

	html = render_page();
	htmx_response_set_content(response, html);
	htmx_response_set_content_type(response, "text/html; charset=utf-8");

	return (HtmxResponse *)g_steal_pointer(&response);
}

/*
 * Handle GET /todos - Get the todo list
 */
static HtmxResponse *
handle_get_todos(HtmxRequest *request, GHashTable *params, gpointer user_data)
{
	g_autoptr(HtmxResponse) response = htmx_response_new();
	g_autofree gchar *html = NULL;

	html = render_todo_list();
	htmx_response_set_content(response, html);

	return (HtmxResponse *)g_steal_pointer(&response);
}

/*
 * Handle POST /todos - Create a new todo
 */
static HtmxResponse *
handle_create_todo(HtmxRequest *request, GHashTable *params, gpointer user_data)
{
	g_autoptr(HtmxResponse) response = htmx_response_new();
	GHashTable *form_data;
	const gchar *text;
	TodoItem *item;
	g_autofree gchar *html = NULL;

	form_data = htmx_request_get_form_data(request);
	if (form_data == NULL) {
		htmx_response_set_status(response, 400);
		htmx_response_set_content(response, "Missing form data");
		return (HtmxResponse *)g_steal_pointer(&response);
	}

	text = g_hash_table_lookup(form_data, "text");
	if (text == NULL || *text == '\0') {
		htmx_response_set_status(response, 400);
		htmx_response_set_content(response, "Todo text is required");
		return (HtmxResponse *)g_steal_pointer(&response);
	}

	/* Create the new todo */
	item = g_new0(TodoItem, 1);
	item->id = next_id++;
	item->text = g_strdup(text);
	item->completed = FALSE;

	todos = g_list_append(todos, item);

	/* Return just the new item */
	html = render_todo_item(item);
	htmx_response_set_content(response, html);

	return (HtmxResponse *)g_steal_pointer(&response);
}

/*
 * Handle DELETE /todos/:id - Delete a todo
 */
static HtmxResponse *
handle_delete_todo(HtmxRequest *request, GHashTable *params, gpointer user_data)
{
	g_autoptr(HtmxResponse) response = htmx_response_new();
	const gchar *id_str;
	guint id;
	TodoItem *item;
	GList *node;

	id_str = g_hash_table_lookup(params, "id");
	if (id_str == NULL) {
		htmx_response_set_status(response, 400);
		htmx_response_set_content(response, "Missing todo ID");
		return (HtmxResponse *)g_steal_pointer(&response);
	}

	id = (guint)g_ascii_strtoull(id_str, NULL, 10);
	item = find_todo(id);

	if (item == NULL) {
		htmx_response_set_status(response, 404);
		htmx_response_set_content(response, "Todo not found");
		return (HtmxResponse *)g_steal_pointer(&response);
	}

	/* Remove from list */
	node = g_list_find(todos, item);
	todos = g_list_delete_link(todos, node);
	free_todo(item);

	/* Return empty content to remove the element */
	htmx_response_set_content(response, "");

	return (HtmxResponse *)g_steal_pointer(&response);
}

/*
 * Handle POST /todos/:id/toggle - Toggle todo completion
 */
static HtmxResponse *
handle_toggle_todo(HtmxRequest *request, GHashTable *params, gpointer user_data)
{
	g_autoptr(HtmxResponse) response = htmx_response_new();
	const gchar *id_str;
	guint id;
	TodoItem *item;
	g_autofree gchar *html = NULL;

	id_str = g_hash_table_lookup(params, "id");
	if (id_str == NULL) {
		htmx_response_set_status(response, 400);
		htmx_response_set_content(response, "Missing todo ID");
		return (HtmxResponse *)g_steal_pointer(&response);
	}

	id = (guint)g_ascii_strtoull(id_str, NULL, 10);
	item = find_todo(id);

	if (item == NULL) {
		htmx_response_set_status(response, 404);
		htmx_response_set_content(response, "Todo not found");
		return (HtmxResponse *)g_steal_pointer(&response);
	}

	/* Toggle completion */
	item->completed = !item->completed;

	/* Return updated item */
	html = render_todo_item(item);
	htmx_response_set_content(response, html);

	return (HtmxResponse *)g_steal_pointer(&response);
}

/*
 * Clean up todos on exit
 */
static void
cleanup_todos(void)
{
	g_list_free_full(todos, (GDestroyNotify)free_todo);
	todos = NULL;
}

int
main(int argc, char *argv[])
{
	g_autoptr(HtmxServer) server = NULL;
	HtmxRouter *router;
	guint port = 8080;

	/* Parse command line for port */
	if (argc > 1) {
		port = (guint)g_ascii_strtoull(argv[1], NULL, 10);
		if (port == 0) {
			port = 8080;
		}
	}

	/* Create server */
	server = htmx_server_new_with_port(port);
	if (server == NULL) {
		g_printerr("Failed to create server\n");
		return 1;
	}

	/* Set up routes */
	router = htmx_server_get_router(server);

	htmx_router_get(router, "/", handle_index, NULL);
	htmx_router_get(router, "/todos", handle_get_todos, NULL);
	htmx_router_post(router, "/todos", handle_create_todo, NULL);
	htmx_router_delete(router, "/todos/:id", handle_delete_todo, NULL);
	htmx_router_post(router, "/todos/:id/toggle", handle_toggle_todo, NULL);

	/* Add some sample todos */
	{
		TodoItem *item;

		item = g_new0(TodoItem, 1);
		item->id = next_id++;
		item->text = g_strdup("Learn htmx-glib");
		item->completed = FALSE;
		todos = g_list_append(todos, item);

		item = g_new0(TodoItem, 1);
		item->id = next_id++;
		item->text = g_strdup("Build something awesome");
		item->completed = FALSE;
		todos = g_list_append(todos, item);

		item = g_new0(TodoItem, 1);
		item->id = next_id++;
		item->text = g_strdup("Star the repository");
		item->completed = TRUE;
		todos = g_list_append(todos, item);
	}

	g_print("htmx-glib Todo App\n");
	g_print("Server running at http://localhost:%u\n", port);
	g_print("Press Ctrl+C to stop\n\n");

	/* Register cleanup */
	atexit(cleanup_todos);

	/* Run the server */
	htmx_server_run(server);

	return 0;
}
