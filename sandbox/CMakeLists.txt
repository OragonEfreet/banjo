
if(MSVC)
    enable_language(CXX)

    add_executable(audio_windows audio_windows.c music.h)
    target_link_libraries(audio_windows PRIVATE banjo)
    target_compile_definitions(audio_windows PRIVATE -DBANJO_ASSETS_DIR="${BANJO_ASSETS_DIR}")

    
    add_executable(olcNoiseMaker olcNoiseMaker.cpp olcNoiseMaker.h)
    target_link_libraries(olcNoiseMaker PRIVATE banjo)
    target_compile_definitions(olcNoiseMaker PRIVATE -DBANJO_ASSETS_DIR="${BANJO_ASSETS_DIR}" UNICODE _UNICODE)
endif()