#include <banjo/api.h>

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

