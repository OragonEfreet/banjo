LIB ?= static
BUILD ?= debug
VERBOSE ?= false

# Define directories
BASE_DIR = make-$(LIB)-$(BUILD)
INCDIR = inc
SRCDIR = src
OBJDIR = $(BASE_DIR)/obj
LIBDIR = $(BASE_DIR)/lib
BINDIR = $(BASE_DIR)/bin

# Define source and object files
SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))
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
TARGET = $(LIBDIR)/libbanjo.a
else ifeq ($(LIB), shared)
TARGET = $(LIBDIR)/libbanjo.so
else
$(error LIB must be either 'static' or 'shared')
endif

# Define command prefix based on verbose mode
ifeq ($(VERBOSE), 1)
V =
else
V = @
endif

# $(info TARGET: $(TARGET))
# $(info BUILD: $(BUILD))
# $(info LIB: $(LIB))

# Default target
all: banjo

# Alias target
banjo: $(TARGET)
	@echo "Project built in $(BASE_DIR)/"

# Create static library
$(LIBDIR)/libbanjo.a: $(OBJS)
	@mkdir -p $(LIBDIR)
	$(V)ar rcs $@ $^
	@$(info Generated $(notdir $@))

# Create shared library
$(LIBDIR)/libbanjo.so: $(OBJS)
	@mkdir -p $(LIBDIR)
	$(V)$(CC) -shared -o $@ $^
	@$(info Generated $(notdir $@))

# Compile source files to object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(V)$(CC) $(CFLAGS) -I$(INCDIR) -MMD -MP -c -o $@ $<
	@$(info Compiled $(notdir $<))

# Clean up
clean:
	$(V)rm -rf $(BASE_DIR)
	@$(info Cleaned project)

.PHONY: all clean banjo

-include $(DEPS)
