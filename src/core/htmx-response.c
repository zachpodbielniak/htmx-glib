/*
 * htmx-response.c - HTMX response builder implementation
 *
 * Copyright (C) 2026 Zach Podbielniak
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "htmx-response.h"
#include "htmx-response-private.h"

/* One registered completion hook (see htmx_response_on_complete()) */
typedef struct {
	HtmxResponseCompleteFunc func;
	gpointer                 user_data;
	GDestroyNotify           destroy;
} CompleteHook;

struct _HtmxResponse {
	GObject parent_instance;

	gchar               *content;       /* Response body */
	GBytes              *bytes;         /* ...or the body, verbatim */
	guint                status_code;   /* HTTP status code */
	gchar               *content_type;  /* Content-Type header */
	GHashTable          *custom_headers;/* Custom headers, one per name */
	GPtrArray           *appended;      /* Headers that may repeat */
	HtmxResponseHeaders *htmx_headers;  /* HTMX response headers */
	gboolean             streaming;     /* Handler owns the message */

	/*
	 * A deferred response is a placeholder: the answer comes later, from
	 * htmx_response_complete().  The server pauses the message meanwhile,
	 * so the main loop goes on serving other requests.
	 */
	gboolean             deferred;
	gboolean             completed;     /* complete() has been called */
	gboolean             abandoned;     /* the client went away first */
	HtmxResponse        *final;         /* what complete() was given */
	GPtrArray           *hooks;         /* CompleteHook*, in order */
	GCancellable        *cancellable;   /* cancelled when abandoned */
	SoupServerMessage   *message;       /* paused, while attached */
	gulong               disconnected_id;
};

G_DEFINE_FINAL_TYPE(HtmxResponse, htmx_response, G_TYPE_OBJECT)

/*
 * Object finalization
 */
static void
htmx_response_finalize(GObject *object)
{
	HtmxResponse *self = HTMX_RESPONSE(object);

	g_clear_pointer(&self->content, g_free);
	g_clear_pointer(&self->bytes, g_bytes_unref);
	g_clear_pointer(&self->content_type, g_free);
	g_clear_pointer(&self->custom_headers, g_hash_table_unref);
	g_clear_pointer(&self->appended, g_ptr_array_unref);
	g_clear_pointer(&self->htmx_headers, htmx_response_headers_free);
	g_clear_pointer(&self->hooks, g_ptr_array_unref);
	g_clear_object(&self->final);
	g_clear_object(&self->cancellable);

	/* Attached means paused: a placeholder dropped without an answer
	 * would leave its client waiting for ever, so answer it now. */
	if (self->message != NULL) {
		g_signal_handler_disconnect(self->message, self->disconnected_id);
		if (!self->abandoned) {
			g_warning("a deferred response was finalized without htmx_response_complete(); answering 500");
			soup_server_message_set_status(self->message, 500, NULL);
			soup_server_message_unpause(self->message);
		}
		g_clear_object(&self->message);
	}

	G_OBJECT_CLASS(htmx_response_parent_class)->finalize(object);
}

/*
 * Class initialization
 */
static void
htmx_response_class_init(HtmxResponseClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);

	object_class->finalize = htmx_response_finalize;
}

/*
 * Instance initialization
 */
static void
htmx_response_init(HtmxResponse *self)
{
	self->content = NULL;
	self->status_code = 200;
	self->content_type = g_strdup("text/html; charset=utf-8");
	self->custom_headers = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
	self->appended = g_ptr_array_new_with_free_func(g_free);
	self->htmx_headers = htmx_response_headers_new();
	self->streaming = FALSE;
	self->deferred = FALSE;
}

/*
 * Create a new empty response
 */
HtmxResponse *
htmx_response_new(void)
{
	return g_object_new(HTMX_TYPE_RESPONSE, NULL);
}

/*
 * Create a new response with content
 */
HtmxResponse *
htmx_response_new_with_content(const gchar *content)
{
	HtmxResponse *self;

	self = htmx_response_new();
	htmx_response_set_content(self, content);

	return self;
}

/*
 * Create a response that says the handler has taken the message over.
 *
 * Without this a route cannot stream.  handle_request() applies whatever
 * the handler returned -- and applying a response replaces Content-Type
 * and appends a body, so an SSE handler's `text/event-stream` was
 * overwritten by `text/html` before the first event went out.  Returning
 * NULL was no better: that is the 404 path.
 */
HtmxResponse *
htmx_response_new_streaming(void)
{
	HtmxResponse *self;

	self = htmx_response_new();
	self->streaming = TRUE;

	return self;
}

/*
 * Whether this response means "leave the message alone"
 */
