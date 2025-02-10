#include <banjo/api.h>

#ifdef NDEBUG
#   define BJ_BUILD_DEBUG false
#else
#   define BJ_BUILD_DEBUG true
#endif

#ifdef BJ_CONFIG_PEDANTIC
#   define BJ_CONFIG_PEDANTIC_VALUE true
#else
#   define BJ_CONFIG_PEDANTIC_VALUE false
#endif

#ifdef BJ_CONFIG_LOG_COLOR
#   define BJ_CONFIG_LOG_COLOR_VALUE true
#else
#   define BJ_CONFIG_LOG_COLOR_VALUE false
#endif

#ifdef BJ_CONFIG_CHECKS
#   define BJ_CONFIG_CHECKS_VALUE true
#else
#   define BJ_CONFIG_CHECKS_VALUE false
#endif

#ifdef BJ_CONFIG_CHECKS_LOG
#   define BJ_CONFIG_CHECKS_LOG_VALUE true
#else
#   define BJ_CONFIG_CHECKS_LOG_VALUE false
#endif

#ifdef BJ_CONFIG_CHECKS_ABORT
#   define BJ_CONFIG_CHECKS_ABORT_VALUE true
#else
#   define BJ_CONFIG_CHECKS_ABORT_VALUE false
#endif

static const bj_build_info s_build_info = {
    .p_name          = BJ_NAME,
    .version         = BJ_VERSION,
    .debug           = BJ_BUILD_DEBUG,

    // Options
    .config_checks       = BJ_CONFIG_CHECKS_VALUE,
    .config_checks_abort = BJ_CONFIG_CHECKS_ABORT_VALUE,
    .config_checks_log   = BJ_CONFIG_CHECKS_LOG_VALUE,
    .config_log_color    = BJ_CONFIG_LOG_COLOR_VALUE,
    .config_pedantic     = BJ_CONFIG_PEDANTIC_VALUE,
};

const bj_build_info* bj_get_build_info(void) {
    return &s_build_info;
}

