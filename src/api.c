#include <banjo/api.h>

#include "config.h"

#ifdef BJ_BUILD_DEBUG
#   undef BJ_BUILD_DEBUG
#   define BJ_BUILD_DEBUG 1
#else
#   define BJ_BUILD_DEBUG 0
#endif

#ifdef BJ_FEATURE_WIN32
#   undef BJ_FEATURE_WIN32
#   define BJ_FEATURE_WIN32 1
#else
#   define BJ_FEATURE_WIN32 0
#endif

#ifdef BJ_FEATURE_X11
#   undef BJ_FEATURE_X11
#   define BJ_FEATURE_X11 1
#else
#   define BJ_FEATURE_X11 0
#endif

#ifdef BJ_CONFIG_ALL
#   undef BJ_CONFIG_ALL
#   define BJ_CONFIG_ALL 1
#else
#   define BJ_CONFIG_ALL 0
#endif

#ifdef BJ_CONFIG_CHECKS_ABORT
#   undef BJ_CONFIG_CHECKS_ABORT
#   define BJ_CONFIG_CHECKS_ABORT 1
#else
#   define BJ_CONFIG_CHECKS_ABORT 0
#endif

#ifdef BJ_CONFIG_CHECKS_LOG
#   undef BJ_CONFIG_CHECKS_LOG
#   define BJ_CONFIG_CHECKS_LOG 1
#else
#   define BJ_CONFIG_CHECKS_LOG 0
#endif

#ifdef BJ_CONFIG_LOG_COLOR
#   undef BJ_CONFIG_LOG_COLOR
#   define BJ_CONFIG_LOG_COLOR 1
#else
#   define BJ_CONFIG_LOG_COLOR 0
#endif

#ifdef BJ_CONFIG_PEDANTIC
#   undef BJ_CONFIG_PEDANTIC
#   define BJ_CONFIG_PEDANTIC 1
#else
#   define BJ_CONFIG_PEDANTIC 0
#endif




static const bj_build_info s_build_info = {
    .p_name           = BJ_NAME,
    .version          = BJ_VERSION,
    .compiler_name    = BJ_COMPILER_NAME,
    .compiler_version = BJ_COMPILER_VERSION,
    .debug            = BJ_BUILD_DEBUG,

    // Features
    .feature_win32 = BJ_FEATURE_WIN32,
    .feature_x11   = BJ_FEATURE_X11,

    // Options
    .config_checks_abort = BJ_CONFIG_CHECKS_ABORT,
    .config_checks_log   = BJ_CONFIG_CHECKS_LOG,
    .config_log_color    = BJ_CONFIG_LOG_COLOR,
    .config_pedantic     = BJ_CONFIG_PEDANTIC,

};

const bj_build_info* bj_get_build_info(void) {
    return &s_build_info;
}

