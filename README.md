# htmx-glib

A GObject-based C library for building [HTMX](https://htmx.org)-powered web applications. Provides a type-safe, introspectable API for constructing HTMX responses, HTML elements, and server-side request handling — all wired up via GObject with full GIR support.

## Features

- **HTML element builders** — Composable GObject hierarchy for constructing HTML trees (`HtmxDiv`, `HtmxForm`, `HtmxTable`, etc.)
- **HTMX model** — First-class types for HTMX attributes, triggers, swap targets, and response headers
- **Request/response handling** — `HtmxRequest`, `HtmxResponse`, `HtmxContext` for server-side HTMX processing
- **Routing** — `HtmxRouter` + `HtmxHandler` for matching HTMX requests to handlers
- **Extensions** — SSE and WebSocket support via `HtmxSse`, `HtmxWebSocket`
- **Middleware stack** — Pluggable `HtmxMiddleware` for auth, CSRF, rate limiting, i18n, sessions
- **Secrets from the kernel** — `htmx_secret_new_hex()` / `htmx_secret_equals()` for session ids, CSRF tokens and anything else that must not be predictable
- **GObject Introspection** — Full `.gir` / `.typelib` for use from Python, Lua, JavaScript, etc.
- **gnu89 C** — No C99+, no C++ — clean, portable, embeddable

## Dependencies

- GLib 2.0 / GObject 2.0 / GIO 2.0
- libsoup 3.0
- json-glib 1.0
- gobject-introspection (optional, for `make GIR=1`)

Install on Fedora:
```
dnf install glib2-devel libsoup3-devel json-glib-devel gobject-introspection-devel
```

## Building

```sh
# Release build (default)
make

# Debug build
make DEBUG=1

# With sanitizers
make DEBUG=1 ASAN=1 UBSAN=1

# Verbose output
make V=1

# Generate GObject introspection data
make GIR=1 gir
```

Output goes to `build/release/` or `build/debug/` depending on `DEBUG`.

## Installation

```sh
make install                       # installs to /usr/local
make install PREFIX=/usr           # installs to /usr
make install DESTDIR=/tmp/stage    # staged install
```

Installed files:
- `$(LIBDIR)/libhtmx-glib-1.0.so.1.0.0` (+ soname symlinks)
- `$(LIBDIR)/libhtmx-glib-1.0.a`
- `$(INCLUDEDIR)/` — public headers
- `$(PKGCONFIGDIR)/htmx-glib-1.0.pc`

## Usage

Link with pkg-config:

```sh
gcc $(pkg-config --cflags htmx-glib-1.0) myapp.c $(pkg-config --libs htmx-glib-1.0)
```

Basic example — building an HTMX fragment:

```c
#include <htmx-glib.h>

HtmxDiv *container = htmx_div_new ();
htmx_element_set_id (HTMX_ELEMENT (container), "result");

HtmxP *para = htmx_p_new ();
htmx_element_set_text (HTMX_ELEMENT (para), "Hello from htmx-glib!");
htmx_element_append_child (HTMX_ELEMENT (container), HTMX_ELEMENT (para));

HtmxFragment *frag = htmx_fragment_new ();
htmx_fragment_append (frag, HTMX_ELEMENT (container));

gchar *html = htmx_fragment_render (frag);
/* send html as HTMX response */
g_free (html);
```

## Build Options

| Option   | Default | Description                              |
|----------|---------|------------------------------------------|
| `DEBUG`  | `0`     | Debug symbols, no optimization (`-O0 -g3`) |
| `ASAN`   | `0`     | AddressSanitizer                         |
| `UBSAN`  | `0`     | UndefinedBehaviorSanitizer               |
| `GIR`    | `0`     | Generate GObject introspection files     |
| `V`      | `0`     | Verbose build output                     |
| `PREFIX` | `/usr/local` | Installation prefix                 |

## Project Layout

```
src/
  htmx-glib.h          — Top-level include
  htmx-types.h         — Common type definitions
  core/                — Server, router, request, response, middleware, etc.
  model/               — HTMX attribute, trigger, swap, headers
  element/             — HTML element builders
  extensions/          — SSE, WebSocket extensions
tests/                 — Unit tests
examples/              — Example applications
build/
  debug/               — Debug build output
  release/             — Release build output
```

## License

AGPL-3.0-or-later. See [COPYING](COPYING).
