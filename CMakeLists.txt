cmake_minimum_required(VERSION 3.21)

project(banjo)

include(CTest)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Vulkan REQUIRED COMPONENTS glslc)

add_subdirectory(vendor)

add_library(banjo
    inc/banjo/app.h
    inc/banjo/array.h
    inc/banjo/core.h
    inc/banjo/log.h
    inc/banjo/memory.h
    inc/banjo/window.h
    src/core/array.c
    src/core/errors.h
    src/core/log.c
    src/core/memory.c
    src/core/memory.h
    src/ecs/world.c
    src/ecs/world.h
    src/engine/app.c
    src/engine/window.c
)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(banjo PRIVATE BANJO_EXPORTS)
else()
    target_compile_definitions(banjo PUBLIC BANJO_STATIC)
endif()

option(BANJO_PEDANTIC "Banjo runtime will make costly extra checks" OFF)
if(BANJO_PEDANTIC)
    target_compile_definitions(banjo PRIVATE BANJO_PEDANTIC)
endif()

target_include_directories(banjo PUBLIC inc PRIVATE src)
target_link_libraries(banjo 
    PRIVATE
        Vulkan::Vulkan
        glfw
)
target_compile_features(banjo PUBLIC c_std_99)

add_subdirectory(examples)

if (BUILD_TESTING)
    add_subdirectory(test)
endif()

