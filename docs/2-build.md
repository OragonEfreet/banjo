# Building the API  {#build}

\brief How to build the documentation from source code

This document is about producing the Banjo API binaries.
If you want to know how to build an application *with* banjo, see \ref use.

You will need at least **a C99-compliant compiler**.
Other requirements may be necessary according to the \ref features "features you enable".
Then, you can either build Banjo \ref build_cmake "using CMake" or \ref build_nocmake "manually".

## Build Options {#build_options}

All build options are controlled through setting macro definitions:
- All macro definition start with `BJ_`.
- If a macro definition is set, the option is enabled, the value of the macro does not matter.
- Conversely, if a macro definition is not set, the option is disabled.
- By default, all options are disabled.





Refer to your compiler's documentation to check how to set macro definitions.

Options listed below are C macro definitions that can be either set or unset. To enable an option, define the corresponding macro when compiling your code.
For example, to enable colored logging support with *gcc*, use `-DBJ_CONFIG_LOG_COLOR`.  

### Feature Options {#features}

Feature options enable additional capabilities in the API.
For example, `BJ_FEATURE_X11` enables support for X11.  
Enabling a feature does **not** necessarily make it available or mandatory at runtime, but only integrates the necessary codepath at compile time to provide this support at runtime.

Most features require additional third-party libraries for proper compilation and execution.

| Option Name       | Description                                                |
|-------------------|------------------------------------------------------------|
| `BJ_FEATURE_X11`  | Enable support for X11 Window system, see \ref feature_x11 |

#### X11 Support {#feature_x11}

X11 Support is enabled when Banjo is compiled with `BJ_FEATURE_X11`.
This feature provides the support for windows and events using X11 displays server.

At runtime, this feature requires that the Xlib runtime libraries are accessible.
At compile time, the include folders for Xlib must be accessible from the compiler.
When using CMake, they are automatically searched and passed to the compiler options if found.

### Configuration Options {#configuration}

Configuration options control finer aspects of the code, such as enabling colored logging or more expensive memory checks.  
Unlike feature options, these do **not** require third-party libraries.  

| Option Name             | Description                                                                 |
|-------------------------|-----------------------------------------------------------------------------|
| `BJ_CONFIG_LOG_COLOR`   | Enable support for colored log outputs with \ref bj_log                     |
| `BJ_CONFIG_CHECK_LOG`   | Failing checks are logged using \ref bj_err                                 |
| `BJ_CONFIG_CHECK_ABORT` | Failing checks call `abort()` (after logging, if enabled)                   |
| `BJ_CONFIG_PEDANTIC`    | Codepaths prioritize safety over performance when there is a choice to make |

## Build with CMake {#build_cmake}

Banjo API uses CMake as its primary development environment, although it's not a mandatory step.
The use of CMake is not different from the standard configure-generate-build workflow.

All build options listed in \ref build_options are available as CMake options with the same names.
As such, to configure with colored logging support, use `-DBJ_CONFIG_LOG_COLOR` when calling CMake, or select it in CMake user interface.

Enabling an option using CMake will provide additional platform checks during configure step to ensure your environment is able to compile with the specific options.
For example, when `BJ_FEATURE_X11` is set, CMake searches for X11 development libraries and add their include folders into the compile options.

CMake is configured with [Presets](https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html), which provide you with the most common configurations:


## Build the Sources Manually {#build_nocmake}

Using the compiler of your choice, compile and link all *.c* files under *src/*, setting *inc/* as an includes search folder.
This example builds Banjo as a static library using gcc:

```
$ gcc -c src/*.c -I inc/ -D BANJO_STATIC && ar rcs libbanjo.a *.o
```

`BANJO_STATIC` must be defined if (and only if) the library is compiled as a static library.
If the library is built for a dynamic library (*.so*, *.dll* or *.dylib), `BANJO_STATIC` must not be set, and `BANJO_EXPORTS` must be set instead:

This line builds banjo as a shared object:

```
$ gcc -shared -fPIC -D BANJO_EXPORTS -o libbanjo.so *.c
```

Refer to your compiler's documentation for more information about building C projects into static of shared libraries.


