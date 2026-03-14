/*
 * test-upload.c - Tests for HtmxUploadedFile boxed type and multipart parsing
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include <htmx-glib.h>
#include <string.h>

static void
test_uploaded_file_new(void)
{
	GBytes *data = g_bytes_new_static("hello", 5);
	g_autoptr(HtmxUploadedFile) file = NULL;

	file = htmx_uploaded_file_new("avatar", "photo.jpg", "image/jpeg", data);

	g_assert_nonnull(file);
	g_assert_cmpstr(htmx_uploaded_file_get_field_name(file), ==, "avatar");
	g_assert_cmpstr(htmx_uploaded_file_get_filename(file), ==, "photo.jpg");
	g_assert_cmpstr(htmx_uploaded_file_get_content_type(file), ==, "image/jpeg");
	g_assert_cmpuint(htmx_uploaded_file_get_size(file), ==, 5);

	g_bytes_unref(data);
}

static void
test_uploaded_file_null_optional_fields(void)
{
	GBytes *data = g_bytes_new_static("x", 1);
	g_autoptr(HtmxUploadedFile) file = NULL;

	file = htmx_uploaded_file_new("field", NULL, NULL, data);

	g_assert_nonnull(file);
	g_assert_cmpstr(htmx_uploaded_file_get_field_name(file), ==, "field");
	g_assert_null(htmx_uploaded_file_get_filename(file));
	g_assert_null(htmx_uploaded_file_get_content_type(file));

	g_bytes_unref(data);
}

static void
test_uploaded_file_copy(void)
{
	GBytes *data = g_bytes_new_static("test", 4);
	g_autoptr(HtmxUploadedFile) file = NULL;
	g_autoptr(HtmxUploadedFile) copy = NULL;

	file = htmx_uploaded_file_new("doc", "test.txt", "text/plain", data);
	copy = htmx_uploaded_file_copy(file);

	g_assert_nonnull(copy);
	g_assert_true(file != copy);
	g_assert_cmpstr(htmx_uploaded_file_get_field_name(copy), ==, "doc");
	g_assert_cmpstr(htmx_uploaded_file_get_filename(copy), ==, "test.txt");
	g_assert_cmpstr(htmx_uploaded_file_get_content_type(copy), ==, "text/plain");
	g_assert_cmpuint(htmx_uploaded_file_get_size(copy), ==, 4);

	g_bytes_unref(data);
}

static void
test_uploaded_file_get_type(void)
{
	GType type = htmx_uploaded_file_get_type();
	g_assert_true(g_type_is_a(type, G_TYPE_BOXED));
}

static void
test_uploaded_file_get_data(void)
{
	const gchar *content = "file content here";
	GBytes *data = g_bytes_new_static(content, strlen(content));
	g_autoptr(HtmxUploadedFile) file = NULL;
	GBytes *retrieved;
	gsize size;
	const gchar *ptr;

	file = htmx_uploaded_file_new("upload", "f.bin", "application/octet-stream", data);
	retrieved = htmx_uploaded_file_get_data(file);

	g_assert_nonnull(retrieved);
	ptr = g_bytes_get_data(retrieved, &size);
	g_assert_cmpuint(size, ==, strlen(content));
	g_assert_true(memcmp(ptr, content, size) == 0);

	g_bytes_unref(data);
}

static void
test_uploaded_file_save(void)
{
	const gchar *content = "saved data";
	GBytes *data = g_bytes_new_static(content, strlen(content));
	g_autoptr(HtmxUploadedFile) file = NULL;
	GError *error = NULL;
	gboolean ok;
	gchar *path;
	gchar *read_back = NULL;
	gsize read_len;
	gint fd;

	file = htmx_uploaded_file_new("upload", "test.bin", "application/octet-stream", data);

	fd = g_file_open_tmp("htmx-test-XXXXXX", &path, &error);
	g_assert_no_error(error);
	close(fd);

	ok = htmx_uploaded_file_save(file, path, &error);
	g_assert_no_error(error);
	g_assert_true(ok);

	g_file_get_contents(path, &read_back, &read_len, &error);
	g_assert_no_error(error);
	g_assert_cmpuint(read_len, ==, strlen(content));
	g_assert_true(memcmp(read_back, content, read_len) == 0);

	g_free(read_back);
	g_unlink(path);
	g_free(path);
	g_bytes_unref(data);
}

/* Multipart parsing tests */

static const gchar multipart_simple[] =
	"--boundary123\r\n"
	"Content-Disposition: form-data; name=\"username\"\r\n"
	"\r\n"
	"johndoe\r\n"
	"--boundary123\r\n"
	"Content-Disposition: form-data; name=\"avatar\"; filename=\"photo.jpg\"\r\n"
	"Content-Type: image/jpeg\r\n"
	"\r\n"
	"JFIFDATA\r\n"
	"--boundary123--\r\n";

static void
test_multipart_parse_simple(void)
{
	GError *error = NULL;
	GHashTable *form_data = NULL;
	GPtrArray *files;
	GBytes *body;
	HtmxUploadedFile *file;

	body = g_bytes_new_static(multipart_simple, strlen(multipart_simple));

	files = htmx_uploaded_file_parse_multipart(
		"multipart/form-data; boundary=boundary123",
		body, &form_data, &error);

	g_assert_no_error(error);
	g_assert_nonnull(files);
	g_assert_nonnull(form_data);

	/* One file, one form field */
	g_assert_cmpuint(files->len, ==, 1);
	g_assert_cmpstr(g_hash_table_lookup(form_data, "username"), ==, "johndoe");

	file = g_ptr_array_index(files, 0);
	g_assert_cmpstr(htmx_uploaded_file_get_field_name(file), ==, "avatar");
	g_assert_cmpstr(htmx_uploaded_file_get_filename(file), ==, "photo.jpg");
	g_assert_cmpstr(htmx_uploaded_file_get_content_type(file), ==, "image/jpeg");
	g_assert_cmpuint(htmx_uploaded_file_get_size(file), ==, 8); /* "JFIFDATA" */

	g_ptr_array_unref(files);
	g_hash_table_unref(form_data);
	g_bytes_unref(body);
}

