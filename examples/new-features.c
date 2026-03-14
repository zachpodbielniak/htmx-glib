/*
 * new-features.c - Examples for new htmx-glib features
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * Demonstrates the six new features:
 *
 *   1. HtmxSync          — hx-sync attribute model
 *   2. HtmxCallbackHandler — HtmxHandler from a function pointer
 *   3. HtmxUploadedFile  — multipart/form-data parsing
 *   4. HtmxCompression   — response compression middleware
 *   5. HtmxValidationGroup — cross-field form validation
 *   6. HtmxMetrics       — request metrics with GObject signals
 */

#include <htmx-glib.h>
#include <stdio.h>
#include <string.h>

/* -------------------------------------------------------------------------
 * 1. HtmxSync — hx-sync attribute model
 *
 * hx-sync controls what happens when multiple requests race. The value is
 * "selector:strategy", e.g. "closest form:abort". HtmxSync parses and
 * renders this attribute value.
 * ----------------------------------------------------------------------- */

static void
demo_sync(void)
{
	GError *error = NULL;

	puts("=== 1. HtmxSync ===");

	/* Create directly */
	{
		g_autoptr(HtmxSync) sync = htmx_sync_new("closest form", HTMX_SYNC_ABORT);
		g_autofree gchar *rendered = htmx_sync_render(sync);

		printf("  created:  hx-sync=\"%s\"\n", rendered);
		printf("  selector: %s\n", htmx_sync_get_selector(sync));
		printf("  strategy: %d (ABORT)\n", htmx_sync_get_strategy(sync));
	}

	/* Parse from attribute string */
	{
		g_autoptr(HtmxSync) sync = htmx_sync_parse("this:queue last", &error);
		g_autofree gchar *rendered = htmx_sync_render(sync);

		g_assert_no_error(error);
		printf("  parsed:   hx-sync=\"%s\" -> strategy=%d (QUEUE_LAST)\n",
		       rendered, htmx_sync_get_strategy(sync));
	}

	/* No selector — just a strategy */
	{
		g_autoptr(HtmxSync) sync = htmx_sync_parse("replace", &error);
		g_autofree gchar *rendered = htmx_sync_render(sync);

		g_assert_no_error(error);
		printf("  no-selector: \"%s\" -> selector=%s\n",
		       rendered,
		       htmx_sync_get_selector(sync) != NULL
		           ? htmx_sync_get_selector(sync) : "(none)");
	}

	puts("");
}

/* -------------------------------------------------------------------------
 * 2. HtmxCallbackHandler — wrap a function pointer as HtmxHandler
 *
 * Bridges the route-callback API with the GObject HtmxHandler interface,
 * so callbacks can be passed to any API that accepts an HtmxHandler.
 * ----------------------------------------------------------------------- */

static HtmxResponse *
greet_callback(HtmxRequest *request,
               GHashTable  *params,
               gpointer     user_data)
{
	const gchar *name;
	gchar *body;
	HtmxResponse *response;

	(void)request;

	name = g_hash_table_lookup(params, "name");
	body = g_strdup_printf("<h1>Hello, %s!</h1>", name != NULL ? name : "World");
	response = htmx_response_new_with_content(body);
	g_free(body);

	return response;
}

static void
demo_callback_handler(void)
{
	puts("=== 2. HtmxCallbackHandler ===");

	{
		g_autoptr(HtmxCallbackHandler) handler =
			htmx_callback_handler_new(greet_callback, NULL, NULL);
		g_autoptr(HtmxRequest) request = g_object_new(HTMX_TYPE_REQUEST, NULL);
		g_autoptr(HtmxResponse) response = NULL;
		GHashTable *params;

		/* Implements HtmxHandler interface */
		printf("  is HtmxHandler: %s\n",
		       HTMX_IS_HANDLER(handler) ? "yes" : "no");

		params = g_hash_table_new(g_str_hash, g_str_equal);
		g_hash_table_insert(params, (gpointer)"name", (gpointer)"htmx-glib");

		response = htmx_handler_handle(HTMX_HANDLER(handler), request, params);
		printf("  response: %s\n", htmx_response_get_content(response));

		g_hash_table_unref(params);
	}

	puts("");
}

/* -------------------------------------------------------------------------
 * 3. HtmxUploadedFile — multipart/form-data parsing
 *
 * Parse a multipart body into HtmxUploadedFile boxed types (for file parts)
 * and a GHashTable (for plain text fields). Files can be saved to disk.
 * ----------------------------------------------------------------------- */

