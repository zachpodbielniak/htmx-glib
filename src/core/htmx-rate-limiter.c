/*
 * htmx-rate-limiter.c - Token bucket rate limiter implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-rate-limiter.h"
#include "htmx-request.h"
#include "htmx-response.h"
#include <libsoup/soup.h>
#include <math.h>

typedef struct {
	gdouble tokens;
	gint64  last_refill;  /* monotonic microseconds */
} TokenBucket;

static TokenBucket *
token_bucket_new(gdouble max_tokens)
{
	TokenBucket *bucket;

	bucket = g_slice_new(TokenBucket);
	bucket->tokens = max_tokens;
	bucket->last_refill = g_get_monotonic_time();

	return bucket;
}

static void
token_bucket_free(gpointer data)
{
	g_slice_free(TokenBucket, (TokenBucket *)data);
}

struct _HtmxRateLimiter {
	GObject     parent_instance;

	GHashTable *buckets;      /* gchar* -> TokenBucket* */
	guint       max_tokens;
	gdouble     refill_rate;  /* tokens per second */
};

G_DEFINE_FINAL_TYPE(HtmxRateLimiter, htmx_rate_limiter, G_TYPE_OBJECT)

static void
htmx_rate_limiter_finalize(GObject *object)
{
	HtmxRateLimiter *self = HTMX_RATE_LIMITER(object);

	g_hash_table_unref(self->buckets);

	G_OBJECT_CLASS(htmx_rate_limiter_parent_class)->finalize(object);
}

static void
htmx_rate_limiter_class_init(HtmxRateLimiterClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);

	object_class->finalize = htmx_rate_limiter_finalize;
}

static void
htmx_rate_limiter_init(HtmxRateLimiter *self)
{
	self->buckets = g_hash_table_new_full(g_str_hash, g_str_equal,
	                                     g_free, token_bucket_free);
	self->max_tokens = 60;
	self->refill_rate = 1.0;
}

HtmxRateLimiter *
htmx_rate_limiter_new(guint max_tokens, gdouble refill_rate)
{
	HtmxRateLimiter *self;

	g_return_val_if_fail(max_tokens > 0, NULL);
	g_return_val_if_fail(refill_rate > 0.0, NULL);

	self = g_object_new(HTMX_TYPE_RATE_LIMITER, NULL);
	self->max_tokens = max_tokens;
	self->refill_rate = refill_rate;

	return self;
}

gboolean
htmx_rate_limiter_allow(HtmxRateLimiter *self, const gchar *client_id)
{
	TokenBucket *bucket;
	gint64 now;
	gdouble elapsed;
	gdouble new_tokens;

	g_return_val_if_fail(HTMX_IS_RATE_LIMITER(self), FALSE);
	g_return_val_if_fail(client_id != NULL, FALSE);

	bucket = (TokenBucket *)g_hash_table_lookup(self->buckets, client_id);
	if (bucket == NULL) {
		bucket = token_bucket_new((gdouble)self->max_tokens);
		g_hash_table_insert(self->buckets, g_strdup(client_id), bucket);
	}

	/* Refill tokens based on elapsed time */
	now = g_get_monotonic_time();
	elapsed = (gdouble)(now - bucket->last_refill) / (gdouble)G_USEC_PER_SEC;
	new_tokens = bucket->tokens + (elapsed * self->refill_rate);

	if (new_tokens > (gdouble)self->max_tokens) {
		new_tokens = (gdouble)self->max_tokens;
	}

	bucket->tokens = new_tokens;
	bucket->last_refill = now;

	/* Try to consume one token */
	if (bucket->tokens < 1.0) {
		return FALSE;
	}

	bucket->tokens -= 1.0;
	return TRUE;
}

/*
 * Extract client IP from the request's SoupServerMessage.
 */
static gchar *
get_client_ip(HtmxRequest *request)
{
	SoupServerMessage *msg;
	GSocketAddress *remote;
	GInetAddress *inet_addr;

	msg = htmx_request_get_message(request);
	remote = soup_server_message_get_remote_address(msg);

	if (!G_IS_INET_SOCKET_ADDRESS(remote)) {
		return g_strdup("unknown");
	}

	inet_addr = g_inet_socket_address_get_address(
		G_INET_SOCKET_ADDRESS(remote));

	return g_inet_address_to_string(inet_addr);
}

void
htmx_rate_limiter_middleware(
	HtmxContext        *context,
	HtmxMiddlewareNext  next,
	gpointer            next_data,
	gpointer            user_data
){
	HtmxRateLimiter *self = HTMX_RATE_LIMITER(user_data);
	HtmxRequest *request;
	g_autofree gchar *client_ip = NULL;

	request = htmx_context_get_request(context);
	client_ip = get_client_ip(request);

	if (!htmx_rate_limiter_allow(self, client_ip)) {
		HtmxResponse *response;

		response = htmx_response_new();
		htmx_response_set_status(response, 429);
		htmx_response_set_content(response, "Rate limit exceeded");
		htmx_response_set_content_type(response, "text/plain");
		htmx_response_add_header(response, "Retry-After", "60");
		htmx_context_set_response(context, response);
		g_object_unref(response);
		return;
	}

	next(context, next_data);
}