static const gchar multipart_quoted_boundary[] =
	"--abc\r\n"
	"Content-Disposition: form-data; name=\"file\"; filename=\"doc.pdf\"\r\n"
	"Content-Type: application/pdf\r\n"
	"\r\n"
	"PDF\r\n"
	"--abc--\r\n";

static void
test_multipart_parse_quoted_boundary(void)
{
	GError *error = NULL;
	GHashTable *form_data = NULL;
	GPtrArray *files;
	GBytes *body;

	body = g_bytes_new_static(multipart_quoted_boundary,
	                          strlen(multipart_quoted_boundary));

	files = htmx_uploaded_file_parse_multipart(
		"multipart/form-data; boundary=\"abc\"",
		body, &form_data, &error);

	g_assert_no_error(error);
	g_assert_nonnull(files);
	g_assert_cmpuint(files->len, ==, 1);

	g_ptr_array_unref(files);
	if (form_data != NULL)
		g_hash_table_unref(form_data);
	g_bytes_unref(body);
}

static const gchar multipart_multiple_files[] =
	"--DELIM\r\n"
	"Content-Disposition: form-data; name=\"file1\"; filename=\"a.txt\"\r\n"
	"Content-Type: text/plain\r\n"
	"\r\n"
	"alpha\r\n"
	"--DELIM\r\n"
	"Content-Disposition: form-data; name=\"file2\"; filename=\"b.txt\"\r\n"
	"Content-Type: text/plain\r\n"
	"\r\n"
	"bravo\r\n"
	"--DELIM\r\n"
	"Content-Disposition: form-data; name=\"note\"\r\n"
	"\r\n"
	"hello\r\n"
	"--DELIM--\r\n";

static void
test_multipart_parse_multiple_files(void)
{
	GError *error = NULL;
	GHashTable *form_data = NULL;
	GPtrArray *files;
	GBytes *body;

	body = g_bytes_new_static(multipart_multiple_files,
	                          strlen(multipart_multiple_files));

	files = htmx_uploaded_file_parse_multipart(
		"multipart/form-data; boundary=DELIM",
		body, &form_data, &error);

	g_assert_no_error(error);
	g_assert_nonnull(files);
	g_assert_cmpuint(files->len, ==, 2);
	g_assert_cmpstr(g_hash_table_lookup(form_data, "note"), ==, "hello");

	g_ptr_array_unref(files);
	g_hash_table_unref(form_data);
	g_bytes_unref(body);
}

static void
test_multipart_parse_no_boundary(void)
{
	GError *error = NULL;
	GHashTable *form_data = NULL;
	GPtrArray *files;
	GBytes *body;

	body = g_bytes_new_static("x", 1);

	files = htmx_uploaded_file_parse_multipart(
		"multipart/form-data",
		body, &form_data, &error);

	g_assert_null(files);
	g_assert_nonnull(error);
	g_error_free(error);
	g_bytes_unref(body);
}

static const gchar multipart_only_fields[] =
	"--b\r\n"
	"Content-Disposition: form-data; name=\"field1\"\r\n"
	"\r\n"
	"value1\r\n"
	"--b\r\n"
	"Content-Disposition: form-data; name=\"field2\"\r\n"
	"\r\n"
	"value2\r\n"
	"--b--\r\n";

static void
test_multipart_parse_only_fields(void)
{
	GError *error = NULL;
	GHashTable *form_data = NULL;
	GPtrArray *files;
	GBytes *body;

	body = g_bytes_new_static(multipart_only_fields,
	                          strlen(multipart_only_fields));

	files = htmx_uploaded_file_parse_multipart(
		"multipart/form-data; boundary=b",
		body, &form_data, &error);

	g_assert_no_error(error);
	g_assert_nonnull(files);
	g_assert_cmpuint(files->len, ==, 0);
	g_assert_nonnull(form_data);
	g_assert_cmpstr(g_hash_table_lookup(form_data, "field1"), ==, "value1");
	g_assert_cmpstr(g_hash_table_lookup(form_data, "field2"), ==, "value2");

	g_ptr_array_unref(files);
	g_hash_table_unref(form_data);
	g_bytes_unref(body);
}

int
main(int argc, char *argv[])
{
	g_test_init(&argc, &argv, NULL);

	g_test_add_func("/upload/new", test_uploaded_file_new);
	g_test_add_func("/upload/null_optional_fields", test_uploaded_file_null_optional_fields);
	g_test_add_func("/upload/copy", test_uploaded_file_copy);
	g_test_add_func("/upload/get_type", test_uploaded_file_get_type);
	g_test_add_func("/upload/get_data", test_uploaded_file_get_data);
	g_test_add_func("/upload/save", test_uploaded_file_save);
	g_test_add_func("/upload/multipart/simple", test_multipart_parse_simple);
	g_test_add_func("/upload/multipart/quoted_boundary", test_multipart_parse_quoted_boundary);
	g_test_add_func("/upload/multipart/multiple_files", test_multipart_parse_multiple_files);
	g_test_add_func("/upload/multipart/no_boundary", test_multipart_parse_no_boundary);
	g_test_add_func("/upload/multipart/only_fields", test_multipart_parse_only_fields);

	return g_test_run();
}
