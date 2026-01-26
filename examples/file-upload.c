/*
 * file-upload.c - File upload example with htmx-glib
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * Demonstrates file upload handling with htmx-glib:
 * - Multipart form data parsing with SoupMultipart
 * - Drag-and-drop file upload with HTMX
 * - File listing and deletion
 * - File download
 */

#include <htmx-glib.h>
#include <stdlib.h>
#include <string.h>

/* Maximum file size (5MB) */
#define MAX_FILE_SIZE (5 * 1024 * 1024)

/* Uploaded file data */
typedef struct {
	guint    id;
	gchar   *filename;
	gchar   *content_type;
	GBytes  *data;
	gsize    size;
	gint64   uploaded_at;
} UploadedFile;

static GList *uploaded_files = NULL;
static guint next_file_id = 1;

/*
 * Find an uploaded file by ID
 */
static UploadedFile *
find_file(guint id)
{
	GList *l;

	for (l = uploaded_files; l != NULL; l = l->next) {
		UploadedFile *file = l->data;
		if (file->id == id) {
			return file;
		}
	}

	return NULL;
}

/*
 * Free an uploaded file
 */
static void
free_file(UploadedFile *file)
{
	if (file != NULL) {
		g_free(file->filename);
		g_free(file->content_type);
		g_bytes_unref(file->data);
		g_free(file);
	}
}

/*
 * Format file size for display
 */
static gchar *
format_file_size(gsize size)
{
	if (size < 1024) {
		return g_strdup_printf("%zu B", size);
	} else if (size < 1024 * 1024) {
		return g_strdup_printf("%.1f KB", (gdouble)size / 1024.0);
	} else {
		return g_strdup_printf("%.1f MB", (gdouble)size / (1024.0 * 1024.0));
	}
}

/*
 * Format timestamp for display
 */
static gchar *
format_timestamp(gint64 timestamp)
{
	g_autoptr(GDateTime) dt = g_date_time_new_from_unix_local(timestamp);

	if (dt == NULL) {
		return g_strdup("Unknown");
	}

	return g_date_time_format(dt, "%Y-%m-%d %H:%M:%S");
}

/*
 * Render a single file table row
 */
static HtmxTr *
render_file_row(UploadedFile *file)
{
	g_autoptr(HtmxTr) row = htmx_tr_new();
	g_autofree gchar *size_str = format_file_size(file->size);
	g_autofree gchar *time_str = format_timestamp(file->uploaded_at);
	g_autofree gchar *delete_url = g_strdup_printf("/files/%u", file->id);
	g_autofree gchar *download_url = g_strdup_printf("/files/%u/download", file->id);
	g_autofree gchar *row_id = g_strdup_printf("file-%u", file->id);

	htmx_element_set_id(HTMX_ELEMENT(row), row_id);
	htmx_element_add_class(HTMX_ELEMENT(row), "file-row");

	/* Filename cell with download link */
	{
		g_autoptr(HtmxTd) cell = htmx_td_new();
		g_autoptr(HtmxA) link = htmx_a_new_with_href(download_url);

		htmx_node_set_text_content(HTMX_NODE(link), file->filename);
		htmx_element_add_class(HTMX_ELEMENT(link), "download-link");

		htmx_node_add_child(HTMX_NODE(cell), HTMX_NODE(link));
		htmx_node_add_child(HTMX_NODE(row), HTMX_NODE(cell));
	}

	/* Content type cell */
	{
		g_autoptr(HtmxTd) cell = htmx_td_new_with_text(
			file->content_type != NULL ? file->content_type : "unknown");
		htmx_node_add_child(HTMX_NODE(row), HTMX_NODE(cell));
	}

	/* Size cell */
	{
		g_autoptr(HtmxTd) cell = htmx_td_new_with_text(size_str);
		htmx_node_add_child(HTMX_NODE(row), HTMX_NODE(cell));
	}

	/* Upload time cell */
	{
		g_autoptr(HtmxTd) cell = htmx_td_new_with_text(time_str);
		htmx_node_add_child(HTMX_NODE(row), HTMX_NODE(cell));
	}

	/* Actions cell */
	{
		g_autoptr(HtmxTd) cell = htmx_td_new();
		g_autoptr(HtmxButton) delete_btn = htmx_button_new_with_label("Delete");
		g_autofree gchar *target = g_strdup_printf("#%s", row_id);

		htmx_element_add_class(HTMX_ELEMENT(delete_btn), "delete-btn");
		htmx_element_set_attribute(HTMX_ELEMENT(delete_btn), "hx-delete", delete_url);
		htmx_element_set_attribute(HTMX_ELEMENT(delete_btn), "hx-target", target);
		htmx_element_set_attribute(HTMX_ELEMENT(delete_btn), "hx-swap", "outerHTML");
		htmx_element_set_attribute(HTMX_ELEMENT(delete_btn), "hx-confirm", "Delete this file?");

		htmx_node_add_child(HTMX_NODE(cell), HTMX_NODE(delete_btn));
		htmx_node_add_child(HTMX_NODE(row), HTMX_NODE(cell));
	}

	return (HtmxTr *)g_steal_pointer(&row);
}

