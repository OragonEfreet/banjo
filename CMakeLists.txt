cmake_minimum_required(VERSION 3.21)

project(banjo)

include(CTest)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Vulkan REQUIRED COMPONENTS glslc)

# add_subdirectory(vendor)

set(_interface_headers
    inc/banjo/api.h
    inc/banjo/array.h
    inc/banjo/error.h
    inc/banjo/forward_list.h
    inc/banjo/hash_table.h
    inc/banjo/log.h
    inc/banjo/memory.h
    inc/banjo/world.h
)

set(_sources
    src/array.c
    src/data/array.h
    src/data/forward_list.h
    src/data/hash_table.h
    src/data/world.h
    src/error.c
    src/forward_list.c
    src/hash_table.c
    src/log.c
    src/memory.c
    src/world.c
)

add_library(banjo ${_interface_headers} ${_sources})

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
# target_link_libraries(banjo 
#     PRIVATE
#         Vulkan::Vulkan
#         glfw
# )
target_compile_features(banjo PUBLIC c_std_99)

add_subdirectory(examples)

if (BUILD_TESTING)
    add_subdirectory(test)
endif()

