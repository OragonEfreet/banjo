# Building the API  {#build}

\brief How to build the documentation from source code

This document is about producing the Banjo API binaries.
If you want to know how to build an application *with* banjo, see \ref use.

You will need at least **a C99-compliant compiler**.

## Manual Build (No Build System) {#build_manual}

Banjo is designed to be easily built without any complex build system.
You only need to feed your compiler with the source files and the correct include paths.

### 1. Core Sources

Compile all `.c` files in `src/` and `src/unix/` (if on Unix) or `src/win32/` (if on Windows).
**Do not** compile files in backend-specific subdirectories (`src/x11/`, `src/cocoa/`, etc.) unless you are enabling that specific backend.

Add `inc/` and `src/` to your include search path.

### 2. Output Type

- **Static Library**: Define `BANJO_STATIC`.
- **Shared Library**: Define `BANJO_EXPORTS` (do NOT define `BANJO_STATIC`).

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
To enable it, you must define the `BJ_CONFIG_WIN32_BACKEND` macro.
You will also need to compile `src/win32/video.c` and link against the standard system libraries `user32`, `gdi32`, and `kernel32` (which are usually linked by default).

### X11 Backend {#opt_x11}

This backend enables support for creating windows on Linux and Unix systems using the X11 display server.
To enable it, define the `BJ_CONFIG_X11_BACKEND` macro and compile `src/x11/video.c`.
You must ensure the X11 headers are in your include path (typically `/usr/include/X11`) and link against the X11 library using `-lX11`.

### Cocoa Backend {#opt_cocoa}

This backend enables support for creating windows on macOS.
To enable it, define the `BJ_CONFIG_COCOA_BACKEND` macro and compile `src/cocoa/video.m`.
You will need to link against the Cocoa framework using `-framework Cocoa`.

### MME Backend {#opt_mme}

This backend enables audio support on Windows using the Multimedia Extensions API.
To enable it, define the `BJ_CONFIG_MME_BACKEND` macro and compile `src/mme/audio.c`.
You will need to link against the `winmm` system library.

### ALSA Backend {#opt_alsa}

This backend enables audio support on Linux using the Advanced Linux Sound Architecture (ALSA).
To enable it, define the `BJ_CONFIG_ALSA_BACKEND` macro and compile `src/alsa/audio.c`.
Ensure the ALSA headers are in your include path and link against the ALSA library using `-lasound`.

### Emscripten Backend {#opt_emscripten}

This backend enables support for WebAssembly builds using Emscripten.
To enable it, define the `BJ_CONFIG_EMSCRIPTEN_BACKEND` macro and compile both `src/emscripten/video.c` and `src/emscripten/audio.c`.

When linking, you need to export specific runtime methods and functions to allow the browser to interact with the Banjo runtime.
Add the following flags to your linker command:
`-sEXPORTED_RUNTIME_METHODS=['ccall','cwrap','_malloc','_free'] -sEXPORTED_FUNCTIONS=['_bj_emscripten_audio_process'] -sALLOW_MEMORY_GROWTH`

### Colored Logs {#opt_log_color}

This option enables ANSI color codes in the log output, making it easier to distinguish between different log levels in your terminal.
To enable it, simply define the `BJ_CONFIG_LOG_COLOR` macro.

### Log Checks {#opt_checks_log}

This option ensures that when a check fails (as described in \ref bj_check), an error message is logged to the standard output.
To enable this behavior, define the `BJ_CONFIG_CHECKS_LOG` macro.

### Abort on Check {#opt_checks_abort}

This option causes the program execution to immediately abort when a check fails.
This is useful for debugging critical errors.
To enable this behavior, define the `BJ_CONFIG_CHECKS_ABORT` macro.

### Pedantic Mode {#opt_pedantic}

This option enables extra runtime checks throughout the API.
These checks might be costly in terms of performance but ensure strict correctness and safety.
To enable pedantic mode, define the `BJ_CONFIG_PEDANTIC` macro.

### Fast Math {#opt_fastmath}

This option enables floating-point optimizations that may violate the IEEE 754 standard but can significantly improve performance for math-heavy applications.
To enable it, define the `BJ_CONFIG_FASTMATH` macro.

You should also pass the appropriate flags to your compiler.
For GCC and Clang, use `-ffast-math -ffp-contract=fast -fno-math-errno -fno-trapping-math`.
For MSVC, use `/fp:fast`.

## Build with CMake {#build_cmake}

Banjo provides a CMake configuration for convenience.
It aims to work "out of the box" by automatically detecting available dependencies and enabling the corresponding backends.

### Auto-Detection

CMake checks your system for libraries (X11, ALSA, etc.) and your platform (Windows, macOS).
If a dependency is found, the corresponding backend is **enabled by default**.

### CMake Options

Every manual option listed above has a corresponding CMake option.
The naming convention is simple: replace the `BJ_` prefix with `BANJO_`.

**Example**:
- Manual Macro: `BJ_CONFIG_X11_BACKEND`
- CMake Option: `BANJO_CONFIG_X11_BACKEND`

You can force options ON or OFF to override auto-detection:

```bash
# Disable X11 even if libraries are present
cmake -B build -DBANJO_CONFIG_X11_BACKEND=OFF

# Enable Pedantic mode and Colored logs
cmake -B build -DBANJO_CONFIG_PEDANTIC=ON -DBANJO_CONFIG_LOG_COLOR=ON
```



