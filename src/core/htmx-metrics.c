/*
 * htmx-metrics.c - Request metrics and observability implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-metrics.h"
#include "htmx-request.h"
#include "htmx-response.h"

enum {
	SIGNAL_REQUEST_COMPLETED,
	N_SIGNALS
};

static guint signals[N_SIGNALS] = { 0 };

struct _HtmxMetrics {
	GObject     parent_instance;
	guint64     total_requests;
	guint64     total_latency_us;
	GHashTable *status_counts;  /* GUINT_TO_POINTER(status) -> GUINT_TO_POINTER(count) */
	GHashTable *path_counts;    /* gchar* -> GUINT_TO_POINTER(count) */
	GMutex      lock;
};

G_DEFINE_FINAL_TYPE(HtmxMetrics, htmx_metrics, G_TYPE_OBJECT)

static void
htmx_metrics_finalize(GObject *object)
{
	HtmxMetrics *self = HTMX_METRICS(object);

	g_hash_table_unref(self->status_counts);
	g_hash_table_unref(self->path_counts);
	g_mutex_clear(&self->lock);

	G_OBJECT_CLASS(htmx_metrics_parent_class)->finalize(object);
}

static void
htmx_metrics_class_init(HtmxMetricsClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);
	object_class->finalize = htmx_metrics_finalize;

	/**
	 * HtmxMetrics::request-completed:
	 * @metrics: the metrics instance
	 * @path: the request path
	 * @method: the HTTP method string
	 * @status_code: the response status code
	 * @latency_us: the request latency in microseconds
	 *
	 * Emitted after each request completes.
	 */
	signals[SIGNAL_REQUEST_COMPLETED] = g_signal_new(
		"request-completed",
		G_TYPE_FROM_CLASS(klass),
		G_SIGNAL_RUN_LAST,
		0,
		NULL, NULL,
		NULL,
		G_TYPE_NONE,
		4,
		G_TYPE_STRING,   /* path */
		G_TYPE_STRING,   /* method */
		G_TYPE_UINT,     /* status_code */
		G_TYPE_UINT64    /* latency_us */
	);
}

static void
htmx_metrics_init(HtmxMetrics *self)
{
	self->total_requests = 0;
	self->total_latency_us = 0;
	self->status_counts = g_hash_table_new(g_direct_hash, g_direct_equal);
	self->path_counts = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
	g_mutex_init(&self->lock);
}

HtmxMetrics *
htmx_metrics_new(void)
{
	return g_object_new(HTMX_TYPE_METRICS, NULL);
}

guint64
htmx_metrics_get_total_requests(HtmxMetrics *self)
{
	guint64 val;

	g_return_val_if_fail(HTMX_IS_METRICS(self), 0);

	g_mutex_lock(&self->lock);
	val = self->total_requests;
	g_mutex_unlock(&self->lock);

	return val;
}

guint64
htmx_metrics_get_average_latency_us(HtmxMetrics *self)
{
	guint64 val;

	g_return_val_if_fail(HTMX_IS_METRICS(self), 0);

	g_mutex_lock(&self->lock);
	val = (self->total_requests > 0)
		? self->total_latency_us / self->total_requests
		: 0;
	g_mutex_unlock(&self->lock);

	return val;
}

guint64
htmx_metrics_get_status_count(HtmxMetrics *self,
                               guint        status_code)
{
	guint64 val;

	g_return_val_if_fail(HTMX_IS_METRICS(self), 0);

	g_mutex_lock(&self->lock);
	val = GPOINTER_TO_SIZE(g_hash_table_lookup(self->status_counts,
	                                            GUINT_TO_POINTER(status_code)));
	g_mutex_unlock(&self->lock);

	return val;
}

guint64
htmx_metrics_get_path_count(HtmxMetrics *self,
                             const gchar *path)
{
	guint64 val;

	g_return_val_if_fail(HTMX_IS_METRICS(self), 0);
	g_return_val_if_fail(path != NULL, 0);

	g_mutex_lock(&self->lock);
	val = GPOINTER_TO_SIZE(g_hash_table_lookup(self->path_counts, path));
	g_mutex_unlock(&self->lock);

	return val;
}

void
htmx_metrics_reset(HtmxMetrics *self)
{
	g_return_if_fail(HTMX_IS_METRICS(self));

	g_mutex_lock(&self->lock);
	self->total_requests = 0;
	self->total_latency_us = 0;
	g_hash_table_remove_all(self->status_counts);
	g_hash_table_remove_all(self->path_counts);
	g_mutex_unlock(&self->lock);
}

void
htmx_metrics_middleware(HtmxContext        *context,
                         HtmxMiddlewareNext  next,
                         gpointer            next_data,
                         gpointer            user_data)
{
	HtmxMetrics *self = HTMX_METRICS(user_data);
	HtmxRequest *request;
	HtmxResponse *response;
	gint64 start_time;
	gint64 end_time;
	guint64 latency_us;
	const gchar *path;
	const gchar *method_str;
	guint status_code;
	guint64 old_count;

	start_time = g_get_monotonic_time();

	/* Run the rest of the pipeline */
	next(context, next_data);

	end_time = g_get_monotonic_time();
	latency_us = (guint64)(end_time - start_time);

	request = htmx_context_get_request(context);
	response = htmx_context_get_response(context);
	path = htmx_request_get_path(request);
	method_str = htmx_method_to_string(htmx_request_get_method(request));
	status_code = (response != NULL) ? htmx_response_get_status(response) : 500;

	g_mutex_lock(&self->lock);

	self->total_requests++;
	self->total_latency_us += latency_us;

	/* Increment status count */
	old_count = GPOINTER_TO_SIZE(g_hash_table_lookup(self->status_counts,
	                                                  GUINT_TO_POINTER(status_code)));
	g_hash_table_insert(self->status_counts,
	                    GUINT_TO_POINTER(status_code),
	                    GSIZE_TO_POINTER(old_count + 1));

	/* Increment path count */
	old_count = GPOINTER_TO_SIZE(g_hash_table_lookup(self->path_counts, path));
	g_hash_table_insert(self->path_counts,
	                    g_strdup(path),
	                    GSIZE_TO_POINTER(old_count + 1));

	g_mutex_unlock(&self->lock);

	/* Emit signal (outside lock) */
	g_signal_emit(self, signals[SIGNAL_REQUEST_COMPLETED], 0,
	              path, method_str, status_code, latency_us);
}
