/*
 * htmx-uploaded-file.c - Multipart file upload model implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-uploaded-file.h"
#include "htmx-error.h"
#include <string.h>

struct _HtmxUploadedFile {
	gchar  *field_name;
	gchar  *filename;
	gchar  *content_type;
	GBytes *data;
};

G_DEFINE_BOXED_TYPE(HtmxUploadedFile, htmx_uploaded_file,
                    htmx_uploaded_file_copy, htmx_uploaded_file_free)

HtmxUploadedFile *
htmx_uploaded_file_new(const gchar *field_name,
                        const gchar *filename,
                        const gchar *content_type,
                        GBytes      *data)
{
	HtmxUploadedFile *self;

	g_return_val_if_fail(field_name != NULL, NULL);
	g_return_val_if_fail(data != NULL, NULL);

	self = g_slice_new0(HtmxUploadedFile);
	self->field_name = g_strdup(field_name);
	self->filename = g_strdup(filename);
	self->content_type = g_strdup(content_type);
	self->data = g_bytes_ref(data);

	return self;
}

HtmxUploadedFile *
htmx_uploaded_file_copy(const HtmxUploadedFile *self)
{
	g_return_val_if_fail(self != NULL, NULL);

	return htmx_uploaded_file_new(self->field_name, self->filename,
	                              self->content_type, self->data);
}

void
htmx_uploaded_file_free(HtmxUploadedFile *self)
{
	if (self == NULL)
		return;

	g_free(self->field_name);
	g_free(self->filename);
	g_free(self->content_type);
	g_bytes_unref(self->data);
	g_slice_free(HtmxUploadedFile, self);
}

const gchar *
htmx_uploaded_file_get_field_name(const HtmxUploadedFile *self)
{
	g_return_val_if_fail(self != NULL, NULL);
	return self->field_name;
}

const gchar *
htmx_uploaded_file_get_filename(const HtmxUploadedFile *self)
{
	g_return_val_if_fail(self != NULL, NULL);
	return self->filename;
}

const gchar *
htmx_uploaded_file_get_content_type(const HtmxUploadedFile *self)
{
	g_return_val_if_fail(self != NULL, NULL);
	return self->content_type;
}

GBytes *
htmx_uploaded_file_get_data(const HtmxUploadedFile *self)
{
	g_return_val_if_fail(self != NULL, NULL);
	return self->data;
}

gsize
htmx_uploaded_file_get_size(const HtmxUploadedFile *self)
{
	g_return_val_if_fail(self != NULL, 0);
	return g_bytes_get_size(self->data);
}

gboolean
htmx_uploaded_file_save(const HtmxUploadedFile *self,
                         const gchar            *path,
                         GError                **error)
{
	const guint8 *data;
	gsize size;

	g_return_val_if_fail(self != NULL, FALSE);
	g_return_val_if_fail(path != NULL, FALSE);

	data = g_bytes_get_data(self->data, &size);

	return g_file_set_contents(path, (const gchar *)data, (gssize)size, error);
}

/*
 * Extract the boundary string from a Content-Type header.
 * e.g. "multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW"
 */
static gchar *
extract_boundary(const gchar *content_type)
{
	const gchar *pos;
	const gchar *start;
	const gchar *end;

	if (content_type == NULL)
		return NULL;

	pos = strstr(content_type, "boundary=");
	if (pos == NULL)
		return NULL;

	start = pos + 9; /* strlen("boundary=") */

	/* Handle quoted boundary */
	if (*start == '"') {
		start++;
		end = strchr(start, '"');
		if (end == NULL)
			return NULL;
		return g_strndup(start, (gsize)(end - start));
	}

	/* Unquoted — ends at whitespace or semicolon */
	end = start;
	while (*end != '\0' && *end != ';' && *end != ' ' && *end != '\t'
	       && *end != '\r' && *end != '\n')
		end++;

	return g_strndup(start, (gsize)(end - start));
}

/*
 * Find the header value in a MIME part header block.
 * Returns newly-allocated string or NULL.
 */
static gchar *
find_header_value(const gchar *headers, const gchar *header_name)
{
	const gchar *pos;
	const gchar *start;
	const gchar *end;
	gchar *search;

	search = g_strdup_printf("%s:", header_name);
	pos = g_strstr_len(headers, -1, search);
	if (pos == NULL) {
		/* Case-insensitive retry */
		gchar *lower_headers = g_ascii_strdown(headers, -1);
		gchar *lower_search = g_ascii_strdown(search, -1);
		const gchar *lower_pos = g_strstr_len(lower_headers, -1, lower_search);
		if (lower_pos != NULL)
			pos = headers + (lower_pos - lower_headers);
		g_free(lower_headers);
		g_free(lower_search);
	}
	g_free(search);

	if (pos == NULL)
		return NULL;

	start = pos + strlen(header_name) + 1;
	while (*start == ' ' || *start == '\t')
		start++;

	end = start;
	while (*end != '\0' && *end != '\r' && *end != '\n')
		end++;

	return g_strndup(start, (gsize)(end - start));
}

/*
 * Extract a parameter value from a header value.
 * e.g. extract "name" from 'form-data; name="field1"; filename="file.txt"'
 */
