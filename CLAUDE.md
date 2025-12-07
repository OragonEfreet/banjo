# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Banjo is a lightweight, self-contained C99 framework for game/multimedia development with no external dependencies. It provides windowing, events, bitmap manipulation, audio, logging, custom allocators, and math utilities (vec2/3/4, mat3/4, quat, 2D physics).

**Core Principles:**
- Simple: Pure C99, readable APIs
- Minimal: Zero external dependencies (even for tests)
- Self-documented: Documentation in headers
- Fast: Quick compilation is a priority

## Build Commands

### CMake Build (Standard)
```bash
# Configure with auto-detected backends
cmake -B build

# Build
cmake --build build

# Run tests
ctest --test-dir build
```

### CMake Options
The build system auto-detects available backends (X11, ALSA, Win32, Cocoa, etc.) and enables them by default. Override with:

```bash
# Disable specific backend
cmake -B build -DBANJO_CONFIG_X11_BACKEND=OFF

# Enable configuration options
cmake -B build -DBANJO_CONFIG_PEDANTIC=ON -DBANJO_CONFIG_LOG_COLOR=ON

# Build as shared library instead of static
cmake -B build -DBUILD_SHARED_LIBS=ON
```

Common options:
- `BANJO_CONFIG_PEDANTIC=ON` - Enable extra runtime checks (slower but safer)
- `BANJO_CONFIG_LOG_COLOR=ON` - Colored log output
- `BANJO_CONFIG_CHECKS_ABORT=ON` - Abort on failed checks (useful for debugging)
- `BANJO_CONFIG_CHECKS_LOG=ON` - Log failed checks
- `BANJO_CONFIG_FASTMATH=ON` - Enable fast-math optimizations
- `BANJO_BUILD_EXAMPLES=OFF` - Skip building examples

### Manual Build (No CMake)
Banjo can be built without any build system:

```bash
# Static library on Unix
gcc -c src/*.c src/unix/*.c -I inc/ -I src/ -D BANJO_STATIC
ar rcs libbanjo.a *.o

# On Windows, use src/win32/*.c instead of src/unix/*.c
```

Backend-specific sources (src/x11/, src/cocoa/, src/alsa/, src/mme/, src/emscripten/) are only compiled when the corresponding backend is enabled via macros like `BJ_CONFIG_X11_BACKEND`.

### Testing
```bash
# Run all tests
ctest --test-dir build

# Run specific test category
ctest --test-dir build -R "^\[Unit\]"

# Available categories: TDD, Unit, Validation
```

Test files follow naming conventions:
- `test/tdd_*.c` - TDD tests
- `test/unit_*.c` and `test/obj_*.c` - Unit tests
- `test/valid_*.c` - Validation tests

### Examples
Examples are built automatically and located in `build/examples/`. Each `.c` file in `examples/` becomes a standalone executable. Run directly:

```bash
./build/examples/window
./build/examples/drawing_2d
```

## Architecture

### Backend System
Banjo uses a compile-time backend selection model. The active backends are determined at build time:

**Video Backends** (mutually compatible):
- Win32: `src/win32/video.c` (Windows native)
- X11: `src/x11/video.c` (Linux/Unix X11)
- Cocoa: `src/cocoa/video.m` (macOS native)
- Emscripten: `src/emscripten/video.c` (WebAssembly)
- Fallback: `src/novideo.c` (no-op implementation)

**Audio Backends** (mutually compatible):
- ALSA: `src/alsa/audio.c` (Linux)
- MME: `src/mme/audio.c` (Windows Multimedia Extensions)
- Emscripten: `src/emscripten/audio.c` (WebAssembly)
- Fallback: `src/noaudio.c` (no-op implementation)

Platform-specific utilities:
- `src/unix/system.c`, `src/unix/time.c` - Unix/Linux/macOS
- `src/win32/system.c`, `src/win32/time.c` - Windows

### Library Build Types
Banjo can be built as either static or shared:

