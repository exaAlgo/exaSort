# Compilers and flags
CC ?= mpicc
CFLAGS ?= -O2
CPP ?= cpp
CPPFLAGS ?=
LDFLAGS ?=

# Dependency locations
GSDIR ?=

# Build options
DEBUG ?= 1
SHARED ?= 0

# Install prefix
PREFIX ?= /home/thilina/local/

# Meta info about the package
SRCDIR ?= src
BUILDDIR ?= build
DEPDIR ?= .deps

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(SRCS))
DEPS = $(patsubst $(BUILDDIR)/%.o,$(DEPDIR)/%.d,$(OBJS))

INCFLAGS = -I$(SRCDIR) -I$(GSDIR)/include
compile.c = $(CC) $(CFLAGS) $(CPPFLAGS) $(INCFLAGS) -c
link.o = $(AR) crs
LIBS = $(GSDIR)/lib/libgs.a
EXT = a

ifneq ($(SHARED),0)
  CFLAGS += -fPIC
  link.o += $(CC) $(LDFLAGS) -shared -o
  LIBS = -L$(GSDIR)/lib -lgs
  EXT = so
endif

.PHONY: lib
lib: $(OBJS)
	$(link.o) $(BUILDDIR)/libexa.$(EXT) $(OBJS) $(LIBS) $(LDFLAGS)

.PHONY: install
install:
	@mkdir -p $(DESTDIR)$(PREFIX)/lib
	@mkdir -p $(DESTDIR)$(PREFIX)/include
	@cp $(SRCDIR)/*.h $(DESTDIR)$(PREFIX)/include/
	@cp $(BUILDDIR)/libexa.$(EXT) $(DESTDIR)$(PREFIX)/lib/

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
