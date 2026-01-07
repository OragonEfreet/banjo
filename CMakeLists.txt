cmake_minimum_required(VERSION 3.21)
project(banjo VERSION 0.1.0 LANGUAGES C)

include(CTest)

if (MSVC)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    add_compile_options(/W4 /D_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra)
endif()

add_library(banjo
    src/api.c
    src/cli.c
    src/audio.c
    src/audio_layer.h
    src/audio.h
    src/bitmap.c
    src/bitmap_blit.c
    src/bitmap_blit_mask.c
    src/bitmap_charsets.h
    src/bitmap_dib.c
    src/bitmap_draw.c
    src/bitmap.h
    src/bitmap_text.c
    src/check.h
    src/error.c
    src/event.c
    src/geometry_2d.c
    src/log.c
    src/main.c
    src/main_callbacks.c
    src/memory.c
    src/physics_angular.c
    src/physics_kinematics.c
    src/physics_particle.c
    src/physics_rigid_body.c
    src/pixel.c
    src/random.c
    src/random_distribution.c
    src/random_pcg32.c
    src/rect.c
    src/renderer.c
    src/renderer.h
    src/shader.c
    src/stream.c
    src/stream.h
    src/system.c
    src/time.c
    src/video.c
    src/video_layer.h
    src/window.c
    src/window.h
    inc/banjo/api.h
    inc/banjo/cli.h
    inc/banjo/assert.h
    inc/banjo/audio.h
    inc/banjo/bitmap.h
    inc/banjo/draw.h
    inc/banjo/error.h
    inc/banjo/event.h
    inc/banjo/geometry_2d.h
    inc/banjo/log.h
    inc/banjo/main.h
    inc/banjo/mat.h
    inc/banjo/math.h
    inc/banjo/memory.h
    inc/banjo/physics_2d.h
    inc/banjo/physics.h
    inc/banjo/pixel.h
    inc/banjo/quat.h
    inc/banjo/random.h
    inc/banjo/rect.h
    inc/banjo/renderer.h
    inc/banjo/shader.h
    inc/banjo/stream.h
    inc/banjo/string.h
    inc/banjo/system.h
    inc/banjo/time.h
    inc/banjo/vec.h
    inc/banjo/window.h
)

if(WIN32)
    target_sources(banjo PRIVATE 
        src/win32/system_win32.c
        src/win32/time_win32.c
    )
else()
    target_sources(banjo PRIVATE 
        src/unix/system_unix.c
        src/unix/time_unix.c
    )
endif()

target_include_directories(banjo
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        src
)
set_target_properties(banjo PROPERTIES C_STANDARD 99 C_STANDARD_REQUIRED TRUE)

if(NOT MSVC)
    target_link_libraries(banjo PUBLIC m)
endif()

if(BUILD_SHARED_LIBS)
    target_compile_definitions(banjo PRIVATE BANJO_EXPORTS)
else()
    target_compile_definitions(banjo PUBLIC BANJO_STATIC)
endif()

target_compile_definitions(banjo PRIVATE BJ_NO_TYPEDEF)

################################################################################
### Win32 Backend
################################################################################
if(WIN32)
    set(BJ_WIN32_AVAILABLE ON)
else()
    set(BJ_WIN32_AVAILABLE OFF)
endif()
message(STATUS "Win32 backend: ${BJ_WIN32_AVAILABLE}")

option(BANJO_CONFIG_WIN32_BACKEND "Add Win32 support" ${BJ_WIN32_AVAILABLE})

if(BANJO_CONFIG_WIN32_BACKEND)
    target_compile_definitions(banjo PUBLIC BJ_CONFIG_WIN32_BACKEND)
    target_sources(banjo PRIVATE src/win32/video_win32.c)
endif()

################################################################################
### X11 Backend
################################################################################
find_package(X11 QUIET)
if(X11_FOUND)
    set(BJ_X11_AVAILABLE ON)
else()
    set(BJ_X11_AVAILABLE OFF)
