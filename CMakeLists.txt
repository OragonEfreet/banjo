cmake_minimum_required(VERSION 3.21)

project(banjo C)

include(CTest)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_C_STANDARD 99)              # TODO CHECK
set(CMAKE_C_STANDARD_REQUIRED TRUE)   # TODO CHECK
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CheckCXXCompilerFlag)

set(_interface_headers
    inc/banjo/api.h
    inc/banjo/array.h
    inc/banjo/containers.h
    inc/banjo/error.h
    inc/banjo/hash_table.h
    inc/banjo/list.h
    inc/banjo/log.h
    inc/banjo/memory.h
    inc/banjo/world.h
)

set(_sources
    src/array.c
    src/error.c
    src/hash_table.c
    src/internal.h
    src/list.c
    src/log.c
    src/memory.c
    src/world.c
)

if(MSVC)
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/runtime")
    set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/runtime")
endif()

add_library(banjo ${_interface_headers} ${_sources})
target_compile_definitions(banjo PRIVATE BJ_NO_OPAQUE)

if(NOT MSVC)
    target_compile_options(banjo PRIVATE -Wall -pedantic)
endif()

if(BUILD_SHARED_LIBS)
    target_compile_definitions(banjo PRIVATE BANJO_EXPORTS)
else()
    target_compile_definitions(banjo PUBLIC BANJO_STATIC)
endif()

macro(banjo_compile_option _name _desc _default)
    string(TOUPPER  "banjo_${_name}" _optmacro)
    option(${_optmacro} ${_desc} ${_default})
    if(${_optmacro})
        target_compile_definitions(banjo PRIVATE "${_optmacro}")
    endif()
endmacro()
banjo_compile_option(pedantic "Banjo runtime will make costly extra checks" OFF)
banjo_compile_option(log_color "Banjo logs will have colored output" ON)

target_include_directories(banjo PUBLIC inc PRIVATE src)
set_property(TARGET banjo PROPERTY C_STANDARD 99) # TODO CHECK
target_compile_features(banjo PUBLIC c_std_99)    # TODO CHECK

add_subdirectory(examples)

if (BUILD_TESTING)
    add_subdirectory(test)
endif()

