
macro(gen_test category pattern)
    file(GLOB _bin_source_paths CONFIGURE_DEPENDS ${pattern})

    foreach(_bin_source_path ${_bin_source_paths})
        get_filename_component(_test_name ${_bin_source_path} NAME_WE)
        set(_target test_${_test_name})
        add_executable(${_target} ${_bin_source_path})
        target_include_directories(${_target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src) # Mouof
        target_link_libraries(${_target} PRIVATE banjo)
        set_target_properties(${_target} PROPERTIES OUTPUT_NAME ${_test_name})
        add_test(NAME "[${category}] ${_test_name}" COMMAND ${_target})
        set_target_properties(${_target} PROPERTIES FOLDER "Tests")
        if (BANJO_CONFIG_EMSCRIPTEN_BACKEND)
            set_target_properties(${_target} PROPERTIES SUFFIX ".html")
        endif()
        target_compile_definitions(${_target} PRIVATE -DBANJO_ASSETS_DIR="${BANJO_ASSETS_DIR}")
    endforeach()
endmacro()

gen_test(EdgeCases "${CMAKE_CURRENT_SOURCE_DIR}/edge_cases_*.c")
gen_test(Integration "${CMAKE_CURRENT_SOURCE_DIR}/integration_*.c")
gen_test(Property "${CMAKE_CURRENT_SOURCE_DIR}/property_*.c")
gen_test(Stress "${CMAKE_CURRENT_SOURCE_DIR}/stress_*.c")
gen_test(TDD "${CMAKE_CURRENT_SOURCE_DIR}/tdd_*.c")
gen_test(Unit "${CMAKE_CURRENT_SOURCE_DIR}/obj_*.c")
gen_test(Unit "${CMAKE_CURRENT_SOURCE_DIR}/unit_*.c")
gen_test(Validation "${CMAKE_CURRENT_SOURCE_DIR}/valid_*.c")


