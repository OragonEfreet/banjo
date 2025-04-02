#pragma once

#include "config.h"

#if BJ_HAS_CONFIG(CHECKS_ABORT)
#   include <stdlib.h>
#endif
#include <banjo/log.h>
#if BJ_HAS_CONFIG(CHECKS_LOG)
#    ifdef BJ_BUILD_RELEASE
#       define bj_check_err_msg(cond) bj_err("Unrecoverable Error (Failed Check)")
#    else
#       define bj_check_err_msg(cond) bj_err("Failed check: " #cond)
#    endif
#else
#    define bj_check_err_msg(cond)
#endif

// #if BJ_HAS_CONFIG(CHECKS_ABORT)
// #   define bj_check(cond) if(!(cond)) {bj_check_err_msg(cond) ; abort();}
// #   define bj_check_or_return(cond, retval) if(!(cond)) {bj_check_err_msg(cond) ; abort();}
// #   define bj_check_or_0(cond) bj_check_or_return(cond, 0)
// #else
#   define bj_check(cond) if(!(cond)) {bj_check_err_msg(cond) ; return;}
#   define bj_check_or_return(cond, retval) if(!(cond)) {bj_check_err_msg(cond) ; return(retval);}
#   define bj_check_or_0(cond) bj_check_or_return(cond, 0)
// #endif
#undef _bj_check_err_msg
