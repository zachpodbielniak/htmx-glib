/*
 * test-metrics.c - Tests for HtmxMetrics
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include <htmx-glib.h>

static void
test_metrics_new(void)
{
	g_autoptr(HtmxMetrics) metrics = htmx_metrics_new();

	g_assert_nonnull(metrics);
	g_assert_true(HTMX_IS_METRICS(metrics));
}

static void
test_metrics_initial_state(void)
{
	g_autoptr(HtmxMetrics) metrics = htmx_metrics_new();

	g_assert_cmpuint(htmx_metrics_get_total_requests(metrics), ==, 0);
	g_assert_cmpuint(htmx_metrics_get_average_latency_us(metrics), ==, 0);
	g_assert_cmpuint(htmx_metrics_get_status_count(metrics, 200), ==, 0);
	g_assert_cmpuint(htmx_metrics_get_status_count(metrics, 404), ==, 0);
	g_assert_cmpuint(htmx_metrics_get_path_count(metrics, "/"), ==, 0);
}

static void
test_metrics_reset(void)
{
	g_autoptr(HtmxMetrics) metrics = htmx_metrics_new();

	/* Reset on empty metrics should not crash */
	htmx_metrics_reset(metrics);

	g_assert_cmpuint(htmx_metrics_get_total_requests(metrics), ==, 0);
	g_assert_cmpuint(htmx_metrics_get_average_latency_us(metrics), ==, 0);
}

static void
test_metrics_get_type(void)
{
	GType type = htmx_metrics_get_type();
	g_assert_true(g_type_is_a(type, G_TYPE_OBJECT));
}

/* Signal tests */

typedef struct {
	gchar   *path;
	gchar   *method;
	guint    status_code;
	guint64  latency_us;
	gint     call_count;
} SignalData;

static void
on_request_completed(HtmxMetrics *metrics,
                     const gchar *path,
                     const gchar *method,
                     guint        status_code,
                     guint64      latency_us,
                     gpointer     user_data)
{
	SignalData *data = (SignalData *)user_data;

	(void)metrics;

	g_free(data->path);
	g_free(data->method);
	data->path = g_strdup(path);
	data->method = g_strdup(method);
	data->status_code = status_code;
	data->latency_us = latency_us;
	data->call_count++;
}

static void
test_metrics_signal_exists(void)
{
	guint signal_id;

	signal_id = g_signal_lookup("request-completed", HTMX_TYPE_METRICS);
	g_assert_cmpuint(signal_id, !=, 0);
}

static void
test_metrics_signal_connect(void)
{
	g_autoptr(HtmxMetrics) metrics = htmx_metrics_new();
	SignalData data = { NULL, NULL, 0, 0, 0 };
	gulong handler_id;

	handler_id = g_signal_connect(metrics, "request-completed",
	                              G_CALLBACK(on_request_completed), &data);

	g_assert_cmpuint(handler_id, >, 0);
	g_assert_cmpint(data.call_count, ==, 0);

	g_free(data.path);
	g_free(data.method);
}

int
main(int argc, char *argv[])
{
	g_test_init(&argc, &argv, NULL);

	g_test_add_func("/metrics/new", test_metrics_new);
	g_test_add_func("/metrics/initial_state", test_metrics_initial_state);
	g_test_add_func("/metrics/reset", test_metrics_reset);
	g_test_add_func("/metrics/get_type", test_metrics_get_type);
	g_test_add_func("/metrics/signal_exists", test_metrics_signal_exists);
	g_test_add_func("/metrics/signal_connect", test_metrics_signal_connect);

	return g_test_run();
}
