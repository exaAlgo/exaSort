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
SRCDIR ?= src
EXAMPLESDIR ?= examples
TESTSDIR ?= tests
BUILDDIR ?= build
DEPDIR ?= .deps

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(SRCS))
DEPS = $(patsubst $(BUILDDIR)/%.o,$(DEPDIR)/%.d,$(OBJS))
EXAMPLESRCS = $(wildcard $(EXAMPLESDIR)/*.c)
EXAMPLEOBJS = $(patsubst $(EXAMPLESDIR)/%.c,$(BUILDDIR)/examples/%,$(EXAMPLESRCS))
TESTSRCS = $(wildcard $(TESTSDIR)/*.c)
TESTOBJS = $(patsubst $(TESTSDIR)/%.c,$(BUILDDIR)/tests/%,$(TESTSRCS))

### Set various flags ###
ifneq ($(DEBUG),0)
  CFLAGS += -g
endif

CFLAGS   += -fPIC
incflags  = -I$(SRCDIR) -I$(EXADIR)/include
compile.c = $(CC) $(CFLAGS) $(CPPFLAGS) $(incflags)
LDFLAGS  += -L$(EXADIR)/lib -lexa
link.o    = $(CC) $(LDFLAGS) -shared -o
EXT       = so
LIBNAME   = libexaSort.$(EXT)

### Make targets ###
.PHONY: all
all: lib install examples tests

.PHONY: lib
lib: $(OBJS)
	$(link.o) $(BUILDDIR)/$(LIBNAME) $(OBJS) $(LDFLAGS)

.PHONY: install
install: lib
	@mkdir -p $(DESTDIR)$(PREFIX)/lib
	@mkdir -p $(DESTDIR)$(PREFIX)/include
	@cp -u $(SRCDIR)/*.h $(DESTDIR)$(PREFIX)/include/
	@cp -u $(BUILDDIR)/$(LIBNAME) $(DESTDIR)$(PREFIX)/lib/

$(DEPDIR)/%.src.d: $(SRCDIR)/%.c
	@$(CPP) $(CFLAGS) $(incflags) $< -MM -MT $(@:$(DEPDIR)/%.src.d=$(BUILDDIR)/%.src.deps) >$@
$(DEPDIR)/%.ex.d: $(EXAMPLESDIR)/%.c
	@$(CPP) $(CFLAGS) $(incflags) $< -MM -MT $(@:$(DEPDIR)/%.ex.d=$(BUILDDIR)/%.ex.deps) >$@

-include $(DEPS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	$(compile.c) -c $< -o $@ $(LDFLAGS)

.PHONY: examples
examples: lib $(EXAMPLEOBJS)

.PHONY: test
test: lib $(TESTOBJS)
	BUILDDIR=$(BUILDDIR) ./tests/run-tests.sh

.PHONY: clean
clean:
	@rm -rf $(BUILDDIR) $(DEPDIR)

.PHONY: print
print :
	@echo $(VAR)=$($(VAR))

$(BUILDDIR)/examples/%: $(EXAMPLESDIR)/%.c
	$(compile.c) $< -o $@ -L$(BUILDDIR) -lexaSort $(LDFLAGS)

$(BUILDDIR)/tests/%: $(TESTSDIR)/%.c
	$(compile.c) $< -o $@ -L$(BUILDDIR) -lexaSort $(LDFLAGS)

$(shell mkdir -p $(BUILDDIR))
$(shell mkdir -p $(BUILDDIR)/examples)
$(shell mkdir -p $(BUILDDIR)/tests)
$(shell mkdir -p $(DEPDIR))
