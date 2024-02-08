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

#define bjLog(LEVEL, ...) bjLogMsg(BJ_LOG_ ## LEVEL, __FILE__, __LINE__, __VA_ARGS__)
#define bjLogTrace(...)   bjLog(TRACE, __VA_ARGS__)
#define bjLogDebug(...)   bjLog(DEBUG, __VA_ARGS__)
#define bjLogInfo(...)    bjLog(INFO, __VA_ARGS__)
#define bjLogWarn(...)    bjLog(WARN, __VA_ARGS__)
#define bjLogError(...)   bjLog(ERROR, __VA_ARGS__)
#define bjLogFatal(...)   bjLog(FATAL, __VA_ARGS__)

BANJO_EXPORT const char* bjGetLogLevelString(int level);
BANJO_EXPORT void bjSetLogLevel(int level);
BANJO_EXPORT int bjGetLogLevel();
BANJO_EXPORT void bjLogMsg(int level, const char* pFile, int line, const char* pFmt, ...);



