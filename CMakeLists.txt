cmake_minimum_required(VERSION 3.21)

project(banjo)

include(CTest)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Vulkan REQUIRED COMPONENTS glslc)

add_subdirectory(vendor)

add_library(banjo_engine STATIC
    inc/banjo/app.h
    inc/banjo/core.h
    inc/banjo/memory.h
    inc/banjo/window.h
    src/core/array.c
    src/core/array.h
    src/core/errors.h
    src/core/memory.c
    src/core/memory.h
    src/ecs/world.c
    src/ecs/world.h
    src/engine/app.c
    src/engine/window.c
)

target_include_directories(banjo_engine PUBLIC inc PRIVATE src)
target_link_libraries(banjo_engine 
    PRIVATE
        Vulkan::Vulkan
        glfw
)
target_compile_features(banjo_engine PUBLIC c_std_99)

add_subdirectory(examples)

if (BUILD_TESTING)
    add_subdirectory(test)
endif()

