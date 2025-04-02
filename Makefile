LIB     ?= static
BUILD   ?= debug

# Define directories
OUTDIR = build-$(LIB)-$(BUILD)

# Compiler settings
CC = cc
CPPFLAGS     = -Iinc -DBJ_CONFIG_ALL
CFLAGS       = -Wall -Wextra -std=c99 -MMD -MP 
LIB_CPPFLAGS =
LIB_CFLAGS   =
EXE_CPPFLAGS = -DBANJO_ASSETS_DIR=\"$(ASSETS_DIR)\" 
EXE_CFLAGS   =

ifeq ($(BUILD), release)
CFLAGS += -O2
else ifeq ($(BUILD), debug)
CFLAGS += -g
else
$(error BUILD must be either 'release' or 'debug')
endif

ifeq ($(LIB), static)
BANJO = $(OUTDIR)/libbanjo.a
CPPFLAGS += -DBANJO_STATIC
else ifeq ($(LIB), shared)
BANJO = $(OUTDIR)/libbanjo.so
LIB_CPPFLAGS += -DBANJO_EXPORTS
else
$(error LIB must be either 'static' or 'shared')
endif

V = $(if $(VERBOSE), , @)

# Default target
all: banjo tests examples

################################################################################
### Banjo ######################################################################

LIB_SRCS = $(wildcard src/*.c)
LIB_OBJS = $(LIB_SRCS:%.c=$(OUTDIR)/%.o)

# Alias target
banjo: $(BANJO)

# Create static library
$(OUTDIR)/libbanjo.a: $(LIB_OBJS)
	@mkdir -p $(dir $@)
	$(V)ar rcs $@ $^

# Create shared library
$(OUTDIR)/libbanjo.so: $(LIB_OBJS)
	@mkdir -p $(dir $@)
	$(V)$(CC) -shared -fPIC -o $@ $^

$(OUTDIR)/src/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(V)$(CC) $(CPPFLAGS) $(LIB_CPPFLAGS) $(CFLAGS) $(LIB_CFLAGS) -c -o $@ $<


$(OUTDIR)/src/%.i: src/%.c
	@mkdir -p $(dir $@)
	$(V)$(CC) $(CPPFLAGS) $(LIB_CPPFLAGS) $(CFLAGS) $(LIB_CFLAGS) -E -P -o $@ $<

################################################################################
### Tests & Examples ###########################################################

TST_SRCS = $(wildcard test/*.c)
TST_BINS = $(TST_SRCS:%.c=$(OUTDIR)/%)
TST_OBJS = $(TST_SRCS:%.c=$(OUTDIR)/%.o)

EXM_SRCS = $(wildcard examples/*.c)
EXM_BINS = $(EXM_SRCS:%.c=$(OUTDIR)/%)
EXM_OBJS = $(EXM_SRCS:%.c=$(OUTDIR)/%.o)

ASSETS_DIR = $(abspath assets)

$(OUTDIR)/test/%: test/%.c $(BANJO)
	@mkdir -p $(dir $@)
	$(V)$(CC) $(CPPFLAGS) $(EXE_CPPFLAGS) $(CFLAGS) $(EXE_CFLAGS) -Isrc -o $@ $< -L$(OUTDIR) -lbanjo

tests: $(TST_BINS)

test: banjo tests
	@for test in $(TST_BINS); do \
		echo "Running $$test"; \
		$$test || exit 1; \
	done

$(OUTDIR)/examples/%: examples/%.c $(BANJO)
	@mkdir -p $(dir $@)
	$(V)$(CC) $(CPPFLAGS) $(EXE_CPPFLAGS) $(CFLAGS) $(EXE_CFLAGS) -o $@ $< -L$(OUTDIR) -lbanjo

examples: $(EXM_BINS)

################################################################################
### Clean up ###################################################################
clean:
	$(V)rm -rf $(OUTDIR) $(LIB_OBJS:.o=.d) $(TST_OBJS:.o=.d) $(EXM_OBJS:.o=.d)

.PHONY: all clean banjo tests examples test

-include $(LIB_OBJS:.o=.d) $(TST_OBJS:.o=.d) $(EXM_OBJS:.o=.d)
