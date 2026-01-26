# tests/Makefile
# Copyright (C) 2026 Zach Podbielniak
# SPDX-License-Identifier: AGPL-3.0-or-later

include ../config.mk

# Test programs
TESTS = test-enums test-attribute test-trigger test-swap test-headers test-builder test-elements test-document

# Compiler flags - use local build
TEST_CFLAGS = $(CFLAGS) -I.. -I../$(SRCDIR) -I../$(BUILDDIR)
TEST_LDFLAGS = -L.. -l$(PROJECT_NAME)-$(API_VERSION) $(LDFLAGS) -Wl,-rpath,$(shell pwd)/..

.PHONY: all clean run

all: $(TESTS)

test-enums: test-enums.c
	$(ECHO) "  CC      $@"
	$(Q)$(CC) $(TEST_CFLAGS) -o $@ $< $(TEST_LDFLAGS)

test-attribute: test-attribute.c
	$(ECHO) "  CC      $@"
	$(Q)$(CC) $(TEST_CFLAGS) -o $@ $< $(TEST_LDFLAGS)

test-trigger: test-trigger.c
	$(ECHO) "  CC      $@"
	$(Q)$(CC) $(TEST_CFLAGS) -o $@ $< $(TEST_LDFLAGS)

test-swap: test-swap.c
	$(ECHO) "  CC      $@"
	$(Q)$(CC) $(TEST_CFLAGS) -o $@ $< $(TEST_LDFLAGS)

test-headers: test-headers.c
	$(ECHO) "  CC      $@"
	$(Q)$(CC) $(TEST_CFLAGS) -o $@ $< $(TEST_LDFLAGS)

test-builder: test-builder.c
	$(ECHO) "  CC      $@"
	$(Q)$(CC) $(TEST_CFLAGS) -o $@ $< $(TEST_LDFLAGS)

test-elements: test-elements.c
	$(ECHO) "  CC      $@"
	$(Q)$(CC) $(TEST_CFLAGS) -o $@ $< $(TEST_LDFLAGS)

test-document: test-document.c
	$(ECHO) "  CC      $@"
	$(Q)$(CC) $(TEST_CFLAGS) -o $@ $< $(TEST_LDFLAGS)

run: all
	@echo "Running tests..."
	@failed=0; \
	for test in $(TESTS); do \
		echo "  TEST    $$test"; \
		if ./$$test; then \
			echo "    PASS"; \
		else \
			echo "    FAIL"; \
			failed=$$((failed + 1)); \
		fi; \
	done; \
	if [ $$failed -gt 0 ]; then \
		echo "$$failed test(s) failed"; \
		exit 1; \
	else \
		echo "All tests passed"; \
	fi

clean:
	$(ECHO) "  CLEAN   tests"
	$(Q)rm -f $(TESTS)
