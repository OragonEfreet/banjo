file(GLOB _bin_source_paths CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.c)

foreach(_bin_source_path ${_bin_source_paths})
    get_filename_component(_target ${_bin_source_path} NAME_WE)

    if(WIN32)
        add_executable(${_target} WIN32 ${_bin_source_path})    
    else()
        add_executable(${_target} ${_bin_source_path})        
    endif()
    target_link_libraries(${_target} PRIVATE banjo)
    set_target_properties(${_target} PROPERTIES FOLDER "Examples")
    if (BANJO_CONFIG_EMSCRIPTEN_BACKEND)
        set_target_properties(${_target} PROPERTIES SUFFIX ".html")
    endif()
    target_compile_definitions(${_target} PRIVATE -DBANJO_ASSETS_DIR="${BANJO_ASSETS_DIR}")

    # if (BUILD_TESTING)
    #         add_test(NAME "[Example] ${_target}" COMMAND ${_target})
    # endif()
endforeach()



