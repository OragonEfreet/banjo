#pragma once

////////////////////////////////////////////////////////////////////////////////

#ifdef BJ_FEATURE_EMSCRIPTEN
#   define _BJ_HAS_FEATURE_EMSCRIPTEN 1
#else
#   define _BJ_HAS_FEATURE_EMSCRIPTEN 0
#endif

#ifdef BJ_FEATURE_WIN32
#   define _BJ_HAS_FEATURE_WIN32 1
#else
#   define _BJ_HAS_FEATURE_WIN32 0
#endif

#ifdef BJ_FEATURE_X11
#   define _BJ_HAS_FEATURE_X11 1
#else
#   define _BJ_HAS_FEATURE_X11 0
#endif

#ifdef BJ_FEATURE_MME
#   define _BJ_HAS_FEATURE_MME 1
#else
#   define _BJ_HAS_FEATURE_MME 0
#endif

#ifdef BJ_FEATURE_ALSA
#   define _BJ_HAS_FEATURE_ALSA 1
#else
#   define _BJ_HAS_FEATURE_ALSA 0
#endif

#ifdef BJ_CONFIG_ALL
#   define _BJ_HAS_CONFIG_ALL 1
#else
#   define _BJ_HAS_CONFIG_ALL 0
#endif

#ifdef BJ_CONFIG_CHECKS_ABORT
#   define _BJ_HAS_CONFIG_CHECKS_ABORT 1
#else
#   define _BJ_HAS_CONFIG_CHECKS_ABORT 0
#endif

#ifdef BJ_CONFIG_CHECKS_LOG
#   define _BJ_HAS_CONFIG_CHECKS_LOG 1
#else
#   define _BJ_HAS_CONFIG_CHECKS_LOG 0
#endif

#ifdef BJ_CONFIG_LOG_COLOR
#   define _BJ_HAS_CONFIG_LOG_COLOR 1
#else
#   define _BJ_HAS_CONFIG_LOG_COLOR 0
#endif

#ifdef BJ_CONFIG_PEDANTIC
#   define _BJ_HAS_CONFIG_PEDANTIC 1
#else
#   define _BJ_HAS_CONFIG_PEDANTIC 0
#endif

#ifdef BJ_CONFIG_FASTMATH
#   define _BJ_HAS_CONFIG_FASTMATH 1
#else
#   define _BJ_HAS_CONFIG_FASTMATH 0
#endif

#define BJ_HAS_CONFIG(NAME) _BJ_HAS_CONFIG_ALL || _BJ_HAS_CONFIG_ ## NAME
#define BJ_HAS_FEATURE(NAME) _BJ_HAS_FEATURE_ ## NAME