static void
demo_uploaded_file(void)
{
	/* Minimal multipart body */
	static const gchar multipart_body[] =
		"--MYBOUNDARY\r\n"
		"Content-Disposition: form-data; name=\"username\"\r\n"
		"\r\n"
		"alice\r\n"
		"--MYBOUNDARY\r\n"
		"Content-Disposition: form-data; name=\"avatar\"; filename=\"face.png\"\r\n"
		"Content-Type: image/png\r\n"
		"\r\n"
		"\x89PNG\r\n"   /* fake PNG magic bytes */
		"--MYBOUNDARY--\r\n";

	GError *error = NULL;
	GHashTable *form_data = NULL;
	GBytes *body;
	GPtrArray *files;
	HtmxUploadedFile *file;

	puts("=== 3. HtmxUploadedFile ===");

	body = g_bytes_new_static(multipart_body, strlen(multipart_body));

	files = htmx_uploaded_file_parse_multipart(
		"multipart/form-data; boundary=MYBOUNDARY",
		body,
		&form_data,
		&error);

	if (error != NULL) {
		fprintf(stderr, "  parse error: %s\n", error->message);
		g_error_free(error);
		g_bytes_unref(body);
		return;
	}

	/* Plain text fields */
	printf("  form field username=%s\n",
	       (const gchar *)g_hash_table_lookup(form_data, "username"));

	/* File parts */
	printf("  %u file(s) uploaded\n", files->len);

	file = g_ptr_array_index(files, 0);
	printf("  file[0]: field=%s  filename=%s  type=%s  size=%zu bytes\n",
	       htmx_uploaded_file_get_field_name(file),
	       htmx_uploaded_file_get_filename(file),
	       htmx_uploaded_file_get_content_type(file),
	       htmx_uploaded_file_get_size(file));

	/* Direct construction */
	{
		GBytes *data = g_bytes_new_static("hello world", 11);
		g_autoptr(HtmxUploadedFile) direct =
			htmx_uploaded_file_new("doc", "readme.txt", "text/plain", data);

		printf("  direct: size=%zu\n", htmx_uploaded_file_get_size(direct));
		g_bytes_unref(data);
	}

	g_ptr_array_unref(files);
	g_hash_table_unref(form_data);
	g_bytes_unref(body);

	puts("");
}

/* -------------------------------------------------------------------------
 * 4. HtmxCompression — response compression middleware
 *
 * Add to your server pipeline with htmx_server_use(). Compresses responses
 * larger than min_size using gzip or deflate based on Accept-Encoding.
 * ----------------------------------------------------------------------- */

static void
demo_compression(void)
{
	puts("=== 4. HtmxCompression ===");

	{
		g_autoptr(HtmxCompression) comp = htmx_compression_new();

		printf("  default min_size: %zu bytes\n",
		       htmx_compression_get_min_size(comp));

		/* Raise threshold — only compress responses > 4 KB */
		htmx_compression_set_min_size(comp, 4096);
		printf("  updated min_size: %zu bytes\n",
		       htmx_compression_get_min_size(comp));

		/* Register with a server (pseudocode — no live server here):
		 *
		 *   HtmxServer *server = htmx_server_new(config);
		 *   htmx_server_use(server, htmx_compression_middleware, comp);
		 *
		 * The middleware transparently selects gzip or deflate based
		 * on Accept-Encoding and skips small responses.
		 */
		puts("  (register: htmx_server_use(server, htmx_compression_middleware, comp))");
	}

	puts("");
}

/* -------------------------------------------------------------------------
 * 5. HtmxValidationGroup — cross-field form validation
 *
 * Combines per-field HtmxValidator instances with cross-field rules that
 * see the entire form. validate() returns a hash of field → error message.
 * ----------------------------------------------------------------------- */

/* Cross-field rule: password must match confirm_password */
static gboolean
passwords_match(GHashTable *form_data,
                gpointer    user_data,
                gchar     **error)
{
	const gchar *pw      = g_hash_table_lookup(form_data, "password");
	const gchar *confirm = g_hash_table_lookup(form_data, "confirm");

	(void)user_data;

	if (g_strcmp0(pw, confirm) != 0) {
		if (error)
			*error = g_strdup("Passwords do not match");
		return FALSE;
	}
	return TRUE;
}

