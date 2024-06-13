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
#endif
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/// Extracts the major version number from a 32-bit version number.
#define BJ_VERSION_MAJOR(version) (((version) >> 22U) & 0x3FFU)

/// Extracts the minor version number from a 32-bit version number.
#define BJ_VERSION_MINOR(version) (((version) >> 12U) & 0x3FFU)

/// Extracts the patch version number from a 32-bit version number.
#define BJ_VERSION_PATCH(version) ((version) & 0xFFFU)

/// Constructs an API version number into 32bits.
///
/// \param major The major version number
/// \param minor The minor version number
/// \param patch The patch version number
#define BJ_MAKE_VERSION(major, minor, patch) \
    ((((uint32_t)(major)) << 22U) | (((uint32_t)(minor)) << 12U) | ((uint32_t)(patch)))

#define BJ_VERSION_MAJOR_NUMBER 0 ///< Banjo current major version
#define BJ_VERSION_MINOR_NUMBER 1 ///< Banjo current minor version
#define BJ_VERSION_PATCH_NUMBER 0 ///< Banjo current patch version
                           ///
/// Expands to a 32bits representatin of the current version the API.
#define BJ_VERSION BJ_MAKE_VERSION(BJ_VERSION_MAJOR_NUMBER, BJ_VERSION_MINOR_NUMBER, BJ_VERSION_PATCH_NUMBER)

/// Name of the library.
#define BJ_NAME "Banjo"

#ifdef BANJO_STATIC
#  define BANJO_EXPORT
#  define BANJO_NO_EXPORT
#else

#ifdef _MSC_VER

#  ifndef BANJO_EXPORT
#    ifdef BANJO_EXPORTS
#      define BANJO_EXPORT __declspec(dllexport)
#    else
#      define BANJO_EXPORT  __declspec( dllexport )
#    endif
#  endif

#  ifndef BANJO_NO_EXPORT
#    define BANJO_NO_EXPORT 
#  endif

#else

#  ifndef BANJO_EXPORT
#    ifdef BANJO_EXPORTS
#      define BANJO_EXPORT __attribute__((visibility("default")))
#    else
#      define BANJO_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef BANJO_NO_EXPORT
#    define BANJO_NO_EXPORT __attribute__((visibility("hidden")))
#  endif

#endif


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
typedef ptrdiff_t size;  ///< Signed integer type used for signed representatin of memory sizes.
typedef size_t    usize; ///< Unsigned integer type which can hold the size of any memory allocation or data structure.
typedef uintptr_t uptr;  ///< Representation of a pointer value as an int

/// Structure holding build information of the binary
typedef struct {
    const char* p_name;           ///< API Name (\ref BJ_NAME)
    uint32_t    version;          ///< Built version (\ref BJ_VERSION)
    bool        debug;            ///< Built with debug information
    bool        pedantic;         ///< If `true`, the API was build in pedantic mode
    bool        log_color;        ///< Logs are colored
    bool        checks;           ///< Programming error checking
    bool        abort_on_checks;  ///< Program aborts when checks fails
} bj_build_info;

////////////////////////////////////////////////////////////////////////////////
/// Returns the build information of the runtime Banjo binaries.
///
/// The function returns a pointer to \ref bj_build_info that is always the same
/// in between different calls.
///
/// \return A description of runtime build information
///
BANJO_EXPORT const bj_build_info* bj_get_build_info(void);


/// \} End of core
