////////////////////////////////////////////////////////////////////////////////
/// \file assert.h
/// \brief Assertion facility for Banjo API.
///
/// Provides the bj_assert macro, similar to the C standard assert(),
/// with behavior controlled by the build configuration.
///
/// - In debug builds (\ref BJ_BUILD_DEBUG), failed assertions log a fatal
///   error and terminate the process with abort().
/// - In release builds (\ref BJ_BUILD_RELEASE), assertions are disabled
///   and compile to a no-op.
///
/// \ingroup core
////////////////////////////////////////////////////////////////////////////////
#ifndef BJ_ASSERT_H
#define BJ_ASSERT_H

#include <banjo/api.h>
#include <banjo/log.h>
#include <stdlib.h>

////////////////////////////////////////////////////////////////////////////////
/// \def bj_assert
/// \brief Runtime assertion macro.
///
/// Evaluates the given expression and verifies it is non-zero (true).
/// - In debug builds, if the expression evaluates to false, a fatal log
///   is emitted and the program is terminated with abort().
/// - In release builds, the macro does nothing.
///
/// \param expr Expression to check for truth.
////////////////////////////////////////////////////////////////////////////////
#ifdef BJ_BUILD_RELEASE
  #define bj_assert(expr) ((void)0)
#else
  #define bj_assert(expr) \
    ((expr) ? (void)0 : (bj_fatal("Assertion failed: %s", #expr), abort(), (void)0))
#endif

#endif /* BJ_ASSERT_H */
