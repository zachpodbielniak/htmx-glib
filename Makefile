# htmx-glib Makefile
# Copyright (C) 2026 Zach Podbielniak
# SPDX-License-Identifier: AGPL-3.0-or-later

# Set default goal before includes
.DEFAULT_GOAL := all

include config.mk
include rules.mk

# Source files by module
CORE_SOURCES = \
	$(COREDIR)/htmx-enums.c \
	$(COREDIR)/htmx-error.c \
	$(COREDIR)/htmx-config.c \
	$(COREDIR)/htmx-request.c \
	$(COREDIR)/htmx-response.c \
	$(COREDIR)/htmx-context.c \
	$(COREDIR)/htmx-middleware.c \
	$(COREDIR)/htmx-cookie.c \
	$(COREDIR)/htmx-session.c \
	$(COREDIR)/htmx-csrf.c \
	$(COREDIR)/htmx-validator.c \
	$(COREDIR)/htmx-cache.c \
	$(COREDIR)/htmx-memory-cache.c \
	$(COREDIR)/htmx-rate-limiter.c \
	$(COREDIR)/htmx-i18n.c \
	$(COREDIR)/htmx-template-engine.c \
	$(COREDIR)/htmx-handler.c \
	$(COREDIR)/htmx-router.c \
	$(COREDIR)/htmx-server.c

MODEL_SOURCES = \
	$(MODELDIR)/htmx-attribute.c \
	$(MODELDIR)/htmx-trigger.c \
	$(MODELDIR)/htmx-swap.c \
	$(MODELDIR)/htmx-headers.c

ELEMENT_SOURCES = \
	$(ELEMENTDIR)/htmx-element.c \
	$(ELEMENTDIR)/htmx-node.c \
	$(ELEMENTDIR)/htmx-builder.c \
	$(ELEMENTDIR)/htmx-fragment.c \
	$(ELEMENTDIR)/htmx-form.c \
	$(ELEMENTDIR)/htmx-input.c \
	$(ELEMENTDIR)/htmx-template.c \
	$(ELEMENTDIR)/htmx-div.c \
	$(ELEMENTDIR)/htmx-span.c \
	$(ELEMENTDIR)/htmx-p.c \
	$(ELEMENTDIR)/htmx-heading.c \
	$(ELEMENTDIR)/htmx-a.c \
	$(ELEMENTDIR)/htmx-button.c \
	$(ELEMENTDIR)/htmx-label.c \
	$(ELEMENTDIR)/htmx-list.c \
	$(ELEMENTDIR)/htmx-img.c \
	$(ELEMENTDIR)/htmx-textarea.c \
	$(ELEMENTDIR)/htmx-select.c \
	$(ELEMENTDIR)/htmx-table.c \
	$(ELEMENTDIR)/htmx-semantic.c \
	$(ELEMENTDIR)/htmx-fieldset.c \
	$(ELEMENTDIR)/htmx-text.c \
	$(ELEMENTDIR)/htmx-deflist.c \
	$(ELEMENTDIR)/htmx-interactive.c \
	$(ELEMENTDIR)/htmx-media.c \
	$(ELEMENTDIR)/htmx-void.c \
	$(ELEMENTDIR)/htmx-meta.c \
	$(ELEMENTDIR)/htmx-document.c

EXTENSION_SOURCES = \
	$(EXTENSIONDIR)/htmx-extension.c \
	$(EXTENSIONDIR)/htmx-extension-registry.c \
	$(EXTENSIONDIR)/htmx-sse.c \
	$(EXTENSIONDIR)/htmx-websocket.c

ALL_SOURCES = $(CORE_SOURCES) $(MODEL_SOURCES) $(ELEMENT_SOURCES) $(EXTENSION_SOURCES)

# Header files for GIR
CORE_HEADERS = \
	$(COREDIR)/htmx-enums.h \
	$(COREDIR)/htmx-error.h \
	$(COREDIR)/htmx-config.h \
	$(COREDIR)/htmx-request.h \
	$(COREDIR)/htmx-response.h \
	$(COREDIR)/htmx-context.h \
	$(COREDIR)/htmx-middleware.h \
	$(COREDIR)/htmx-cookie.h \
	$(COREDIR)/htmx-session.h \
	$(COREDIR)/htmx-csrf.h \
	$(COREDIR)/htmx-validator.h \
	$(COREDIR)/htmx-cache.h \
	$(COREDIR)/htmx-memory-cache.h \
	$(COREDIR)/htmx-rate-limiter.h \
	$(COREDIR)/htmx-i18n.h \
	$(COREDIR)/htmx-template-engine.h \
	$(COREDIR)/htmx-handler.h \
	$(COREDIR)/htmx-router.h \
	$(COREDIR)/htmx-server.h

MODEL_HEADERS = \
	$(MODELDIR)/htmx-attribute.h \
	$(MODELDIR)/htmx-trigger.h \
	$(MODELDIR)/htmx-swap.h \
	$(MODELDIR)/htmx-headers.h

