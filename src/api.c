#include <banjo/api.h>
#include "config.h"

#ifdef BJ_BUILD_RELEASE
#   undef BJ_BUILD_RELEASE
#   define BJ_BUILD_RELEASE true
#else
#   define BJ_BUILD_RELEASE false
#endif

#ifdef BJ_BUILD_DEBUG
#   undef BJ_BUILD_DEBUG
#   define BJ_BUILD_DEBUG true
#else
#   define BJ_BUILD_DEBUG false
#endif

#if BJ_HAS_FEATURE(WIN32)
#   undef BJ_FEATURE_WIN32
#   define BJ_FEATURE_WIN32 true
#else
#   define BJ_FEATURE_WIN32 false
#endif

#if BJ_HAS_FEATURE(X11)
#   undef BJ_FEATURE_X11
#   define BJ_FEATURE_X11 true
#else
#   define BJ_FEATURE_X11 false
#endif

#if BJ_HAS_CONFIG(LOG_COLOR)
#   undef BJ_CONFIG_LOG_COLOR
#   define BJ_CONFIG_LOG_COLOR true
#else
#   define BJ_CONFIG_LOG_COLOR false
#endif

#if BJ_HAS_CONFIG(CHECKS)
#   undef BJ_CONFIG_CHECKS
#   define BJ_CONFIG_CHECKS true
#else
#   define BJ_CONFIG_CHECKS false
#endif

#if BJ_HAS_CONFIG(CHECKS_ABORT)
#   undef BJ_CONFIG_CHECKS_ABORT
#   define BJ_CONFIG_CHECKS_ABORT true
#else
#   define BJ_CONFIG_CHECKS_ABORT false
#endif

#if BJ_HAS_CONFIG(CHECKS_LOG)
#   undef BJ_CONFIG_CHECKS_LOG
#   define BJ_CONFIG_CHECKS_LOG true
#else
#   define BJ_CONFIG_CHECKS_LOG false
#endif

#if BJ_HAS_CONFIG(PEDANTIC)
#   undef BJ_CONFIG_PEDANTIC
#   define BJ_CONFIG_PEDANTIC true
#else
#   define BJ_CONFIG_PEDANTIC false
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
    .config_checks       = BJ_CONFIG_CHECKS,
    .config_checks_abort = BJ_CONFIG_CHECKS_ABORT,
    .config_checks_log   = BJ_CONFIG_CHECKS_LOG,
    .config_log_color    = BJ_CONFIG_LOG_COLOR,
    .config_pedantic     = BJ_CONFIG_PEDANTIC,

};

#include <banjo/log.h>
const bj_build_info* bj_get_build_info(void) {
    return &s_build_info;
}