/*
 * Render the file list table
 */
static HtmxTable *
render_file_table(void)
{
	g_autoptr(HtmxTable) table = htmx_table_new();
	g_autoptr(HtmxThead) thead = htmx_thead_new();
	g_autoptr(HtmxTbody) tbody = htmx_tbody_new();

	htmx_element_set_id(HTMX_ELEMENT(table), "file-list");
	htmx_element_add_class(HTMX_ELEMENT(table), "file-table");
	htmx_element_set_id(HTMX_ELEMENT(tbody), "file-list-body");

	/* Table header row */
	{
		g_autoptr(HtmxTr) header_row = htmx_tr_new();
		const gchar *headers[] = { "Filename", "Type", "Size", "Uploaded", "Actions" };
		guint i;

		for (i = 0; i < G_N_ELEMENTS(headers); i++) {
			g_autoptr(HtmxTh) th = htmx_th_new_with_text(headers[i]);
			htmx_node_add_child(HTMX_NODE(header_row), HTMX_NODE(th));
		}

		htmx_node_add_child(HTMX_NODE(thead), HTMX_NODE(header_row));
	}

	/* Table body with files or empty message */
	if (uploaded_files == NULL) {
		g_autoptr(HtmxTr) empty_row = htmx_tr_new();
		g_autoptr(HtmxTd) empty_cell = htmx_td_new();

		htmx_element_set_id(HTMX_ELEMENT(empty_row), "empty-row");
		htmx_td_set_colspan(empty_cell, 5);
		htmx_element_add_class(HTMX_ELEMENT(empty_cell), "empty-message");
		htmx_node_set_text_content(HTMX_NODE(empty_cell),
			"No files uploaded yet. Drag and drop files above or click to browse.");

		htmx_node_add_child(HTMX_NODE(empty_row), HTMX_NODE(empty_cell));
		htmx_node_add_child(HTMX_NODE(tbody), HTMX_NODE(empty_row));
	} else {
		GList *l;
		for (l = uploaded_files; l != NULL; l = l->next) {
			UploadedFile *file = l->data;
			g_autoptr(HtmxTr) row = render_file_row(file);
			htmx_node_add_child(HTMX_NODE(tbody), HTMX_NODE(row));
		}
	}

	htmx_node_add_child(HTMX_NODE(table), HTMX_NODE(thead));
	htmx_node_add_child(HTMX_NODE(table), HTMX_NODE(tbody));

	return (HtmxTable *)g_steal_pointer(&table);
}

/*
 * Render the upload form with drop zone
 */
