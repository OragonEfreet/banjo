# Banjo API

![Documentation](https://github.com/OragonEfreet/banjo/actions/workflows/documentation.yml/badge.svg)
![BuildTest](https://github.com/OragonEfreet/banjo/actions/workflows/cmake-multi-platform.yml/badge.svg)

Banjo is a small projet in making myself a C99 framework for game development.
The work aims at the following objectives:

- **Little to no dependency**: The API must implement everything by itself and rely on the least amount of dependencies. Even the testing framework is local.
- **Simple**:                  Easy to grasp for newcomers as long as they know C.
- **Self-Documented**:         Documentation is fully available from header files, making code API sufficient to learn.
- **Fast compile times**:      Banjo **must** compile fast.

Current status of the projet is **early draft**, consisting in:
- Designing the API
- Providing most standard core features (base containers, memory function)
- Developping an Entity-Component-System framework

## Features

This section only shows the currently available features.

### Container types

- `BjArray`: *Sequence container* that wraps a classic C-Style array.
- `BjList`: Forward-only *linked list*.
- `BjHashTable`: Associative array.

### Misc

* Simple *logging* system with _log.h_.
* Custom allocators with `BjAllocationCallbacks`.

## Support

Code is multi-platform, but I'm testing on the following environments:

| **Distribution**        | **Compiler**                                   | **CMake** |
|-------------------------|------------------------------------------------|-----------|
| **Manjaro Linux**       | GCC 13.2.1 20230801 / GNU libc 2.38            | 3.28.1    |
| **Ubuntu 23.10 Mantic** | GCC 13.2.0-4ubuntu3 / GNU libc 2.38-1ubuntu6.1 | 3.27.4    |
| **Windows 11 64 bit**   | Microsoft Visual C++ 2022 4.8.09032            | 3.28.1    |

Here, "testing" corresponds to building the entire project and running all tests successfully.

## Demo Code

You can browse a full work set of examples from the _examples_ folder in the repository.


### Dta Structures

\example BjArray.c
\example BjHashTable.c
\example BjList.c

### Worlflows

\example handling_errors.c







