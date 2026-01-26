/*
 * data-table.c - Data table example with sorting and filtering
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * Demonstrates htmx-glib table elements:
 * - HtmxTable, HtmxThead, HtmxTbody, HtmxTfoot
 * - HtmxTr, HtmxTh, HtmxTd
 * - HtmxCaption, HtmxColgroup, HtmxCol
 * - HTMX-powered sorting and live search
 */

#include <htmx-glib.h>
#include <stdio.h>
#include <string.h>

/* Sample employee data */
typedef struct {
	guint        id;
	const gchar *name;
	const gchar *department;
	const gchar *email;
	guint        salary;
} Employee;

static Employee employees[] = {
	{ 1, "Alice Johnson", "Engineering", "alice@example.com", 95000 },
	{ 2, "Bob Smith", "Marketing", "bob@example.com", 75000 },
	{ 3, "Carol Williams", "Engineering", "carol@example.com", 105000 },
	{ 4, "David Brown", "Sales", "david@example.com", 85000 },
	{ 5, "Eve Davis", "Engineering", "eve@example.com", 92000 },
	{ 6, "Frank Miller", "HR", "frank@example.com", 68000 },
	{ 7, "Grace Wilson", "Marketing", "grace@example.com", 72000 },
	{ 8, "Henry Moore", "Sales", "henry@example.com", 88000 },
	{ 9, "Ivy Taylor", "Engineering", "ivy@example.com", 98000 },
	{ 10, "Jack Anderson", "Finance", "jack@example.com", 82000 },
};

static const guint num_employees = G_N_ELEMENTS(employees);

/*
 * Render a single table row using explicit element types
 */
static void
render_employee_row(
	HtmxTbody *tbody,
	Employee  *emp
){
	g_autoptr(HtmxTr) row = htmx_tr_new();
	g_autoptr(HtmxTd) td_id = htmx_td_new_with_text(g_strdup_printf("%u", emp->id));
	g_autoptr(HtmxTd) td_name = htmx_td_new_with_text(emp->name);
	g_autoptr(HtmxTd) td_dept = htmx_td_new_with_text(emp->department);
	g_autoptr(HtmxTd) td_email = htmx_td_new();
	g_autoptr(HtmxTd) td_salary = htmx_td_new_with_text(g_strdup_printf("$%u", emp->salary));

	/* Email cell with mailto link */
	{
		g_autofree gchar *mailto = g_strdup_printf("mailto:%s", emp->email);
		g_autoptr(HtmxA) email_link = htmx_a_new_with_href(mailto);
		htmx_node_set_text_content(HTMX_NODE(email_link), emp->email);
		htmx_node_add_child(HTMX_NODE(td_email), HTMX_NODE(email_link));
	}

	/* Add all cells to row */
	htmx_node_add_child(HTMX_NODE(row), HTMX_NODE(td_id));
	htmx_node_add_child(HTMX_NODE(row), HTMX_NODE(td_name));
	htmx_node_add_child(HTMX_NODE(row), HTMX_NODE(td_dept));
	htmx_node_add_child(HTMX_NODE(row), HTMX_NODE(td_email));
	htmx_node_add_child(HTMX_NODE(row), HTMX_NODE(td_salary));

	htmx_node_add_child(HTMX_NODE(tbody), HTMX_NODE(row));
}

/*
 * Render the table body with optional search filter
 */
static gchar *
render_table_body(const gchar *search_term)
{
	g_autoptr(HtmxTbody) tbody = htmx_tbody_new();
	guint i;

	htmx_element_set_id(HTMX_ELEMENT(tbody), "employee-data");

	for (i = 0; i < num_employees; i++) {
		Employee *emp = &employees[i];

		/* Filter by search term if provided */
		if (search_term != NULL && *search_term != '\0') {
			g_autofree gchar *lower_name = g_utf8_strdown(emp->name, -1);
			g_autofree gchar *lower_dept = g_utf8_strdown(emp->department, -1);
			g_autofree gchar *lower_search = g_utf8_strdown(search_term, -1);

			if (strstr(lower_name, lower_search) == NULL &&
			    strstr(lower_dept, lower_search) == NULL) {
				continue;
			}
		}

		render_employee_row(tbody, emp);
	}

	return htmx_element_render(HTMX_ELEMENT(tbody));
}

/*
 * Render the complete data table
 */
