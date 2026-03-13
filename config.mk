# config.mk - Build configuration for htmx-glib
# Copyright (C) 2026 Zach Podbielniak
# SPDX-License-Identifier: AGPL-3.0-or-later

# Project info
PROJECT_NAME    = htmx-glib
VERSION_MAJOR   = 1
VERSION_MINOR   = 0
VERSION_MICRO   = 0
VERSION         = $(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_MICRO)
API_VERSION     = 1.0

# Installation paths
PREFIX          ?= /usr/local
LIBDIR          = $(PREFIX)/lib64
INCLUDEDIR      = $(PREFIX)/include/$(PROJECT_NAME)-$(API_VERSION)
PKGCONFIGDIR    = $(LIBDIR)/pkgconfig
GIRDIR          = $(PREFIX)/share/gir-1.0
TYPELIBDIR      = $(LIBDIR)/girepository-1.0

# Compiler settings
CC              = gcc
AR              = ar
CSTD            = -std=gnu89
WARNINGS        = -Wall -Wextra -Wno-unused-parameter -Wformat=2 -Wshadow
CFLAGS_BASE     = $(CSTD) $(WARNINGS) -fPIC

# pkg-config dependencies
PKG_DEPS        = glib-2.0 gobject-2.0 gio-2.0 libsoup-3.0 json-glib-1.0

# Get flags from pkg-config
PKG_CFLAGS      := $(shell pkg-config --cflags $(PKG_DEPS))
PKG_LIBS        := $(shell pkg-config --libs $(PKG_DEPS))

# Build directories
BUILDDIR        := build
OBJDIR_DEBUG    := $(BUILDDIR)/debug/obj
OBJDIR_RELEASE  := $(BUILDDIR)/release/obj
BINDIR_DEBUG    := $(BUILDDIR)/debug
BINDIR_RELEASE  := $(BUILDDIR)/release

# Source and test directories
SRCDIR          = src
TESTDIR         = tests
EXAMPLEDIR      = examples
DOCDIR          = docs

# Source subdirectories
COREDIR         = $(SRCDIR)/core
MODELDIR        = $(SRCDIR)/model
ELEMENTDIR      = $(SRCDIR)/element
EXTENSIONDIR    = $(SRCDIR)/extensions

# Build options (0 or 1)
DEBUG           ?= 0
ASAN            ?= 0
UBSAN           ?= 0

# Select build directories based on DEBUG
ifeq ($(DEBUG),1)
    OBJDIR      := $(OBJDIR_DEBUG)
    OUTDIR      := $(BINDIR_DEBUG)
    BUILD_TYPE  := debug
    CFLAGS_OPT   = -O0 -g3 -DHTMX_DEBUG
else
    OBJDIR      := $(OBJDIR_RELEASE)
    OUTDIR      := $(BINDIR_RELEASE)
    BUILD_TYPE  := release
    CFLAGS_OPT   = -O2 -DNDEBUG
endif

# Sanitizer flags
ifeq ($(ASAN),1)
    CFLAGS_SAN   = -fsanitize=address -fno-omit-frame-pointer
    LDFLAGS_SAN  = -fsanitize=address -lasan
endif

ifeq ($(UBSAN),1)
    CFLAGS_SAN  += -fsanitize=undefined
    LDFLAGS_SAN += -fsanitize=undefined
endif

# Git version info (computed in Makefile, will be defined there)
GIT_SHA         ?= unknown

# Verbose mode
V               ?= 0
ifeq ($(V),0)
    Q            = @
    ECHO         = @echo
else
    Q            =
    ECHO         = @true
endif

# Combined flags
CFLAGS          = $(CFLAGS_BASE) $(CFLAGS_OPT) $(CFLAGS_SAN) $(PKG_CFLAGS) \
                  -DHTMX_GLIB_COMPILATION \
                  -DHTMX_VERSION_MAJOR=$(VERSION_MAJOR) \
                  -DHTMX_VERSION_MINOR=$(VERSION_MINOR) \
                  -DHTMX_VERSION_MICRO=$(VERSION_MICRO) \
                  -DHTMX_GIT_SHA=\"$(GIT_SHA)\" \
                  -I$(SRCDIR) -I$(OUTDIR)

LDFLAGS         = $(LDFLAGS_SAN) $(PKG_LIBS)

# GObject Introspection (opt-in: make GIR=1)
GIR             ?= 0
GIR_NAMESPACE   = HtmxGlib
GIR_VERSION     = $(API_VERSION)
GIR_FILE        = $(OUTDIR)/$(GIR_NAMESPACE)-$(GIR_VERSION).gir
TYPELIB_FILE    = $(OUTDIR)/$(GIR_NAMESPACE)-$(GIR_VERSION).typelib
G_IR_SCANNER    = g-ir-scanner
G_IR_COMPILER   = g-ir-compiler

# Library names
LIB_NAME        = libhtmx-glib-$(API_VERSION)
LIB_SHARED      = $(OUTDIR)/$(LIB_NAME).so.$(VERSION)
LIB_SONAME      = $(LIB_NAME).so.$(VERSION_MAJOR)
LIB_STATIC      = $(OUTDIR)/$(LIB_NAME).a
