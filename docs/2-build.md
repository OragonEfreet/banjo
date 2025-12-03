# Building the API  {#build}

\brief How to build the documentation from source code

This document is about producing the Banjo API binaries.
If you want to know how to build an application *with* banjo, see \ref use.

You will need at least **a C99-compliant compiler**.

## Manual Build (No Build System) {#build_manual}

Banjo is designed to be easily built without any complex build system.
You only need to feed your compiler with the source files and the correct include paths.

### 1. Core Sources

Compile all \c .c files in \c src/ and \c src/unix/ (if on Unix) or \c src/win32/ (if on Windows).
**Do not** compile files in backend-specific subdirectories (\c src/x11/, \c src/cocoa/, etc.) unless you are enabling that specific backend.

Add \c inc/ and \c src/ to your include search path.

### 2. Output Type

- **Static Library**: Define \c BANJO_STATIC.
- **Shared Library**: Define \c BANJO_EXPORTS (do NOT define \c BANJO_STATIC).

### Example: Building a Static Library (GCC/Clang)

```bash
# Compile core sources (excluding backends for this example)
gcc -c src/*.c src/unix/*.c -I inc/ -I src/ -D BANJO_STATIC
# (Note: This example assumes a Unix-like system. On Windows, use src/win32/*.c instead.
#  We are not including src/x11/*.c or src/alsa/*.c here.)

# Create archive
ar rcs libbanjo.a *.o
```

## Build Options {#build_options}

You can customize the build by enabling backends or configuration options.
To enable an option manually, you simply need to define the corresponding **C Macro** and compile any required **Additional Sources**.

| Option Name                              | Description                                |
| :--------------------------------------- | :----------------------------------------- |
| \ref opt_win32 "Win32 Backend"           | Enable Win32 window support                |
| \ref opt_x11 "X11 Backend"               | Enable X11 window support                  |
| \ref opt_cocoa "Cocoa Backend"           | Enable Cocoa/macOS support                 |
| \ref opt_mme "MME Backend"               | Enable Windows Multimedia Extensions audio |
| \ref opt_alsa "ALSA Backend"             | Enable ALSA audio support                  |
| \ref opt_emscripten "Emscripten Backend" | Enable Emscripten/WebAssembly support      |
| \ref opt_log_color "Colored Logs"        | Enable support for colored log outputs     |
| \ref opt_checks_log "Log Checks"         | Failing checks are logged                  |
| \ref opt_checks_abort "Abort on Check"   | Failing checks call `abort()`              |
| \ref opt_pedantic "Pedantic Mode"        | Prioritize safety over performance         |
| \ref opt_fastmath "Fast Math"            | Enable fast-math optimizations             |

---

### Win32 Backend {#opt_win32}

This backend enables support for creating windows on the Windows platform.

**Additional source files:** \c src/win32/video.c

| Compiler    | Compiler Flags                       | Linker Flags                         |
|-------------|--------------------------------------|--------------------------------------|
| MSVC        | \c /D \c BJ_CONFIG_WIN32_BACKEND     | \c user32.lib \c gdi32.lib \c kernel32.lib |
| GCC/Clang   | \c -D \c BJ_CONFIG_WIN32_BACKEND     | \c -luser32 \c -lgdi32 \c -lkernel32 |

### X11 Backend {#opt_x11}

This backend enables support for creating windows on Linux and Unix systems using the X11 display server.

**Additional source files:** \c src/x11/video.c

| Compiler    | Compiler Flags                       | Linker Flags                         |
|-------------|--------------------------------------|--------------------------------------|
| GCC/Clang   | \c -D \c BJ_CONFIG_X11_BACKEND       | \c -lX11                             |

### Cocoa Backend {#opt_cocoa}

This backend enables support for creating windows on macOS.

**Additional source files:** \c src/cocoa/video.m

| Compiler    | Compiler Flags                       | Linker Flags                         |
|-------------|--------------------------------------|--------------------------------------|
| GCC/Clang   | \c -D \c BJ_CONFIG_COCOA_BACKEND     | \c -framework \c Cocoa               |

### MME Backend {#opt_mme}

This backend enables audio support on Windows using the Multimedia Extensions API.

**Additional source files:** \c src/mme/audio.c

| Compiler    | Compiler Flags                       | Linker Flags                         |
|-------------|--------------------------------------|--------------------------------------|
| MSVC        | \c /D \c BJ_CONFIG_MME_BACKEND       | \c winmm.lib                         |
| GCC/Clang   | \c -D \c BJ_CONFIG_MME_BACKEND       | \c -lwinmm                           |