static gchar *
render_table(void)
{
	g_autoptr(HtmxTable) table = htmx_table_new();
	g_autoptr(HtmxCaption) caption = htmx_caption_new_with_text("Employee Directory");
	g_autoptr(HtmxColgroup) colgroup = htmx_colgroup_new();
	g_autoptr(HtmxThead) thead = htmx_thead_new();
	g_autoptr(HtmxTfoot) tfoot = htmx_tfoot_new();

	/* Set up table attributes */
	htmx_element_set_id(HTMX_ELEMENT(table), "employees");
	htmx_element_add_class(HTMX_ELEMENT(table), "data-table");

	/* Column group for styling */
	{
		g_autoptr(HtmxCol) col_id = htmx_col_new();
		g_autoptr(HtmxCol) col_name = htmx_col_new();
		g_autoptr(HtmxCol) col_dept = htmx_col_new();
		g_autoptr(HtmxCol) col_email = htmx_col_new();
		g_autoptr(HtmxCol) col_salary = htmx_col_new();

		htmx_element_add_class(HTMX_ELEMENT(col_id), "col-id");
		htmx_element_add_class(HTMX_ELEMENT(col_name), "col-name");
		htmx_element_add_class(HTMX_ELEMENT(col_dept), "col-dept");
		htmx_element_add_class(HTMX_ELEMENT(col_email), "col-email");
		htmx_element_add_class(HTMX_ELEMENT(col_salary), "col-salary");

		htmx_node_add_child(HTMX_NODE(colgroup), HTMX_NODE(col_id));
		htmx_node_add_child(HTMX_NODE(colgroup), HTMX_NODE(col_name));
		htmx_node_add_child(HTMX_NODE(colgroup), HTMX_NODE(col_dept));
		htmx_node_add_child(HTMX_NODE(colgroup), HTMX_NODE(col_email));
		htmx_node_add_child(HTMX_NODE(colgroup), HTMX_NODE(col_salary));
	}

	/* Table header */
	{
		g_autoptr(HtmxTr) header_row = htmx_tr_new();
		g_autoptr(HtmxTh) th_id = htmx_th_new_with_text("ID");
		g_autoptr(HtmxTh) th_name = htmx_th_new_with_text("Name");
		g_autoptr(HtmxTh) th_dept = htmx_th_new_with_text("Department");
		g_autoptr(HtmxTh) th_email = htmx_th_new_with_text("Email");
		g_autoptr(HtmxTh) th_salary = htmx_th_new_with_text("Salary");

		htmx_th_set_scope(th_id, "col");
		htmx_th_set_scope(th_name, "col");
		htmx_th_set_scope(th_dept, "col");
		htmx_th_set_scope(th_email, "col");
		htmx_th_set_scope(th_salary, "col");

		htmx_node_add_child(HTMX_NODE(header_row), HTMX_NODE(th_id));
		htmx_node_add_child(HTMX_NODE(header_row), HTMX_NODE(th_name));
		htmx_node_add_child(HTMX_NODE(header_row), HTMX_NODE(th_dept));
		htmx_node_add_child(HTMX_NODE(header_row), HTMX_NODE(th_email));
		htmx_node_add_child(HTMX_NODE(header_row), HTMX_NODE(th_salary));

		htmx_node_add_child(HTMX_NODE(thead), HTMX_NODE(header_row));
	}

	/* Table footer with total */
	{
		g_autoptr(HtmxTr) footer_row = htmx_tr_new();
		g_autoptr(HtmxTd) td_label = htmx_td_new_with_text("Total Employees:");
		g_autoptr(HtmxTd) td_count = htmx_td_new_with_text(
			g_strdup_printf("%u", num_employees)
		);

		htmx_td_set_colspan(td_label, 4);
		htmx_element_add_class(HTMX_ELEMENT(td_label), "footer-label");
		htmx_element_add_class(HTMX_ELEMENT(td_count), "footer-count");

		htmx_node_add_child(HTMX_NODE(footer_row), HTMX_NODE(td_label));
		htmx_node_add_child(HTMX_NODE(footer_row), HTMX_NODE(td_count));
		htmx_node_add_child(HTMX_NODE(tfoot), HTMX_NODE(footer_row));
	}

	/* Assemble table */
	htmx_node_add_child(HTMX_NODE(table), HTMX_NODE(caption));
	htmx_node_add_child(HTMX_NODE(table), HTMX_NODE(colgroup));
	htmx_node_add_child(HTMX_NODE(table), HTMX_NODE(thead));

	/* Add body with initial data - create tbody and add directly */
	{
		g_autoptr(HtmxTbody) tbody = htmx_tbody_new();
		guint i;

		htmx_element_set_id(HTMX_ELEMENT(tbody), "employee-data");

		for (i = 0; i < num_employees; i++) {
			render_employee_row(tbody, &employees[i]);
		}

		htmx_node_add_child(HTMX_NODE(table), HTMX_NODE(tbody));
	}

	htmx_node_add_child(HTMX_NODE(table), HTMX_NODE(tfoot));

	return htmx_element_render(HTMX_ELEMENT(table));
}

