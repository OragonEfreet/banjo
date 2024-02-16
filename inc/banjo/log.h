#pragma once

#include <banjo/api.h>

enum {
    BJ_LOG_TRACE,
    BJ_LOG_DEBUG,
    BJ_LOG_INFO,
    BJ_LOG_WARN,
    BJ_LOG_ERROR,
    BJ_LOG_FATAL,
};

#define bj_log(LEVEL, ...) bj_message(BJ_LOG_ ## LEVEL, __FILE__, __LINE__, __VA_ARGS__)
#define bj_trace(...)   bj_log(TRACE, __VA_ARGS__)
#define bj_debug(...)   bj_log(DEBUG, __VA_ARGS__)
#define bj_info(...)    bj_log(INFO, __VA_ARGS__)
#define bj_warn(...)    bj_log(WARN, __VA_ARGS__)
#define bj_error(...)   bj_log(ERROR, __VA_ARGS__)
#define bj_fatal(...)   bj_log(FATAL, __VA_ARGS__)

BANJO_EXPORT const char* bj_log_get_levelString(int level);
BANJO_EXPORT void bj_log_set_level(int level);
BANJO_EXPORT int bj_log_get_level(void);
BANJO_EXPORT void bj_message(int level, const char* pFile, int line, const char* pFmt, ...);



