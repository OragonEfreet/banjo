# Banjo C99 API

![Documentation](https://github.com/OragonEfreet/banjo/actions/workflows/documentation.yml/badge.svg)
![Build:Windows](https://github.com/OragonEfreet/banjo/actions/workflows/windows-msvc.yml/badge.svg)
![Build:Unix](https://github.com/OragonEfreet/banjo/actions/workflows/unix-make.yml/badge.svg)

Banjo is a lightweight, self-contained C99 framework built on four core principles:  

- **Simple** – If you know C, you can use Banjo.  
- **Minimal** – Avoids external dependencies, even for testing.  
- **Self-Documented** – Learn directly from the header files.  
- **Fast** – Banjo **must compile fast**.  

## Features At a Glance

- Basic container types (sequential array, linked list, hash map)
- Bitmap manipulation and loading, blitting
- Basic primitive drawings
- Windowing and Event
- Logging system
- Custom allocators

## Support

Code is multi-platform, but I'm testing on the following environments:

| **Distribution**        | **Compiler**                                   | **CMake** |
|-------------------------|------------------------------------------------|-----------|
| **Manjaro Linux**       | GCC 13.2.1 20230801 / GNU libc 2.38            | 3.28.1    |
| **Ubuntu 23.10 Mantic** | GCC 13.2.0-4ubuntu3 / GNU libc 2.38-1ubuntu6.1 | 3.27.4    |
| **Windows 11 64 bit**   | Microsoft Visual C++ 2022 4.8.09032            | 3.28.1    |

Here, "testing" corresponds to building the entire project and running all tests successfully.

## Demo Code

See [Examples](examples.html)