ELEMENT_HEADERS = \
	$(ELEMENTDIR)/htmx-element.h \
	$(ELEMENTDIR)/htmx-node.h \
	$(ELEMENTDIR)/htmx-builder.h \
	$(ELEMENTDIR)/htmx-fragment.h \
	$(ELEMENTDIR)/htmx-form.h \
	$(ELEMENTDIR)/htmx-input.h \
	$(ELEMENTDIR)/htmx-template.h \
	$(ELEMENTDIR)/htmx-div.h \
	$(ELEMENTDIR)/htmx-span.h \
	$(ELEMENTDIR)/htmx-p.h \
	$(ELEMENTDIR)/htmx-heading.h \
	$(ELEMENTDIR)/htmx-a.h \
	$(ELEMENTDIR)/htmx-button.h \
	$(ELEMENTDIR)/htmx-label.h \
	$(ELEMENTDIR)/htmx-list.h \
	$(ELEMENTDIR)/htmx-img.h \
	$(ELEMENTDIR)/htmx-textarea.h \
	$(ELEMENTDIR)/htmx-select.h \
	$(ELEMENTDIR)/htmx-table.h \
	$(ELEMENTDIR)/htmx-semantic.h \
	$(ELEMENTDIR)/htmx-fieldset.h \
	$(ELEMENTDIR)/htmx-text.h \
	$(ELEMENTDIR)/htmx-deflist.h \
	$(ELEMENTDIR)/htmx-interactive.h \
	$(ELEMENTDIR)/htmx-media.h \
	$(ELEMENTDIR)/htmx-void.h \
	$(ELEMENTDIR)/htmx-meta.h \
	$(ELEMENTDIR)/htmx-document.h

EXTENSION_HEADERS = \
	$(EXTENSIONDIR)/htmx-extension.h \
	$(EXTENSIONDIR)/htmx-extension-registry.h \
	$(EXTENSIONDIR)/htmx-sse.h \
	$(EXTENSIONDIR)/htmx-websocket.h

ALL_HEADERS = \
	$(SRCDIR)/htmx-glib.h \
	$(SRCDIR)/htmx-types.h \
	$(CORE_HEADERS) \
	$(MODEL_HEADERS) \
	$(ELEMENT_HEADERS) \
	$(EXTENSION_HEADERS)

# Object files
OBJECTS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(ALL_SOURCES))

# Generated files
GENERATED = \
	$(BUILDDIR)/config.h \
	$(BUILDDIR)/htmx-version.h \
	$(BUILDDIR)/$(PROJECT_NAME)-$(API_VERSION).pc

# Default target
.PHONY: all
all: $(GENERATED) $(LIB_FILENAME) $(LIB_STATIC)

# Shared library
$(LIB_FILENAME): $(OBJECTS)
	$(ECHO) "  LD      $@"
	$(Q)$(CC) -shared -Wl,-soname,$(LIB_SONAME) -o $@ $(OBJECTS) $(LDFLAGS)
	$(Q)ln -sf $(LIB_FILENAME) $(LIB_SONAME)
	$(Q)ln -sf $(LIB_SONAME) $(LIB_NAME).so

# Static library
$(LIB_STATIC): $(OBJECTS)
	$(ECHO) "  AR      $@"
	$(Q)$(AR) rcs $@ $(OBJECTS)

# Ensure generated headers exist before compiling
$(OBJECTS): $(BUILDDIR)/config.h $(BUILDDIR)/htmx-version.h

# GObject Introspection
.PHONY: gir
gir: $(GIR_FILE) $(TYPELIB_FILE)

$(GIR_FILE): $(LIB_FILENAME) $(ALL_HEADERS) $(ALL_SOURCES)
	$(ECHO) "  GIR     $@"
	$(Q)$(G_IR_SCANNER) \
		--namespace=$(GIR_NAMESPACE) \
		--nsversion=$(GIR_VERSION) \
		--library=$(LIB_NAME) \
		--library-path=. \
		--include=GLib-2.0 \
		--include=GObject-2.0 \
		--include=Gio-2.0 \
		--include=Soup-3.0 \
		--include=Json-1.0 \
		--pkg=glib-2.0 \
		--pkg=gobject-2.0 \
		--pkg=gio-2.0 \
		--pkg=libsoup-3.0 \
		--pkg=json-glib-1.0 \
		--warn-all \
		--c-include="htmx-glib.h" \
		-I$(SRCDIR) \
		-I$(BUILDDIR) \
		--output=$@ \
		$(ALL_HEADERS) \
		$(ALL_SOURCES)

$(TYPELIB_FILE): $(GIR_FILE)
	$(ECHO) "  TYPELIB $@"
	$(Q)$(G_IR_COMPILER) --output=$@ $<

# Tests
.PHONY: test
test: $(LIB_FILENAME)
	$(ECHO) "  TEST    Running tests..."
	$(Q)$(MAKE) -C $(TESTDIR) run

# Examples
.PHONY: examples
examples: $(LIB_FILENAME)
	$(ECHO) "  BUILD   examples"
	$(Q)$(MAKE) -C $(EXAMPLEDIR)