endif()
message(STATUS "X11 backend: ${BJ_X11_AVAILABLE}")

option(BANJO_CONFIG_X11_BACKEND "Add X11 support" ${BJ_X11_AVAILABLE})

if(BANJO_CONFIG_X11_BACKEND AND X11_FOUND)
    target_compile_definitions(banjo PUBLIC BJ_CONFIG_X11_BACKEND)
    target_sources(banjo PRIVATE src/x11/video_x11.c)
    target_include_directories(banjo PRIVATE ${X11_INCLUDE_DIR})
    target_link_libraries(banjo PRIVATE ${X11_LIBRARIES})
endif()

################################################################################
### Cocoa Backend
################################################################################
if(APPLE)
    set(BJ_COCOA_AVAILABLE ON)
else()
    set(BJ_COCOA_AVAILABLE OFF)
endif()
message(STATUS "Cocoa backend: ${BJ_COCOA_AVAILABLE}")

option(BANJO_CONFIG_COCOA_BACKEND "Add Cocoa/macOS support" ${BJ_COCOA_AVAILABLE})

if(BANJO_CONFIG_COCOA_BACKEND AND APPLE)
    target_compile_definitions(banjo PUBLIC BJ_CONFIG_COCOA_BACKEND)
    target_sources(banjo PRIVATE src/cocoa/video_cocoa.m)
    target_link_libraries(banjo PUBLIC "-framework Cocoa")
endif()

################################################################################
### Emscripten Backend
################################################################################
if(EMSCRIPTEN)
    set(BJ_EMSCRIPTEN_AVAILABLE ON)
else()
    set(BJ_EMSCRIPTEN_AVAILABLE OFF)
endif()
message(STATUS "Emscripten backend: ${BJ_EMSCRIPTEN_AVAILABLE}")

option(BANJO_CONFIG_EMSCRIPTEN_BACKEND "Add support for Emscripten" ${BJ_EMSCRIPTEN_AVAILABLE})

if(BANJO_CONFIG_EMSCRIPTEN_BACKEND)
    target_compile_definitions(banjo PUBLIC BJ_CONFIG_EMSCRIPTEN_BACKEND)
    target_sources(banjo PRIVATE src/emscripten/video_emscripten.c src/emscripten/audio_emscripten.c)
    target_link_options(banjo PRIVATE
        "-sEXPORTED_RUNTIME_METHODS=['ccall','cwrap','_malloc','_free']"
        "-sEXPORTED_FUNCTIONS=['_bj_emscripten_audio_process']"
        "-sALLOW_MEMORY_GROWTH"
    )
endif()

################################################################################
### ALSA Backend
################################################################################
find_package(ALSA QUIET)
if(ALSA_FOUND)
    set(BJ_ALSA_AVAILABLE ON)
else()
    set(BJ_ALSA_AVAILABLE OFF)
endif()
message(STATUS "ALSA backend: ${BJ_ALSA_AVAILABLE}")

option(BANJO_CONFIG_ALSA_BACKEND "Add support for audio with ALSA" ${BJ_ALSA_AVAILABLE})

if(BANJO_CONFIG_ALSA_BACKEND AND ALSA_FOUND)
    target_compile_definitions(banjo PUBLIC BJ_CONFIG_ALSA_BACKEND)
    target_sources(banjo PRIVATE src/alsa/audio_alsa.c)
    target_include_directories(banjo PRIVATE ${ALSA_INCLUDE_DIRS})
    target_link_libraries(banjo PRIVATE ${ALSA_LIBRARIES})
endif()

################################################################################
### MME Backend
################################################################################
if(WIN32)
    set(BJ_MME_AVAILABLE ON)
else()
    set(BJ_MME_AVAILABLE OFF)
endif()
message(STATUS "MME backend: ${BJ_MME_AVAILABLE}")

option(BANJO_CONFIG_MME_BACKEND "Add support for audio with Windows Multimedia Extensions" ${BJ_MME_AVAILABLE})