static HtmxForm *
render_upload_form(void)
{
	g_autoptr(HtmxForm) form = htmx_form_new();
	g_autoptr(HtmxInput) file_input = htmx_input_new(HTMX_INPUT_FILE);
	g_autoptr(HtmxLabel) drop_zone = htmx_label_new_for("file-input");
	g_autoptr(HtmxDiv) drop_content = htmx_div_new();
	g_autoptr(HtmxDiv) form_actions = htmx_div_new();
	g_autoptr(HtmxButton) submit_btn = htmx_button_new_submit();
	g_autoptr(HtmxDiv) progress = htmx_div_new();

	/* Form setup with HTMX attributes */
	htmx_element_set_id(HTMX_ELEMENT(form), "upload-form");
	htmx_element_set_attribute(HTMX_ELEMENT(form), "hx-post", "/upload");
	htmx_element_set_attribute(HTMX_ELEMENT(form), "hx-target", "#file-list-body");
	htmx_element_set_attribute(HTMX_ELEMENT(form), "hx-swap", "beforeend");
	htmx_element_set_attribute(HTMX_ELEMENT(form), "hx-encoding", "multipart/form-data");
	htmx_element_set_attribute(HTMX_ELEMENT(form), "hx-on::after-request",
		"this.reset(); document.getElementById('empty-row')?.remove();");

	/* Hidden file input */
	htmx_input_set_name(file_input, "file");
	htmx_element_set_id(HTMX_ELEMENT(file_input), "file-input");
	htmx_element_add_class(HTMX_ELEMENT(file_input), "file-input");
	htmx_element_set_attribute(HTMX_ELEMENT(file_input), "multiple", "multiple");

	/* Drop zone (label wrapping content) */
	htmx_element_set_id(HTMX_ELEMENT(drop_zone), "drop-zone");
	htmx_element_add_class(HTMX_ELEMENT(drop_zone), "drop-zone");

	/* Drop zone content */
	htmx_element_add_class(HTMX_ELEMENT(drop_content), "drop-zone-content");

	{
		g_autoptr(HtmxP) icon = htmx_p_new();
		g_autoptr(HtmxP) text = htmx_p_new();
		g_autoptr(HtmxP) hint = htmx_p_new();

		htmx_element_add_class(HTMX_ELEMENT(icon), "drop-icon");
		htmx_node_set_text_content(HTMX_NODE(icon), "📁");

		htmx_element_set_id(HTMX_ELEMENT(text), "drop-zone-text");
		htmx_node_set_text_content(HTMX_NODE(text), "Drag and drop files here");

		htmx_element_add_class(HTMX_ELEMENT(hint), "drop-hint");
		htmx_node_set_text_content(HTMX_NODE(hint),
			"or click anywhere in this box to browse (max 5MB per file)");

		htmx_node_add_child(HTMX_NODE(drop_content), HTMX_NODE(icon));
		htmx_node_add_child(HTMX_NODE(drop_content), HTMX_NODE(text));
		htmx_node_add_child(HTMX_NODE(drop_content), HTMX_NODE(hint));
	}

	htmx_node_add_child(HTMX_NODE(drop_zone), HTMX_NODE(drop_content));

	/* Submit button */
	htmx_element_add_class(HTMX_ELEMENT(form_actions), "form-actions");
	htmx_node_set_text_content(HTMX_NODE(submit_btn), "Upload Files");
	htmx_element_add_class(HTMX_ELEMENT(submit_btn), "upload-btn");
	htmx_node_add_child(HTMX_NODE(form_actions), HTMX_NODE(submit_btn));

	/* Progress indicator */
	htmx_element_set_id(HTMX_ELEMENT(progress), "upload-progress");
	htmx_element_add_class(HTMX_ELEMENT(progress), "progress-container");
	htmx_element_set_attribute(HTMX_ELEMENT(progress), "style", "display: none;");

	{
		g_autoptr(HtmxDiv) bar = htmx_div_new();
		g_autoptr(HtmxDiv) fill = htmx_div_new();
		g_autoptr(HtmxSpan) text = htmx_span_new();

		htmx_element_add_class(HTMX_ELEMENT(bar), "progress-bar");
		htmx_element_set_id(HTMX_ELEMENT(fill), "progress-fill");
		htmx_element_add_class(HTMX_ELEMENT(fill), "progress-fill");

		htmx_node_add_child(HTMX_NODE(bar), HTMX_NODE(fill));

		htmx_element_set_id(HTMX_ELEMENT(text), "progress-text");
		htmx_node_set_text_content(HTMX_NODE(text), "Uploading...");

		htmx_node_add_child(HTMX_NODE(progress), HTMX_NODE(bar));
		htmx_node_add_child(HTMX_NODE(progress), HTMX_NODE(text));
	}

	/* Assemble form */
	htmx_node_add_child(HTMX_NODE(form), HTMX_NODE(file_input));
	htmx_node_add_child(HTMX_NODE(form), HTMX_NODE(drop_zone));
	htmx_node_add_child(HTMX_NODE(form), HTMX_NODE(form_actions));
	htmx_node_add_child(HTMX_NODE(form), HTMX_NODE(progress));

	return (HtmxForm *)g_steal_pointer(&form);
}

