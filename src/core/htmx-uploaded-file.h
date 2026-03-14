/*
 * htmx-uploaded-file.h - Multipart file upload model
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * HtmxUploadedFile is a boxed type representing a file uploaded
 * via multipart/form-data. Provides accessors for the field name,
 * original filename, content type, and binary data.
 */

#ifndef HTMX_UPLOADED_FILE_H
#define HTMX_UPLOADED_FILE_H

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define HTMX_TYPE_UPLOADED_FILE (htmx_uploaded_file_get_type())

typedef struct _HtmxUploadedFile HtmxUploadedFile;

GType htmx_uploaded_file_get_type(void) G_GNUC_CONST;

/**
 * htmx_uploaded_file_new:
 * @field_name: the form field name
 * @filename: (nullable): the original filename
 * @content_type: (nullable): the MIME content type
 * @data: the file data
 *
 * Creates a new #HtmxUploadedFile.
 *
 * Returns: (transfer full): a new #HtmxUploadedFile
 */
HtmxUploadedFile *
htmx_uploaded_file_new(const gchar *field_name,
                        const gchar *filename,
                        const gchar *content_type,
                        GBytes      *data);

/**
 * htmx_uploaded_file_copy:
 * @self: an #HtmxUploadedFile
 *
 * Creates a deep copy.
 *
 * Returns: (transfer full): a copy of @self
 */
HtmxUploadedFile *
htmx_uploaded_file_copy(const HtmxUploadedFile *self);

/**
 * htmx_uploaded_file_free:
 * @self: an #HtmxUploadedFile
 *
 * Frees the uploaded file.
 */
void
htmx_uploaded_file_free(HtmxUploadedFile *self);

G_DEFINE_AUTOPTR_CLEANUP_FUNC(HtmxUploadedFile, htmx_uploaded_file_free)

const gchar *
htmx_uploaded_file_get_field_name(const HtmxUploadedFile *self);

const gchar *
htmx_uploaded_file_get_filename(const HtmxUploadedFile *self);

const gchar *
htmx_uploaded_file_get_content_type(const HtmxUploadedFile *self);

/**
 * htmx_uploaded_file_get_data:
 * @self: an #HtmxUploadedFile
 *
 * Gets the file data.
 *
 * Returns: (transfer none): the file data as #GBytes
 */
GBytes *
htmx_uploaded_file_get_data(const HtmxUploadedFile *self);

/**
 * htmx_uploaded_file_get_size:
 * @self: an #HtmxUploadedFile
 *
 * Gets the file size in bytes.
 *
 * Returns: the file size
 */
gsize
htmx_uploaded_file_get_size(const HtmxUploadedFile *self);

/**
 * htmx_uploaded_file_save:
 * @self: an #HtmxUploadedFile
 * @path: the filesystem path to save to
 * @error: (nullable): return location for an error
 *
 * Saves the uploaded file to disk.
 *
 * Returns: %TRUE on success
 */
gboolean
htmx_uploaded_file_save(const HtmxUploadedFile *self,
                         const gchar            *path,
                         GError                **error);

/**
 * htmx_uploaded_file_parse_multipart:
 * @content_type: the Content-Type header (must contain boundary)
 * @body: the raw request body
 * @form_data: (out) (element-type utf8 utf8) (nullable): non-file fields
 * @error: (nullable): return location for an error
 *
 * Parses a multipart/form-data body into uploaded files and form fields.
 *
 * Returns: (transfer full) (element-type HtmxUploadedFile): array of files
 */
GPtrArray *
htmx_uploaded_file_parse_multipart(const gchar  *content_type,
                                    GBytes       *body,
                                    GHashTable  **form_data,
                                    GError      **error);

G_END_DECLS

#endif /* HTMX_UPLOADED_FILE_H */
