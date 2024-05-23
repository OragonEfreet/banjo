#include <banjo/api.h>

#ifdef NDEBUG
#   define BJ_BUILD_DEBUG false
#else
#   define BJ_BUILD_DEBUG true
#endif

#ifdef BJ_FEAT_PEDANTIC_ENABLED
#   define BJ_FEAT_VAL_PEDANTIC true
#else
#   define BJ_FEAT_VAL_PEDANTIC false
#endif

#ifdef BJ_FEAT_LOG_COLOR_ENABLED
#   define BJ_FEAT_VAL_LOG_COLOR true
#else
#   define BJ_FEAT_VAL_LOG_COLOR false
#endif

#ifdef BJ_FEAT_CHECKS_ENABLED
#   define BJ_FEAT_VAL_CHECKS true
#else
#   define BJ_FEAT_VAL_CHECKS false
#endif

#ifdef BJ_FEAT_ABORT_ON_CHECKS_ENABLED
#   define BJ_FEAT_VAL_ABORT_ON_CHECKS true
#else
#   define BJ_FEAT_VAL_ABORT_ON_CHECKS false
#endif

static const bj_build_info s_build_info = {
    .p_name          = BJ_NAME,
    .version         = BJ_VERSION,
    .debug           = BJ_BUILD_DEBUG,
    .pedantic        = BJ_FEAT_VAL_PEDANTIC,
    .log_color       = BJ_FEAT_VAL_LOG_COLOR,
    .checks          = BJ_FEAT_VAL_CHECKS,
    .abort_on_checks = BJ_FEAT_VAL_ABORT_ON_CHECKS,
};

const bj_build_info* bj_get_build_info(void) {
    return &s_build_info;
}

