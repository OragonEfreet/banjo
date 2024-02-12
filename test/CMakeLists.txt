file(GLOB _bin_source_paths CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.c)

foreach(_bin_source_path ${_bin_source_paths})
    get_filename_component(_test_name ${_bin_source_path} NAME_WE)
    set(_target test_${_test_name})
    add_executable(${_target} ${_bin_source_path})
    target_include_directories(${_target} PRIVATE ${CMAKE_SOURCE_DIR}/src/data)
    target_link_libraries(${_target} PRIVATE banjo)
    set_target_properties(${_target} PROPERTIES OUTPUT_NAME ${_test_name})
    add_test(NAME ${_test_name} COMMAND ${_target})
endforeach()