# Install
.PHONY: install
install: all gir
	$(ECHO) "  INSTALL $(PREFIX)"
	$(Q)install -d $(DESTDIR)$(LIBDIR)
	$(Q)install -d $(DESTDIR)$(INCLUDEDIR)
	$(Q)install -d $(DESTDIR)$(INCLUDEDIR)/core
	$(Q)install -d $(DESTDIR)$(INCLUDEDIR)/model
	$(Q)install -d $(DESTDIR)$(INCLUDEDIR)/element
	$(Q)install -d $(DESTDIR)$(INCLUDEDIR)/extensions
	$(Q)install -d $(DESTDIR)$(PKGCONFIGDIR)
	$(Q)install -d $(DESTDIR)$(GIRDIR)
	$(Q)install -d $(DESTDIR)$(TYPELIBDIR)
	$(Q)install -m 755 $(LIB_FILENAME) $(DESTDIR)$(LIBDIR)/
	$(Q)ln -sf $(LIB_FILENAME) $(DESTDIR)$(LIBDIR)/$(LIB_SONAME)
	$(Q)ln -sf $(LIB_SONAME) $(DESTDIR)$(LIBDIR)/$(LIB_NAME).so
	$(Q)install -m 644 $(LIB_STATIC) $(DESTDIR)$(LIBDIR)/
	$(Q)install -m 644 $(SRCDIR)/htmx-glib.h $(DESTDIR)$(INCLUDEDIR)/
	$(Q)install -m 644 $(SRCDIR)/htmx-types.h $(DESTDIR)$(INCLUDEDIR)/
	$(Q)install -m 644 $(BUILDDIR)/config.h $(DESTDIR)$(INCLUDEDIR)/
	$(Q)install -m 644 $(BUILDDIR)/htmx-version.h $(DESTDIR)$(INCLUDEDIR)/
	$(Q)install -m 644 $(CORE_HEADERS) $(DESTDIR)$(INCLUDEDIR)/core/
	$(Q)install -m 644 $(MODEL_HEADERS) $(DESTDIR)$(INCLUDEDIR)/model/
	$(Q)install -m 644 $(ELEMENT_HEADERS) $(DESTDIR)$(INCLUDEDIR)/element/
	$(Q)install -m 644 $(EXTENSION_HEADERS) $(DESTDIR)$(INCLUDEDIR)/extensions/
	$(Q)install -m 644 $(BUILDDIR)/$(PROJECT_NAME)-$(API_VERSION).pc $(DESTDIR)$(PKGCONFIGDIR)/
	$(Q)install -m 644 $(GIR_FILE) $(DESTDIR)$(GIRDIR)/
	$(Q)install -m 644 $(TYPELIB_FILE) $(DESTDIR)$(TYPELIBDIR)/

# Uninstall
.PHONY: uninstall
uninstall:
	$(ECHO) "  UNINSTALL $(PREFIX)"
	$(Q)rm -f $(DESTDIR)$(LIBDIR)/$(LIB_FILENAME)
	$(Q)rm -f $(DESTDIR)$(LIBDIR)/$(LIB_SONAME)
	$(Q)rm -f $(DESTDIR)$(LIBDIR)/$(LIB_NAME).so
	$(Q)rm -f $(DESTDIR)$(LIBDIR)/$(LIB_STATIC)
	$(Q)rm -rf $(DESTDIR)$(INCLUDEDIR)
	$(Q)rm -f $(DESTDIR)$(PKGCONFIGDIR)/$(PROJECT_NAME)-$(API_VERSION).pc
	$(Q)rm -f $(DESTDIR)$(GIRDIR)/$(GIR_FILE)
	$(Q)rm -f $(DESTDIR)$(TYPELIBDIR)/$(TYPELIB_FILE)

# Help
.PHONY: help
help:
	@echo "htmx-glib build system"
	@echo ""
	@echo "Targets:"
	@echo "  all       - Build shared and static libraries (default)"
	@echo "  gir       - Generate GObject introspection files"
	@echo "  test      - Run unit tests"
	@echo "  examples  - Build example programs"
	@echo "  install   - Install to PREFIX (default: /usr/local)"
	@echo "  uninstall - Remove installed files"
	@echo "  clean     - Remove build artifacts"
	@echo "  distclean - Remove all generated files"
	@echo "  help      - Show this help"
	@echo ""
	@echo "Variables:"
	@echo "  PREFIX     - Installation prefix (default: /usr/local)"
	@echo "  BUILD_TYPE - debug or release (default: debug)"
	@echo "  V          - Verbose output (V=1 for verbose)"
	@echo ""
	@echo "Examples:"
	@echo "  make                    - Build in debug mode"
	@echo "  make BUILD_TYPE=release - Build in release mode"
	@echo "  make V=1                - Build with verbose output"
	@echo "  make install PREFIX=/usr - Install to /usr"

# Dependencies (auto-generated could go here)
-include $(OBJECTS:.o=.d)
