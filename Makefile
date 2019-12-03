## User configurations ###
# Compilers and flags
CC ?= mpicc
CFLAGS ?= -O0
CPP ?= cpp
CPPFLAGS ?=
LDFLAGS ?=

# Dependency locations
EXADIR ?=

# Build options
DEBUG ?= 1

# Install prefix
PREFIX ?= $(HOME)/local/exaSort

### Meta info about the package ###
SRCDIR      = src
BUILDDIR    = build
DEPDIR      = .deps
EXAMPLESDIR = examples
TESTSDIR    = tests

LDFLAGS += -L$(EXADIR)/lib -lexa
libname  = exaSort

### Include template makefile ###
-include $(EXADIR)/share/Makefile.in

.PHONY: install
install: lib
	@mkdir -p $(DESTDIR)$(PREFIX)/include
	@cp -u $(SRCDIR)/*.h $(GSDIR)/include/*.h $(DESTDIR)$(PREFIX)/include/
	@mkdir -p $(DESTDIR)$(PREFIX)/lib
	@cp -u $(BUILDDIR)/$(prefix)$(libname).$(ext) $(DESTDIR)$(PREFIX)/lib/