if(BANJO_CONFIG_MME_BACKEND)
    target_compile_definitions(banjo PUBLIC BJ_CONFIG_MME_BACKEND)
    target_sources(banjo PRIVATE src/mme/audio_mme.c)
endif()

################################################################################
### CoreAudio Backend
################################################################################
if(APPLE)
    set(BJ_COREAUDIO_AVAILABLE ON)
else()
    set(BJ_COREAUDIO_AVAILABLE OFF)
endif()
message(STATUS "CoreAudio backend: ${BJ_COREAUDIO_AVAILABLE}")

option(BANJO_CONFIG_COREAUDIO_BACKEND "Add support for audio with CoreAudio (macOS)" ${BJ_COREAUDIO_AVAILABLE})

if(BANJO_CONFIG_COREAUDIO_BACKEND AND APPLE)
    target_compile_definitions(banjo PUBLIC BJ_CONFIG_COREAUDIO_BACKEND)
    target_sources(banjo PRIVATE src/coreaudio/audio_coreaudio.c)
    target_link_libraries(banjo PUBLIC "-framework AudioToolbox")
endif()

################################################################################
### Configuration Options
################################################################################
option(BANJO_CONFIG_CHECKS_ABORT "When checks feature is on, a failed check will abort execution" OFF)
if(BANJO_CONFIG_CHECKS_ABORT)
    target_compile_definitions(banjo PUBLIC BJ_CONFIG_CHECKS_ABORT)
endif()

option(BANJO_CONFIG_CHECKS_LOG "If checks feature is on, failed check with log" OFF)
if(BANJO_CONFIG_CHECKS_LOG)
    target_compile_definitions(banjo PUBLIC BJ_CONFIG_CHECKS_LOG)
endif()

option(BANJO_CONFIG_LOG_COLOR "Banjo logs will have colored output" OFF)
if(BANJO_CONFIG_LOG_COLOR)
    target_compile_definitions(banjo PUBLIC BJ_CONFIG_LOG_COLOR)
endif()

option(BANJO_CONFIG_PEDANTIC "Banjo runtime will make costly extra checks" OFF)
if(BANJO_CONFIG_PEDANTIC)
    target_compile_definitions(banjo PUBLIC BJ_CONFIG_PEDANTIC)
endif()

option(BANJO_CONFIG_FASTMATH "Enable fast math (FMA, reassociation, etc.)" OFF)
if(BANJO_CONFIG_FASTMATH)
    target_compile_definitions(banjo PUBLIC BJ_CONFIG_FASTMATH)
    if (MSVC)
        target_compile_options(banjo PRIVATE /fp:fast)
    else()
        target_compile_options(banjo PRIVATE
            -ffast-math
            -ffp-contract=fast
            -fno-math-errno
            -fno-trapping-math
        )
    endif()
endif()

################################################################################
### Installation
################################################################################
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Install the library
install(TARGETS banjo
    EXPORT banjo-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install public headers
install(DIRECTORY inc/banjo
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

# Generate and install CMake package configuration files
set(BANJO_CMAKE_CONFIG_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/banjo)

install(EXPORT banjo-targets
    FILE banjo-targets.cmake
    NAMESPACE banjo::
    DESTINATION ${BANJO_CMAKE_CONFIG_INSTALL_DIR}
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/banjo-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/banjo-config.cmake
    INSTALL_DESTINATION ${BANJO_CMAKE_CONFIG_INSTALL_DIR}
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/banjo-config-version.cmake
    VERSION 0.1.0
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/banjo-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/banjo-config-version.cmake
    DESTINATION ${BANJO_CMAKE_CONFIG_INSTALL_DIR}
)

# Generate and install pkg-config file
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/banjo.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/banjo.pc
    @ONLY
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/banjo.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

################################################################################
### Examples and Tests
################################################################################
set(BANJO_ASSETS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets)

option(BANJO_BUILD_EXAMPLES "Build Banjo Examples" FALSE)
if(BANJO_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if (BUILD_TESTING)
    add_subdirectory(test)
endif()
