# Banjo C99 API

![Documentation](https://github.com/OragonEfreet/banjo/actions/workflows/documentation.yml/badge.svg)
[![Tests](https://github.com/OragonEfreet/banjo/actions/workflows/cmake-multi-platform.yml/badge.svg)](https://github.com/OragonEfreet/banjo/actions/workflows/cmake-multi-platform.yml)

Banjo is a lightweight, self-contained C99 framework built on four core principles:  

- **Simple** – If you know C, you can use Banjo.  
- **Minimal** – Avoids external dependencies, even for testing.  
- **Self-Documented** – Learn directly from the header files.  
- **Fast** – Banjo **must compile fast**.  

## Features At a Glance

- Windowing and Event
- Bitmap manipulation and loading, blitting
- Basic primitive drawings
- Logging system
- Custom allocators
- Math functions (vec2, vec3, vec4, mat3, mat4, quat)

See [Development Status](https://codework-orange.io/banjo/roadmap.html) for more.

## How to Build and Test the Project

Banjo main build system uses CMake, but it's possible to build without.
See [Building the API](https://codework-orange.io/banjo/build.html)

## Support

Code is multi-platform, but I'm testing on the following environments:

| **Distribution**        | **Compiler**               | **CMake** |
|-------------------------|----------------------------|-----------|
| **Ubuntu 25.04 Plucky** | GCC 14.2.0 / GNU libc 2.41 | 3.31.6    |
| **Ubuntu 23.10 Mantic** | GCC 13.2.0 / GNU libc 2.38 | 3.27.4    |
| **Windows 11 64 bit**   | Microsoft Visual C++ 2022  | 3.28.1    |
| **macOS 15.6.1**        | Apple clang 16.0.0         | 3.29.3    |

Here, "testing" corresponds to building the entire project and running all tests successfully.

## Demo Code

See [Examples](examples.html)

