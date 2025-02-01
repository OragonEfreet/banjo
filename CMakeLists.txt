cmake_minimum_required(VERSION 3.21)

project(banjo C)

include(CTest)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CheckCXXCompilerFlag)

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

set(_private_headers
    src/array_t.h
    src/bitmap_t.h
    src/htable_t.h
    src/internals.h
    src/list_t.h
    src/stream_t.h
    src/system_t.h
    src/window_t.h
)

set(_sources
    src/api.c
    src/array.c
    src/bitmap.c
    src/bitmap_dib.c
    src/error.c
    src/fake.c
    src/htable.c
    src/internals.c
    src/list.c
    src/log.c
    src/memory.c
    src/pixel.c
    src/rect.c
    src/stream.c
    src/system.c
    src/window.c
)

if(MSVC)
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/runtime")
    set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/runtime")
endif()

add_library(banjo ${_interface_headers} ${_private_headers} ${_sources})

find_package(X11)
if(X11_FOUND)
    target_sources(banjo PRIVATE src/x11.c)
    target_compile_definitions(banjo PRIVATE "BANJO_HAVE_X11")
    # target_link_libraries(banjo PRIVATE X11::X11)
endif()

macro(banjo_feature _name _desc _default)
    string(TOUPPER  "banjo_feature_${_name}" _optmacro)
    string(TOUPPER  "BJ_FEAT_${_name}_ENABLED" _optdef)
    option(${_optmacro} "${_desc} (${_optdef})" ${_default})
    if(${_optmacro})
        target_compile_definitions(banjo PUBLIC "${_optdef}")
    endif()
endmacro()

banjo_feature(checks "Banjo Performs runtim checks and assertions" ON)
banjo_feature(log_checks "If checks feature is on, failed check with log" OFF)
banjo_feature(pedantic "Banjo runtime will make costly extra checks" ON)
banjo_feature(abort_on_checks "When checks feature is on, a failed check will abort execution" OFF)
banjo_feature(log_color "Banjo logs will have colored output" ON)

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