static void
demo_validation_group(void)
{
	puts("=== 5. HtmxValidationGroup ===");

	{
		g_autoptr(HtmxValidationGroup) group = htmx_validation_group_new();

		/* Per-field validators */
		g_autoptr(HtmxValidator) req_username =
			htmx_validator_required("Username is required");
		g_autoptr(HtmxValidator) min_username =
			htmx_validator_min_length(3, "Username must be at least 3 characters");
		g_autoptr(HtmxValidator) req_email =
			htmx_validator_required("Email is required");
		g_autoptr(HtmxValidator) email_fmt =
			htmx_validator_email("Invalid email address");
		g_autoptr(HtmxValidator) req_pw =
			htmx_validator_required("Password is required");

		htmx_validation_group_add_validator(group, "username", req_username);
		htmx_validation_group_add_validator(group, "username", min_username);
		htmx_validation_group_add_validator(group, "email",    req_email);
		htmx_validation_group_add_validator(group, "email",    email_fmt);
		htmx_validation_group_add_validator(group, "password", req_pw);

		/* Cross-field rule */
		htmx_validation_group_add_cross_field_rule(
			group, "passwords_match", passwords_match, NULL, NULL);

		/* Test: valid form */
		{
			GHashTable *form = g_hash_table_new(g_str_hash, g_str_equal);
			g_hash_table_insert(form, (gpointer)"username", (gpointer)"alice");
			g_hash_table_insert(form, (gpointer)"email",    (gpointer)"alice@example.com");
			g_hash_table_insert(form, (gpointer)"password", (gpointer)"secret");
			g_hash_table_insert(form, (gpointer)"confirm",  (gpointer)"secret");

			printf("  valid form: %s\n",
			       htmx_validation_group_is_valid(group, form) ? "PASS" : "FAIL");
			g_hash_table_unref(form);
		}

		/* Test: invalid form — short username, bad email, mismatched passwords */
		{
			GHashTable *form = g_hash_table_new(g_str_hash, g_str_equal);
			GHashTable *errors;
			GHashTableIter iter;
			gpointer key, val;

			g_hash_table_insert(form, (gpointer)"username", (gpointer)"ab");
			g_hash_table_insert(form, (gpointer)"email",    (gpointer)"not-an-email");
			g_hash_table_insert(form, (gpointer)"password", (gpointer)"secret");
			g_hash_table_insert(form, (gpointer)"confirm",  (gpointer)"different");

			errors = htmx_validation_group_validate(group, form);
			printf("  invalid form: %u error(s)\n", g_hash_table_size(errors));

			g_hash_table_iter_init(&iter, errors);
			while (g_hash_table_iter_next(&iter, &key, &val)) {
				printf("    [%s]: %s\n", (const gchar *)key, (const gchar *)val);
			}

			g_hash_table_unref(errors);
			g_hash_table_unref(form);
		}
	}

	puts("");
}

/* -------------------------------------------------------------------------
 * 6. HtmxMetrics — request metrics with GObject signals
 *
 * Records total requests, per-path counts, per-status counts, and average
 * latency. Emits "request-completed" after each request passes through
 * the middleware.
 * ----------------------------------------------------------------------- */

static void
on_request_completed(HtmxMetrics *metrics,
                     const gchar *path,
                     const gchar *method,
                     guint        status_code,
                     guint64      latency_us,
                     gpointer     user_data)
{
	(void)metrics;
	(void)user_data;

	printf("  [signal] %s %s -> %u  (%.2f ms)\n",
	       method, path, status_code, latency_us / 1000.0);
}

static void
demo_metrics(void)
{
	puts("=== 6. HtmxMetrics ===");

	{
		g_autoptr(HtmxMetrics) metrics = htmx_metrics_new();

		g_signal_connect(metrics, "request-completed",
		                 G_CALLBACK(on_request_completed), NULL);

		printf("  initial total_requests: %llu\n",
		       (unsigned long long)htmx_metrics_get_total_requests(metrics));

		/* The signal is emitted by htmx_metrics_middleware after each
		 * request passes through. Manually emit for demonstration: */
		g_signal_emit_by_name(metrics, "request-completed",
		                      "/api/todos", "GET", 200u, (guint64)3500);
		g_signal_emit_by_name(metrics, "request-completed",
		                      "/api/todos", "POST", 201u, (guint64)8200);
		g_signal_emit_by_name(metrics, "request-completed",
		                      "/api/todos/1", "DELETE", 204u, (guint64)1100);

		/* (In a real server these would be recorded by the middleware) */
		printf("  register: htmx_server_use(server, htmx_metrics_middleware, metrics)\n");
		printf("  total_requests (via middleware): %llu\n",
		       (unsigned long long)htmx_metrics_get_total_requests(metrics));

		htmx_metrics_reset(metrics);
		printf("  after reset: %llu\n",
		       (unsigned long long)htmx_metrics_get_total_requests(metrics));
	}

	puts("");
}

/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */

int
main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	puts("htmx-glib new features demo\n");

	demo_sync();
	demo_callback_handler();
	demo_uploaded_file();
	demo_compression();
	demo_validation_group();
	demo_metrics();

	puts("Done.");
	return 0;
}
