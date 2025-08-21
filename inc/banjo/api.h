////////////////////////////////////////////////////////////////////////////////
/// \file
/// General-purpose definitions for Banjo API.

////////////////////////////////////////////////////////////////////////////////
/// \defgroup core Core
/// General-purpose API
///
/// \{
#ifndef BJ_API_H
#define BJ_API_H

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

/// Expands to a 32bits representatin of the current version the API.
#define BJ_VERSION BJ_MAKE_VERSION(BJ_VERSION_MAJOR_NUMBER, BJ_VERSION_MINOR_NUMBER, BJ_VERSION_PATCH_NUMBER)

/// Name of the library.
#define BJ_NAME "Banjo"

/// Platform detection
#if defined(__EMSCRIPTEN__)
#   define BJ_OS_EMSCRIPTEN
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#   define BJ_OS_WINDOWS
#elif defined(__linux__) || defined(__gnu_linux__)
#   define BJ_OS_LINUX
#elif __APPLE__
#   define BJ_OS_APPLE
#   include <TargetConditionals.h>
#   if TARGET_OS_IPHONE
#       define BJ_OS_IOS
#   elif TARGET_IPHONE_SIMULATOR
#       define BJ_OS_IOS
#       define BJ_OS_IOS_SIMULATOR
#   elif TARGET_OS_MAC
#       define BJ_OS_MACOS
#       define BJ_OS_UNIX
#   else
#       define BJ_OS_APPLE_UNKNOWN
#   endif
#else
#   define BJ_OS_UNKNOWN
#endif

#if defined(__unix__)
#   define BJ_OS_UNIX
#endif

// Compiler Detection
#if defined(__EMSCRIPTEN__)
    #include <emscripten/version.h>
    #define BJ_COMPILER_EMSCRIPTEN
    #define BJ_COMPILER_NAME "Emscripten"
    #define BJ_COMPILER_VERSION __EMSCRIPTEN_major__
#elif defined(__GNUC__) && !defined(__clang__)
    #define BJ_COMPILER_GCC
    #define BJ_COMPILER_NAME "GCC"
    #define BJ_COMPILER_VERSION __GNUC__
#elif defined(__clang__)
    #define BJ_COMPILER_CLANG
    #define BJ_COMPILER_NAME "Clang"
    #define BJ_COMPILER_VERSION __clang_major__
#elif defined(_MSC_VER)
    #define BJ_COMPILER_MSVC
    #define BJ_COMPILER_NAME "MSVC"
    #define BJ_COMPILER_VERSION _MSC_VER
#elif defined(__MINGW32__)
    #define BJ_COMPILER_MINGW
    #define BJ_COMPILER_NAME "MinGW"
    #define BJ_COMPILER_VERSION 0
#else
    #define BJ_COMPILER_UNKNOWN
    #define BJ_COMPILER_NAME "Unknown"
    #define BJ_COMPILER_VERSION 0
#endif

#ifdef NDEBUG
#   define BJ_BUILD_RELEASE
#else
#   define BJ_BUILD_DEBUG
#endif

#ifdef BANJO_STATIC
#  define BANJO_EXPORT
#  define BANJO_NO_EXPORT
#else
#    ifdef _MSC_VER
#      ifndef BANJO_EXPORT
#        ifdef BANJO_EXPORTS
#          define BANJO_EXPORT __declspec(dllexport)
#        else
#          define BANJO_EXPORT  __declspec( dllexport )
#        endif
#      endif
#      ifndef BANJO_NO_EXPORT
#        define BANJO_NO_EXPORT 
#      endif
#    else
#      ifndef BANJO_EXPORT
#        ifdef BANJO_EXPORTS
#          define BANJO_EXPORT __attribute__((visibility("default")))
#        else
#          define BANJO_EXPORT __attribute__((visibility("default")))
#        endif
#      endif
#      ifndef BANJO_NO_EXPORT
#        define BANJO_NO_EXPORT __attribute__((visibility("hidden")))
#      endif
#    endif
#endif

////////////////////////////////////////////////////////////////////////////////
/// \typedef bj_bool
/// \brief Boolean type used throughout the BJ codebase.
///
/// Defined as a 32-bit unsigned integer for compatibility and clarity.
///
/// \see BJ_FALSE
/// \see BJ_TRUE
////////////////////////////////////////////////////////////////////////////////
typedef uint32_t bj_bool;

////////////////////////////////////////////////////////////////////////////////
/// \def BJ_FALSE
/// \brief Boolean false value.
///
/// Represents the false value for the bj_bool type, defined as zero.
///
/// \see bj_bool
/// \see BJ_TRUE
////////////////////////////////////////////////////////////////////////////////
#define BJ_FALSE ((bj_bool)0)

////////////////////////////////////////////////////////////////////////////////
/// \def BJ_TRUE
/// \brief Boolean true value.
///
/// Represents the true value for the bj_bool type, defined as one.
///
/// \see bj_bool
/// \see BJ_FALSE
////////////////////////////////////////////////////////////////////////////////
#define BJ_TRUE ((bj_bool)1)

/// Structure holding build information of the binary
typedef struct {
    const char* p_name;              ///< API Name (\ref BJ_NAME)
    uint32_t    version;             ///< Built version (\ref BJ_VERSION)
    const char* compiler_name;       ///< Compiler C-String name
    int         compiler_version;    ///< Compiler version specifier
    bj_bool     debug;               ///< Built with debug information
    bj_bool     feature_win32;       ///< Compiled with support for Win32 windows.
    bj_bool     feature_emscripten;  ///< Compiled with support for Emscripten.
    bj_bool     feature_x11;         ///< Compiled with support for Win32 windows.
    bj_bool     feature_mme;         ///< Compiled with support for Windows Multimedia Extension (for audio).
    bj_bool     feature_alsa;        ///< Compiled with support for ALSA (for audio).
    bj_bool     config_checks_abort; ///< When checks feature is on, a failed check will abort execution
    bj_bool     config_checks_log;   ///< If checks feature is on, failed check with log
    bj_bool     config_log_color;    ///< Banjo logs will have colored output
    bj_bool     config_pedantic;     ///< Banjo runtime will make costly extra checks
    bj_bool     config_fastmath;     ///< Banjo compiled with floating point math optimizations
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

#endif
/// \} End of core
