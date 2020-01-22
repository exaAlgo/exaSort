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
INCFLAGS = -I$(EXADIR)/include
libName  = exaSort

# No need of occa (TODO: Should read from exa install config)
OCCA=0

### Include template makefile ###
-include $(EXADIR)/share/exa-base.mk

.PHONY: lib
lib: lib-base

.PHONY: examples
examples: examples-base

.PHONY: tests
tests: tests-base
	@cp $(EXADIR)/share/run-tests.sh $(BUILDDIR)/$(TESTSDIR)
	@cd $(BUILDDIR)/$(TESTSDIR) && ./run-tests.sh

.PHONY: install
install: install-base

.PHONY: all
all: lib examples tests install
