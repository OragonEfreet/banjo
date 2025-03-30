LIB ?= static
BUILD ?= debug
VERBOSE ?= false

# Define directories
OUTDIR = build/make-$(LIB)-$(BUILD)
INCDIR = inc
SRCDIR = src

# Define source and object files
SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c, $(OUTDIR)/%.o, $(SRCS))
DEPS = $(OBJS:.o=.d)

CC = cc
CFLAGS = -Wall -Wextra -std=c99

# Define flags for debug and release builds
ifeq ($(BUILD), release)
CFLAGS += -O2
else ifeq ($(BUILD), debug)
CFLAGS += -g
else
$(error BUILD must be either 'release' or 'debug')
endif

# Define target based on library type
ifeq ($(LIB), static)
TARGET = $(OUTDIR)/libbanjo.a
else ifeq ($(LIB), shared)
TARGET = $(OUTDIR)/libbanjo.so
else
$(error LIB must be either 'static' or 'shared')
endif

# Define command prefix based on verbose mode
ifeq ($(VERBOSE), 1)
V =
else
V = @
endif

# Default target
all: banjo

# Alias target
banjo: $(TARGET)
	@echo "Project built in $(OUTDIR)/"

# Create static library
$(OUTDIR)/libbanjo.a: $(OBJS)
	@mkdir -p $(OUTDIR)
	$(V)ar rcs $@ $^
	@$(info Generated $(notdir $@))

# Create shared library
$(OUTDIR)/libbanjo.so: $(OBJS)
	@mkdir -p $(OUTDIR)
	$(V)$(CC) -shared -o $@ $^
	@$(info Generated $(notdir $@))

# Compile source files to object files
$(OUTDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OUTDIR)
	$(V)$(CC) $(CFLAGS) -I$(INCDIR) -MMD -MP -c -o $@ $<
	@$(info Compiled $(notdir $<))

# Clean up
clean:
	$(V)rm -rf $(OUTDIR)
	@$(info Cleaned project)

.PHONY: all clean banjo

-include $(DEPS)
