
if(BJ_FEATURE_MME)
    enable_language(CXX)

    add_executable(olcNoiseMaker olcNoiseMaker.cpp olcNoiseMaker.h)
    target_link_libraries(olcNoiseMaker PRIVATE banjo)
    target_compile_definitions(olcNoiseMaker PRIVATE -DBANJO_ASSETS_DIR="${BANJO_ASSETS_DIR}" UNICODE _UNICODE)
endif()

if(BJ_FEATURE_ALSA)
    find_package(ALSA)
    add_executable(demo_alsa demo_alsa.c)
    target_link_libraries(demo_alsa PRIVATE banjo ALSA::ALSA)
    target_compile_definitions(demo_alsa PRIVATE -DBANJO_ASSETS_DIR="${BANJO_ASSETS_DIR}")
endif()
