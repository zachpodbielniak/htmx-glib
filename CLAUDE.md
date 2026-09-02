# htmx-glib Development Guidelines

## Project Overview

htmx-glib is a GObject-based library for building HTMX-powered web applications in C. It provides typed HTML element classes and server-side rendering capabilities.

## Code Style

### Element Creation - Use Typed Element Classes

**ALWAYS** use the explicit typed element classes (e.g., `HtmxDiv`, `HtmxForm`, `HtmxButton`) instead of the low-level `HtmxBuilder` API.

**Correct approach:**
```c
static HtmxDiv *
render_content(void)
{
    g_autoptr(HtmxDiv) container = htmx_div_new();
    g_autoptr(HtmxP) paragraph = htmx_p_new();
    g_autoptr(HtmxButton) button = htmx_button_new_with_label("Click me");

    htmx_element_add_class(HTMX_ELEMENT(container), "content");
    htmx_node_set_text_content(HTMX_NODE(paragraph), "Hello world");

    htmx_node_add_child(HTMX_NODE(container), HTMX_NODE(paragraph));
    htmx_node_add_child(HTMX_NODE(container), HTMX_NODE(button));

    return (HtmxDiv *)g_steal_pointer(&container);
}
```

**Incorrect approach (do NOT use):**
```c
/* Do NOT use HtmxBuilder for element creation */
static gchar *
render_content(void)
{
    g_autoptr(HtmxBuilder) builder = htmx_builder_new();

    htmx_builder_begin(builder, "div");
    htmx_builder_class(builder, "content");
    htmx_builder_begin(builder, "p");
    htmx_builder_text(builder, "Hello world");
    htmx_builder_end(builder);
    htmx_builder_end(builder);

    return htmx_builder_render(builder);
}
```

### When to Use HtmxBuilder

The `HtmxBuilder` API should **only** be used for:
- Full HTML document structure (`<!DOCTYPE>`, `<html>`, `<head>`, `<body>`)
- Inline `<style>` and `<script>` tags
- Raw HTML injection via `htmx_builder_raw_html()`

See `examples/blog-layout.c` for the correct pattern: typed elements for content, builder only for document wrapper.

### Available Element Types

Common element types in `src/element/`:
- **Container**: `HtmxDiv`, `HtmxSpan`, `HtmxSection`, `HtmxArticle`, `HtmxMain`, `HtmxAside`
- **Text**: `HtmxP`, `HtmxHeading` (h1-h6), `HtmxStrong`, `HtmxEm`, `HtmxCode`, `HtmxPre`
- **Links**: `HtmxA`
- **Lists**: `HtmxUl`, `HtmxOl`, `HtmxLi`
- **Forms**: `HtmxForm`, `HtmxInput`, `HtmxButton`, `HtmxLabel`, `HtmxTextarea`, `HtmxSelect`
- **Tables**: `HtmxTable`, `HtmxThead`, `HtmxTbody`, `HtmxTr`, `HtmxTh`, `HtmxTd`
- **Semantic**: `HtmxHeader`, `HtmxFooter`, `HtmxNav`, `HtmxFigure`, `HtmxFigcaption`
- **Media**: `HtmxImg`, `HtmxVideo`, `HtmxAudio`

### Element API Patterns

```c
/* Create element */
g_autoptr(HtmxDiv) div = htmx_div_new();

/* Set ID */
htmx_element_set_id(HTMX_ELEMENT(div), "my-id");

/* Add CSS class */
htmx_element_add_class(HTMX_ELEMENT(div), "my-class");

/* Set arbitrary attribute */
htmx_element_set_attribute(HTMX_ELEMENT(div), "data-value", "123");

/* Set HTMX attributes */
htmx_element_set_attribute(HTMX_ELEMENT(div), "hx-get", "/api/data");
htmx_element_set_attribute(HTMX_ELEMENT(div), "hx-target", "#result");
htmx_element_set_attribute(HTMX_ELEMENT(div), "hx-swap", "innerHTML");

/* Set text content */
htmx_node_set_text_content(HTMX_NODE(div), "Hello");

/* Add child elements */
htmx_node_add_child(HTMX_NODE(parent), HTMX_NODE(child));

/* Render to HTML string */
g_autofree gchar *html = htmx_element_render(HTMX_ELEMENT(div));
```

### Return Patterns

Functions that build elements should return the typed element, not a string:

```c
/* Return typed element for composition */
static HtmxArticle *
render_article(BlogPost *post)
{
    g_autoptr(HtmxArticle) article = htmx_article_new();
    /* ... build element ... */
    return (HtmxArticle *)g_steal_pointer(&article);
}

/* Use in parent */
{
    g_autoptr(HtmxArticle) article = render_article(post);
    htmx_node_add_child(HTMX_NODE(container), HTMX_NODE(article));
}
```

### Memory Management

- Use `g_autoptr()` for automatic cleanup
- Use `g_steal_pointer()` when returning ownership
- Use `g_autofree` for strings that should be freed

