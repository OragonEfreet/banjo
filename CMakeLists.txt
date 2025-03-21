cmake_minimum_required(VERSION 3.21)

project(banjo C)

include(CTest)
include(CheckCXXCompilerFlag)

option(BJ_FEATURE_WIN32 "Add Win32 support" OFF)
option(BJ_FEATURE_X11 "Add X11 support" OFF)

option(BJ_CONFIG_CHECKS_ABORT "When checks feature is on, a failed check will abort execution" OFF)
option(BJ_CONFIG_CHECKS_LOG   "If checks feature is on, failed check with log"                 OFF)
option(BJ_CONFIG_LOG_COLOR    "Banjo logs will have colored output"                            OFF)
option(BJ_CONFIG_PEDANTIC     "Banjo runtime will make costly extra checks"                    OFF)

add_library(banjo
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
    src/api.c
    src/array.c
    src/array_t.h
    src/bitmap.c
    src/bitmap_dib.c
    src/bitmap_t.h
    src/config.h
    src/error.c
    src/fake.c
    src/htable.c
    src/htable_t.h
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
    src/system_unix.c
    src/system_win32.c
    src/win32.c
    src/window.c
    src/window_t.h
    src/x11.c
)

target_include_directories(banjo PUBLIC inc)
target_compile_features(banjo PUBLIC c_std_99)

set_target_properties(banjo PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED TRUE
)

if(BJ_FEATURE_X11)
    find_package(X11)
    if(X11_FOUND)
        target_include_directories(banjo PRIVATE ${X11_INCLUDE_DIR})
    endif()
endif()

get_cmake_property(ALL_CACHE_VARS CACHE_VARIABLES)
foreach(VAR ${ALL_CACHE_VARS})
    get_property(VAR_TYPE CACHE ${VAR} PROPERTY TYPE)
    if(VAR_TYPE STREQUAL "BOOL" AND VAR MATCHES "^BJ_")
        if(${VAR})
            target_compile_definitions(banjo PUBLIC "${VAR}")
            message(STATUS "Added macro: ${VAR}")
        endif()
    endif()
endforeach()

if (MSVC)
    target_compile_options(banjo PRIVATE /W4)
else()
    target_compile_options(banjo PRIVATE -Wall -Wextra)
endif()

if(BUILD_SHARED_LIBS)
    target_compile_definitions(banjo PRIVATE BANJO_EXPORTS)
else()
    target_compile_definitions(banjo PUBLIC BANJO_STATIC)
endif()

set(BANJO_ASSETS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets)
option(BANJO_BUILD_EXAMPLES "Build Banjo Examples" TRUE)
if(BANJO_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
if (BUILD_TESTING)
    add_subdirectory(test)
endif()

