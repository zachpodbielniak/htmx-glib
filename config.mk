# htmx-glib build configuration
# Copyright (C) 2026 Zach Podbielniak
# SPDX-License-Identifier: AGPL-3.0-or-later

# Project info
PROJECT_NAME    = htmx-glib
VERSION_MAJOR   = 1
VERSION_MINOR   = 0
VERSION_MICRO   = 0
VERSION         = $(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_MICRO)
API_VERSION     = 1.0

# Library naming
LIB_NAME        = libhtmx-glib-$(API_VERSION)
LIB_SONAME      = $(LIB_NAME).so.$(VERSION_MAJOR)
LIB_FILENAME    = $(LIB_NAME).so.$(VERSION)
LIB_STATIC      = $(LIB_NAME).a

# Namespace for GIR
GIR_NAMESPACE   = HtmxGlib
GIR_VERSION     = $(API_VERSION)
GIR_FILE        = $(GIR_NAMESPACE)-$(GIR_VERSION).gir
TYPELIB_FILE    = $(GIR_NAMESPACE)-$(GIR_VERSION).typelib

# Directories
PREFIX          ?= /usr/local
LIBDIR          = $(PREFIX)/lib64
INCLUDEDIR      = $(PREFIX)/include/$(PROJECT_NAME)-$(API_VERSION)
PKGCONFIGDIR    = $(LIBDIR)/pkgconfig
GIRDIR          = $(PREFIX)/share/gir-1.0
TYPELIBDIR      = $(LIBDIR)/girepository-1.0

# Build directories
SRCDIR          = src
BUILDDIR        = build
OBJDIR          = $(BUILDDIR)/obj
TESTDIR         = tests
EXAMPLEDIR      = examples
DOCDIR          = docs

# Source subdirectories
COREDIR         = $(SRCDIR)/core
MODELDIR        = $(SRCDIR)/model
ELEMENTDIR      = $(SRCDIR)/element
EXTENSIONDIR    = $(SRCDIR)/extensions

# Compiler and flags
CC              = gcc
AR              = ar
CSTD            = -std=gnu89
WARNINGS        = -Wall -Wextra -Wno-unused-parameter -Wformat-security
DEBUG_FLAGS     = -g -O0
RELEASE_FLAGS   = -O2
PIC_FLAGS       = -fPIC

# pkg-config dependencies
PKG_DEPS        = glib-2.0 gobject-2.0 gio-2.0 libsoup-3.0 json-glib-1.0

# Get flags from pkg-config
PKG_CFLAGS      = $(shell pkg-config --cflags $(PKG_DEPS))
PKG_LIBS        = $(shell pkg-config --libs $(PKG_DEPS))

# Include paths
INCLUDES        = -I$(SRCDIR) -I$(BUILDDIR)

# Combined flags
CFLAGS          = $(CSTD) $(WARNINGS) $(PIC_FLAGS) $(INCLUDES) $(PKG_CFLAGS)
LDFLAGS         = $(PKG_LIBS)

# GObject Introspection
G_IR_SCANNER    = g-ir-scanner
G_IR_COMPILER   = g-ir-compiler

# Build type (debug or release)
BUILD_TYPE      ?= debug
ifeq ($(BUILD_TYPE),debug)
    CFLAGS += $(DEBUG_FLAGS) -DHTMX_DEBUG
else
    CFLAGS += $(RELEASE_FLAGS)
endif

# Verbose mode
V               ?= 0
ifeq ($(V),0)
    Q = @
    ECHO = @echo
else
    Q =
    ECHO = @true
endif