/*
 * Render the main content area
 */
static HtmxMain *
render_main_content(void)
{
	g_autoptr(HtmxMain) main_content = htmx_main_new();
	g_autoptr(HtmxHeading) title = htmx_h1_new();
	g_autoptr(HtmxP) subtitle = htmx_p_new();
	g_autoptr(HtmxForm) upload_form = render_upload_form();
	g_autoptr(HtmxHeading) list_title = htmx_h2_new();
	g_autoptr(HtmxTable) file_table = render_file_table();

	htmx_element_set_id(HTMX_ELEMENT(main_content), "main-content");

	htmx_node_set_text_content(HTMX_NODE(title), "File Upload Demo");
	htmx_element_add_class(HTMX_ELEMENT(subtitle), "subtitle");
	htmx_node_set_text_content(HTMX_NODE(subtitle),
		"Upload files using HTMX with drag-and-drop support");

	htmx_node_set_text_content(HTMX_NODE(list_title), "Uploaded Files");

	htmx_node_add_child(HTMX_NODE(main_content), HTMX_NODE(title));
	htmx_node_add_child(HTMX_NODE(main_content), HTMX_NODE(subtitle));
	htmx_node_add_child(HTMX_NODE(main_content), HTMX_NODE(upload_form));
	htmx_node_add_child(HTMX_NODE(main_content), HTMX_NODE(list_title));
	htmx_node_add_child(HTMX_NODE(main_content), HTMX_NODE(file_table));

	return (HtmxMain *)g_steal_pointer(&main_content);
}

/*
 * Render the complete page using HtmxBuilder for document structure
 */
