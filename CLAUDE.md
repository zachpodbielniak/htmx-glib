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
