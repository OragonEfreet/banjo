#include <banjo/api.h>

#include "check.h"

#ifdef BJ_BUILD_DEBUG
#   define BJ_BUILD_DEBUG_ENABLED 1
#else
#   define BJ_BUILD_DEBUG_ENABLED 0
#endif

#if BJ_HAS_FEATURE(EMSCRIPTEN)
#   define BJ_INFO_EMSCRIPTEN 1
#else
#   define BJ_INFO_EMSCRIPTEN 0
#endif

#if BJ_HAS_FEATURE(WIN32)
#   define BJ_INFO_WIN32 1
#else
#   define BJ_INFO_WIN32 0
#endif

#if BJ_HAS_FEATURE(X11)
#   define BJ_INFO_X11 1
#else
#   define BJ_INFO_X11 0
#endif

#if BJ_HAS_FEATURE(ALSA)
#   define BJ_INFO_ALSA 1
#else
#   define BJ_INFO_ALSA 0
#endif

#if BJ_HAS_FEATURE(MME)
#   define BJ_INFO_MME 1
#else
#   define BJ_INFO_MME 0
#endif

#if BJ_HAS_CONFIG(CHECKS_ABORT)
#   define BJ_INFO_CHECKS_ABORT 1
#else
#   define BJ_INFO_CHECKS_ABORT 0
#endif

#if BJ_HAS_CONFIG(CHECKS_LOG)
#   define BJ_INFO_CHECKS_LOG 1
#else
#   define BJ_INFO_CHECKS_LOG 0
#endif

#if BJ_HAS_CONFIG(LOG_COLOR)
#   define BJ_INFO_LOG_COLOR 1
#else
#   define BJ_INFO_LOG_COLOR 0
#endif

#if BJ_HAS_CONFIG(PEDANTIC)
#   define BJ_INFO_PEDANTIC 1
#else
#   define BJ_INFO_PEDANTIC 0
#endif

static const bj_build_info s_build_info = {
    .p_name           = BJ_NAME,
    .version          = BJ_VERSION,
    .compiler_name    = BJ_COMPILER_NAME,
    .compiler_version = BJ_COMPILER_VERSION,
    .debug            = BJ_BUILD_DEBUG_ENABLED,

    // Features
    .feature_win32      = BJ_INFO_WIN32,
    .feature_emscripten = BJ_INFO_EMSCRIPTEN,
    .feature_x11        = BJ_INFO_X11,
    .feature_mme        = BJ_INFO_MME,
    .feature_alsa       = BJ_INFO_ALSA,

    // Options
    .config_checks_abort = BJ_INFO_CHECKS_ABORT,
    .config_checks_log   = BJ_INFO_CHECKS_LOG,
    .config_log_color    = BJ_INFO_LOG_COLOR,
    .config_pedantic     = BJ_INFO_PEDANTIC,

};

const bj_build_info* bj_get_build_info(void) {
    return &s_build_info;
}


