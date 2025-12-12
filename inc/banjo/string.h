////////////////////////////////////////////////////////////////////////////////
/// \file string.h
/// \brief String utility functions
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// \defgroup string String Utilities
///
/// \brief Lightweight wrappers around standard C string functions
///
/// This module provides banjo-prefixed aliases for common string operations
/// from the C standard library. These macros maintain API consistency while
/// delegating to the standard implementations.
///
/// \{
////////////////////////////////////////////////////////////////////////////////
#ifndef BJ_STRING_H
#define BJ_STRING_H

#include <string.h>

////////////////////////////////////////////////////////////////////////////////
/// \def bj_strlen
/// \brief Calculate the length of a null-terminated string.
///
/// Wrapper around the standard C library strlen function.
///
/// \param s Pointer to a null-terminated string.
/// \return The number of characters in the string, excluding the null terminator.
///
/// \see strlen
////////////////////////////////////////////////////////////////////////////////
#define bj_strlen strlen

////////////////////////////////////////////////////////////////////////////////
/// \def bj_strcmp
/// \brief Compare two null-terminated strings lexicographically.
///
/// Wrapper around the standard C library strcmp function.
///
/// \param s1 Pointer to the first null-terminated string.
/// \param s2 Pointer to the second null-terminated string.
/// \return An integer less than, equal to, or greater than zero if s1 is found,
///         respectively, to be less than, to match, or be greater than s2.
///
/// \see strcmp
////////////////////////////////////////////////////////////////////////////////
#define bj_strcmp strcmp

#endif /* BJ_STRING_H */
/// \} // end of string group

