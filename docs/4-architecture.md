# Architecture {#architecture}

\brief Banjo API presentation

Being a C Library, Banjo has the possibility of being built, distributed and consumed in two ways: either a **static** or a **shared** library.

The build type is set by CMake options and communicated to producing as well as consuming code through C macro definitions (defined in _api.h_):

* `BANJO_STATIC` is set when building and using a static library.
   This macro is used to entirely disable the other macros.
* `BANJO_EXPORTS` is set when building (but not using) a dynamic library.
   It is used to communicate both producers and consumers that the API functions are dynamically loaded.
* `BANJO_EXPORT` and `BANJO_NO_EXPORT` are set when building a dynamc library and their expansions depend on the compiler.
   These macros are used to define various call attributes the compilers require to either export or import a through through binary compatibility.

While using CMake, you don't directly set these macro but instead toggle ON/OF CMake macros.

## Banjo as a Static Library

\note
This is guaranteed to be the default when configuring the project the first time.

A static library is directly incorporated into the consumer binary during the last step of compiling the latter.
This operation is performed by the consumer's linker tool.

Pros:

- Simplicity of build and distribution (a single final executable).
- No compatibility issue on a library update.

Cons:

- Larger final executable size.
- Potentially longer runtime load time.
- Updating the library means compiling the executable again.

**Banjo is set as a static library when the `BUILD_SHARED_LIBS` CMake variable is _0_ or not set.**

This corresponds to the following macro configuration:

- `BANJO_STATIC`:    Set
- `BANJO_EXPORT`:    Set
- `BANJO_EXPORTS`:   _Unset_
- `BANJO_NO_EXPORT`: Set

## Banjo as a Dynamically Library

A dynamic library is loaded at runtime by the operating system, instead of at link time by the compiler.

Pros:

- Smaller final executable size.
- Easier library update.

Cons:

- Variable support from operating systems.
- More files to distribute.

**Banjo is set as a dynamic library when the `BUILD_SHARED_LIBS` CMake variable is _1_.**

The macro expansions depends on if you build or use the library:

- Building the library:
    - `BANJO_STATIC`:     _Unset_
    - `BANJO_EXPORT`:     Compiler attributes for function export
    - `BANJO_EXPORTS`:    Set
    - `BANJO_NO_EXPORT`:  Set
- Using the library:
    - `BANJO_STATIC`:     _Unset_
    - `BANJO_EXPORT`:     Compiler attributes for function import
    - `BANJO_EXPORTS`:    Set
    - `BANJO_NO_EXPORT`:  Set



