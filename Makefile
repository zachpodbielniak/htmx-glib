# Makefile - Main build file for htmx-glib
# Copyright (C) 2026 Zach Podbielniak
# SPDX-License-Identifier: AGPL-3.0-or-later
#
# Usage:
#   make             - Release build to build/release/
#   make DEBUG=1     - Debug build to build/debug/
#   make test        - Run tests
#   make install     - Install to PREFIX
#   make help        - Show all targets

# When clean and build targets appear together (e.g. make clean all -j),
# serialize them via sub-make so clean finishes before the build starts.
ifneq ($(filter clean clean-all distclean,$(MAKECMDGOALS)),)
ifneq ($(filter-out clean clean-all distclean,$(MAKECMDGOALS)),)

.PHONY: $(MAKECMDGOALS) __serialize
$(MAKECMDGOALS): __serialize ;
__serialize:
	$(MAKE) --no-print-directory $(filter clean clean-all distclean,$(MAKECMDGOALS))
	$(MAKE) --no-print-directory $(filter-out clean clean-all distclean,$(MAKECMDGOALS))

__MIXED := 1
endif
endif

ifndef __MIXED

# Capture git commit SHA for version info; append -UNSTAGED if working tree is dirty
GIT_SHA_BASE := $(shell git rev-parse --short=12 HEAD 2>/dev/null || echo "unknown")
GIT_DIRTY    := $(shell git diff --quiet && git diff --cached --quiet || echo "-UNSTAGED")
GIT_SHA      := $(GIT_SHA_BASE)$(GIT_DIRTY)

include config.mk

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
	$(COREDIR)/htmx-callback-handler.c \
	$(COREDIR)/htmx-uploaded-file.c \
	$(COREDIR)/htmx-compression.c \
	$(COREDIR)/htmx-validation-group.c \
	$(COREDIR)/htmx-metrics.c \
	$(COREDIR)/htmx-router.c \
	$(COREDIR)/htmx-server.c

MODEL_SOURCES = \
	$(MODELDIR)/htmx-attribute.c \
	$(MODELDIR)/htmx-trigger.c \
	$(MODELDIR)/htmx-swap.c \
	$(MODELDIR)/htmx-sync.c \
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

# Header files
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
	$(OUTDIR)/config.h \
	$(OUTDIR)/htmx-version.h \
	$(OUTDIR)/$(PROJECT_NAME)-$(API_VERSION).pc

# Include common rules
include rules.mk

# Default target
.PHONY: all
all: $(GENERATED) $(LIB_SHARED) $(LIB_STATIC)

# Shared library
.PHONY: shared
shared: $(LIB_SHARED)

$(LIB_SHARED): $(OBJECTS) | $(OUTDIR)
	$(ECHO) "  LD      $@"
	$(Q)$(CC) -shared -Wl,-soname,$(LIB_SONAME) -o $@ $(OBJECTS) $(LDFLAGS)
	$(Q)cd $(OUTDIR) && ln -sf $(notdir $(LIB_SHARED)) $(LIB_SONAME) 2>/dev/null || true
	$(Q)cd $(OUTDIR) && ln -sf $(LIB_SONAME) $(LIB_NAME).so 2>/dev/null || true

# Static library
.PHONY: static
static: $(LIB_STATIC)

$(LIB_STATIC): $(OBJECTS) | $(OUTDIR)
	$(ECHO) "  AR      $@"
	$(Q)$(AR) rcs $@ $(OBJECTS)

# Ensure generated headers exist before compiling
$(OBJECTS): $(OUTDIR)/config.h $(OUTDIR)/htmx-version.h

# GObject Introspection (opt-in: make GIR=1)
ifeq ($(GIR),1)

.PHONY: gir
gir: $(GIR_FILE) $(TYPELIB_FILE)

$(GIR_FILE): $(LIB_SHARED) $(ALL_HEADERS) $(ALL_SOURCES) | $(OUTDIR)
	$(ECHO) "  GIR     $@"
	$(Q)$(G_IR_SCANNER) \
		--namespace=$(GIR_NAMESPACE) \
		--nsversion=$(GIR_VERSION) \
		--library=$(LIB_NAME) \
		--library-path=$(OUTDIR) \
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
		-I$(OUTDIR) \
		-DHTMX_GLIB_COMPILATION \
		--output=$@ \
		$(ALL_HEADERS) \
		$(ALL_SOURCES)

$(TYPELIB_FILE): $(GIR_FILE)
	$(ECHO) "  TYPELIB $@"
	$(Q)$(G_IR_COMPILER) --output=$@ $<

else

