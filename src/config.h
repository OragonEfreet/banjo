#pragma once

////////////////////////////////////////////////////////////////////////////////

#ifdef BJ_CONFIG_EMSCRIPTEN_BACKEND
#   define _BJ_HAS_CONFIG_EMSCRIPTEN_BACKEND 1
#else
#   define _BJ_HAS_CONFIG_EMSCRIPTEN_BACKEND 0
#endif

#ifdef BJ_CONFIG_WIN32_BACKEND
#   define _BJ_HAS_CONFIG_WIN32_BACKEND 1
#else
#   define _BJ_HAS_CONFIG_WIN32_BACKEND 0
#endif

#ifdef BJ_CONFIG_X11_BACKEND
#   define _BJ_HAS_CONFIG_X11_BACKEND 1
#else
#   define _BJ_HAS_CONFIG_X11_BACKEND 0
#endif

#ifdef BJ_CONFIG_COCOA_BACKEND
#   define _BJ_HAS_CONFIG_COCOA_BACKEND 1
#else
#   define _BJ_HAS_CONFIG_COCOA_BACKEND 0
#endif

#ifdef BJ_CONFIG_MME_BACKEND
#   define _BJ_HAS_CONFIG_MME_BACKEND 1
#else
#   define _BJ_HAS_CONFIG_MME_BACKEND 0
#endif

#ifdef BJ_CONFIG_ALSA_BACKEND
#   define _BJ_HAS_CONFIG_ALSA_BACKEND 1
#else
#   define _BJ_HAS_CONFIG_ALSA_BACKEND 0
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

#define BJ_HAS_CONFIG(NAME) _BJ_HAS_CONFIG_ ## NAME