gboolean
htmx_response_is_streaming(HtmxResponse *self)
{
	g_return_val_if_fail(HTMX_IS_RESPONSE(self), FALSE);

	return self->streaming;
}

/* ── Deferred responses ──────────────────────────────────────────── */

static void
complete_hook_free(gpointer data)
{
	CompleteHook *hook = data;

	if (hook->destroy != NULL) {
		hook->destroy(hook->user_data);
	}
	g_free(hook);
}

/*
 * Create a placeholder for an answer that is not ready yet.
 */
HtmxResponse *
htmx_response_new_deferred(void)
{
	HtmxResponse *self;

	self = htmx_response_new();
	self->deferred = TRUE;
	self->hooks = g_ptr_array_new_with_free_func(complete_hook_free);
	self->cancellable = g_cancellable_new();

	return self;
}

gboolean
htmx_response_is_deferred(HtmxResponse *self)
{
	g_return_val_if_fail(HTMX_IS_RESPONSE(self), FALSE);

	return self->deferred;
}

gboolean
htmx_response_is_completed(HtmxResponse *self)
{
	g_return_val_if_fail(HTMX_IS_RESPONSE(self), FALSE);

	return self->completed;
}

GCancellable *
htmx_response_get_cancellable(HtmxResponse *self)
{
	g_return_val_if_fail(HTMX_IS_RESPONSE(self), NULL);
	g_return_val_if_fail(self->deferred, NULL);

	return self->cancellable;
}

void
htmx_response_on_complete(
	HtmxResponse             *self,
	HtmxResponseCompleteFunc  func,
	gpointer                  user_data,
	GDestroyNotify            destroy
){
	CompleteHook *hook;

	g_return_if_fail(HTMX_IS_RESPONSE(self));
	g_return_if_fail(self->deferred);
	g_return_if_fail(func != NULL);

	/*
	 * Completed inside its own handler, before middleware saw it: the
	 * answer is not written until the server attaches the message, so
	 * the hook can still run on it now, in the order it would have.
	 */
	if (self->completed) {
		if (self->message != NULL || self->final == NULL) {
			g_critical("htmx_response_on_complete() on a response already written");
			if (destroy != NULL)
				destroy(user_data);
			return;
		}
		func(self, self->final, user_data);
		if (destroy != NULL)
			destroy(user_data);
		return;
	}

	hook = g_new0(CompleteHook, 1);
	hook->func = func;
	hook->user_data = user_data;
	hook->destroy = destroy;
	g_ptr_array_add(self->hooks, hook);
}

/*
 * Write the final answer onto the paused message and let it go.
 */
static void
deliver(HtmxResponse *self)
{
	SoupServerMessage *message;

	if (self->message == NULL) {
		return;
	}

	message = g_steal_pointer(&self->message);
	g_signal_handler_disconnect(message, self->disconnected_id);
	self->disconnected_id = 0;

	if (!self->abandoned) {
		htmx_response_apply(self->final, message);
		soup_server_message_unpause(message);
	}

	g_object_unref(message);
}

void
htmx_response_complete(
	HtmxResponse *self,
	HtmxResponse *response
){
	guint i;

	g_return_if_fail(HTMX_IS_RESPONSE(self));
	g_return_if_fail(self->deferred);
	g_return_if_fail(HTMX_IS_RESPONSE(response));
	g_return_if_fail(!htmx_response_is_deferred(response));

	if (self->completed) {
		g_critical("htmx_response_complete() called twice on one deferred response");
		return;
	}

	self->completed = TRUE;
	self->final = g_object_ref(response);

	/*
	 * Hooks run in the order they were registered.  Middleware registers
	 * its hook after next() returns, so the innermost registers first:
	 * the same order its code after next() would have run in.
	 */
	for (i = 0; i < self->hooks->len; i++) {
		CompleteHook *hook = g_ptr_array_index(self->hooks, i);

		hook->func(self, self->final, hook->user_data);
	}
	g_ptr_array_set_size(self->hooks, 0);

	deliver(self);
}

static void
on_disconnected(SoupServerMessage *message, gpointer user_data)
{
	HtmxResponse *self = user_data;

	(void)message;
	self->abandoned = TRUE;
	g_cancellable_cancel(self->cancellable);
}

void
htmx_response_attach_message(
	HtmxResponse      *self,
	SoupServerMessage *message
){
	g_return_if_fail(HTMX_IS_RESPONSE(self));
	g_return_if_fail(self->deferred);
	g_return_if_fail(self->message == NULL);

	/* Answered already, inside the handler: nothing to wait for. */
	if (self->completed) {
		htmx_response_apply(self->final, message);
		return;
	}

	self->message = g_object_ref(message);
	self->disconnected_id = g_signal_connect(message, "disconnected",
	                                         G_CALLBACK(on_disconnected), self);
	soup_server_message_pause(message);
}

