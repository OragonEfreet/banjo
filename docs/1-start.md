# Getting Started {#start}

\brief Your first Banjo code

This tutorial will guide you through creating your first Banjo application from scratch.
You'll learn how to set up a project, create a simple graphics program, and build it using CMake.

## What You'll Build

A simple application that opens a window and draws colorful shapes on the screen.

## Prerequisites

- **CMake** (version 3.21 or higher)
- **A C99-compliant compiler** (GCC, Clang, or MSVC)
- **Git** (for fetching Banjo)
- **Internet connection** (for the first build)

## Step 1: Create Your Project Directory

Create a new directory for your project:

```bash
mkdir my_banjo_app
cd my_banjo_app
```

## Step 2: Create main.c

Create a file named `main.c` with the following content:

\include start.c

## Step 3: Create CMakeLists.txt

Create a file named `CMakeLists.txt` with the following content:

```cmake
cmake_minimum_required(VERSION 3.21)
project(MyBanjoApp C)

include(FetchContent)

# Fetch Banjo from GitHub
FetchContent_Declare(
    banjo
    GIT_REPOSITORY https://github.com/oragonefreet/banjo.git
    GIT_TAG        main  # Or use a specific version tag like v0.1.0
)
FetchContent_MakeAvailable(banjo)

# Create your executable
add_executable(my_banjo_app main.c)

# Link against Banjo
target_link_libraries(my_banjo_app PRIVATE banjo)
```

## Step 4: Build and Run

Configure and build your project:

```bash
# Configure the project
cmake -B build

# Build the executable
cmake --build build

# Run your application
./build/my_banjo_app
```

On Windows, the executable will be located at `build\Debug\my_banjo_app.exe` or `build\Release\my_banjo_app.exe`.

## What's Happening?

Let's break down the key parts of the code:

### Program Structure

The program uses a standard `main()` function with three main sections:

1. **Initialization**: Set up Banjo, create a window and renderer
2. **Drawing**: Render shapes to the framebuffer and present it
3. **Main Loop**: Keep the window open and handle events until the user closes it
4. **Cleanup**: Free resources before exiting

### Drawing Graphics

- **Get the framebuffer**: `bj_get_framebuffer(renderer)` returns a bitmap you can draw on
- **Create colors**: `bj_make_bitmap_pixel()` creates pixel values in the correct format
- **Draw shapes**: Use functions like `bj_draw_filled_circle()` and `bj_draw_rectangle()`
- **Present**: `bj_present()` displays your rendered frame in the window

### Event Handling

- `bj_dispatch_events()` processes window events (keyboard, mouse, etc.)
- `bj_set_key_callback(bj_close_on_escape, 0)` sets up ESC key to close the window
- `bj_should_close_window()` checks if the user requested to close the window

## Next Steps

Now that you have a working Banjo application, you can:

- Explore more examples in the Banjo repository's `examples/` directory
- Learn about different \ref build_options "build options" and backends
- Read the full API documentation to discover all available features
- Check out \ref use "Using Banjo" for advanced integration methods

For more information on building Banjo itself from source, see \ref build.