static gchar *
render_page(void)
{
	g_autoptr(HtmxBuilder) builder = htmx_builder_new();
	g_autoptr(HtmxMain) main_content = render_main_content();
	g_autofree gchar *main_html = htmx_element_render(HTMX_ELEMENT(main_content));

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
	htmx_builder_text(builder, "htmx-glib File Upload");
	htmx_builder_end(builder);

	htmx_builder_begin(builder, "script");
	htmx_builder_attr(builder, "src", "https://unpkg.com/htmx.org@1.9.10");
	htmx_builder_end(builder);

	htmx_builder_begin(builder, "style");
	htmx_builder_text(builder,
		"* { box-sizing: border-box; }\n"
		"body { font-family: system-ui, sans-serif; max-width: 900px; margin: 2em auto; padding: 0 1em; background: #f5f5f5; }\n"
		"h1 { color: #333; margin-bottom: 0.5em; }\n"
		".subtitle { color: #666; margin-top: 0; margin-bottom: 2em; }\n"
		".drop-zone { display: block; border: 2px dashed #ccc; border-radius: 8px; padding: 3em 2em; text-align: center; "
		"background: white; cursor: pointer; transition: all 0.3s ease; margin-bottom: 1em; }\n"
		".drop-zone:hover, .drop-zone.dragover { border-color: #007bff; background: #f0f7ff; }\n"
		".drop-zone-content { pointer-events: none; }\n"
		".drop-icon { font-size: 3em; margin: 0; }\n"
		".drop-hint { color: #888; font-size: 0.9em; margin: 0.5em 0 0; }\n"
		".file-input { display: none; }\n"
		".form-actions { margin-bottom: 1em; }\n"
		".upload-btn { background: #007bff; color: white; border: none; padding: 0.75em 2em; "
		"font-size: 1em; border-radius: 4px; cursor: pointer; }\n"
		".upload-btn:hover { background: #0056b3; }\n"
		".upload-btn:disabled { background: #ccc; cursor: not-allowed; }\n"
		".progress-container { background: white; padding: 1em; border-radius: 4px; margin-bottom: 1em; }\n"
		".progress-bar { height: 20px; background: #e9ecef; border-radius: 4px; overflow: hidden; margin-bottom: 0.5em; }\n"
		".progress-fill { height: 100%; background: #007bff; width: 0%; transition: width 0.3s ease; }\n"
		".file-table { width: 100%; border-collapse: collapse; background: white; border-radius: 8px; overflow: hidden; box-shadow: 0 1px 3px rgba(0,0,0,0.1); }\n"
		".file-table th, .file-table td { padding: 0.75em 1em; text-align: left; border-bottom: 1px solid #eee; }\n"
		".file-table th { background: #f8f9fa; font-weight: 600; color: #333; }\n"
		".file-table tbody tr:hover { background: #f8f9fa; }\n"
		".download-link { color: #007bff; text-decoration: none; }\n"
		".download-link:hover { text-decoration: underline; }\n"
		".delete-btn { background: #dc3545; color: white; border: none; padding: 0.25em 0.75em; "
		"border-radius: 3px; cursor: pointer; font-size: 0.9em; }\n"
		".delete-btn:hover { background: #c82333; }\n"
		".empty-message { text-align: center; color: #888; font-style: italic; padding: 2em !important; }\n"
		".error-message { background: #f8d7da; color: #721c24; padding: 1em; border-radius: 4px; margin-bottom: 1em; }\n"
		".htmx-request .upload-btn { opacity: 0.5; }\n"
		".htmx-request #upload-progress { display: block !important; }\n"
	);
	htmx_builder_end(builder);

	htmx_builder_end(builder); /* head */

	/* Body */
	htmx_builder_begin(builder, "body");

	htmx_builder_raw_html(builder, main_html);

	/* JavaScript for drag-and-drop */
	htmx_builder_begin(builder, "script");
	htmx_builder_text(builder,
		"(function() {\n"
		"  const dropZone = document.getElementById('drop-zone');\n"
		"  const fileInput = document.getElementById('file-input');\n"
		"  const dropText = document.getElementById('drop-zone-text');\n"
		"  \n"
		"  dropZone.addEventListener('dragover', (e) => {\n"
		"    e.preventDefault();\n"
		"    dropZone.classList.add('dragover');\n"
		"  });\n"
		"  \n"
		"  dropZone.addEventListener('dragleave', () => {\n"
		"    dropZone.classList.remove('dragover');\n"
		"  });\n"
		"  \n"
		"  dropZone.addEventListener('drop', (e) => {\n"
		"    e.preventDefault();\n"
		"    dropZone.classList.remove('dragover');\n"
		"    fileInput.files = e.dataTransfer.files;\n"
		"    fileInput.dispatchEvent(new Event('change'));\n"
		"  });\n"
		"  \n"
		"  fileInput.addEventListener('change', () => {\n"
		"    const count = fileInput.files.length;\n"
		"    if (count > 0) {\n"
		"      dropText.textContent = count + ' file' + (count > 1 ? 's' : '') + ' selected';\n"
		"    }\n"
		"  });\n"
		"  \n"
		"  document.body.addEventListener('htmx:afterRequest', (e) => {\n"
		"    if (e.detail.target.id === 'file-list-body') {\n"
		"      dropText.textContent = 'Drag and drop files here';\n"
		"    }\n"
		"  });\n"
		"})();\n"
	);
	htmx_builder_end(builder);

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
 * Parse multipart form data from request
 *
 * Returns a list of UploadedFile structures (caller must free)
 */
static GList *
parse_multipart_files(HtmxRequest *request)
{
	GList *files = NULL;
	SoupServerMessage *message;
	SoupMessageHeaders *headers;
	SoupMessageBody *body;
	const gchar *content_type;
	GHashTable *ct_params = NULL;
	const gchar *boundary;
	g_autoptr(SoupMultipart) multipart = NULL;
	gint i;
	gint length;

	message = htmx_request_get_message(request);
	headers = soup_server_message_get_request_headers(message);
	body = soup_server_message_get_request_body(message);

	/* Get content type and check for multipart */
	content_type = soup_message_headers_get_content_type(headers, &ct_params);
	if (content_type == NULL || !g_str_has_prefix(content_type, "multipart/form-data")) {
		if (ct_params != NULL) {
			g_hash_table_unref(ct_params);
		}
		return NULL;
	}

	/* Get boundary from parameters */
	if (ct_params != NULL) {
		boundary = g_hash_table_lookup(ct_params, "boundary");
		g_hash_table_unref(ct_params);
	} else {
		boundary = NULL;
	}

	if (boundary == NULL || body == NULL || body->length == 0) {
		return NULL;
	}

	/* Parse multipart data */
	{
		g_autoptr(GBytes) body_bytes = g_bytes_new(body->data, body->length);
		multipart = soup_multipart_new_from_message(headers, body_bytes);
	}

	if (multipart == NULL) {
		return NULL;
	}

	/* Extract files from multipart */
	length = soup_multipart_get_length(multipart);
	for (i = 0; i < length; i++) {
		SoupMessageHeaders *part_headers = NULL;
		GBytes *part_body = NULL;
		GHashTable *disp_params = NULL;
		const gchar *filename;
		const gchar *part_content_type;
		gchar *disp_value = NULL;

		if (!soup_multipart_get_part(multipart, i, &part_headers, &part_body)) {
			continue;
		}

		/* Check content-disposition for filename */
		if (!soup_message_headers_get_content_disposition(part_headers, &disp_value, &disp_params)) {
			continue;
		}
		g_free(disp_value);

		if (disp_params == NULL) {
			continue;
		}

		filename = g_hash_table_lookup(disp_params, "filename");
		if (filename == NULL || filename[0] == '\0') {
			g_hash_table_unref(disp_params);
			continue;
		}

		/* Check file size */
		if (g_bytes_get_size(part_body) > MAX_FILE_SIZE) {
			g_hash_table_unref(disp_params);
			continue;
		}

		/* Create uploaded file entry */
		{
			UploadedFile *file = g_new0(UploadedFile, 1);

			file->id = next_file_id++;
			file->filename = g_strdup(filename);
			part_content_type = soup_message_headers_get_content_type(part_headers, NULL);
			file->content_type = g_strdup(part_content_type != NULL ? part_content_type : "application/octet-stream");
			file->data = g_bytes_ref(part_body);
			file->size = g_bytes_get_size(part_body);
			file->uploaded_at = g_get_real_time() / G_USEC_PER_SEC;

			files = g_list_append(files, file);
		}

		g_hash_table_unref(disp_params);
	}

	return files;
}

/*
 * Handle POST /upload - Upload files
 */
static HtmxResponse *
handle_upload(HtmxRequest *request, GHashTable *params, gpointer user_data)
{
	g_autoptr(HtmxResponse) response = htmx_response_new();
	g_autoptr(GString) html = g_string_new(NULL);
	GList *new_files;
	GList *l;

	/* Parse multipart data */
	new_files = parse_multipart_files(request);

	if (new_files == NULL) {
		g_autoptr(HtmxTr) error_row = htmx_tr_new();
		g_autoptr(HtmxTd) error_cell = htmx_td_new();
		g_autofree gchar *error_html = NULL;

		htmx_td_set_colspan(error_cell, 5);
		htmx_element_add_class(HTMX_ELEMENT(error_cell), "error-message");
		htmx_node_set_text_content(HTMX_NODE(error_cell),
			"No valid files received. Please select files to upload (max 5MB each).");

		htmx_node_add_child(HTMX_NODE(error_row), HTMX_NODE(error_cell));
		error_html = htmx_element_render(HTMX_ELEMENT(error_row));

		htmx_response_set_status(response, 400);
		htmx_response_set_content(response, error_html);
		htmx_response_set_content_type(response, "text/html; charset=utf-8");
		return (HtmxResponse *)g_steal_pointer(&response);
	}

	/* Add files to global list and render rows */
	for (l = new_files; l != NULL; l = l->next) {
		UploadedFile *file = l->data;
		g_autoptr(HtmxTr) row = render_file_row(file);
		g_autofree gchar *row_html = htmx_element_render(HTMX_ELEMENT(row));

		uploaded_files = g_list_append(uploaded_files, file);
		g_string_append(html, row_html);
	}

	g_list_free(new_files);

	htmx_response_set_content(response, html->str);
	htmx_response_set_content_type(response, "text/html; charset=utf-8");

	return (HtmxResponse *)g_steal_pointer(&response);
}

/*
 * Handle DELETE /files/:id - Delete a file
 */
static HtmxResponse *
handle_delete_file(HtmxRequest *request, GHashTable *params, gpointer user_data)
{
	g_autoptr(HtmxResponse) response = htmx_response_new();
	const gchar *id_str;
	guint id;
	UploadedFile *file;
	GList *node;

	id_str = g_hash_table_lookup(params, "id");
	if (id_str == NULL) {
		htmx_response_set_status(response, 400);
		htmx_response_set_content(response, "Missing file ID");
		return (HtmxResponse *)g_steal_pointer(&response);
	}

	id = (guint)g_ascii_strtoull(id_str, NULL, 10);
	file = find_file(id);

	if (file == NULL) {
		htmx_response_set_status(response, 404);
		htmx_response_set_content(response, "File not found");
		return (HtmxResponse *)g_steal_pointer(&response);
	}

	node = g_list_find(uploaded_files, file);
	uploaded_files = g_list_delete_link(uploaded_files, node);
	free_file(file);

	htmx_response_set_content(response, "");
	return (HtmxResponse *)g_steal_pointer(&response);
}

/*
 * Handle GET /files/:id/download - Download a file
 */
static HtmxResponse *
handle_download_file(HtmxRequest *request, GHashTable *params, gpointer user_data)
{
	g_autoptr(HtmxResponse) response = htmx_response_new();
	const gchar *id_str;
	guint id;
	UploadedFile *file;
	g_autofree gchar *disposition = NULL;
	SoupServerMessage *message;
	SoupMessageBody *body;
	const guint8 *data;
	gsize size;

	id_str = g_hash_table_lookup(params, "id");
	if (id_str == NULL) {
		htmx_response_set_status(response, 400);
		htmx_response_set_content(response, "Missing file ID");
		return (HtmxResponse *)g_steal_pointer(&response);
	}

	id = (guint)g_ascii_strtoull(id_str, NULL, 10);
	file = find_file(id);

	if (file == NULL) {
		htmx_response_set_status(response, 404);
		htmx_response_set_content(response, "File not found");
		return (HtmxResponse *)g_steal_pointer(&response);
	}

	/* Set response headers */
	disposition = g_strdup_printf("attachment; filename=\"%s\"", file->filename);
	htmx_response_add_header(response, "Content-Disposition", disposition);
	htmx_response_set_content_type(response, file->content_type);

	/* Set binary body directly on SoupServerMessage */
	message = htmx_request_get_message(request);
	body = soup_server_message_get_response_body(message);
	data = g_bytes_get_data(file->data, &size);

	soup_message_body_append(body, SOUP_MEMORY_COPY, data, size);

	return (HtmxResponse *)g_steal_pointer(&response);
}

/*
 * Handle GET /files - Get file list (for refresh)
 */
static HtmxResponse *
handle_get_files(HtmxRequest *request, GHashTable *params, gpointer user_data)
{
	g_autoptr(HtmxResponse) response = htmx_response_new();
	g_autoptr(HtmxTable) table = render_file_table();
	g_autofree gchar *html = htmx_element_render(HTMX_ELEMENT(table));

	htmx_response_set_content(response, html);
	htmx_response_set_content_type(response, "text/html; charset=utf-8");

	return (HtmxResponse *)g_steal_pointer(&response);
}

/*
 * Clean up files on exit
 */
static void
cleanup_files(void)
{
	g_list_free_full(uploaded_files, (GDestroyNotify)free_file);
	uploaded_files = NULL;
}

int
main(int argc, char *argv[])
{
	g_autoptr(HtmxServer) server = NULL;
	HtmxRouter *router;
	guint port = 8080;

	if (argc > 1) {
		port = (guint)g_ascii_strtoull(argv[1], NULL, 10);
		if (port == 0) {
			port = 8080;
		}
	}

	server = htmx_server_new_with_port(port);
	if (server == NULL) {
		g_printerr("Failed to create server\n");
		return 1;
	}

	router = htmx_server_get_router(server);

	htmx_router_get(router, "/", handle_index, NULL);
	htmx_router_get(router, "/files", handle_get_files, NULL);
	htmx_router_post(router, "/upload", handle_upload, NULL);
	htmx_router_delete(router, "/files/:id", handle_delete_file, NULL);
	htmx_router_get(router, "/files/:id/download", handle_download_file, NULL);

	g_print("htmx-glib File Upload Demo\n");
	g_print("Server running at http://localhost:%u\n", port);
	g_print("Press Ctrl+C to stop\n\n");

	atexit(cleanup_files);

	htmx_server_run(server);

	return 0;
}
