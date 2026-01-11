#include <banjo/api.h>
#include <banjo/version.h>

#include <check.h>

#ifdef BJ_BUILD_DEBUG
#   define BJ_BUILD_DEBUG_ENABLED 1
#else
#   define BJ_BUILD_DEBUG_ENABLED 0
#endif

// Convert config defines to boolean values
#ifdef BJ_CONFIG_WIN32_BACKEND
#   define BJ_HAS_WIN32_BACKEND 1
#else
#   define BJ_HAS_WIN32_BACKEND 0
#endif

#ifdef BJ_CONFIG_EMSCRIPTEN_BACKEND
#   define BJ_HAS_EMSCRIPTEN_BACKEND 1
#else
#   define BJ_HAS_EMSCRIPTEN_BACKEND 0
#endif

#ifdef BJ_CONFIG_X11_BACKEND
#   define BJ_HAS_X11_BACKEND 1
#else
#   define BJ_HAS_X11_BACKEND 0
#endif

#ifdef BJ_CONFIG_COCOA_BACKEND
#   define BJ_HAS_COCOA_BACKEND 1
#else
#   define BJ_HAS_COCOA_BACKEND 0
#endif

#ifdef BJ_CONFIG_MME_BACKEND
#   define BJ_HAS_MME_BACKEND 1
#else
#   define BJ_HAS_MME_BACKEND 0
#endif

#ifdef BJ_CONFIG_ALSA_BACKEND
#   define BJ_HAS_ALSA_BACKEND 1
#else
#   define BJ_HAS_ALSA_BACKEND 0
#endif

#ifdef BJ_CONFIG_CHECKS_ABORT
#   define BJ_HAS_CHECKS_ABORT 1
#else
#   define BJ_HAS_CHECKS_ABORT 0
#endif

#ifdef BJ_CONFIG_CHECKS_LOG
#   define BJ_HAS_CHECKS_LOG 1
#else
#   define BJ_HAS_CHECKS_LOG 0
#endif

#ifdef BJ_CONFIG_LOG_COLOR
#   define BJ_HAS_LOG_COLOR 1
#else
#   define BJ_HAS_LOG_COLOR 0
#endif

#ifdef BJ_CONFIG_PEDANTIC
#   define BJ_HAS_PEDANTIC 1
#else
#   define BJ_HAS_PEDANTIC 0
#endif

#ifdef BJ_CONFIG_FASTMATH
#   define BJ_HAS_FASTMATH 1
#else
#   define BJ_HAS_FASTMATH 0
#endif

const struct bj_build_info* bj_build_information(void) {

    static const struct bj_build_info s_build_info = {
        .name             = BJ_NAME,
        .variant          = BJ_NAME_VARIANT,
        .version          = BJ_VERSION,
        .compiler_name    = BJ_COMPILER_NAME,
        .compiler_version = BJ_COMPILER_VERSION,
        .debug            = BJ_BUILD_DEBUG_ENABLED,

        // Backends (alphabetically sorted)
        .backend_alsa       = BJ_HAS_ALSA_BACKEND,
        .backend_cocoa      = BJ_HAS_COCOA_BACKEND,
        .backend_emscripten = BJ_HAS_EMSCRIPTEN_BACKEND,
        .backend_mme        = BJ_HAS_MME_BACKEND,
        .backend_win32      = BJ_HAS_WIN32_BACKEND,
        .backend_x11        = BJ_HAS_X11_BACKEND,

        // Configs (alphabetically sorted)
        .checks_abort = BJ_HAS_CHECKS_ABORT,
        .checks_log   = BJ_HAS_CHECKS_LOG,
        .fastmath     = BJ_HAS_FASTMATH,
        .log_color    = BJ_HAS_LOG_COLOR,
        .pedantic     = BJ_HAS_PEDANTIC,
    };

    return &s_build_info;
}


