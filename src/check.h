#pragma once



#ifdef BJ_CONFIG_CHECKS_ABORT
#   include <stdlib.h>
#endif
#include <banjo/log.h>
#ifdef BJ_CONFIG_CHECKS_LOG
#    ifdef BJ_BUILD_RELEASE
#       define bj_check_err_msg(cond) bj_err("Unrecoverable Error (Failed Check)")
#    else
#       define bj_check_err_msg(cond) bj_err("Failed check: " #cond)
#    endif
#else
#    define bj_check_err_msg(cond)
#endif

#define bj_check(cond) if(!(cond)) {bj_check_err_msg(cond) ; return;}
#define bj_check_or_return(cond, retval) if(!(cond)) {bj_check_err_msg(cond) ; return(retval);}
#define bj_check_or_0(cond) bj_check_or_return(cond, 0)
#undef _bj_check_err_msg
