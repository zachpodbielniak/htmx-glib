# rules.mk - Common build rules for htmx-glib
# Copyright (C) 2026 Zach Podbielniak
# SPDX-License-Identifier: AGPL-3.0-or-later

# Generic pattern rule — mkdir -p handles all subdirectory creation
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(ECHO) "  CC      $<"
	$(Q)$(CC) $(CFLAGS) -MMD -MP -c -o $@ $<

# Create output directory
$(OUTDIR):
	mkdir -p $(OUTDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

# Generate config.h from template
$(OUTDIR)/config.h: $(SRCDIR)/config.h.in | $(OUTDIR)
	$(ECHO) "  GEN     $@"
	$(Q)sed \
		-e 's/@VERSION_MAJOR@/$(VERSION_MAJOR)/g' \
		-e 's/@VERSION_MINOR@/$(VERSION_MINOR)/g' \
		-e 's/@VERSION_MICRO@/$(VERSION_MICRO)/g' \
		-e 's/@VERSION@/$(VERSION)/g' \
		$< > $@

# Generate htmx-version.h from template
$(OUTDIR)/htmx-version.h: $(SRCDIR)/htmx-version.h.in | $(OUTDIR)
	$(ECHO) "  GEN     $@"
	$(Q)sed \
		-e 's/@VERSION_MAJOR@/$(VERSION_MAJOR)/g' \
		-e 's/@VERSION_MINOR@/$(VERSION_MINOR)/g' \
		-e 's/@VERSION_MICRO@/$(VERSION_MICRO)/g' \
		-e 's/@VERSION@/$(VERSION)/g' \
		$< > $@

# Generate pkg-config file
$(OUTDIR)/$(PROJECT_NAME)-$(API_VERSION).pc: $(PROJECT_NAME)-$(API_VERSION).pc.in | $(OUTDIR)
	$(ECHO) "  GEN     $@"
	$(Q)sed \
		-e 's|@PREFIX@|$(PREFIX)|g' \
		-e 's|@LIBDIR@|$(LIBDIR)|g' \
		-e 's|@INCLUDEDIR@|$(INCLUDEDIR)|g' \
		-e 's|@VERSION@|$(VERSION)|g' \
		-e 's|@API_VERSION@|$(API_VERSION)|g' \
		-e 's|@PKG_DEPS@|$(PKG_DEPS)|g' \
		$< > $@

# Clean current build type only
.PHONY: clean
clean:
	$(ECHO) "  CLEAN   $(BUILD_TYPE)"
	$(Q)rm -rf $(BUILDDIR)/$(BUILD_TYPE)

# Clean all build types
.PHONY: clean-all
clean-all:
	$(ECHO) "  CLEAN   all"
	$(Q)rm -rf $(BUILDDIR)

# Distclean rule
.PHONY: distclean
distclean: clean-all

# Help
.PHONY: help
help:
	@echo "htmx-glib build system"
	@echo ""
	@echo "Targets:"
	@echo "  all       - Build shared and static libraries (default)"
	@echo "  shared    - Build shared library only"
	@echo "  static    - Build static library only"
	@echo "  gir       - Generate GObject introspection files"
	@echo "  test      - Run unit tests"
	@echo "  examples  - Build example programs"
	@echo "  install   - Install to PREFIX (default: /usr/local)"
	@echo "  uninstall - Remove installed files"
	@echo "  clean     - Remove current build type ($(BUILD_TYPE))"
	@echo "  clean-all - Remove all build artifacts"
	@echo "  distclean - Same as clean-all"
	@echo "  vars      - Print build variables"
	@echo "  help      - Show this help"
	@echo ""
	@echo "Build options:"
	@echo "  DEBUG=1      - Build with debug symbols and no optimization"
	@echo "  ASAN=1       - Enable address sanitizer"
	@echo "  UBSAN=1      - Enable undefined behavior sanitizer"
	@echo "  GIR=1        - Enable GObject introspection generation"
	@echo "  V=1          - Verbose build output"
	@echo "  PREFIX=/path - Set installation prefix (default: /usr/local)"
	@echo ""
	@echo "Output directories:"
	@echo "  make         -> build/release/"
	@echo "  make DEBUG=1 -> build/debug/"
