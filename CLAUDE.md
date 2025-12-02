# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Banjo is a lightweight, self-contained C99 framework for game development and multimedia applications. It provides windowing, events, bitmap manipulation, audio, logging, custom allocators, and math utilities (vec2, vec3, vec4, mat3, mat4, quat).

**Core Principles:**
- Simple: If you know C, you can use it
- Minimal: No external dependencies, even for tests
- Self-documented: APIs are explained in the headers
- Fast: Must compile quickly

## Build Commands

### Building with CMake

Standard CMake workflow:
```bash
# Configure (from project root)
cmake -B build

# Build
cmake --build build

# Run tests
cd build && ctest
```

### Build Options

All build options are CMake options starting with `BANJO_` prefix. These CMake options set corresponding `BJ_` preprocessor definitions in the code.

**Backend Options** (enable platform/API support):

These backends are **automatically detected and enabled by default** if their dependencies are available:
- `BANJO_CONFIG_WIN32_BACKEND` - Win32 window support (auto-enabled on Windows)
- `BANJO_CONFIG_X11_BACKEND` - X11 window support (auto-enabled if Xlib found)
- `BANJO_CONFIG_COCOA_BACKEND` - macOS/Cocoa support (auto-enabled on macOS)
- `BANJO_CONFIG_MME_BACKEND` - Windows Multimedia Extensions audio (auto-enabled on Windows)
- `BANJO_CONFIG_ALSA_BACKEND` - ALSA audio support (auto-enabled if ALSA found)
- `BANJO_CONFIG_EMSCRIPTEN_BACKEND` - Emscripten/WebAssembly support (auto-enabled with Emscripten)

You can explicitly disable a backend even if it's available:
```bash
cmake -B build -DBANJO_CONFIG_X11_BACKEND=OFF
```

Or force enable a backend (if dependencies are available):
```bash
cmake -B build -DBANJO_CONFIG_ALSA_BACKEND=ON
```

**Configuration Options** (control behavior):

These are OFF by default and must be explicitly enabled:
- `BANJO_CONFIG_LOG_COLOR` - Enable colored log output
- `BANJO_CONFIG_CHECKS_LOG` - Log failing checks
- `BANJO_CONFIG_CHECKS_ABORT` - Abort on failing checks
- `BANJO_CONFIG_PEDANTIC` - Enable extra runtime checks (prioritizes safety over performance)
- `BANJO_CONFIG_FASTMATH` - Enable fast-math optimizations

Example with configuration options:
```bash
cmake -B build -DBANJO_CONFIG_LOG_COLOR=ON -DBANJO_CONFIG_PEDANTIC=ON
```

### Building Without CMake

Compile all .c files in src/ with inc/ as include path:

Static library:
```bash
gcc -c src/*.c -I inc/ -D BANJO_STATIC && ar rcs libbanjo.a *.o
```

Shared library:
```bash
gcc -shared -fPIC -D BANJO_EXPORTS -o libbanjo.so src/*.c -I inc/
```

### Running Tests

```bash
# All tests
cd build && ctest

# Specific test
cd build && ctest -R test_name

# Run test binary directly
./build/test/test_name
```

Test categories:
- `tdd_*.c` - TDD tests
- `unit_*.c` - Unit tests
- `valid_*.c` - Validation tests

### Building Examples

Examples are automatically built by default. Disable with:
```bash
cmake -B build -DBANJO_BUILD_EXAMPLES=OFF
```

Each .c file in examples/ becomes a separate executable.

## Architecture

### Library Structure

- `inc/banjo/` - Public API headers (what users include)
- `src/` - Implementation files (.c) and internal headers (.h)
- `test/` - Test files
- `examples/` - Example applications
- `docs/` - Documentation source files

### Static vs Dynamic Library

Controlled by CMake's `BUILD_SHARED_LIBS`:
- **Static** (default): `BANJO_STATIC` is defined
- **Dynamic**: `BANJO_EXPORTS` is defined when building, not when using

### Platform Abstraction

Platform-specific code is conditionally compiled based on backend configuration flags:
- Video: `video_x11.c`, `video_win32.c`, `video_cocoa.m`, `video_emscripten.c`, `novideo.c`
- Audio: `audio_alsa.c`, `audio_mme.c`, `audio_emscripten.c`, `noaudio.c`
- Time: `time_unix.c`, `time_win32.c`
- System: `system_unix.c`, `system_win32.c`

The `src/config.h` file provides the `BJ_HAS_CONFIG()` macro to check enabled options at compile time (both backends and configuration options).

### Main Entry Point System

Banjo provides automatic main function handling for certain platforms:
- Define `BJ_NO_AUTOMAIN` to provide your own main()
- Define `BJ_AUTOMAIN_CALLBACKS` to use the callback system (begin/iterate/end pattern)
- Otherwise, use standard `int main(int argc, char* argv[])` renamed to `bj_main()`

See `inc/banjo/main.h` for details on the callback system.

### Key API Modules

Headers in `inc/banjo/`:
- `api.h` - Core definitions, platform detection, version macros
- `window.h`, `video.h`, `event.h` - Windowing and events
- `bitmap.h`, `draw.h`, `pixel.h` - Graphics and drawing
- `audio.h` - Audio playback
- `memory.h` - Custom allocators
- `log.h`, `error.h`, `assert.h` - Logging and error handling
- `time.h`, `system.h` - Time and system utilities
- `vec.h`, `mat.h`, `quat.h`, `math.h` - Math utilities
- `physics.h`, `physics_2d.h`, `geometry_2d.h` - Physics and geometry
- `random.h` - Random number generation
- `shader.h` - Shader support
- `stream.h`, `string.h` - Data streams and string utilities

### Internal Implementation Details

Internal headers in `src/` (not part of public API):
- `*_t.h` - Internal type definitions (e.g., `bitmap_t.h`, `audio_t.h`)
- `check.h` - Internal assertion/check macros
- `config.h` - Build configuration detection macros

### Version Information

Version is defined in `inc/banjo/api.h`:
- `BJ_VERSION_MAJOR_NUMBER`, `BJ_VERSION_MINOR_NUMBER`, `BJ_VERSION_PATCH_NUMBER`
- `BJ_MAKE_VERSION()` macro packs version as [major:10 | minor:10 | patch:12]
- Runtime build info via `bj_build_information()` function

### Test Infrastructure

Tests use a custom minimal test framework defined in `test/test.h`. No external test dependencies.

## Development Notes

### Compiler Support

Tested on:
- GCC 13.2.0+ / GNU libc 2.38+
- Microsoft Visual C++ 2022
- Apple Clang 16.0.0
- Emscripten

### Compiler Warnings

MSVC: `/W4 /D_CRT_SECURE_NO_WARNINGS`
Others: `-Wall -Wextra`

### Fast Math

When `BJ_CONFIG_FASTMATH` is enabled:
- MSVC uses `/fp:fast`
- GCC/Clang use `-ffast-math -ffp-contract=fast -fno-math-errno -fno-trapping-math`

### Assets

Examples reference assets via `BANJO_ASSETS_DIR` macro, set to `${CMAKE_CURRENT_SOURCE_DIR}/assets` by CMake.
