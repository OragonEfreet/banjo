#pragma once

/// Features
#define BJ_FEATURE_WIN32
#define BJ_FEATURE_X11

/// Configuration option
#define BJ_CONFIG_ALL
#define BJ_CONFIG_CHECKS_ABORT
#define BJ_CONFIG_CHECKS_LOG
#define BJ_CONFIG_LOG_COLOR
#define BJ_CONFIG_PEDANTIC



#define BJ_HAS_CONFIG(NAME) BJ_CONFIG_ALL || BJ_CONFIG_ ## NAME
#define BJ_HAS_FEATURE(NAME) BJ_FEATURE_ ## NAME


