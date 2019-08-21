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
EXAMPLEDIR ?= examples
BUILDDIR ?= build
DEPDIR ?= .deps

SRCS     = $(wildcard $(SRCDIR)/*.c)
EXAMPLES = $(wildcard $(EXAMPLEDIR)/*.c)

OBJS     = $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.src.o,$(SRCS))
OBJS    += $(patsubst $(EXAMPLEDIR)/%.c,$(BUILDDIR)/%.ex.o,$(EXAMPLES))
DEPS     = $(patsubst $(BUILDDIR)/%.o,$(DEPDIR)/%.d,$(OBJS))

INCFLAGS  = -I$(SRCDIR) -I$(EXADIR)/include
compile.c = $(CC) $(CFLAGS) $(CPPFLAGS) $(INCFLAGS)
LDFLAGS  += -L$(EXADIR)/lib -lexa

.PHONY: example
example: $(EXAMPLES:$(EXAMPLEDIR)/%.c=$(BUILDDIR)/%.ex.o)

.PHONY: install
install:
	@mkdir -p $(DESTDIR)$(PREFIX)/include
	@cp $(SRCDIR)/*.h $(DESTDIR)$(PREFIX)/include/

$(DEPDIR)/%.src.d: $(SRCDIR)/%.c
	@$(CPP) $(CFLAGS) $(INCFLAGS) $< -MM -MT $(@:$(DEPDIR)/%.src.d=$(BUILDDIR)/%.src.deps) >$@
$(DEPDIR)/%.ex.d: $(EXAMPLEDIR)/%.c
	@$(CPP) $(CFLAGS) $(INCFLAGS) $< -MM -MT $(@:$(DEPDIR)/%.ex.d=$(BUILDDIR)/%.ex.deps) >$@

-include $(DEPS)

$(BUILDDIR)/%.ex.o: $(EXAMPLEDIR)/%.c $(BUILDDIR)/%.ex.deps
	$(compile.c) $< -o $@ $(LDFLAGS)
$(BUILDDIR)/%.src.o: $(SRCDIR)/%.c $(BUILDDIR)/%.src.deps
	$(compile.c) $< -o $@ $(LDFLAGS)

.PHONY: all
all: example install

.PHONY: clean
clean:
	@rm -rf $(BUILDDIR) $(DEPDIR)

.PHONY: print
print :
	@echo $(VAR)=$($(VAR))

$(shell mkdir -p $(BUILDDIR))
$(shell mkdir -p $(DEPDIR))
