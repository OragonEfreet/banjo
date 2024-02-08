cmake_minimum_required(VERSION 3.21)

project(banjo)

include(CTest)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Vulkan REQUIRED COMPONENTS glslc)

# add_subdirectory(vendor)

set(_interface_headers
    # inc/banjo/app.h
    # inc/banjo/window.h
    inc/banjo/api.h
    inc/banjo/array.h
    inc/banjo/forward_list.h
    inc/banjo/hash_table.h
    inc/banjo/log.h
    inc/banjo/memory.h
)

set(_sources
    # src/engine/app.c
    # src/engine/window.c
    src/core/array.c
    src/core/errors.h
    src/core/forward_list.c
    src/core/hash_table.c
    src/core/log.c
    src/core/memory.c
    src/ecs/world.c
    src/ecs/world.h
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