.PHONY: gir
gir:
	@echo "GIR support is opt-in. Run 'make GIR=1 gir' to generate .gir and .typelib files."
	@echo "Requires gobject-introspection to be installed."

endif

# Tests
.PHONY: test
test: $(LIB_SHARED)
	$(ECHO) "  TEST    Running tests..."
	$(Q)$(MAKE) -C $(TESTDIR) run

# Examples
.PHONY: examples
examples: $(LIB_SHARED)
	$(ECHO) "  BUILD   examples"
	$(Q)$(MAKE) -C $(EXAMPLEDIR)

# Install
.PHONY: install
install: all
	$(ECHO) "  INSTALL $(PREFIX)"
	$(Q)install -d $(DESTDIR)$(LIBDIR)
	$(Q)install -d $(DESTDIR)$(INCLUDEDIR)
	$(Q)install -d $(DESTDIR)$(INCLUDEDIR)/core
	$(Q)install -d $(DESTDIR)$(INCLUDEDIR)/model
	$(Q)install -d $(DESTDIR)$(INCLUDEDIR)/element
	$(Q)install -d $(DESTDIR)$(INCLUDEDIR)/extensions
	$(Q)install -d $(DESTDIR)$(PKGCONFIGDIR)
	$(Q)install -m 755 $(LIB_SHARED) $(DESTDIR)$(LIBDIR)/
	$(Q)cd $(DESTDIR)$(LIBDIR) && ln -sf $(notdir $(LIB_SHARED)) $(LIB_SONAME)
	$(Q)cd $(DESTDIR)$(LIBDIR) && ln -sf $(LIB_SONAME) $(LIB_NAME).so
	$(Q)install -m 644 $(LIB_STATIC) $(DESTDIR)$(LIBDIR)/
	$(Q)install -m 644 $(SRCDIR)/htmx-glib.h $(DESTDIR)$(INCLUDEDIR)/
	$(Q)install -m 644 $(SRCDIR)/htmx-types.h $(DESTDIR)$(INCLUDEDIR)/
	$(Q)install -m 644 $(OUTDIR)/config.h $(DESTDIR)$(INCLUDEDIR)/
	$(Q)install -m 644 $(OUTDIR)/htmx-version.h $(DESTDIR)$(INCLUDEDIR)/
	$(Q)install -m 644 $(CORE_HEADERS) $(DESTDIR)$(INCLUDEDIR)/core/
	$(Q)install -m 644 $(MODEL_HEADERS) $(DESTDIR)$(INCLUDEDIR)/model/
	$(Q)install -m 644 $(ELEMENT_HEADERS) $(DESTDIR)$(INCLUDEDIR)/element/
	$(Q)install -m 644 $(EXTENSION_HEADERS) $(DESTDIR)$(INCLUDEDIR)/extensions/
	$(Q)install -m 644 $(OUTDIR)/$(PROJECT_NAME)-$(API_VERSION).pc $(DESTDIR)$(PKGCONFIGDIR)/

# Uninstall
.PHONY: uninstall
uninstall:
	$(ECHO) "  UNINSTALL $(PREFIX)"
	$(Q)rm -f $(DESTDIR)$(LIBDIR)/$(LIB_NAME).so*
	$(Q)rm -f $(DESTDIR)$(LIBDIR)/$(notdir $(LIB_STATIC))
	$(Q)rm -rf $(DESTDIR)$(INCLUDEDIR)
	$(Q)rm -f $(DESTDIR)$(PKGCONFIGDIR)/$(PROJECT_NAME)-$(API_VERSION).pc

# Print variables (for debugging)
.PHONY: vars
vars:
	@echo "PROJECT_NAME  = $(PROJECT_NAME)"
	@echo "VERSION       = $(VERSION)"
	@echo "GIT_SHA       = $(GIT_SHA)"
	@echo "BUILD_TYPE    = $(BUILD_TYPE)"
	@echo "DEBUG         = $(DEBUG)"
	@echo "ASAN          = $(ASAN)"
	@echo "UBSAN         = $(UBSAN)"
	@echo "GIR           = $(GIR)"
	@echo "CC            = $(CC)"
	@echo "CFLAGS        = $(CFLAGS)"
	@echo "LDFLAGS       = $(LDFLAGS)"
	@echo "OUTDIR        = $(OUTDIR)"
	@echo "OBJDIR        = $(OBJDIR)"
	@echo "PREFIX        = $(PREFIX)"
	@echo "LIB_SHARED    = $(LIB_SHARED)"
	@echo "LIB_STATIC    = $(LIB_STATIC)"
	@echo "OBJECTS       = $(OBJECTS)"

-include $(OBJECTS:.o=.d)

endif # ifndef __MIXED
