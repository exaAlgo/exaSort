# Compilers and flags
CC ?= mpicc
CFLAGS ?= -O2
CPP ?= cpp
CPPFLAGS ?=
LDFLAGS ?=

# Dependency locations
EXADIR ?=

# Build options
DEBUG ?= 1
SHARED ?= 0

# Install prefix
PREFIX ?= $(HOME)/local/exaSort

# Meta info about the package
SRCDIR ?= src
BUILDDIR ?= build
DEPDIR ?= .deps

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(SRCS))
DEPS = $(patsubst $(BUILDDIR)/%.o,$(DEPDIR)/%.d,$(OBJS))

INCFLAGS = -I$(SRCDIR) -I$(EXADIR)/include
compile.c = $(CC) $(CFLAGS) $(CPPFLAGS) $(INCFLAGS) -c
link.o = $(AR) crs
LDFLAGS = $(EXADIR)/lib/libexa.a
EXT = a

ifneq ($(SHARED),0)
  CFLAGS  += -fPIC
  link.o   = $(CC) $(LDFLAGS) -shared -o
  LDFLAGS += -L$(EXADIR)/lib -lexa
  EXT      = so
endif

LIBNAME = libexaSort.$(EXT)

.PHONY: lib
lib: $(OBJS)
	$(link.o) $(BUILDDIR)/$(LIBNAME) $(OBJS) $(LDFLAGS)

.PHONY: install
install:
	@mkdir -p $(DESTDIR)$(PREFIX)/lib
	@mkdir -p $(DESTDIR)$(PREFIX)/include
	@cp $(SRCDIR)/*.h $(EXADIR)/include/*.h $(DESTDIR)$(PREFIX)/include/
	@cp $(BUILDDIR)/$(LIBNAME) $(DESTDIR)$(PREFIX)/lib/

$(DEPDIR)/%.d: $(SRCDIR)/%.c
	@$(CPP) $(CFLAGS) $(INCFLAGS) $< -MM -MT $(@:$(DEPDIR)/%.d=$(BUILDDIR)/%.deps) >$@

-include $(DEPS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c $(BUILDDIR)/%.deps
	$(compile.c) $< -o $@

.PHONY: clean
clean:
	@rm -rf $(BUILDDIR) $(DEPDIR)

.PHONY: print
print :
	@echo $(VAR)=$($(VAR))

$(shell mkdir -p $(BUILDDIR))
$(shell mkdir -p $(DEPDIR))
