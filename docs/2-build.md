# Building the API  {#build}

\brief How to build the documentation from source code

This document is about producing the Banjo API binaries.
If you want to know how to build an application *with* banjo, see \ref use.

## Requirements

The full set of requirements varies according to the development environment and features you want. But at least, you will need **a C99-compliant compiler**.

## Using CMake

Banjo API uses CMake as its primary development environment, although it's not a mandatory step.
The use of CMake is not different from the standard configure-generate-build workflow.
Thus if you are already familiar with CMake, you can directly jump to CMake Options below.

\todo Complete the CMake section

### CMake Options 

All build options listed in \ref build_options are available as CMake options with the same names.
As such, to configure with colored logging support, use `-DBJ_CONFIG_LOG_COLOR` when calling CMake, or select it in CMake user interface.

\todo Complete the CMake options section

## Build Options {#build_options}

Options listed below are C macro definitions that can be either set or unset. To enable an option, define the corresponding macro when compiling your code.  
For example, to enable colored logging support with *gcc*, use `-DBJ_CONFIG_LOG_COLOR`.  

By default, all features as disabled, unless you are using CMake.

### Feature Options  

Feature options enable additional capabilities in the API. For example, `BJ_FEATURE_X11` enables support for X11.  

Enabling a feature does **not** necessarily make it available or mandatory at runtime, but only integrates the necessary codepath at compile time to provide this support at runtime.

Most features require additional third-party libraries for proper compilation and execution. They are documented alongside the corresponding feature.  

| Option Name       | Description                          | CMake Defaults                                                                          |
|-------------------|--------------------------------------|-----------------------------------------------------------------------------------------|
| `BJ_FEATURE_X11`  | Enable support for X11 Window system | `ON` if [X11_FOUND](https://cmake.org/cmake/help/latest/module/FindX11.html) is defined |

### Configuration Options  

Configuration options control finer aspects of the code, such as enabling colored logging or more expensive memory checks.  
Unlike feature options, these do **not** require third-party libraries.  

| Option Name             | Description                                                                 | CMake Defaults |
|-------------------------|-----------------------------------------------------------------------------|----------------|
| `BJ_CONFIG_LOG_COLOR`   | Enable support for colored log outputs with \ref bj_log                     | `ON`           |
| `BJ_CONFIG_CHECK`       | Enable additional checks on function call parameters                        | `ON`           |
| `BJ_CONFIG_CHECK_LOG`   | Failing checks are logged using \ref bj_err                                 | `ON`           |
| `BJ_CONFIG_CHECK_ABORT` | Failing checks call `abort()` (after logging, if enabled)                   | `OFF`          |
| `BJ_CONFIG_PEDANTIC`    | Codepaths prioritize safety over performance when there is a choice to make | `ON`           |

