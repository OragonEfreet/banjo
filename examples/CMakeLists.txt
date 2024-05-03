file(GLOB _bin_source_paths CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.c)

foreach(_bin_source_path ${_bin_source_paths})
    get_filename_component(_target ${_bin_source_path} NAME_WE)
    add_executable(${_target} ${_bin_source_path})
    target_link_libraries(${_target} PRIVATE banjo)
    set_target_properties(${_target} PROPERTIES FOLDER "Examples")

    if (BUILD_TESTING)
        add_test(NAME "[Example] ${_target}" COMMAND ${_target})
    endif()
endforeach()
