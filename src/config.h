#pragma once

/// Features
// #define BJ_FEATURE_WIN32
// #define BJ_FEATURE_X11

/// Configuration option

// #define BJ_CONFIG_ALL
// #define BJ_CONFIG_CHECKS
// #define BJ_CONFIG_CHECKS_ABORT
// #define BJ_CONFIG_CHECKS_LOG
// #define BJ_CONFIG_LOG_COLOR
// #define BJ_CONFIG_PEDANTIC

#define BJ_HAS_CONFIG(NAME) (defined(BJ_CONFIG_ALL) || defined(BJ_CONFIG_ ## NAME))
#define BJ_HAS_FEATURE(NAME) (defined(BJ_FEATURE_ ## NAME))

#if BJ_HAS_CONFIG(CHECKS)
#    if BJ_HAS_CONFIG(CHECKS_ABORT)
#       include <stdlib.h>
#    endif
#    include <banjo/log.h>
#   if BJ_HAS_CONFIG(CHECKS_LOG)
#       ifdef BJ_BUILD_RELEASE
#          define bj_check_err_msg(cond) bj_err("Unrecoverable Error (Failed Check)")
#       else
#          define bj_check_err_msg(cond) bj_err("Failed check: " #cond)
#       endif
#   else
#       define bj_check_err_msg(cond)
#   endif
#   if BJ_HAS_CONFIG(CHECKS_ABORT)
#      define bj_check(cond) if(!(cond)) {bj_check_err_msg(cond) ; abort();}
#      define bj_check_or_return(cond, retval) if(!(cond)) {bj_check_err_msg(cond) ; abort();}
#      define bj_check_or_0(cond) bj_check_or_return(cond, 0)
#   else
#      define bj_check(cond) if(!(cond)) {bj_check_err_msg(cond) ; return;}
#      define bj_check_or_return(cond, retval) if(!(cond)) {bj_check_err_msg(cond) ; return(retval);}
#      define bj_check_or_0(cond) bj_check_or_return(cond, 0)
#   endif
#   undef _bj_check_err_msg
#else
#   define bj_check(cond)
#   define bj_check_or_return(cond, retval)
#   define bj_check_or_0(cond)
#endif