```c
static HtmxDiv *
render_something(void)
{
    g_autoptr(HtmxDiv) div = htmx_div_new();
    g_autofree gchar *text = g_strdup_printf("Count: %d", count);

    htmx_node_set_text_content(HTMX_NODE(div), text);

    return (HtmxDiv *)g_steal_pointer(&div);
}
```

## File Organization

- `src/core/` - Server, router, request/response handling
- `src/element/` - HTML element types
- `src/model/` - Data models (headers, attributes, etc.)
- `examples/` - Example applications
- `tests/` - GLib-based tests

## Streaming (SSE)

A route that streams returns `htmx_response_new_streaming()`, which tells
`htmx_response_apply()` to leave the message alone.  Without it the
router applies an ordinary response over the top -- replacing
`Content-Type` and appending a body -- and returning `NULL` is the 404
path, so neither works.

```c
static HtmxResponse *
handle_events(HtmxRequest *request, GHashTable *params, gpointer user_data)
{
	HtmxSseConnection *conn;

	conn = htmx_sse_connection_new(htmx_request_get_message(request));

	/* The connection outlives this call.  Something has to own it --
	 * here, the timer that writes to it. */
	g_signal_connect(conn, "closed", G_CALLBACK(on_closed), NULL);
	g_timeout_add_seconds(1, push_an_event, conn);

	return htmx_response_new_streaming();
}
```

`htmx_sse_connection_new()` sets the status, the SSE headers, chunked
encoding and the pause for you.  The chunked encoding is not optional:
without it libsoup takes the body's length at the moment the handler
returns as the `Content-Length`, writes exactly that, and finishes -- so
the stream delivers whatever the handler wrote inline and nothing after
it.

Connect to `HtmxSseConnection::closed` and drop whatever the connection
owns.  A browser tab closes without saying anything at the HTTP level, so
that signal is the only thing that will ever tell you.  It fires exactly
once, whichever of the three routes gets there first: the peer hanging
up, soup finishing the message, or your own `htmx_sse_connection_close()`.

## Listening on more than one address

`htmx_server_start()` binds exactly one address, which cannot express
"the loopback and this one other interface".  `htmx_server_listen_on()`
is additive and may be called several times; using it at all opts out of
the configured host, so a server told explicitly where to listen is not
also given the config's default of every interface.

```c
htmx_server_listen_on(server, "127.0.0.1", 8790, &error);
htmx_server_listen_on(server, "100.84.12.7", 8790, &error);

/* Report what was bound, not what was asked for: an address whose bind
 * failed is the interesting case. */
uris = htmx_server_get_listen_uris(server);
```

## Anything unguessable comes from the kernel

`htmx_secret_new_hex()` in `core/htmx-secret.h`, and `htmx_secret_equals()`
to compare.  Never `g_uuid_string_random()` and never `g_random_*` for a
value somebody must not be able to predict: those are a pure function of
GLib's global `GRand`, a Mersenne Twister whose 624-word state is
recoverable from a few hundred consecutive outputs.

Session ids and CSRF tokens were both minted that way, and both are handed
to whoever asks for a page -- the session id in a `Set-Cookie`, the CSRF
token in the form -- so an attacker collects the generator's output by
loading pages and then computes everybody else's.  Nothing about the values
looks wrong; the generator is the whole defect, which is why the test for
it reseeds `g_random_set_seed()` and asserts the value does *not* repeat
rather than measuring how random it looks.

A UUID is still right for a filename or a correlation id.  The question is
whether guessing it gets anybody anything.

`htmx_secret_new_hex()` has no userspace fallback and aborts if neither
`getrandom(2)` nor `/dev/urandom` will answer.  A predictable session id is
not a degraded session id, it is somebody else's session, and it would be
indistinguishable from a working one from the outside.

## Sessions and CSRF

**`Set-Cookie` is emitted with `htmx_response_append_header()`.**
`htmx_response_add_header()` inserts into a hash table keyed by name, so
the last writer wins -- right for `Content-Type`, wrong for the one header
in HTTP whose design is that there is more than one of it.  The session
middleware runs after the handler on the way out, so through `add_header()`
it silently replaced any cookie the handler had set.

**There is one CSRF token per session and it does not rotate per request.**
A page renders the token into a form and submits it later, so anything that
replaces the token in between turns that submit into a 403 -- and a page
makes more than one request: a favicon, a stylesheet through the same
middleware stack, an `hx-get` that fills a panel, a second tab.
`htmx_csrf_middleware()` called `htmx_csrf_generate_token()` on every safe
request and did exactly that.  `htmx_csrf_generate_token()` still means
"mint a new one", for the application to call when the identity behind the
session changes.

**Method safety is read from the request's method string, not from
`HtmxMethod`.** That enum has five members -- GET, POST, PUT, PATCH,
DELETE -- and `parse_method()` answers `HTMX_METHOD_GET` for every string
that is not one of them.  So `method == HTMX_METHOD_GET` was not a test for
GET: HEAD and OPTIONS passed it, which is right, and so did `FROB`,
`PROPFIND` and anything else at all, which is a fail-open in the function
whose answer decides whether a credential is required.  The safe methods
are the four of RFC 9110 §9.2.1: GET, HEAD, OPTIONS, TRACE.

