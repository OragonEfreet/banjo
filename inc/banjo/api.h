////////////////////////////////////////////////////////////////////////////////
/// \file
/// General-purpose definitions for Banjo API.

////////////////////////////////////////////////////////////////////////////////
/// \defgroup core Core
/// General-purpose API
///
/// \{
#pragma once

#ifndef NDEBUG
#  include <assert.h>
#endif
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/// Declares an opaque handle type to `object`
#define BJ_DEFINE_HANDLE(object) typedef struct object##_T* object

/// Value for a handle pointing to nothing
#define BJ_NULL_HANDLE 0

#define BJ_VERSION_MAJOR 0 ///< Banjo Major Version
#define BJ_VERSION_MINOR 0 ///< Banjo Minor Version
#define BJ_VERSION_PATCH 1 ///< Banjo Patch version

/// Constructs an API version number into 32bits.
///
/// \param major The major version number
/// \param minor The minor version number
/// \param patch The patch version number
#define BJ_MAKE_VERSION(major, minor, patch) \
    ((((uint32_t)(major)) << 22U) | (((uint32_t)(minor)) << 12U) | ((uint32_t)(patch)))

/// Expands to a 32bits representatin of the current version the API.
#define BJ_VERSION BJ_MAKE_VERSION(BJ_VERSION_MAJOR, BJ_VERSION_MINOR, BJ_VERSION_PATCH)

/// Name of the library.
#define BJ_NAME "Banjo"

#ifdef BANJO_STATIC
#  define BANJO_EXPORT
#  define BANJO_NO_EXPORT
#else
#  ifndef BANJO_EXPORT
#    ifdef BANJO_EXPORTS
        /* We are building this library */
#      define BANJO_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define BANJO_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef BANJO_NO_EXPORT
#    define BANJO_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

typedef char      byte;  ///< Single byte integer
typedef uint16_t  c16;   ///< 16-bit char type
typedef float     f32;   ///< IEEE-754 single precision floating point number.
typedef double    f64;   ///< IEEE-754 double precision floating point number.
typedef int8_t    i8;    ///< 8 bit signed integer.
typedef uint8_t   u8;    ///< 8 bit unsigned integer.
typedef int16_t   i16;   ///< 16 signed integer.
typedef uint16_t  u16;   ///< 16 unsigned integer.
typedef int32_t   i32;   ///< 32 signed integer.
typedef uint32_t  u32;   ///< 32 unsigned integer.
typedef int64_t   i64;   ///< 64 signed integer.
typedef uint64_t  u64;   ///< 64 unsigned integer.
typedef ptrdiff_t size;  ///< Signed integer type used for signed representatin of memory sizes.
typedef size_t    usize; ///< Unsigned integer type which can hold the size of any memory allocation or data structure.

/// \def bj_assert
/// Expands to either nothing or standard assert

#ifdef NDEBUG
#    define bj_assert(cond)
#else
#    define bj_assert(cond) assert(cond)
#endif

/// \} End of core