static gchar *
extract_param(const gchar *header_value, const gchar *param_name)
{
	gchar *search;
	const gchar *pos;
	const gchar *start;
	const gchar *end;

	search = g_strdup_printf("%s=\"", param_name);
	pos = strstr(header_value, search);
	g_free(search);

	if (pos == NULL) {
		/* Try without quotes */
		search = g_strdup_printf("%s=", param_name);
		pos = strstr(header_value, search);
		g_free(search);

		if (pos == NULL)
			return NULL;

		start = pos + strlen(param_name) + 1;
		end = start;
		while (*end != '\0' && *end != ';' && *end != ' ')
			end++;
		return g_strndup(start, (gsize)(end - start));
	}

	start = pos + strlen(param_name) + 2; /* skip name=" */
	end = strchr(start, '"');
	if (end == NULL)
		return NULL;

	return g_strndup(start, (gsize)(end - start));
}

GPtrArray *
htmx_uploaded_file_parse_multipart(const gchar  *content_type,
                                    GBytes       *body,
                                    GHashTable  **form_data,
                                    GError      **error)
{
	GPtrArray *files;
	GHashTable *fields;
	gchar *boundary;
	gchar *delimiter;
	gchar *end_delimiter;
	const gchar *body_str;
	gsize body_len;
	const gchar *pos;
	gsize delim_len;

	g_return_val_if_fail(content_type != NULL, NULL);
	g_return_val_if_fail(body != NULL, NULL);

	boundary = extract_boundary(content_type);
	if (boundary == NULL) {
		g_set_error(error, HTMX_ERROR, HTMX_ERROR_INVALID_REQUEST,
		            "Missing boundary in Content-Type");
		return NULL;
	}

	delimiter = g_strdup_printf("--%s", boundary);
	end_delimiter = g_strdup_printf("--%s--", boundary);
	delim_len = strlen(delimiter);

	body_str = g_bytes_get_data(body, &body_len);

	files = g_ptr_array_new_with_free_func((GDestroyNotify)htmx_uploaded_file_free);
	fields = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);

	/* Find first delimiter */
	pos = g_strstr_len(body_str, (gssize)body_len, delimiter);
	if (pos == NULL) {
		g_set_error(error, HTMX_ERROR, HTMX_ERROR_INVALID_REQUEST,
		            "No multipart boundary found in body");
		g_ptr_array_unref(files);
		g_hash_table_unref(fields);
		g_free(boundary);
		g_free(delimiter);
		g_free(end_delimiter);
		return NULL;
	}

	pos += delim_len;

	/* Process each part */
	while (pos < body_str + body_len) {
		const gchar *next_delim;
		const gchar *part_start;
		const gchar *header_end;
		gchar *content_disp;
		gchar *name;
		gchar *filename;
		gchar *part_content_type;

		/* Skip CRLF after delimiter */
		if (pos[0] == '\r' && pos[1] == '\n')
			pos += 2;
		else if (pos[0] == '-' && pos[1] == '-')
			break; /* End delimiter */

		part_start = pos;

		/* Find next delimiter */
		next_delim = g_strstr_len(pos, (gssize)(body_str + body_len - pos), delimiter);
		if (next_delim == NULL)
			break;

		/* Find header/body separator (empty line) */
		header_end = strstr(part_start, "\r\n\r\n");
		if (header_end == NULL || header_end >= next_delim) {
			pos = next_delim + delim_len;
			continue;
		}

		{
			gchar *headers = g_strndup(part_start, (gsize)(header_end - part_start));
			const gchar *data_start = header_end + 4;
			const gchar *data_end = next_delim;

			/* Trim trailing CRLF before delimiter */
			if (data_end >= data_start + 2 &&
			    data_end[-2] == '\r' && data_end[-1] == '\n')
				data_end -= 2;

			content_disp = find_header_value(headers, "Content-Disposition");
			name = content_disp != NULL ? extract_param(content_disp, "name") : NULL;
			filename = content_disp != NULL ? extract_param(content_disp, "filename") : NULL;
			part_content_type = find_header_value(headers, "Content-Type");

			if (name != NULL) {
				if (filename != NULL) {
					/* File upload */
					GBytes *file_data = g_bytes_new(data_start, (gsize)(data_end - data_start));
					HtmxUploadedFile *file;

					file = htmx_uploaded_file_new(
						name, filename,
						part_content_type != NULL ? part_content_type : "application/octet-stream",
						file_data);
					g_ptr_array_add(files, file);
					g_bytes_unref(file_data);
				} else {
					/* Regular form field */
					gchar *value = g_strndup(data_start, (gsize)(data_end - data_start));
					g_hash_table_insert(fields, g_strdup(name), value);
				}
			}

			g_free(headers);
			g_free(content_disp);
			g_free(name);
			g_free(filename);
			g_free(part_content_type);
		}

		pos = next_delim + delim_len;
	}

	g_free(boundary);
	g_free(delimiter);
	g_free(end_delimiter);

	if (form_data != NULL)
		*form_data = fields;
	else
		g_hash_table_unref(fields);

	return files;
}
