#pragma once

/// Features
#define BJ_FEATURE_WIN32 0
#define BJ_FEATURE_X11   0

/// Configuration option
#define BJ_CONFIG_ALL          1
#define BJ_CONFIG_CHECKS_ABORT 0
#define BJ_CONFIG_CHECKS_LOG   0
#define BJ_CONFIG_LOG_COLOR    0
#define BJ_CONFIG_PEDANTIC     0

#define BJ_HAS_CONFIG(NAME) BJ_CONFIG_ALL || BJ_CONFIG_ ## NAME
#define BJ_HAS_FEATURE(NAME) BJ_FEATURE_ ## NAME


