# examples/Makefile
# Copyright (C) 2026 Zach Podbielniak
# SPDX-License-Identifier: AGPL-3.0-or-later

# Include parent config
include ../config.mk

# Output directory for example binaries
EXAMPLE_BUILDDIR = ../$(BUILDDIR)/examples

# Example programs
EXAMPLES = \
	todo-app \
	data-table \
	blog-layout \
	faq-accordion \
	media-gallery \
	documentation \
	file-upload \
	new-features

# Full paths to example binaries
EXAMPLE_BINS = $(addprefix $(EXAMPLE_BUILDDIR)/,$(EXAMPLES))

# Compiler flags - use local build
EXAMPLE_CFLAGS = $(CFLAGS) -I.. -I../$(SRCDIR) -I../$(OUTDIR)
EXAMPLE_LDFLAGS = -L../$(OUTDIR) -l$(PROJECT_NAME)-$(API_VERSION) $(LDFLAGS) -Wl,-rpath,$(shell cd .. && pwd)/$(OUTDIR)

.PHONY: all clean

all: $(EXAMPLE_BUILDDIR) $(EXAMPLE_BINS)

$(EXAMPLE_BUILDDIR):
	$(ECHO) "  MKDIR   $(EXAMPLE_BUILDDIR)"
	$(Q)mkdir -p $(EXAMPLE_BUILDDIR)

# Generic rule for building examples
$(EXAMPLE_BUILDDIR)/%: %.c | $(EXAMPLE_BUILDDIR)
	$(ECHO) "  CC      $@"
	$(Q)$(CC) $(EXAMPLE_CFLAGS) -o $@ $< $(EXAMPLE_LDFLAGS)

clean:
	$(ECHO) "  CLEAN   examples"
	$(Q)rm -f $(EXAMPLE_BINS)
	$(Q)rmdir $(EXAMPLE_BUILDDIR) 2>/dev/null || true

# Run targets for each example
.PHONY: run-todo run-data-table run-blog run-faq run-media run-docs run-upload run-new-features

run-todo: $(EXAMPLE_BUILDDIR)/todo-app
	$(EXAMPLE_BUILDDIR)/todo-app

run-data-table: $(EXAMPLE_BUILDDIR)/data-table
	$(EXAMPLE_BUILDDIR)/data-table

run-blog: $(EXAMPLE_BUILDDIR)/blog-layout
	$(EXAMPLE_BUILDDIR)/blog-layout

run-faq: $(EXAMPLE_BUILDDIR)/faq-accordion
	$(EXAMPLE_BUILDDIR)/faq-accordion

run-media: $(EXAMPLE_BUILDDIR)/media-gallery
	$(EXAMPLE_BUILDDIR)/media-gallery

run-docs: $(EXAMPLE_BUILDDIR)/documentation
	$(EXAMPLE_BUILDDIR)/documentation

run-upload: $(EXAMPLE_BUILDDIR)/file-upload
	$(EXAMPLE_BUILDDIR)/file-upload

run-new-features: $(EXAMPLE_BUILDDIR)/new-features
	$(EXAMPLE_BUILDDIR)/new-features