/* ── Answers computed on a worker thread ─────────────────────────── */

typedef struct {
	HtmxResponseThreadFunc func;
	gpointer               user_data;
	GDestroyNotify         destroy;
} ThreadJob;

static void
thread_job_free(gpointer data)
{
	ThreadJob *job = data;

	if (job->destroy != NULL) {
		job->destroy(job->user_data);
	}
	g_free(job);
}

static void
thread_job_run(
	GTask        *task,
	gpointer      source_object,
	gpointer      task_data,
	GCancellable *cancellable
){
	ThreadJob *job = task_data;
	HtmxResponse *response;

	(void)source_object;
	response = job->func(cancellable, job->user_data);
	g_task_return_pointer(task, response, g_object_unref);
}

static void
thread_job_done(
	GObject      *source_object,
	GAsyncResult *result,
	gpointer      user_data
){
	HtmxResponse *self = HTMX_RESPONSE(source_object);
	g_autoptr(HtmxResponse) response = NULL;

	(void)user_data;
	response = g_task_propagate_pointer(G_TASK(result), NULL);
	if (response == NULL) {
		response = htmx_response_error("The request produced no response");
	}
	htmx_response_complete(self, response);
}

HtmxResponse *
htmx_response_new_threaded(
	HtmxResponseThreadFunc func,
	gpointer               user_data,
	GDestroyNotify         destroy
){
	HtmxResponse *self;
	ThreadJob *job;
	g_autoptr(GTask) task = NULL;

	g_return_val_if_fail(func != NULL, NULL);

	self = htmx_response_new_deferred();
	job = g_new0(ThreadJob, 1);
	job->func = func;
	job->user_data = user_data;
	job->destroy = destroy;

	/*
	 * The task holds the placeholder as its source object, so it lives
	 * until the answer is written however early the caller drops it; and
	 * the callback comes back on the calling thread's main context, which
	 * is where the message must be unpaused.
	 */
	task = g_task_new(self, self->cancellable, thread_job_done, NULL);
	g_task_set_source_tag(task, htmx_response_new_threaded);
	g_task_set_task_data(task, job, thread_job_free);
	g_task_set_return_on_cancel(task, FALSE);
	g_task_run_in_thread(task, thread_job_run);

	return self;
}

/*
 * Set the response content
 */
HtmxResponse *
htmx_response_set_content(
	HtmxResponse *self,
	const gchar  *content
){
	g_return_val_if_fail(HTMX_IS_RESPONSE(self), NULL);

	g_free(self->content);
	self->content = g_strdup(content);

	return self;
}

/*
 * Set the response body to bytes, verbatim
 */
HtmxResponse *
htmx_response_set_bytes(
	HtmxResponse *self,
	GBytes       *bytes
){
	g_return_val_if_fail(HTMX_IS_RESPONSE(self), NULL);

	g_clear_pointer(&self->bytes, g_bytes_unref);

	if (bytes != NULL) {
		self->bytes = g_bytes_ref(bytes);
	}

	return self;
}

/*
 * Get the response body bytes, if it was given any
 */
GBytes *
htmx_response_get_bytes(HtmxResponse *self)
{
	g_return_val_if_fail(HTMX_IS_RESPONSE(self), NULL);

	return self->bytes;
}

/*
 * Get the response content
 */
const gchar *
htmx_response_get_content(HtmxResponse *self)
{
	g_return_val_if_fail(HTMX_IS_RESPONSE(self), NULL);

	return self->content;
}

/*
 * Set the HTTP status code
 */
HtmxResponse *
htmx_response_set_status(
	HtmxResponse *self,
	guint         status_code
){
	g_return_val_if_fail(HTMX_IS_RESPONSE(self), NULL);

	self->status_code = status_code;

	return self;
}

/*
 * Get the HTTP status code
 */
guint
htmx_response_get_status(HtmxResponse *self)
{
	g_return_val_if_fail(HTMX_IS_RESPONSE(self), 200);

	return self->status_code;
}

/*
 * Set the Content-Type header
 */
HtmxResponse *
htmx_response_set_content_type(
	HtmxResponse *self,
	const gchar  *content_type
){
	g_return_val_if_fail(HTMX_IS_RESPONSE(self), NULL);

	g_free(self->content_type);
	self->content_type = g_strdup(content_type);

	return self;
}

/*
 * Add a custom header
 */