- **Static** (default): `BUILD_SHARED_LIBS=OFF` or unset
  - Defines `BANJO_STATIC`
  - Library embedded in consumer binary
  - Simpler distribution, no runtime dependencies

- **Shared**: `BUILD_SHARED_LIBS=ON`
  - Defines `BANJO_EXPORTS` during build
  - Uses `BANJO_EXPORT` attributes (platform-specific)
  - Smaller executables, runtime loading

All public API functions are marked with `BANJO_EXPORT` (in inc/banjo/api.h).

### Source Organization
- `inc/banjo/*.h` - Public API headers (self-documented)
- `src/*.c` - Core implementation (platform-agnostic)
- `src/*_t.h` - Private internal headers (opaque types)
- `src/check.h` - Internal assertion/check macros
- `examples/*.c` - Standalone example programs
- `test/*.c` - Test suite (uses internal test.h framework, no external dependencies)
- `assets/` - Resources used by examples and tests

### Memory Management
Banjo provides custom allocator support. All allocations go through `bj_malloc`/`bj_free` (defined in inc/banjo/memory.h), which can be redirected via `bj_set_memory_defaults()`. This allows per-object or global custom allocators.

### Error Handling
Banjo uses a check-based error system (`src/check.h`):
- `bj_check(condition)` - Returns early if false
- `bj_check_or_return(condition, value)` - Returns value if false
- Behavior controlled by `BJ_CONFIG_CHECKS_LOG` and `BJ_CONFIG_CHECKS_ABORT`

Public API also provides `bj_error.h` for retrieving error codes/messages.

### Key Modules
- **Windowing**: `window.c` + backend video implementations
- **Events**: `event.c` - Input/window event handling
- **Bitmap**: `bitmap.c`, `bitmap_draw.c`, `bitmap_blit.c`, `bitmap_text.c` - Image manipulation and rendering
- **Audio**: `audio.c` + backend audio implementations
- **Math**: `vec.h`, `mat.h`, `quat.h`, `geometry_2d.c` - Vector/matrix/quaternion math
- **Physics**: `physics_*.c` - 2D kinematics, particles, rigid bodies
- **Logging**: `log.c` - Leveled logging system
- **Random**: `random.c`, `random_pcg32.c`, `random_distribution.c` - PCG32-based RNG
- **Time**: `time.c` + platform time implementations
- **Shaders**: `shader.c` - Software shader/pixel processing
- **Streams**: `stream.c` - File I/O abstraction

## Development Notes

### Adding New Features
1. Add public API to appropriate header in `inc/banjo/`
2. Implement in corresponding `src/*.c` file
3. Mark public functions with `BANJO_EXPORT`
4. Use `bj_check*` macros for parameter validation
5. Use `bj_malloc`/`bj_free` for allocations (never stdlib directly)
6. Add tests in `test/` following naming conventions
7. Update documentation in header (Doxygen format)

### Platform-Specific Code
- Use existing platform detection macros in `inc/banjo/api.h` (`BJ_OS_WINDOWS`, `BJ_OS_LINUX`, `BJ_OS_MACOS`, etc.)
- Implement platform differences in backend files (src/x11/, src/win32/, etc.)
- Keep core logic platform-agnostic

### Coding Style
- C99 standard strictly enforced
- No external dependencies (not even standard headers like math.h where avoidable)
- Prefix all public API with `bj_`
- Prefix all internal functions/types with `bj_` + module name
- Use typedef for structs: `typedef struct {...} bj_foo;`
- Document all public APIs in headers using Doxygen `///` comments

### Testing Strategy
Banjo has its own testing framework (no external test library). Tests are standalone executables that return 0 on success, non-zero on failure. Use macros from `test/test.h` (check the file for available assertions).

### Build System Quirks
- The `BANJO_ASSETS_DIR` macro is defined during build, pointing to project assets directory
- Examples and tests are automatically discovered via glob patterns in CMakeLists.txt
- Each backend has find_package() checks in CMakeLists.txt that auto-enable when dependencies found
