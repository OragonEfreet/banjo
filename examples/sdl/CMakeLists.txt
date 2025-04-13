include(FetchContent)

set(SDL_AUDIO      ON CACHE BOOL "Override SDL_AUDIO option"      FORCE)
set(SDL_FILE       ON CACHE BOOL "Override SDL_FILE option"       FORCE)
set(SDL_FILESYSTEM ON CACHE BOOL "Override SDL_FILESYSTEM option" FORCE)
set(SDL_HAPTIC     ON CACHE BOOL "Override SDL_HAPTIC option"     FORCE)
set(SDL_JOYSTICK   ON CACHE BOOL "Override SDL_JOYSTICK option"   FORCE)
set(SDL_SENSOR     ON CACHE BOOL "Override SDL_SENSOR option"     FORCE)
set(SDL_ATOMIC     ON CACHE BOOL "Override SDL_ATOMIC option"     FORCE)
set(SDL_CPUINFO    ON CACHE BOOL "Override SDL_CPUINFO option"    FORCE)
set(SDL_LOCALE     ON CACHE BOOL "Override SDL_LOCALE option"     FORCE)
set(SDL_SHARED     ON CACHE BOOL "Override SDL_VIDEO option"      FORCE)
set(SDL_POWER      ON  CACHE BOOL "Override SDL_POWER option"      FORCE)
set(SDL_EVENTS     ON  CACHE BOOL "Override SDL_EVENTS option"     FORCE)
set(SDL_HIDAPI     ON  CACHE BOOL "Override SDL_HIDAPI option"     FORCE)
set(SDL_LOADSO     ON  CACHE BOOL "Override SDL_LOADSO option"     FORCE)
set(SDL_MISC       ON  CACHE BOOL "Override SDL_MISC option"       FORCE)
set(SDL_RENDER     ON  CACHE BOOL "Override SDL_RENDER option"     FORCE)
set(SDL_THREADS    ON  CACHE BOOL "Override SDL_THREADS option"    FORCE)
set(SDL_TIMERS     ON  CACHE BOOL "Override SDL_TIMERS option"     FORCE)
set(SDL_VIDEO      ON  CACHE BOOL "Override SDL_VIDEO option"      FORCE)
set(SDL_STATIC     ON  CACHE BOOL "Override SDL_VIDEO option"      FORCE)

FetchContent_Declare(
    SDL3
    # URL "https://github.com/libsdl-org/SDL/archive/refs/tags/release-2.30.3.zip"
    URL "https://github.com/libsdl-org/SDL/archive/refs/heads/main.zip"
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(SDL3)

file(GLOB _bin_source_paths CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
list(REMOVE_ITEM _bin_source_paths ${CMAKE_CURRENT_SOURCE_DIR}/sdl_helpers.c)

foreach(_bin_source_path ${_bin_source_paths})
    get_filename_component(_target ${_bin_source_path} NAME_WE)
    add_executable(${_target} sdl_helpers.c ${_bin_source_path})
    target_link_libraries(${_target} PRIVATE banjo SDL3::SDL3-static)
    if(NOT MSVC)
        target_link_libraries(${_target} PRIVATE m)
    endif()
    set_target_properties(${_target} PROPERTIES FOLDER "Examples")
    target_compile_definitions(${_target} PRIVATE -DBANJO_ASSETS_DIR="${BANJO_ASSETS_DIR}")
endforeach()



