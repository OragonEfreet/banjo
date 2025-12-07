# Banjo API Design Guide

**Document Version**: 1.0
**Last Updated**: 2025-12-07
**Status**: Analysis & Recommendations

---

## Executive Summary

This document analyzes the current Banjo API design and identifies consistency issues that impact developer experience. While Banjo demonstrates excellent documentation practices and a clear philosophy, the API exhibits multiple naming and design patterns that can confuse users and make the library harder to learn.

**Overall Assessment**: The API is functional and well-documented but lacks consistency across modules. Addressing these issues would significantly improve usability without changing core functionality.

**Key Findings**:
- 6 different patterns for resource creation/destruction
- 4 different error handling approaches
- 3 different getter naming conventions
- Inconsistent const-correctness
- Mixed return type conventions

**Recommendation**: Establish and enforce API conventions before 1.0 release. These are breaking changes that become increasingly difficult to make as adoption grows.

---

## Table of Contents

1. [Creation/Destruction Naming Patterns](#1-creationdestruction-naming-patterns)
2. [Error Handling Inconsistencies](#2-error-handling-inconsistencies)
3. [Parameter Ordering](#3-parameter-ordering)
4. [Return Type Inconsistencies](#4-return-type-inconsistencies)
5. [Getter Naming Conventions](#5-getter-naming-conventions)
6. [Boolean Query Functions](#6-boolean-query-functions)
7. [Setter Naming Patterns](#7-setter-naming-patterns)
8. [Prefix Overuse](#8-prefix-overuse)
9. [Mixed Styles Within Modules](#9-mixed-styles-within-modules)
10. [Memory Management Conventions](#10-memory-management-conventions)
11. [Implementation Roadmap](#implementation-roadmap)
12. [Appendix: Style Guide](#appendix-style-guide)

---

## 1. Creation/Destruction Naming Patterns

### Problem

Banjo uses **6 different patterns** for resource lifecycle management across different modules:

| Pattern | Modules | Example |
|---------|---------|---------|
| `create/destroy` | bitmap, shader | `bj_create_bitmap()` / `bj_destroy_bitmap()` |
| `bind/unbind` | window | `bj_bind_window()` / `bj_unbind_window()` |
| `open/close` | audio, stream | `bj_open_audio_device()` / `bj_close_audio_device()` |
| `allocate/free` | bitmap (internal) | `bj_allocate_bitmap()` / `bj_free()` |
| `init/reset` | bitmap (internal) | `bj_init_bitmap()` / `bj_reset_bitmap()` |
| Constructor variants | Multiple | `bj_create_bitmap_from_file()` |

### Examples from Code

**Pattern 1: create/destroy** (`inc/banjo/bitmap.h`)
```c
bj_bitmap* bj_create_bitmap(
    size_t width,
    size_t height,
    bj_pixel_mode mode,
    size_t stride
);

void bj_destroy_bitmap(bj_bitmap* p_bitmap);
```

**Pattern 2: bind/unbind** (`inc/banjo/window.h`)
```c
bj_window* bj_bind_window(
    const char* p_title,
    uint16_t x,
    uint16_t y,
    uint16_t width,
    uint16_t height,
    uint8_t flags
);

void bj_unbind_window(bj_window* p_window);
```

**Pattern 3: open/close** (`inc/banjo/audio.h`)
```c
bj_audio_device* bj_open_audio_device(
    const bj_audio_properties* p_properties,
    bj_audio_callback_t p_callback,
    void* p_callback_user_data,
    bj_error** p_error
);

void bj_close_audio_device(bj_audio_device* p_device);
```

**Pattern 4: open/close** (`inc/banjo/stream.h`)
```c
bj_stream* bj_open_stream_file(
    const char* p_path,
    bj_error** p_error
);

void bj_close_stream(bj_stream* p_stream);
```

### Impact

- **Discoverability**: Users must remember different verbs for different types
- **Autocomplete**: IDE suggestions show `bj_create_*`, `bj_bind_*`, `bj_open_*` separately
- **Mental Load**: No semantic consistency to guide naming choices
- **Documentation**: Each pattern needs separate explanation

### Recommendation

**Standardize on `create/destroy` for all resource types.**

#### Rationale

1. **Clarity**: "create" clearly indicates memory allocation and initialization
2. **Universality**: Works for all resource types (windows, devices, streams, objects)
3. **Familiarity**: Matches common C++ RAII patterns and other C libraries
4. **Existing Usage**: Already used in majority of Banjo modules

#### Proposed Changes

| Current | Recommended |
|---------|-------------|
| `bj_bind_window()` | `bj_create_window()` |
| `bj_unbind_window()` | `bj_destroy_window()` |
| `bj_open_audio_device()` | `bj_create_audio_device()` |
| `bj_close_audio_device()` | `bj_destroy_audio_device()` |
| `bj_open_stream_file()` | `bj_create_stream_from_file()` |
| `bj_close_stream()` | `bj_destroy_stream()` |

#### Migration Strategy

1. Add new `create/destroy` functions alongside existing ones
2. Mark old functions as deprecated with `BJ_DEPRECATED` macro
3. Update all examples to use new functions
4. Remove deprecated functions in next major version

---

## 2. Error Handling Inconsistencies

### Problem

Banjo uses **4 different error handling patterns**:

| Pattern | Example | Modules |
|---------|---------|---------|
| `bj_error**` parameter | `bj_create_bitmap_from_file(path, &error)` | bitmap, stream, window |
| No error reporting | `bj_create_bitmap(w, h, mode, stride)` | bitmap, shader |
| Return code | `bj_get_window_size()` returns int | window |
| Internal `bj_check` macros | `bj_check_or_0(condition)` | All implementations |

### Examples from Code

**Pattern 1: Error pointer** (`inc/banjo/bitmap.h`)
```c
bj_bitmap* bj_create_bitmap_from_file(
    const char* p_path,
    bj_error** p_error  // Optional error output
);
```

**Pattern 2: No error reporting** (`inc/banjo/bitmap.h`)
```c
bj_bitmap* bj_create_bitmap(
    size_t width,
    size_t height,
    bj_pixel_mode mode,
    size_t stride
    // No error parameter - returns NULL on failure
);
```

**Pattern 3: Return code** (`inc/banjo/window.h`)
```c
int bj_get_window_size(
    const bj_window* p_window,
    int* width,
    int* height
);
// Returns 1 on success, 0 on error
// But no error details available
```

**Pattern 4: Internal checks** (`src/check.h`)
```c
#define bj_check_or_0(cond) if(!(cond)) {bj_check_err_msg(cond); return 0;}
```

### Impact

- **User Experience**: No consistent way to handle errors
- **Debugging**: Some failures provide no diagnostic information
- **Reliability**: Silent failures hard to detect and fix
- **Documentation**: Each pattern needs different usage instructions

### Current Error API

```c
// From inc/banjo/error.h
typedef struct bj_error_t bj_error;

void bj_forward_error(bj_error* p_from, bj_error** p_to);
const char* bj_error_message(const bj_error* p_error);
int bj_error_code(const bj_error* p_error);
void bj_destroy_error(bj_error* p_error);
```

### Recommendation

**Standardize on optional `bj_error**` parameter for all fallible operations.**

#### Proposed Convention

```c
// Pattern for functions that return pointers:
bj_type* bj_create_type(
    /* parameters */,
    bj_error** p_error  // ALWAYS LAST, nullable
);

// Pattern for functions that return status:
bj_bool bj_operation(
    /* parameters */,
    bj_error** p_error  // ALWAYS LAST, nullable
);

// Pattern for functions that cannot fail:
void bj_operation(
    /* parameters */
    // No error parameter
);
```

#### Usage Examples

```c
// User can ignore errors
bj_bitmap* bmp = bj_create_bitmap_from_file("test.bmp", NULL);
if (!bmp) {
    // Handle failure without details
}

// User can get error details
bj_error* error = NULL;
bj_bitmap* bmp = bj_create_bitmap_from_file("test.bmp", &error);
if (error) {
    bj_err("Failed to load bitmap: %s", bj_error_message(error));
    bj_destroy_error(error);
}
```

#### Specific Changes

| Current | Recommended |
|---------|-------------|
| `bj_get_window_size()` returns int | Return `bj_bool`, add `bj_error**` param |
| `bj_create_bitmap()` no error param | Add optional `bj_error**` param |
| Internal `bj_check` returns early | Set error details before returning |

---

## 3. Parameter Ordering

### Problem

Error parameters appear in **inconsistent positions**:

- Sometimes last (most common)
- Sometimes second-to-last
- Sometimes in middle of parameter list

### Examples from Code

**Error parameter LAST** (`inc/banjo/bitmap.h`)
```c
bj_bitmap* bj_create_bitmap_from_file(
    const char* p_path,
    bj_error** p_error  // LAST
);
```

**Error parameter in MIDDLE** (`inc/banjo/window.h`)
```c
bj_bitmap* bj_get_window_framebuffer(
    bj_window* p_window,
    bj_error** p_error  // Not last - but nothing follows
);
```

### Impact

- **Muscle Memory**: Users can't rely on consistent parameter order
- **Refactoring**: Adding parameters requires careful placement
- **Readability**: No predictable pattern to follow

### Recommendation

**Always place error parameter last in all functions.**

#### Rationale

1. **Convention**: Matches standard C patterns (e.g., POSIX)
2. **Optionality**: Last position emphasizes it's optional
3. **Extensibility**: Easy to add parameters without breaking error position
4. **Consistency**: One rule, no exceptions

#### Standard Order

```c
bj_type* bj_function(
    primary_input_params,      // 1. Main inputs
    output_params,              // 2. Output pointers (if any)
    config_params,              // 3. Optional configuration
    callback_params,            // 4. Callbacks + user data
    bj_error** p_error          // 5. ALWAYS LAST
);
```

#### Examples

```c
// Audio device creation - complex example
bj_audio_device* bj_create_audio_device(
    const bj_audio_properties* p_properties,  // Config
    bj_audio_callback_t p_callback,           // Callback
    void* p_callback_user_data,               // User data
    bj_error** p_error                        // LAST
);

// Window size query - simpler example
bj_bool bj_get_window_size(
    const bj_window* p_window,  // Input
    int* p_width,               // Output
    int* p_height,              // Output
    bj_error** p_error          // LAST
);
```

---

## 4. Return Type Inconsistencies

### Problem

Similar operations return **different types**:

| Operation | Return Type | Example |
|-----------|-------------|---------|
| Size queries | `size_t` | `bj_bitmap_width()` |
| Size queries | `int` via out-param | `bj_get_window_size()` |
| Property queries | Enum | `bj_bitmap_mode()` returns `int` |
| Success/failure | `int` (0/1) | `bj_get_window_size()` |
| Success/failure | `bj_bool` | Should be used |

### Examples from Code

**Returning `size_t`** (`inc/banjo/bitmap.h`)
```c
size_t bj_bitmap_width(const bj_bitmap* p_bitmap);
size_t bj_bitmap_height(const bj_bitmap* p_bitmap);
size_t bj_bitmap_stride(bj_bitmap* p_bitmap);
```

**Returning via out-params** (`inc/banjo/window.h`)
```c
int bj_get_window_size(
    const bj_window* p_window,
    int* width,   // Output as int, not size_t
    int* height   // Output as int, not size_t
);
```

**Returning int for enum** (`inc/banjo/bitmap.h`)
```c
int bj_bitmap_mode(bj_bitmap* p_bitmap);
// Should return bj_pixel_mode, not int
```

### Impact

- **Type Safety**: Casting required, potential overflow bugs
- **Clarity**: Return type doesn't indicate what's being returned
- **Portability**: Mixing `int` and `size_t` causes warnings

### Recommendation

**Use appropriate types for return values.**

#### Type Selection Guidelines

| Return Value | Type | Rationale |
|--------------|------|-----------|
| Size/count/dimension | `size_t` | Standard for sizes, can't be negative |
| Success/failure | `bj_bool` | Clear intent, type-safe |
| Enumeration | Actual enum type | Type safety, self-documenting |
| Pointer | Actual type | Never `void*` unless generic container |

#### Proposed Changes

```c
// CURRENT - returns int
int bj_bitmap_mode(bj_bitmap* p_bitmap);

// RECOMMENDED - returns enum
bj_pixel_mode bj_bitmap_mode(const bj_bitmap* p_bitmap);


// CURRENT - outputs int
int bj_get_window_size(
    const bj_window* p_window,
    int* width,
    int* height
);

// RECOMMENDED - consistent with bitmap API
size_t bj_window_width(const bj_window* p_window);
size_t bj_window_height(const bj_window* p_window);

// OR if combined query needed:
bj_bool bj_get_window_size(
    const bj_window* p_window,
    size_t* p_width,
    size_t* p_height,
    bj_error** p_error
);
```

---

## 5. Getter Naming Conventions

### Problem

Banjo uses **3 different patterns** for property getters:

| Pattern | Example | Modules |
|---------|---------|---------|
| `bj_<type>_<property>` | `bj_bitmap_width()` | bitmap |
| `bj_get_<property>` | `bj_get_key()` | window |
| `bj_get_<type>_<property>` | `bj_get_window_size()` | window |

### Examples from Code

**Pattern 1: Direct naming** (`inc/banjo/bitmap.h`)
```c
size_t bj_bitmap_width(const bj_bitmap* p_bitmap);
size_t bj_bitmap_height(const bj_bitmap* p_bitmap);
int bj_bitmap_mode(bj_bitmap* p_bitmap);
size_t bj_bitmap_stride(bj_bitmap* p_bitmap);
void* bj_bitmap_pixels(bj_bitmap* p_bitmap);
```

**Pattern 2: get_ prefix** (`inc/banjo/window.h`)
```c
int bj_get_key(const bj_window* p_window, int key);
uint8_t bj_get_window_flags(bj_window* p_window, uint8_t flags);
```

**Pattern 3: get_type_ prefix** (`inc/banjo/window.h`)
```c
bj_bitmap* bj_get_window_framebuffer(
    bj_window* p_window,
    bj_error** p_error
);

int bj_get_window_size(
    const bj_window* p_window,
    int* width,
    int* height
);
```

**Mixed within same module** (`inc/banjo/stream.h`)
```c
size_t bj_read_stream(bj_stream* p_stream, void* p_dest, size_t count);
size_t bj_get_stream_length(bj_stream* p_stream);
//     ^^^ Why "get_" here but not in bj_read_stream?
```

### Impact

- **Discoverability**: Users can't predict function names
- **Autocomplete**: IDE shows scattered related functions
- **Consistency**: No clear rule when to use `get_` prefix

### Recommendation

**Use `bj_<type>_<property>` for simple getters, reserve `get_` for complex operations.**

#### Decision Tree

```
Is it a simple property access?
├─ YES → Use bj_<type>_<property>
│        Example: bj_bitmap_width()
│
└─ NO → Does it require computation or I/O?
   ├─ YES → Use bj_get_<type>_<property> or bj_<type>_get_<property>
   │        Example: bj_get_window_framebuffer() (may allocate)
   │
   └─ NO → Does it query state rather than a property?
      └─ Use bj_<type>_<query_verb>
             Example: bj_window_should_close()
```

#### Examples

```c
// SIMPLE PROPERTIES - No "get_"
size_t bj_bitmap_width(const bj_bitmap* p_bitmap);
size_t bj_bitmap_height(const bj_bitmap* p_bitmap);
bj_pixel_mode bj_bitmap_mode(const bj_bitmap* p_bitmap);
void* bj_bitmap_pixels(bj_bitmap* p_bitmap);

size_t bj_window_width(const bj_window* p_window);
size_t bj_window_height(const bj_window* p_window);

// COMPLEX OPERATIONS - Use "get_"
bj_bitmap* bj_get_window_framebuffer(
    bj_window* p_window,
    bj_error** p_error
);  // May allocate, may fail

// STATE QUERIES - Descriptive verb
bj_bool bj_window_should_close(const bj_window* p_window);
bj_bool bj_key_is_pressed(const bj_window* p_window, int key);
```

#### Specific Changes

| Current | Recommended | Reason |
|---------|-------------|---------|
| `bj_get_window_size()` | `bj_window_width()` / `bj_window_height()` | Simple property |
| `bj_get_key()` | `bj_key_is_pressed()` | State query, not property |
| `bj_get_window_flags()` | `bj_window_flags()` | Simple property |
| `bj_get_stream_length()` | `bj_stream_length()` | Simple property |
| `bj_get_window_framebuffer()` | Keep as-is | Complex operation (may allocate) |

---

## 6. Boolean Query Functions

### Problem

Functions that return boolean states use **inconsistent naming**:

| Function | Pattern | Returns |
|----------|---------|---------|
| `bj_should_close_window()` | Verb first | `bj_bool` |
| `bj_get_key()` | Generic "get" | `int` (0 or 1) |

### Examples from Code

**Boolean query with verb** (`inc/banjo/window.h`)
```c
bj_bool bj_should_close_window(bj_window* p_window);
// GOOD: Clear boolean intent
```

**State query without boolean type** (`inc/banjo/window.h`)
```c
int bj_get_key(const bj_window* p_window, int key);
// Returns BJ_PRESS or BJ_RELEASE (defined as ints)
// UNCLEAR: Not obviously a state query
```

### Recommendation

**Use `is_` or `has_` prefix for boolean properties, action verbs for state queries.**

#### Patterns

```c
// BOOLEAN PROPERTIES - is_/has_
bj_bool bj_<type>_is_<state>(const bj_type* p_obj);
bj_bool bj_<type>_has_<feature>(const bj_type* p_obj);

// STATE QUERIES - should_/can_/will_
bj_bool bj_<type>_should_<action>(const bj_type* p_obj);
bj_bool bj_<type>_can_<action>(const bj_type* p_obj);

// PREDICATE TESTS - descriptive verb
bj_bool bj_<predicate>_<type>(const bj_type* p_obj, params);
```

#### Examples

```c
// CURRENT
bj_bool bj_should_close_window(bj_window* p_window);
int bj_get_key(const bj_window* p_window, int key);

// RECOMMENDED
bj_bool bj_window_should_close(const bj_window* p_window);
bj_bool bj_key_is_pressed(const bj_window* p_window, int key);
bj_bool bj_key_is_released(const bj_window* p_window, int key);

// Additional examples
bj_bool bj_window_is_minimized(const bj_window* p_window);
bj_bool bj_window_has_focus(const bj_window* p_window);
bj_bool bj_audio_device_can_pause(const bj_audio_device* p_device);
bj_bool bj_stream_has_data(const bj_stream* p_stream);
```

---

## 7. Setter Naming Patterns

### Problem

Setter functions use **inconsistent patterns**:

- Some use `set_<type>_<property>`
- Some use `<type>_set_<property>`
- Some operations modify without "set" prefix

### Examples from Code

**set_ prefix before type** (`inc/banjo/window.h`)
```c
void bj_set_window_should_close(bj_window* p_window);
```

**set_ prefix after type** (`inc/banjo/bitmap.h`)
```c
void bj_set_bitmap_clear_color(
    bj_bitmap* p_bitmap,
    uint32_t clear_color
);
```

**Callbacks** (`inc/banjo/event.h`)
```c
bj_cursor_callback_fn_t bj_set_cursor_callback(
    bj_cursor_callback_fn_t p_callback,
    void* p_user_data
);
```

### Recommendation

**Use `bj_<type>_set_<property>` for all setters.**

#### Rationale

1. **Grouping**: All window functions start with `bj_window_`
2. **Autocomplete**: Type prefix shows all related functions together
3. **Consistency**: Matches getter pattern `bj_<type>_<property>()`

#### Pattern

```c
// GETTER
<type> bj_<type>_<property>(const bj_object* p_obj);

// SETTER
void bj_<type>_set_<property>(bj_object* p_obj, <type> value);
```

#### Examples

```c
// CURRENT
void bj_set_window_should_close(bj_window* p_window);
void bj_set_bitmap_clear_color(bj_bitmap* p_bitmap, uint32_t color);

// RECOMMENDED
void bj_window_set_should_close(bj_window* p_window);
void bj_bitmap_set_clear_color(bj_bitmap* p_bitmap, uint32_t color);

// More examples
void bj_window_set_title(bj_window* p_window, const char* title);
void bj_window_set_size(bj_window* p_window, size_t width, size_t height);
void bj_audio_device_set_volume(bj_audio_device* p_device, float volume);
```

---

## 8. Prefix Overuse

### Problem

Some types repeat information unnecessarily:

- `bj_pixel_mode` - mode of what else but pixels?
- `bj_window_flag` - obvious it's for windows
- `bj_audio_properties` - audio is redundant

While others don't include context:
- `bj_stream` - stream of what?
- `bj_shader` - shader for what?

### Examples from Code

**Redundant prefix** (`inc/banjo/pixel.h`, `inc/banjo/bitmap.h`)
```c
typedef enum {
    BJ_PIXEL_MODE_1BPP,
    BJ_PIXEL_MODE_4BPP,
    // ...
} bj_pixel_mode;

// Used as:
bj_bitmap* bj_create_bitmap(
    size_t width,
    size_t height,
    bj_pixel_mode mode,  // "pixel" is obvious from "mode"
    size_t stride
);
```

**Redundant prefix** (`inc/banjo/window.h`)
```c
typedef enum bj_window_flag_t {
    BJ_WINDOW_FLAG_NONE,
    BJ_WINDOW_FLAG_CLOSE,
    BJ_WINDOW_FLAG_KEY_REPEAT,
} bj_window_flag;

// Used as:
bj_window* bj_bind_window(
    const char* p_title,
    uint16_t x,
    uint16_t y,
    uint16_t width,
    uint16_t height,
    uint8_t flags  // Could just be bj_window_flags
);
```

### Recommendation

**Balance specificity with brevity. Context often makes prefixes redundant.**

#### Guidelines

1. **When type is used in scoped context** (as parameter to type-specific function):
   - Prefer short name: `mode`, `flags`, `properties`
   - Type safety provides context

2. **When type is used in generic context** (global scope, mixed usage):
   - Include context: `bj_audio_properties`, `bj_stream_mode`

3. **Enum values always include full context**:
   - Always: `BJ_WINDOW_FLAG_CLOSE` not `BJ_FLAG_CLOSE`
   - Prevents global namespace pollution

#### Proposed Pattern

```c
// TYPES - Context when needed
typedef struct bj_audio_properties_t bj_audio_properties;  // Good
typedef enum bj_pixel_mode_t bj_pixel_mode;  // Consider bj_format
typedef enum bj_window_flags_t bj_window_flags;  // Consider bj_window_flags

// ENUM VALUES - Always fully qualified
typedef enum {
    BJ_WINDOW_FLAG_NONE      = 0x00,
    BJ_WINDOW_FLAG_CLOSE     = 0x01,
    BJ_WINDOW_FLAG_KEY_REPEAT = 0x02,
} bj_window_flags;

// USAGE - Context makes it clear
bj_window* bj_create_window(..., bj_window_flags flags);
bj_bitmap* bj_create_bitmap(..., bj_pixel_mode mode);
```

---

## 9. Mixed Styles Within Modules

### Problem

Some modules mix different naming patterns internally.

### Example: Stream Module

**From `inc/banjo/stream.h`:**

```c
// Action verbs - consistent
bj_stream* bj_open_stream_file(const char* p_path, bj_error** p_error);
void bj_close_stream(bj_stream* p_stream);
size_t bj_read_stream(bj_stream* p_stream, void* p_dest, size_t count);

// Property getter - INCONSISTENT "get_" prefix
size_t bj_get_stream_length(bj_stream* p_stream);
//     ^^^ Why "get_" here but not in read_stream?
```

**Better consistency:**
```c
size_t bj_stream_length(bj_stream* p_stream);  // Matches bj_bitmap_width pattern
```

### Example: Window Module

**From `inc/banjo/window.h`:**

```c
// Mix of patterns
bj_bool bj_should_close_window(bj_window* p_window);  // Verb-first
void bj_set_window_should_close(bj_window* p_window);  // Set-first
uint8_t bj_get_window_flags(...);  // Get-first
```

**Better consistency:**
```c
bj_bool bj_window_should_close(const bj_window* p_window);
void bj_window_set_should_close(bj_window* p_window);
bj_window_flags bj_window_flags(const bj_window* p_window);
```

### Recommendation

**Audit each module for internal consistency before worrying about cross-module patterns.**

---

## 10. Memory Management Conventions

### Problem

Memory management functions don't follow the same conventions as regular API:

| Function | Pattern | Module |
|----------|---------|--------|
| `bj_malloc()` | C standard naming | memory |
| `bj_free()` | C standard naming | memory |
| `bj_create_bitmap()` | Banjo convention | bitmap |
| `bj_destroy_bitmap()` | Banjo convention | bitmap |

### Examples from Code

**Memory primitives** (`inc/banjo/memory.h`)
```c
void* bj_malloc(size_t size);
void bj_free(void* p_memory);
void* bj_realloc(void* p_memory, size_t new_size);
```

**Object lifecycle** (`inc/banjo/bitmap.h`)
```c
bj_bitmap* bj_create_bitmap(...);
void bj_destroy_bitmap(bj_bitmap* p_bitmap);
```

### Analysis

This is actually **intentional and good**:

- `bj_malloc/free` mirror C standard library (direct replacements)
- `bj_create/destroy` indicate higher-level object management
- Clear semantic difference between raw memory and object lifecycle

### Recommendation

**Keep this distinction. Document it explicitly.**

#### Documentation Recommendation

Add to public headers:

```c
/// \defgroup memory Memory Management
///
/// Banjo provides two levels of memory management:
///
/// 1. **Low-level allocation**: bj_malloc(), bj_free(), bj_realloc()
///    - Direct memory allocation (like C stdlib)
///    - Redirectable via bj_set_memory_defaults()
///    - Use for custom allocators and internal implementation
///
/// 2. **Object lifecycle**: bj_create_*(), bj_destroy_*()
///    - High-level object construction/destruction
///    - May involve multiple allocations
///    - Handles initialization and cleanup
///    - Always use for Banjo objects
///
/// \{
```

---

## Implementation Roadmap

### Phase 1: Documentation & Audit (Non-Breaking)

**Goal**: Establish conventions and identify all violations without changing code.

#### Tasks

1. **Create API Style Guide** (this document)
2. **Audit all public headers** - Generate spreadsheet of violations
3. **Document current state** - Add deprecation notices to inconsistent functions
4. **Update CONTRIBUTING.md** - Enforce conventions for new code

**Timeline**: 1-2 weeks
**Breaking Changes**: None

---

### Phase 2: Deprecation Warnings (Non-Breaking)

**Goal**: Mark old patterns as deprecated while adding new consistent alternatives.

#### Tasks

1. **Add `BJ_DEPRECATED` macro** to `inc/banjo/api.h`:
```c
#if defined(__GNUC__) || defined(__clang__)
    #define BJ_DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER)
    #define BJ_DEPRECATED __declspec(deprecated)
#else
    #define BJ_DEPRECATED
#endif
```

2. **Add new functions alongside old ones**:
```c
// Old function - deprecated
BJ_DEPRECATED
bj_window* bj_bind_window(const char* p_title, ...);

// New function - recommended
bj_window* bj_create_window(const char* p_title, ...);
```

3. **Update all examples** to use new functions
4. **Update documentation** to show new patterns

**Timeline**: 2-3 weeks
**Breaking Changes**: None (old functions still work, emit warnings)

---

### Phase 3: Migration (Breaking Changes)

**Goal**: Remove deprecated functions in next major version.

#### Tasks

1. **Version 0.2.0**: Introduce new functions, deprecate old
2. **Version 0.9.0**: Remove deprecated functions (last beta before 1.0)
3. **Version 1.0.0**: Stable API with consistent conventions

#### Migration Guide Example

```c
// banjo/migration-0.2.md

## Migrating from 0.1.x to 0.2.x

### Window API Changes

// Before (0.1.x)
bj_window* window = bj_bind_window("Title", 0, 0, 800, 600, 0);
// ...
bj_unbind_window(window);

// After (0.2.x)
bj_window* window = bj_create_window("Title", 0, 0, 800, 600, 0, NULL);
// ...
bj_destroy_window(window);

### Audio API Changes

// Before
bj_audio_device* device = bj_open_audio_device(&props, callback, NULL, &err);
bj_close_audio_device(device);

// After
bj_audio_device* device = bj_create_audio_device(&props, callback, NULL, &err);
bj_destroy_audio_device(device);
```

**Timeline**: Version-dependent (spread across releases)
**Breaking Changes**: Yes (in major version only)

---

### Phase 4: Enforcement (Ongoing)

**Goal**: Prevent regressions.

#### Tasks

1. **Add API linter** - Script to check naming conventions
2. **CI integration** - Fail PRs that violate conventions
3. **Documentation generator** - Auto-check for missing `bj_error**` params
4. **Review checklist** - Mandate convention adherence

**Example Linter Checks**:
```python
# pseudo-code
def check_creation_functions(header):
    for func in header.functions:
        if func.returns_pointer and "create" not in func.name:
            warn(f"{func.name} creates object but doesn't use 'create' verb")

        if func.name.startswith("bj_create_"):
            if func.parameters[-1].type != "bj_error**":
                warn(f"{func.name} missing error parameter")
```

---

## Appendix: Style Guide

### Quick Reference Table

| Element | Convention | Example |
|---------|------------|---------|
| **Resource Creation** | `bj_create_<type>` | `bj_create_window()` |
| **Resource Destruction** | `bj_destroy_<type>` | `bj_destroy_window()` |
| **Simple Getter** | `bj_<type>_<property>` | `bj_window_width()` |
| **Complex Getter** | `bj_get_<type>_<property>` | `bj_get_window_framebuffer()` |
| **Setter** | `bj_<type>_set_<property>` | `bj_window_set_title()` |
| **Boolean Query** | `bj_<type>_is_<state>` | `bj_window_is_minimized()` |
| **State Query** | `bj_<type>_<verb>` | `bj_window_should_close()` |
| **Action** | `bj_<verb>_<type>` | `bj_update_window()` |
| **Error Parameter** | Always last, nullable | `bj_error** p_error` |
| **Return Types** | Sizes: `size_t`<br>Success: `bj_bool`<br>Enums: actual type | |

### Complete Function Signature Template

```c
/// \brief [One-line description]
///
/// [Detailed description]
///
/// \param p_<name> [Description]
/// \param [other params] [Descriptions]
/// \param p_error Optional error output (nullable)
///
/// \return [What is returned and when]
///
/// \par Memory Management
/// [Who owns returned memory]
///
/// \par Thread Safety
/// [Thread safety guarantees]
///
BANJO_EXPORT <return_type> bj_<type>_<operation>(
    const bj_<type>* p_object,        // Primary input (const if not modified)
    <param_type> parameter,            // Required parameters
    <out_type>* p_output,              // Output parameters (pointer)
    bj_callback_t p_callback,          // Callbacks
    void* p_user_data,                 // Callback user data
    bj_error** p_error                 // Error output (ALWAYS LAST)
);
```

### Const Correctness Rules

1. **Input pointers to objects**: `const bj_type*` if not modified
2. **Output pointers**: Never const
3. **String parameters**: `const char*`
4. **Error parameters**: `bj_error**` (not const - may be written)
5. **Return values**: `const` only for string literals

### Examples - Before and After

#### Example 1: Window Creation

```c
// BEFORE (0.1.x)
bj_window* bj_bind_window(
    const char* p_title,
    uint16_t x,
    uint16_t y,
    uint16_t width,
    uint16_t height,
    uint8_t flags
);

void bj_unbind_window(bj_window* p_window);

int bj_get_window_size(
    const bj_window* p_window,
    int* width,
    int* height
);

// AFTER (0.2.x)
bj_window* bj_create_window(
    const char* p_title,
    uint16_t x,
    uint16_t y,
    uint16_t width,
    uint16_t height,
    bj_window_flags flags,
    bj_error** p_error  // Added
);

void bj_destroy_window(bj_window* p_window);

size_t bj_window_width(const bj_window* p_window);
size_t bj_window_height(const bj_window* p_window);
```

#### Example 2: Audio Device

```c
// BEFORE
bj_audio_device* bj_open_audio_device(
    const bj_audio_properties* p_properties,
    bj_audio_callback_t p_callback,
    void* p_callback_user_data,
    bj_error** p_error
);

void bj_close_audio_device(bj_audio_device* p_device);

// AFTER
bj_audio_device* bj_create_audio_device(
    const bj_audio_properties* p_properties,
    bj_audio_callback_t p_callback,
    void* p_callback_user_data,
    bj_error** p_error  // Still last
);

void bj_destroy_audio_device(bj_audio_device* p_device);
```

#### Example 3: Stream Operations

```c
// BEFORE
bj_stream* bj_open_stream_file(
    const char* p_path,
    bj_error** p_error
);

void bj_close_stream(bj_stream* p_stream);

size_t bj_get_stream_length(bj_stream* p_stream);

// AFTER
bj_stream* bj_create_stream_from_file(
    const char* p_path,
    bj_error** p_error
);

void bj_destroy_stream(bj_stream* p_stream);

size_t bj_stream_length(const bj_stream* p_stream);
```

---

## Conclusion

The Banjo API demonstrates solid fundamentals - excellent documentation, clear philosophy, and zero dependencies. However, inconsistent naming conventions impact developer experience and long-term maintainability.

**Key Recommendations**:

1. **Standardize creation/destruction**: Use `create/destroy` everywhere
2. **Consistent error handling**: Optional `bj_error**` always last
3. **Predictable getters**: `bj_<type>_<property>` for simple access
4. **Type-safe returns**: Use appropriate types (`size_t`, `bj_bool`, enums)
5. **Document conventions**: Make style guide part of official docs

**Implementation Strategy**:

- Phase 1: Document and audit (non-breaking)
- Phase 2: Deprecation warnings (non-breaking)
- Phase 3: Remove deprecated (breaking, major version)
- Phase 4: Enforce going forward (CI/linting)

These changes will significantly improve Banjo's usability without compromising its core principles. The earlier they're implemented (before 1.0), the less painful the migration will be for users.

---

**Document Status**: Draft for review
**Next Steps**: Review with maintainers, prioritize changes, create implementation tickets