/*
 * Render the complete page
 */
static gchar *
render_page(void)
{
	g_autoptr(HtmxBuilder) builder = htmx_builder_new();
	g_autofree gchar *table_html = render_table();

	htmx_builder_doctype(builder);
	htmx_builder_begin(builder, "html");
	htmx_builder_attr(builder, "lang", "en");

	/* Head */
	htmx_builder_begin(builder, "head");

	htmx_builder_begin_void(builder, "meta");
	htmx_builder_attr(builder, "charset", "utf-8");
	htmx_builder_end_void(builder);

	htmx_builder_begin(builder, "title");
	htmx_builder_text(builder, "Data Table - htmx-glib Example");
	htmx_builder_end(builder);

	htmx_builder_begin(builder, "script");
	htmx_builder_attr(builder, "src", "https://unpkg.com/htmx.org@1.9.10");
	htmx_builder_end(builder);

	htmx_builder_begin(builder, "style");
	htmx_builder_text(builder,
		"body { font-family: system-ui, sans-serif; max-width: 900px; margin: 2em auto; padding: 0 1em; }\n"
		"h1 { color: #333; }\n"
		".search-box { margin-bottom: 1em; }\n"
		".search-box input { padding: 0.5em; width: 300px; border: 1px solid #ccc; border-radius: 4px; }\n"
		".data-table { width: 100%; border-collapse: collapse; margin-top: 1em; }\n"
		".data-table caption { font-size: 1.2em; font-weight: bold; margin-bottom: 0.5em; }\n"
		".data-table th, .data-table td { padding: 0.75em; text-align: left; border-bottom: 1px solid #ddd; }\n"
		".data-table thead { background: #f5f5f5; }\n"
		".data-table th { font-weight: 600; }\n"
		".data-table tbody tr:hover { background: #f9f9f9; }\n"
		".data-table tfoot { background: #e9ecef; font-weight: 600; }\n"
		".col-id { width: 50px; }\n"
		".col-salary { text-align: right; }\n"
		".footer-label { text-align: right; }\n"
		".footer-count { text-align: right; }\n"
		"a { color: #0066cc; text-decoration: none; }\n"
		"a:hover { text-decoration: underline; }\n"
		".htmx-request tbody { opacity: 0.5; }\n"
	);
	htmx_builder_end(builder);

	htmx_builder_end(builder); /* head */

	/* Body */
	htmx_builder_begin(builder, "body");

	htmx_builder_begin(builder, "h1");
	htmx_builder_text(builder, "Employee Data Table");
	htmx_builder_end(builder);

	/* Search box with HTMX live search */
	htmx_builder_begin(builder, "div");
	htmx_builder_class(builder, "search-box");

	htmx_builder_begin_void(builder, "input");
	htmx_builder_attr(builder, "type", "search");
	htmx_builder_attr(builder, "name", "search");
	htmx_builder_attr(builder, "placeholder", "Search by name or department...");
	htmx_builder_attr(builder, "hx-get", "/search");
	htmx_builder_attr(builder, "hx-trigger", "input changed delay:300ms, search");
	htmx_builder_attr(builder, "hx-target", "#employee-data");
	htmx_builder_attr(builder, "hx-swap", "outerHTML");
	htmx_builder_attr(builder, "hx-indicator", "#employees");
	htmx_builder_end_void(builder);

	htmx_builder_end(builder); /* search-box */

	/* Data table */
	htmx_builder_raw_html(builder, table_html);

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
 * Handle GET /search - Search employees
 */
static HtmxResponse *
handle_search(HtmxRequest *request, GHashTable *params, gpointer user_data)
{
	g_autoptr(HtmxResponse) response = htmx_response_new();
	GHashTable *query;
	const gchar *search_term = NULL;
	g_autofree gchar *html = NULL;

	query = htmx_request_get_query_params(request);
	if (query != NULL) {
		search_term = g_hash_table_lookup(query, "search");
	}

	html = render_table_body(search_term);
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
	htmx_router_get(router, "/search", handle_search, NULL);

	g_print("Data Table Example\n");
	g_print("Server running at http://localhost:%u\n", port);
	g_print("Press Ctrl+C to stop\n\n");

	htmx_server_run(server);

	return 0;
}
