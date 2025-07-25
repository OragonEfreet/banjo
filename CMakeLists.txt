cmake_minimum_required(VERSION 3.21)
project(banjo C)

include(CTest)

if (MSVC)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    add_compile_options(/W4 /D_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra)
endif()

option(BJ_FEATURE_WIN32       "Add Win32 support"                                              OFF)
option(BJ_FEATURE_X11         "Add X11 support"                                                OFF)
option(BJ_FEATURE_MME         "Add support for audio with Windows Multimedia Extensions"       OFF)
option(BJ_FEATURE_ALSA        "Add support for audio with ALSA"                                OFF)
option(BJ_CONFIG_ALL          "Enable all CONFIG options at once"                              OFF)
option(BJ_CONFIG_CHECKS_ABORT "When checks feature is on, a failed check will abort execution" OFF)
option(BJ_CONFIG_CHECKS_LOG   "If checks feature is on, failed check with log"                 OFF)
option(BJ_CONFIG_LOG_COLOR    "Banjo logs will have colored output"                            OFF)
option(BJ_CONFIG_PEDANTIC     "Banjo runtime will make costly extra checks"                    OFF)

file(GLOB SOURCES
    "${CMAKE_SOURCE_DIR}/src/*.c"
    "${CMAKE_SOURCE_DIR}/src/*.h"
    "${CMAKE_SOURCE_DIR}/inc/banjo/*.h"
)
add_library(banjo ${SOURCES})
target_include_directories(banjo PUBLIC inc)

set_target_properties(banjo PROPERTIES C_STANDARD 99 C_STANDARD_REQUIRED TRUE)

if(NOT MSVC)
    target_link_libraries(banjo PRIVATE m)
endif()

if(BUILD_SHARED_LIBS)
    target_compile_definitions(banjo PRIVATE BANJO_EXPORTS)
else()
    target_compile_definitions(banjo PUBLIC BANJO_STATIC)
endif()

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

set(BANJO_ASSETS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets)
option(BANJO_BUILD_EXAMPLES "Build Banjo Examples" TRUE)
if(BANJO_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
if (BUILD_TESTING)
    add_subdirectory(test)
endif()