HtmxResponse *
htmx_response_add_header(
	HtmxResponse *self,
	const gchar  *name,
	const gchar  *value
){
	g_return_val_if_fail(HTMX_IS_RESPONSE(self), NULL);
	g_return_val_if_fail(name != NULL, NULL);

	g_hash_table_insert(self->custom_headers, g_strdup(name), g_strdup(value));

	return self;
}

/*
 * Add a header that may appear more than once
 */
HtmxResponse *
htmx_response_append_header(
	HtmxResponse *self,
	const gchar  *name,
	const gchar  *value
){
	g_return_val_if_fail(HTMX_IS_RESPONSE(self), NULL);
	g_return_val_if_fail(name != NULL, NULL);

	g_ptr_array_add(self->appended, g_strdup(name));
	g_ptr_array_add(self->appended, g_strdup(value));

	return self;
}

/*
 * Get the HTMX headers
 */
HtmxResponseHeaders *
htmx_response_get_headers(HtmxResponse *self)
{
	g_return_val_if_fail(HTMX_IS_RESPONSE(self), NULL);

	return self->htmx_headers;
}

/*
 * Set HX-Location
 */
HtmxResponse *
htmx_response_location(
	HtmxResponse *self,
	const gchar  *location
){
	g_return_val_if_fail(HTMX_IS_RESPONSE(self), NULL);

	htmx_response_headers_set_location(self->htmx_headers, location);

	return self;
}

/*
 * Set HX-Push-Url
 */
HtmxResponse *
htmx_response_push_url(
	HtmxResponse *self,
	const gchar  *url
){
	g_return_val_if_fail(HTMX_IS_RESPONSE(self), NULL);

	htmx_response_headers_set_push_url(self->htmx_headers, url);

	return self;
}

/*
 * Set HX-Redirect
 */
HtmxResponse *
htmx_response_redirect(
	HtmxResponse *self,
	const gchar  *url
){
	g_return_val_if_fail(HTMX_IS_RESPONSE(self), NULL);

	htmx_response_headers_set_redirect(self->htmx_headers, url);

	return self;
}

/*
 * Set HX-Refresh
 */
HtmxResponse *
htmx_response_refresh(HtmxResponse *self)
{
	g_return_val_if_fail(HTMX_IS_RESPONSE(self), NULL);

	htmx_response_headers_set_refresh(self->htmx_headers, TRUE);

	return self;
}

/*
 * Set HX-Replace-Url
 */
HtmxResponse *
htmx_response_replace_url(
	HtmxResponse *self,
	const gchar  *url
){
	g_return_val_if_fail(HTMX_IS_RESPONSE(self), NULL);

	htmx_response_headers_set_replace_url(self->htmx_headers, url);

	return self;
}

/*
 * Set HX-Reswap with style
 */
HtmxResponse *
htmx_response_reswap(
	HtmxResponse *self,
	HtmxSwapStyle swap_style
){
	const gchar *swap_str;

	g_return_val_if_fail(HTMX_IS_RESPONSE(self), NULL);

	swap_str = htmx_swap_style_to_string(swap_style);
	htmx_response_headers_set_reswap(self->htmx_headers, swap_str);

	return self;
}

/*
 * Set HX-Reswap with full configuration
 */
HtmxResponse *
htmx_response_reswap_with_config(
	HtmxResponse   *self,
	const HtmxSwap *swap
){
	g_autofree gchar *swap_str = NULL;

	g_return_val_if_fail(HTMX_IS_RESPONSE(self), NULL);
	g_return_val_if_fail(swap != NULL, NULL);

	swap_str = htmx_swap_render(swap);
	htmx_response_headers_set_reswap(self->htmx_headers, swap_str);

	return self;
}

/*
 * Set HX-Retarget
 */
HtmxResponse *
htmx_response_retarget(
	HtmxResponse *self,
	const gchar  *target
){
	g_return_val_if_fail(HTMX_IS_RESPONSE(self), NULL);

	htmx_response_headers_set_retarget(self->htmx_headers, target);

	return self;
}

/*
 * Set HX-Reselect
 */
HtmxResponse *
htmx_response_reselect(
	HtmxResponse *self,
	const gchar  *selector
){
	g_return_val_if_fail(HTMX_IS_RESPONSE(self), NULL);

	htmx_response_headers_set_reselect(self->htmx_headers, selector);

	return self;
}

/*
 * Set HX-Trigger
 */
HtmxResponse *
htmx_response_trigger(
	HtmxResponse *self,
	const gchar  *event
){
	g_return_val_if_fail(HTMX_IS_RESPONSE(self), NULL);

	htmx_response_headers_set_trigger(self->htmx_headers, event);

	return self;
}