### ALSA Backend {#opt_alsa}

This backend enables audio support on Linux using the Advanced Linux Sound Architecture (ALSA).

**Additional source files:** \c src/alsa/audio.c

| Compiler    | Compiler Flags                       | Linker Flags                         |
|-------------|--------------------------------------|--------------------------------------|
| GCC/Clang   | \c -D \c BJ_CONFIG_ALSA_BACKEND      | \c -lasound                          |

### Emscripten Backend {#opt_emscripten}

This backend enables support for WebAssembly builds using Emscripten.

**Additional source files:** \c src/emscripten/video.c and \c src/emscripten/audio.c

| Compiler    | Compiler Flags                       | Linker Flags                         |
|-------------|--------------------------------------|--------------------------------------|
| Emscripten  | \c -D \c BJ_CONFIG_EMSCRIPTEN_BACKEND | \c -sEXPORTED_RUNTIME_METHODS=['ccall','cwrap','_malloc','_free']<br>\c -sEXPORTED_FUNCTIONS=['_bj_emscripten_audio_process']<br>\c -sALLOW_MEMORY_GROWTH |

### Colored Logs {#opt_log_color}

This option enables ANSI color codes in the log output, making it easier to distinguish between different log levels in your terminal.

| Compiler    | Compiler Flags                       |
|-------------|--------------------------------------|
| MSVC        | \c /D \c BJ_CONFIG_LOG_COLOR         |
| GCC/Clang   | \c -D \c BJ_CONFIG_LOG_COLOR         |

### Log Checks {#opt_checks_log}

This option ensures that when a check fails (as described in \ref bj_check), an error message is logged to the standard output.

| Compiler    | Compiler Flags                       |
|-------------|--------------------------------------|
| MSVC        | \c /D \c BJ_CONFIG_CHECKS_LOG        |
| GCC/Clang   | \c -D \c BJ_CONFIG_CHECKS_LOG        |

### Abort on Check {#opt_checks_abort}

This option causes the program execution to immediately abort when a check fails.
This is useful for debugging critical errors.

| Compiler    | Compiler Flags                       |
|-------------|--------------------------------------|
| MSVC        | \c /D \c BJ_CONFIG_CHECKS_ABORT      |
| GCC/Clang   | \c -D \c BJ_CONFIG_CHECKS_ABORT      |

### Pedantic Mode {#opt_pedantic}

This option enables extra runtime checks throughout the API.
These checks might be costly in terms of performance but ensure strict correctness and safety.

| Compiler    | Compiler Flags                       |
|-------------|--------------------------------------|
| MSVC        | \c /D \c BJ_CONFIG_PEDANTIC          |
| GCC/Clang   | \c -D \c BJ_CONFIG_PEDANTIC          |

### Fast Math {#opt_fastmath}

This option enables floating-point optimizations that may violate the IEEE 754 standard but can significantly improve performance for math-heavy applications.

| Compiler    | Compiler Flags                       |
|-------------|--------------------------------------|
| MSVC        | \c /D \c BJ_CONFIG_FASTMATH \c /fp:fast |
| GCC/Clang   | \c -D \c BJ_CONFIG_FASTMATH \c -ffast-math \c -ffp-contract=fast \c -fno-math-errno \c -fno-trapping-math |

## Build with CMake {#build_cmake}

Banjo provides a CMake configuration for convenience.
It aims to work "out of the box" by automatically detecting available dependencies and enabling the corresponding backends.

### Auto-Detection

CMake checks your system for libraries (X11, ALSA, etc.) and your platform (Windows, macOS).
If a dependency is found, the corresponding backend is **enabled by default**.

### CMake Options

Every manual option listed above has a corresponding CMake option.
The naming convention is simple: replace the \c BJ_ prefix with \c BANJO_.

**Example**:
- Manual Macro: \c BJ_CONFIG_X11_BACKEND
- CMake Option: \c BANJO_CONFIG_X11_BACKEND

You can force options ON or OFF to override auto-detection:

```bash
# Disable X11 even if libraries are present
cmake -B build -DBANJO_CONFIG_X11_BACKEND=OFF

# Enable Pedantic mode and Colored logs
cmake -B build -DBANJO_CONFIG_PEDANTIC=ON -DBANJO_CONFIG_LOG_COLOR=ON
```



