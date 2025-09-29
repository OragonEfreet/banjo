![Documentation](https://github.com/OragonEfreet/banjo/actions/workflows/documentation.yml/badge.svg)
[![Tests](https://github.com/OragonEfreet/banjo/actions/workflows/cmake-multi-platform.yml/badge.svg)](https://github.com/OragonEfreet/banjo/actions/workflows/cmake-multi-platform.yml)

Banjo is a lightweight, self-contained C99 framework based on four principles:  
**Simple** – if you know C, you can use it.  
**Minimal** – no external dependencies, even for tests.  
**Self-documented** – APIs are explained in the headers.  
**Fast** – must compile quickly.  

Features include windowing and events, bitmap manipulation (loading, blitting, primitives), logging, custom allocators, and math utilities (vec2, vec3, vec4, mat3, mat4, quat). A roadmap is available at [Development Status](https://codework-orange.io/banjo/roadmap.html).  

The build system is CMake, though the code can also be compiled without it. Instructions are at [Building the API](https://codework-orange.io/banjo/build.html).  

Banjo is tested on the following environments, where “tested” means the full project builds and all tests run successfully:  

| Distribution            | Compiler                    | CMake   |
|-------------------------|-----------------------------|---------|
| Ubuntu 25.04 Plucky     | GCC 14.2.0 / GNU libc 2.41  | 3.31.6  |
| Ubuntu 23.10 Mantic     | GCC 13.2.0 / GNU libc 2.38  | 3.27.4  |
| Windows 11 64-bit       | Microsoft Visual C++ 2022   | 3.28.1  |
| macOS 15.6.1            | Apple clang 16.0.0          | 3.29.3  |

Demo code and usage examples are provided at [Examples](examples.html).  