/*
 * Set HX-Trigger-After-Settle
 */
HtmxResponse *
htmx_response_trigger_after_settle(
	HtmxResponse *self,
	const gchar  *event
){
	g_return_val_if_fail(HTMX_IS_RESPONSE(self), NULL);

	htmx_response_headers_set_trigger_after_settle(self->htmx_headers, event);

	return self;
}

/*
 * Set HX-Trigger-After-Swap
 */
HtmxResponse *
htmx_response_trigger_after_swap(
	HtmxResponse *self,
	const gchar  *event
){
	g_return_val_if_fail(HTMX_IS_RESPONSE(self), NULL);

	htmx_response_headers_set_trigger_after_swap(self->htmx_headers, event);

	return self;
}

/*
 * Iterator callback for adding custom headers
 */
static void
add_custom_header(gpointer key, gpointer value, gpointer user_data)
{
	SoupMessageHeaders *headers = (SoupMessageHeaders *)user_data;

	soup_message_headers_replace(headers, (const gchar *)key, (const gchar *)value);
}

/*
 * Apply the response to a SoupServerMessage
 */
void
htmx_response_apply(
	HtmxResponse      *self,
	SoupServerMessage *message
){
	SoupMessageHeaders *headers;
	SoupMessageBody *body;

	g_return_if_fail(HTMX_IS_RESPONSE(self));
	g_return_if_fail(SOUP_IS_SERVER_MESSAGE(message));

	/*
	 * The handler is writing this message itself -- an SSE stream, or a
	 * body produced a chunk at a time.  Touching the status, the headers
	 * or the body here would undo what it has already set.
	 */
	if (self->streaming) {
		return;
	}

	/* A placeholder carries no answer; attach_message() waits for one. */
	if (self->deferred) {
		return;
	}

	/* Set status */
	soup_server_message_set_status(message, self->status_code, NULL);

	/* Get response headers */
	headers = soup_server_message_get_response_headers(message);

	/* Set Content-Type */
	if (self->content_type != NULL) {
		soup_message_headers_replace(headers, "Content-Type", self->content_type);
	}

	/* Add custom headers */
	g_hash_table_foreach(self->custom_headers, add_custom_header, headers);

	/* Then the ones allowed to repeat, in the order they were added */
	{
		guint i;

		for (i = 0; i + 1 < self->appended->len; i += 2) {
			soup_message_headers_append(
				headers,
				g_ptr_array_index(self->appended, i),
				g_ptr_array_index(self->appended, i + 1));
		}
	}

	/* Apply HTMX headers */
	htmx_response_headers_apply(self->htmx_headers, message);

	/* Set body */
	body = soup_server_message_get_response_body(message);

	/*
	 * Bytes win over content.
	 *
	 * `content` is a NUL-terminated string, which is right for HTML and
	 * cannot carry a PNG: the first zero byte would end the body, so an
	 * image served that way arrives truncated at a length that depends
	 * on the picture.  A response that has been given bytes uses them
	 * and its length rather than strlen().
	 */
	if (self->bytes != NULL) {
		gsize size = 0;
		const gchar *data = g_bytes_get_data(self->bytes, &size);

		soup_message_body_append(body, SOUP_MEMORY_COPY, data, size);
	} else if (self->content != NULL) {
		soup_message_body_append(body, SOUP_MEMORY_COPY, self->content, strlen(self->content));
	}
}

/*
 * Create an empty 200 OK response
 */
HtmxResponse *
htmx_response_empty(void)
{
	return htmx_response_new();
}

/*
 * Create a 286 response to stop polling
 */
HtmxResponse *
htmx_response_stop_polling(void)
{
	HtmxResponse *self;

	self = htmx_response_new();
	htmx_response_set_status(self, 286);

	return self;
}

/*
 * Create a 404 Not Found response
 */
HtmxResponse *
htmx_response_not_found(void)
{
	HtmxResponse *self;

	self = htmx_response_new();
	htmx_response_set_status(self, 404);
	htmx_response_set_content(self, "<h1>404 Not Found</h1>");

	return self;
}

/*
 * Create a 500 error response
 */
HtmxResponse *
htmx_response_error(const gchar *message)
{
	HtmxResponse *self;
	g_autofree gchar *content = NULL;

	self = htmx_response_new();
	htmx_response_set_status(self, 500);

	content = g_strdup_printf("<h1>500 Internal Server Error</h1><p>%s</p>",
	                          message != NULL ? message : "An error occurred");
	htmx_response_set_content(self, content);

	return self;
}
