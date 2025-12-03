# Using Banjo {#use}

\brief Configure your project to compile against Banjo API

This document describes how to integrate Banjo into your project.
If you want to know how to build Banjo itself, see \ref build.

## Integration with CMake {#use_cmake}

Banjo provides comprehensive CMake support with three integration methods.
Choose the method that best fits your project's needs.

### Method 1: Installed Library (find_package) {#use_cmake_find_package}

This method is appropriate when Banjo has been installed system-wide or to a specific prefix.
It is the recommended approach for production deployments and packaged distributions.

**Step 1: Install Banjo**

```bash
cd /path/to/banjo
cmake -B build
cmake --build build
cmake --install build --prefix /usr/local
```

**Step 2: Use in Your Project**

In your \c CMakeLists.txt:

```cmake
cmake_minimum_required(VERSION 3.21)
project(MyGame C)

# Find the installed Banjo package
find_package(banjo REQUIRED)

add_executable(mygame main.c)

# Link against the installed library
target_link_libraries(mygame PRIVATE banjo::banjo)
```

**Step 3: Configure Your Project**

If Banjo was installed to a non-standard prefix, specify \c CMAKE_PREFIX_PATH:

```bash
cmake -B build -DCMAKE_PREFIX_PATH=/custom/install/prefix
cmake --build build
```

**What Gets Installed:**
- Library files: \c lib/libbanjo.a (or \c .so for shared builds)
- Public headers: \c include/banjo/*.h
- CMake configuration: \c lib/cmake/banjo/
- pkg-config file: \c lib/pkgconfig/banjo.pc

### Method 2: Source Subdirectory (add_subdirectory) {#use_cmake_add_subdirectory}

This method embeds Banjo directly into your project's source tree.
It is useful for tightly coupled projects or when you need fine-grained control over Banjo's build configuration.

**Step 1: Add Banjo to Your Project**

```bash
# Option A: Git submodule
git submodule add https://github.com/yourname/banjo.git external/banjo

# Option B: Copy the source directly
cp -r /path/to/banjo external/banjo
```

**Step 2: Use in Your Project**

In your \c CMakeLists.txt:

```cmake
cmake_minimum_required(VERSION 3.21)
project(MyGame C)

# Add Banjo as a subdirectory
add_subdirectory(external/banjo)

add_executable(mygame main.c)

# Link against the Banjo target
target_link_libraries(mygame PRIVATE banjo)
```

**Step 3: Configure Build Options (Optional)**

You can control Banjo's configuration from your parent project:

```cmake
# Disable examples and tests
set(BANJO_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

# Configure specific backends
set(BANJO_CONFIG_X11_BACKEND ON CACHE BOOL "" FORCE)
set(BANJO_CONFIG_PEDANTIC OFF CACHE BOOL "" FORCE)

add_subdirectory(external/banjo)
```

**Note:** When using \c add_subdirectory(), link against the \c banjo target (not \c banjo::banjo).

### Method 3: Fetch at Configure Time (FetchContent) {#use_cmake_fetch_content}

This method automatically downloads Banjo during the CMake configuration step.
It is ideal for open-source projects and continuous integration environments where dependencies should be reproducible.

**Use in Your Project**

In your \c CMakeLists.txt:

```cmake
cmake_minimum_required(VERSION 3.21)
project(MyGame C)

include(FetchContent)

# Declare the Banjo dependency
FetchContent_Declare(
    banjo
    GIT_REPOSITORY https://github.com/yourname/banjo.git
    GIT_TAG        v0.1.0  # Use a specific tag or commit hash
)

# Optional: Configure Banjo before making it available
set(BANJO_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

# Download and make available
FetchContent_MakeAvailable(banjo)

add_executable(mygame main.c)

# Link against the Banjo target
target_link_libraries(mygame PRIVATE banjo)
```

**Advantages:**
- No manual installation or submodule management required
- Reproducible builds with version pinning (\c GIT_TAG)
- CMake handles downloading and caching automatically

**Note:** \c FetchContent requires an internet connection during the first configuration.

### Backend Selection {#use_cmake_backends}

Regardless of which integration method you use, Banjo automatically enables platform-appropriate backends.
However, you may want to explicitly control backend selection:

```cmake
# Force specific backends (when using add_subdirectory or FetchContent)
set(BANJO_CONFIG_COCOA_BACKEND ON CACHE BOOL "" FORCE)
set(BANJO_CONFIG_X11_BACKEND OFF CACHE BOOL "" FORCE)

# Or when using find_package, configure backends during Banjo's installation
cmake -B build \
    -DBANJO_CONFIG_COCOA_BACKEND=ON \
    -DBANJO_CONFIG_X11_BACKEND=OFF
cmake --install build
```

See \ref build_cmake for a complete list of available CMake options.

## Integration with pkg-config {#use_pkgconfig}

For non-CMake build systems, Banjo provides a \c pkg-config file.

**Prerequisites:**
Banjo must be installed (see \ref use_cmake_find_package).

**Usage:**

```bash
# Compile and link in one step
gcc main.c $(pkg-config --cflags --libs banjo) -o mygame

# Or separately
gcc -c main.c $(pkg-config --cflags banjo)
gcc main.o $(pkg-config --libs banjo) -o mygame
```

**Makefile Example:**

```makefile
CC = gcc
CFLAGS = $(shell pkg-config --cflags banjo)
LDFLAGS = $(shell pkg-config --libs banjo)

mygame: main.o
	$(CC) main.o $(LDFLAGS) -o mygame

main.o: main.c
	$(CC) -c main.c $(CFLAGS)
```

If Banjo is installed to a non-standard prefix:

```bash
export PKG_CONFIG_PATH=/custom/install/prefix/lib/pkgconfig
gcc main.c $(pkg-config --cflags --libs banjo) -o mygame
```

## Manual Integration {#use_manual}

If you prefer not to use a build system, you can link against Banjo manually.

**Prerequisites:**
- Banjo must be built (see \ref build_manual)
- You must have the compiled library (\c libbanjo.a or \c libbanjo.so)

**Compile Your Application:**

```bash
# Link against static library
gcc main.c -I/path/to/banjo/inc -L/path/to/banjo -lbanjo -lm -o mygame

# Add additional libraries based on enabled backends
# For X11:
gcc main.c -I/path/to/banjo/inc -L/path/to/banjo -lbanjo -lm -lX11 -o mygame

# For Cocoa (macOS):
gcc main.c -I/path/to/banjo/inc -L/path/to/banjo -lbanjo -lm -framework Cocoa -o mygame
```

**Include Paths:**
- Add \c -I/path/to/banjo/inc to access public headers

**Library Paths:**
- Add \c -L/path/to/banjo to locate \c libbanjo.a
- Always link with \c -lm (math library) on Unix-like systems

**Backend Dependencies:**
Depending on which backends were enabled during Banjo's build, you must link additional platform libraries.
See \ref build_options for the complete list of backend linker flags.

## Verifying Integration {#use_verify}

After integrating Banjo, verify your setup with a minimal test program:

```c
#include <banjo/api.h>
#include <stdio.h>

int main(void) {
    printf("Hello Banjo!");
    return 0;
}
```

If compilation succeeds, your integration is configured correctly.

For a functional example with window creation and rendering, see the \c examples/ directory in the Banjo source tree.
