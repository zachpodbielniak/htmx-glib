# tests/Makefile
# Copyright (C) 2026 Zach Podbielniak
# SPDX-License-Identifier: AGPL-3.0-or-later

include ../config.mk

# Test programs
TESTS = test-enums test-attribute test-trigger test-swap test-headers \
        test-builder test-elements test-document \
        test-sync test-handler test-upload test-compression \
        test-validation-group test-validator test-metrics test-streaming \
        test-response-bytes test-serve-static test-template-engine \
        test-secret test-csrf test-session test-request test-method \
        test-deferred test-wipe

# Compiler flags - use the build output directory for library and generated headers
TEST_CFLAGS = $(CFLAGS) -I.. -I../$(SRCDIR) -I../$(OUTDIR)
TEST_LDFLAGS = -L../$(OUTDIR) -lhtmx-glib-$(API_VERSION) $(LDFLAGS) -Wl,-rpath,$(shell cd .. && pwd)/$(OUTDIR)

.PHONY: all clean run

all: $(TESTS)

test-validator: test-validator.c
	$(ECHO) "  CC      $@"
	$(Q)$(CC) $(TEST_CFLAGS) -o $@ $< $(TEST_LDFLAGS)

test-deferred: test-deferred.c
	$(ECHO) "  CC      $@"
	$(Q)$(CC) $(TEST_CFLAGS) -o $@ $< $(TEST_LDFLAGS)

test-streaming: test-streaming.c
	$(ECHO) "  CC      $@"
	$(Q)$(CC) $(TEST_CFLAGS) -o $@ $< $(TEST_LDFLAGS)

test-response-bytes: test-response-bytes.c
	$(ECHO) "  CC      $@"
	$(Q)$(CC) $(TEST_CFLAGS) -o $@ $< $(TEST_LDFLAGS)

test-serve-static: test-serve-static.c
	$(ECHO) "  CC      $@"
	$(Q)$(CC) $(TEST_CFLAGS) -o $@ $< $(TEST_LDFLAGS)

test-template-engine: test-template-engine.c
	$(ECHO) "  CC      $@"
	$(Q)$(CC) $(TEST_CFLAGS) -o $@ $< $(TEST_LDFLAGS)

test-wipe: test-wipe.c
	$(ECHO) "  CC      $@"
	$(Q)$(CC) $(TEST_CFLAGS) -o $@ $< $(TEST_LDFLAGS)

test-secret: test-secret.c
	$(ECHO) "  CC      $@"
	$(Q)$(CC) $(TEST_CFLAGS) -o $@ $< $(TEST_LDFLAGS)

test-csrf: test-csrf.c
	$(ECHO) "  CC      $@"
	$(Q)$(CC) $(TEST_CFLAGS) -o $@ $< $(TEST_LDFLAGS)

test-session: test-session.c
	$(ECHO) "  CC      $@"
	$(Q)$(CC) $(TEST_CFLAGS) -o $@ $< $(TEST_LDFLAGS)

test-request: test-request.c
	$(ECHO) "  CC      $@"
	$(Q)$(CC) $(TEST_CFLAGS) -o $@ $< $(TEST_LDFLAGS)

test-method: test-method.c
	$(ECHO) "  CC      $@"
	$(Q)$(CC) $(TEST_CFLAGS) -o $@ $< $(TEST_LDFLAGS)

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

test-sync: test-sync.c
	$(ECHO) "  CC      $@"
	$(Q)$(CC) $(TEST_CFLAGS) -o $@ $< $(TEST_LDFLAGS)

test-handler: test-handler.c
	$(ECHO) "  CC      $@"
	$(Q)$(CC) $(TEST_CFLAGS) -o $@ $< $(TEST_LDFLAGS)

test-upload: test-upload.c
	$(ECHO) "  CC      $@"
	$(Q)$(CC) $(TEST_CFLAGS) -o $@ $< $(TEST_LDFLAGS)

test-compression: test-compression.c
	$(ECHO) "  CC      $@"
	$(Q)$(CC) $(TEST_CFLAGS) -o $@ $< $(TEST_LDFLAGS)

test-validation-group: test-validation-group.c
	$(ECHO) "  CC      $@"
	$(Q)$(CC) $(TEST_CFLAGS) -o $@ $< $(TEST_LDFLAGS)

test-metrics: test-metrics.c
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
