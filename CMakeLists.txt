cmake_minimum_required(VERSION 3.21)

project(banjo C)

include(CTest)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# For MSVC, we put everything in the same folder
if(MSVC)
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/runtime")
    set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/runtime")
endif()

set(_interface_headers
    inc/banjo/api.h
    inc/banjo/array.h
    inc/banjo/bitmap.h
    inc/banjo/error.h
    inc/banjo/event.h
    inc/banjo/htable.h
    inc/banjo/list.h
    inc/banjo/log.h
    inc/banjo/memory.h
    inc/banjo/rect.h
    inc/banjo/stream.h
    inc/banjo/system.h
    inc/banjo/window.h
)

set(_sources
    src/api.c
    src/array.c
    src/array_t.h
    src/bitmap.c
    src/bitmap_dib.c
    src/bitmap_t.h
    src/error.c
    src/fake.c
    src/htable.c
    src/htable_t.h
    src/internals.c
    src/internals.h
    src/list.c
    src/list_t.h
    src/log.c
    src/memory.c
    src/pixel.c
    src/rect.c
    src/stream.c
    src/stream_t.h
    src/system.c
    src/system_t.h
    src/window.c
    src/window_t.h
    src/x11.c
)

add_library(banjo ${_interface_headers} ${_sources})

# --- Option-to-defines funcion ------------------------------------------------
macro(banjo_option _name _desc _default)
    string(TOUPPER  "BJ_${_name}" _optmacro)
    option(${_optmacro} "${_desc}" ${_default})
    if(${_optmacro})
        target_compile_definitions(banjo PUBLIC "${_optmacro}")
    endif()
endmacro()

banjo_option(config_checks "Banjo performs runtim checks and assertions" ON)
banjo_option(config_checks_log "If checks feature is on, failed check with log" ON)
banjo_option(config_checks_abort "When checks feature is on, a failed check will abort execution" OFF)
banjo_option(config_pedantic "Banjo runtime will make costly extra checks" ON)
banjo_option(config_log_color "Banjo logs will have colored output" OFF)

# --- X11 Detection - If found, feature is enabled by default ------------------
# If X11 is found and feature is enabled, CMake adds proper include directories.
find_package(X11)
banjo_option(feature_x11 "Add X11 support" "${X11_FOUND}")
if(BANJO_FEATURE_X11)
    if(X11_FOUND)
        target_include_directories(banjo PRIVATE ${X11_INCLUDE_DIR})
    endif()
endif()


if(NOT MSVC)
    target_compile_options(banjo PRIVATE -Wall)
endif()

if(BUILD_SHARED_LIBS)
    target_compile_definitions(banjo PRIVATE BANJO_EXPORTS)
else()
    target_compile_definitions(banjo PUBLIC BANJO_STATIC)
endif()


target_include_directories(banjo PUBLIC inc)
set_property(TARGET banjo PROPERTY C_STANDARD 99)
target_compile_features(banjo PUBLIC c_std_99)

set(BANJO_ASSETS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets)

option(BANJO_BUILD_EXAMPLES "Build Banjo Examples" TRUE)
if(BANJO_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if (BUILD_TESTING)
    add_subdirectory(test)
endif()