## A name that becomes a filename

Two places turn an outside name into a filename and read it:
`htmx_router_serve_static()` and the template engine's `load_template()`.
Both go through `htmx_path_is_inside()` (`core/htmx-path.h`), which
compares canonical paths and requires the separator after the root, so
`/srv/static-secrets` does not count as inside `/srv/static`.

The check used to be a static function beside the router, which made it a
rule about that one caller: a template name -- from
`htmx_template_engine_render(self, name, ...)` or from a `{{> name}}`
inside a template -- was joined with `g_build_filename()` and read, so
`../../etc/passwd` worked.

What it does not catch: `g_canonicalize_filename()` is lexical, so a
symlink inside the root pointing out of it is inside as far as this
function is concerned.  Resolving those needs the file to exist, which is
not true of every path a caller wants to check.

## `@error` is optional, so `*error` is not a pointer you have

`(out) (optional)` on a `GError **` means `NULL` is a legal thing to pass,
and every branch that wants to ask "did that fail?" needs a `GError *local`
plus `g_propagate_error()`.  `render_template_string()` read `*error`
unconditionally and segfaulted on a template naming a partial that is not
there.

## Compression

The compressed body is applied with `htmx_response_set_bytes()`.  It was
`htmx_response_set_content()`, which `g_strdup()`s and is written out with
`strlen()` -- and a gzip stream's third byte is zero, so every compressed
response was a three-byte body under a `Content-Encoding: gzip` promising
the rest.

The compression loop ends on `G_CONVERTER_FINISHED` and on nothing else.
"All the input has been consumed" is a different question: a deflate stream
is not finished until its final block, and a gzip stream is not finished
until the trailer after that.  A body zlib swallows in one pass -- a
megabyte of ordinary HTML does -- came out a few hundred bytes short of
right and decompressed to an error.  A length check cannot see it; the test
decompresses.

## One reader for `application/x-www-form-urlencoded`

`parse_query_string()` in `htmx-request.c` handles a URL's query string and
a POST body alike, because they are the same encoding and differ only in
where they arrive.  There was a separate `parse_form_data()` that wrapped
it and added the `+`-means-space substitution, so a GET form and an
`hx-get` came back with literal `+` characters where the spaces were while
the same form as a POST came back correct.

The substitution happens **before** the `%XX` unescaping, so a genuine plus
sent as `%2B` still decodes to `+`.  The other order cannot tell the two
apart, because by then they are both `+`.

## Testing a route without a server

`htmx_request_new_for_path()` builds a request with no `SoupServerMessage`
behind it, so `htmx_router_match()` can be exercised directly.  Use it for
the cases a real client cannot send -- libsoup normalises and rejects a
lot before a handler ever runs, and those are the cases worth testing
hardest.  `htmx_request_get_message()` is `NULL` on such a request, so it
cannot be used with anything that streams.

## Building

```bash
make          # Build library
make examples # Build examples
make test     # Run tests
make clean    # Clean build artifacts
```

## Example Structure

Examples should follow this pattern (see `blog-layout.c`):

1. Data structures at top
2. Render functions returning typed elements
3. Route handlers using render functions
4. `main()` with server setup and route registration

## Route Handlers

```c
static HtmxResponse *
handle_something(HtmxRequest *request, GHashTable *params, gpointer user_data)
{
    g_autoptr(HtmxResponse) response = htmx_response_new();

    /* Check for HTMX request vs full page load */
    if (htmx_request_is_htmx(request)) {
        /* Return partial HTML */
        g_autoptr(HtmxDiv) content = render_partial();
        g_autofree gchar *html = htmx_element_render(HTMX_ELEMENT(content));
        htmx_response_set_content(response, html);
    } else {
        /* Return full page */
        g_autofree gchar *html = render_full_page();
        htmx_response_set_content(response, html);
    }

    htmx_response_set_content_type(response, "text/html; charset=utf-8");
    return (HtmxResponse *)g_steal_pointer(&response);
}
```

## HTMX Patterns

### Navigation with History

```c
htmx_element_set_attribute(HTMX_ELEMENT(link), "hx-get", "/page");
htmx_element_set_attribute(HTMX_ELEMENT(link), "hx-target", "#main-content");
htmx_element_set_attribute(HTMX_ELEMENT(link), "hx-push-url", "true");
```

### Form Submission

```c
g_autoptr(HtmxForm) form = htmx_form_new();
htmx_element_set_attribute(HTMX_ELEMENT(form), "hx-post", "/submit");
htmx_element_set_attribute(HTMX_ELEMENT(form), "hx-target", "#result");
htmx_element_set_attribute(HTMX_ELEMENT(form), "hx-swap", "innerHTML");
```

### Delete with Confirmation

```c
htmx_element_set_attribute(HTMX_ELEMENT(button), "hx-delete", "/items/1");
htmx_element_set_attribute(HTMX_ELEMENT(button), "hx-target", "#item-1");
htmx_element_set_attribute(HTMX_ELEMENT(button), "hx-swap", "outerHTML");
htmx_element_set_attribute(HTMX_ELEMENT(button), "hx-confirm", "Delete this item?");
```
