#ifndef BJ_ASSERT_H
#define BJ_ASSERT_H

#include <banjo/api.h>
#include <banjo/log.h>

#include <stdlib.h>

#ifdef BJ_BUILD_RELEASE
  #define bj_assert(expr) ((void)0)
#else
  #define bj_assert(expr) ((expr) ? (void)0 : (bj_fatal("Assertion failed: %s", #expr), abort(), (void)0))
#endif

#endif

