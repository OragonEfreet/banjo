LIB     ?= static
BUILD   ?= debug

# Define directories
OUTDIR = build-$(LIB)-$(BUILD)

# Compiler settings
CC = cc
CPPFLAGS     = -Iinc -DBJ_CONFIG_ALL
CFLAGS       = -Wall -Wextra -std=c99
CFLAGS_EXTRA = -MMD -MP 
LIB_CPPFLAGS =
LIB_CFLAGS   =
EXE_CPPFLAGS = -DBANJO_ASSETS_DIR=\"$(ASSETS_DIR)\" 
EXE_CFLAGS   =

ifeq ($(LIB), static)
BANJO = $(OUTDIR)/libbanjo.a
CPPFLAGS += -DBANJO_STATIC
else ifeq ($(LIB), shared)
BANJO = $(OUTDIR)/libbanjo.so
LIB_CPPFLAGS += -DBANJO_EXPORTS
else
$(error LIB must be either 'static' or 'shared')
endif

ifeq ($(BUILD), release)
CFLAGS += -O2
else ifeq ($(BUILD), debug)
CFLAGS += -g
else
$(error BUILD must be either 'release' or 'debug')
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
	$(V)$(CC) $(CPPFLAGS) $(LIB_CPPFLAGS) $(CFLAGS) $(LIB_CFLAGS) $(CFLAGS_EXTRA) -c -o $@ $<


$(OUTDIR)/src/%.i: src/%.c
	@mkdir -p $(dir $@)
	$(V)$(CC) $(CPPFLAGS) $(LIB_CPPFLAGS) $(CFLAGS) $(LIB_CFLAGS) $(CFLAGS_EXTRA) -E -P -o $@ $<

flags:
	@echo "$(CPPFLAGS) $(LIB_CPPFLAGS) $(CFLAGS) $(LIB_CFLAGS)"

outdir:
	@echo "$(OUTDIR)"

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
	$(V)$(CC) $(CPPFLAGS) $(EXE_CPPFLAGS) $(CFLAGS) $(EXE_CFLAGS) $(CFLAGS_EXTRA) -Isrc -o $@ $< -L$(OUTDIR) -lbanjo

tests: $(TST_BINS)

test: banjo tests
	@for test in $(TST_BINS); do \
		echo "Running $$test"; \
		$$test || exit 1; \
	done

$(OUTDIR)/examples/%: examples/%.c $(BANJO)
	@mkdir -p $(dir $@)
	$(V)$(CC) $(CPPFLAGS) $(EXE_CPPFLAGS) $(CFLAGS) $(EXE_CFLAGS) $(CFLAGS_EXTRA) -o $@ $< -L$(OUTDIR) -lbanjo

examples: $(EXM_BINS)

################################################################################
### Clean up ###################################################################
clean:
	$(V)rm -rf $(OUTDIR) $(LIB_OBJS:.o=.d) $(TST_OBJS:.o=.d) $(EXM_OBJS:.o=.d)

.PHONY: all clean banjo tests examples test flags help outdir

help:
	@echo "Main targets:"
	@echo "  all         - [Default] Build banjo, tests, and examples"
	@echo "  banjo       - Build banjo library"
	@echo "  examples    - Build example code"
	@echo "  test        - Build tests"
	@echo ""
	@echo "Tools & Object File Targets:"
	@echo "  tests              - Run tests"
	@echo "  clean              - Clean all build files"
	@echo "  flags              - Display the CPPFLAGS and CFLAGS"
	@echo "  outdir             - Display OUTDIR"
	@echo "  OUTDIR/.../file.o  - Build object file for .../file.c"
	@echo "  OUTDIR/.../file.i  - Generate preprocessor output for .../file.c"
	@echo ""
	@echo "Configuration:"
	@echo "  VERBOSE [0 | 1]             - Enable verbose output (default: 0)"
	@echo "  LIB     [static | shared]   - Choose static or shared library (default: static)"
	@echo "  BUILD   [debug | release]   - Set build type (default: debug)"

-include $(LIB_OBJS:.o=.d) $(TST_OBJS:.o=.d) $(EXM_OBJS:.o=.d)
