# htmx-glib common build rules
# Copyright (C) 2026 Zach Podbielniak
# SPDX-License-Identifier: AGPL-3.0-or-later

# Pattern rule for compiling C source files
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(ECHO) "  CC      $<"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/core/%.o: $(COREDIR)/%.c | $(OBJDIR)/core
	$(ECHO) "  CC      $<"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/model/%.o: $(MODELDIR)/%.c | $(OBJDIR)/model
	$(ECHO) "  CC      $<"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/element/%.o: $(ELEMENTDIR)/%.c | $(OBJDIR)/element
	$(ECHO) "  CC      $<"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/extensions/%.o: $(EXTENSIONDIR)/%.c | $(OBJDIR)/extensions
	$(ECHO) "  CC      $<"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

# Create object directories
$(OBJDIR):
	$(Q)mkdir -p $@

$(OBJDIR)/core:
	$(Q)mkdir -p $@

$(OBJDIR)/model:
	$(Q)mkdir -p $@

$(OBJDIR)/element:
	$(Q)mkdir -p $@

$(OBJDIR)/extensions:
	$(Q)mkdir -p $@

$(BUILDDIR):
	$(Q)mkdir -p $@

# Generate config.h from template
$(BUILDDIR)/config.h: $(SRCDIR)/config.h.in | $(BUILDDIR)
	$(ECHO) "  GEN     $@"
	$(Q)sed \
		-e 's/@VERSION_MAJOR@/$(VERSION_MAJOR)/g' \
		-e 's/@VERSION_MINOR@/$(VERSION_MINOR)/g' \
		-e 's/@VERSION_MICRO@/$(VERSION_MICRO)/g' \
		-e 's/@VERSION@/$(VERSION)/g' \
		$< > $@

# Generate htmx-version.h from template
$(BUILDDIR)/htmx-version.h: $(SRCDIR)/htmx-version.h.in | $(BUILDDIR)
	$(ECHO) "  GEN     $@"
	$(Q)sed \
		-e 's/@VERSION_MAJOR@/$(VERSION_MAJOR)/g' \
		-e 's/@VERSION_MINOR@/$(VERSION_MINOR)/g' \
		-e 's/@VERSION_MICRO@/$(VERSION_MICRO)/g' \
		-e 's/@VERSION@/$(VERSION)/g' \
		$< > $@

# Generate pkg-config file
$(BUILDDIR)/$(PROJECT_NAME)-$(API_VERSION).pc: $(PROJECT_NAME)-$(API_VERSION).pc.in | $(BUILDDIR)
	$(ECHO) "  GEN     $@"
	$(Q)sed \
		-e 's|@PREFIX@|$(PREFIX)|g' \
		-e 's|@LIBDIR@|$(LIBDIR)|g' \
		-e 's|@INCLUDEDIR@|$(INCLUDEDIR)|g' \
		-e 's|@VERSION@|$(VERSION)|g' \
		-e 's|@API_VERSION@|$(API_VERSION)|g' \
		-e 's|@PKG_DEPS@|$(PKG_DEPS)|g' \
		$< > $@

# Clean rule
.PHONY: clean
clean:
	$(ECHO) "  CLEAN"
	$(Q)rm -rf $(BUILDDIR)
	$(Q)rm -f $(LIB_NAME).so $(LIB_SONAME) $(LIB_FILENAME) $(LIB_STATIC)
	$(Q)rm -f $(GIR_FILE) $(TYPELIB_FILE)

# Distclean rule
.PHONY: distclean
distclean: clean
	$(Q)rm -f *~
	$(Q)rm -f $(SRCDIR)/*~
	$(Q)rm -f $(SRCDIR)/**/*~
