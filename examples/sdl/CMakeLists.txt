find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)

if(TARGET SDL2::SDL2main)
        file(GLOB _bin_source_paths CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.c)

        foreach(_bin_source_path ${_bin_source_paths})
            get_filename_component(_target ${_bin_source_path} NAME_WE)
            add_executable(${_target} ${_bin_source_path})
            target_link_libraries(${_target} PRIVATE banjo)
            set_target_properties(${_target} PROPERTIES FOLDER "Examples")
        endforeach()

        if(TARGET SDL2::SDL2main)
            target_link_libraries(${_target} PRIVATE SDL2::SDL2main)
        endif()
        target_link_libraries(${_target} PRIVATE SDL2::SDL2 m)
endif()



