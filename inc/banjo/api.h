////////////////////////////////////////////////////////////////////////////////
/// \file api.h
/// \brief General-purpose definitions for Banjo API.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// \defgroup core Core
/// \brief General-purpose API.
///
/// \{
////////////////////////////////////////////////////////////////////////////////
#ifndef BJ_API_H
#define BJ_API_H

#include <stddef.h>
#include <stdint.h>

/// \brief Extract major version from a 32-bit version value.
/// \param version Packed version from BJ_MAKE_VERSION.
/// \return Major version in [0, 1023].
#define BJ_VERSION_MAJOR(version) (((version) >> 22U) & 0x3FFU)

/// \brief Extract minor version from a 32-bit version value.
/// \param version Packed version from BJ_MAKE_VERSION.
/// \return Minor version in [0, 1023].
#define BJ_VERSION_MINOR(version) (((version) >> 12U) & 0x3FFU)

/// \brief Extract patch version from a 32-bit version value.
/// \param version Packed version from BJ_MAKE_VERSION.
/// \return Patch version in [0, 4095].
#define BJ_VERSION_PATCH(version) ((version) & 0xFFFU)

/// \brief Construct a packed 32-bit version value: [major:10 | minor:10 | patch:12].
/// \param major Major version in [0, 1023].
/// \param minor Minor version in [0, 1023].
/// \param patch Patch version in [0, 4095].
/// \return Packed version suitable for BJ_VERSION_* macros.
#define BJ_MAKE_VERSION(major, minor, patch) \
    ((((uint32_t)(major)) << 22U) | (((uint32_t)(minor)) << 12U) | ((uint32_t)(patch)))

#define BJ_VERSION_MAJOR_NUMBER 0 ///< Current major version number.
#define BJ_VERSION_MINOR_NUMBER 1 ///< Current minor version number.
#define BJ_VERSION_PATCH_NUMBER 0 ///< Current patch version number.

/// \brief Current API version as a packed 32-bit representation.
#define BJ_VERSION BJ_MAKE_VERSION(BJ_VERSION_MAJOR_NUMBER, BJ_VERSION_MINOR_NUMBER, BJ_VERSION_PATCH_NUMBER)

/// \brief Library name string.
#define BJ_NAME "Banjo"

/// \name Platform detection
/// \brief One of these will be defined to indicate the target OS.
/// @{
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
/// @}

/// \name Compiler detection
/// \brief One of these will be defined to indicate the compiler in use.
///
/// Additionally BJ_COMPILER_NAME and BJ_COMPILER_VERSION are provided.
/// @{
#if defined(BJ_COMPILER_DOXYGEN)
#    define BJ_COMPILER_NAME "Doxygen"
#    define BJ_COMPILER_VERSION 0
#elif defined(__EMSCRIPTEN__)
#    include <emscripten/version.h>
#    define BJ_COMPILER_EMSCRIPTEN
#    define BJ_COMPILER_NAME "Emscripten"
#    define BJ_COMPILER_VERSION __EMSCRIPTEN_major__
#elif defined(__GNUC__) && !defined(__clang__)
#    define BJ_COMPILER_GCC
#    define BJ_COMPILER_NAME "GCC"
#    define BJ_COMPILER_VERSION __GNUC__
#elif defined(__clang__)
#    define BJ_COMPILER_CLANG
#    define BJ_COMPILER_NAME "Clang"
#    define BJ_COMPILER_VERSION __clang_major__
#elif defined(_MSC_VER)
#    define BJ_COMPILER_MSVC
#    define BJ_COMPILER_NAME "MSVC"
#    define BJ_COMPILER_VERSION _MSC_VER
#elif defined(__MINGW32__)
#    define BJ_COMPILER_MINGW
#    define BJ_COMPILER_NAME "MinGW"
#    define BJ_COMPILER_VERSION 0
#else
#    define BJ_COMPILER_UNKNOWN
#    define BJ_COMPILER_NAME "Unknown"
#    define BJ_COMPILER_VERSION 0
#endif
/// @}

/// \name Build configuration
/// \brief Exactly one of BJ_BUILD_DEBUG or BJ_BUILD_RELEASE is defined.
/// @{
/// \def BJ_BUILD_RELEASE
/// Set when Banjo was built on release mode (`NDEBUG` defined).
#if defined(NDEBUG) || defined(BJ_COMPILER_DOXYGEN)
#   define BJ_BUILD_RELEASE
#endif
/// \def BJ_BUILD_DEBUG
/// Set when Banjo was built on debug mode (`NDEBUG` not defined).
#if !defined(NDEBUG) || defined(BJ_COMPILER_DOXYGEN)
#   define BJ_BUILD_DEBUG
#endif

/// @}

/// \name Export/visibility helpers
/// \brief BANJO_EXPORT marks public symbols. BANJO_NO_EXPORT hides symbols.
/// @{
#if defined(BANJO_STATIC) || defined(BJ_COMPILER_DOXYGEN)
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
/// @}

/// \name Restrict qualifier macro
/// \def BJ_RESTRICT
/// \brief BJ_RESTRICT expands to the appropriate restrict qualifier per toolchain.
/// @{
#if defined(__cplusplus)
    #if defined(__GNUC__) || defined(__clang__)
        #define BJ_RESTRICT __restrict__
    #elif defined(_MSC_VER)
        #define BJ_RESTRICT __restrict
    #else
        #define BJ_RESTRICT /* nothing */
    #endif
#else
    /* pure C (C99 or newer) */
    #if defined(BJ_COMPILER_DOXYGEN) || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L)
        #define BJ_RESTRICT restrict
    #elif defined(__GNUC__) || defined(__clang__)
        #define BJ_RESTRICT __restrict__
    #elif defined(_MSC_VER)
        #define BJ_RESTRICT __restrict
    #else
        #define BJ_RESTRICT /* nothing */
    #endif
#endif
/// @}

/// \name Array parameter helper macros
/// \def BJ_CONST_ARRAY
/// \def BJ_ARRAY
/// \def BJ_CONST_ARRAY_2D
/// \def BJ_ARRAY_2D
/// \brief Expands to array parameter declarations appropriate for the toolchain.
/// BJ_CONST_ARRAY/BJ_ARRAY are 1-D. BJ_CONST_ARRAY_2D/BJ_ARRAY_2D are 2-D (rows=n, cols=m).
/// @{
#if defined(_MSC_VER)
    #if defined(__has_include)
        #if __has_include(<sal.h>)
            #include <sal.h>
            #define BJ_CONST_ARRAY(T,n,name)      _In_reads_(n)             const T* BJ_RESTRICT name
            #define BJ_ARRAY(T,n,name)            _Out_writes_(n)                  T* BJ_RESTRICT name
            #define BJ_CONST_ARRAY_2D(T,n,m,name) _In_reads_((n)*(m)) const T (* BJ_RESTRICT name)[m]
            #define BJ_ARRAY_2D(T,n,m,name)       _Out_writes_((n)*(m))        T (* BJ_RESTRICT name)[m]
        #else
            #define BJ_CONST_ARRAY(T,n,name)      const T* BJ_RESTRICT name
            #define BJ_ARRAY(T,n,name)                  T* BJ_RESTRICT name
            #define BJ_CONST_ARRAY_2D(T,n,m,name) const T (* BJ_RESTRICT name)[m]
            #define BJ_ARRAY_2D(T,n,m,name)            T (* BJ_RESTRICT name)[m]
        #endif
    #else
        #include <sal.h>
        #define BJ_CONST_ARRAY(T,n,name)      _In_reads_(n)             const T* BJ_RESTRICT name
        #define BJ_ARRAY(T,n,name)            _Out_writes_(n)                  T* BJ_RESTRICT name
        #define BJ_CONST_ARRAY_2D(T,n,m,name) _In_reads_((n)*(m)) const T (* BJ_RESTRICT name)[m]
        #define BJ_ARRAY_2D(T,n,m,name)       _Out_writes_((n)*(m))        T (* BJ_RESTRICT name)[m]
    #endif
#elif defined(BJ_COMPILER_DOXYGEN) || (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L))
    #define BJ_CONST_ARRAY(T,n,name)      const T name[BJ_RESTRICT static (n)]
    #define BJ_ARRAY(T,n,name)                  T name[BJ_RESTRICT static (n)]
    #define BJ_CONST_ARRAY_2D(T,n,m,name) const T name[BJ_RESTRICT static (n)][m]
    #define BJ_ARRAY_2D(T,n,m,name)             T name[BJ_RESTRICT static (n)][m]
#else
    #define BJ_CONST_ARRAY(T,n,name)      const T* BJ_RESTRICT name        ///< pointer to >=n const T
    #define BJ_ARRAY(T,n,name)                  T* BJ_RESTRICT name        ///< pointer to >=n T
    #define BJ_CONST_ARRAY_2D(T,n,m,name) const T (* BJ_RESTRICT name)[m]  ///< pointer to >=n rows of m const T
    #define BJ_ARRAY_2D(T,n,m,name)            T (* BJ_RESTRICT name)[m]   ///< pointer to >=n rows of m T
#endif
/// @}


/// \name Inline helper macro
/// \def BJ_INLINE
/// \brief BJ_INLINE expands to an inline specifier appropriate for the toolchain.
/// If BJ_API_FORCE_INLINE is defined, stronger inlining is requested.
/// @{
#if defined(_MSC_VER)
    #if defined(BJ_API_FORCE_INLINE)
        #define BJ_INLINE __forceinline
    #else
        #if !defined(__cplusplus) && !defined(inline)
            #define BJ_INLINE __inline
        #else
            #define BJ_INLINE inline
        #endif
    #endif
#elif defined(__GNUC__) || defined(__clang__)
    #if defined(BJ_API_FORCE_INLINE)
        #define BJ_INLINE inline __attribute__((always_inline))
    #else
        #define BJ_INLINE inline
    #endif
#else
    #if defined(BJ_COMPILER_DOXYGEN) || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L)
        #define BJ_INLINE inline
    #else
        #define BJ_INLINE /* no inline available */
    #endif
#endif
/// @}

////////////////////////////////////////////////////////////////////////////////
/// \typedef bj_bool
/// \brief Boolean type used throughout the Banjo API.
///
/// Defined as a 32-bit unsigned integer for portability and ABI clarity.
///
/// \see BJ_FALSE
/// \see BJ_TRUE
////////////////////////////////////////////////////////////////////////////////
typedef uint32_t bj_bool;

////////////////////////////////////////////////////////////////////////////////
/// \def BJ_FALSE
/// \brief Boolean false value (0).
///
/// \see bj_bool
/// \see BJ_TRUE
////////////////////////////////////////////////////////////////////////////////
#define BJ_FALSE ((bj_bool)0)

////////////////////////////////////////////////////////////////////////////////
/// \def BJ_TRUE
/// \brief Boolean true value (1).
///
/// \see bj_bool
/// \see BJ_FALSE
////////////////////////////////////////////////////////////////////////////////
#define BJ_TRUE ((bj_bool)1)

/// \brief Structure holding build information of the binary.
typedef struct {
    const char* p_name;              ///< API name (see BJ_NAME).
    uint32_t    version;             ///< Packed API version (see BJ_VERSION).
    const char* compiler_name;       ///< Compiler name string.
    int         compiler_version;    ///< Compiler version number.
    bj_bool     debug;               ///< Non-zero if built with debug info.
    bj_bool     feature_win32;       ///< Built with Win32 window support.
    bj_bool     feature_emscripten;  ///< Built with Emscripten support.
    bj_bool     feature_x11;         ///< Built with X11 window support.
    bj_bool     feature_mme;         ///< Built with Windows MME audio.
    bj_bool     feature_alsa;        ///< Built with ALSA audio.
    bj_bool     config_checks_abort; ///< Checks abort execution on failure.
    bj_bool     config_checks_log;   ///< Checks log failures.
    bj_bool     config_log_color;    ///< Colored log output enabled.
    bj_bool     config_pedantic;     ///< Extra runtime checks enabled.
    bj_bool     config_fastmath;     ///< Built with fast-math optimizations.
} bj_build_info;

////////////////////////////////////////////////////////////////////////////////
/// \brief Get runtime build information for the loaded Banjo binaries.
///
/// The returned pointer is owned by the library and remains valid for the
/// lifetime of the process. The content is immutable.
///
/// \return Pointer to a bj_build_info structure describing the runtime build.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT const bj_build_info* bj_build_information(void);

#endif /* BJ_API_H */
/// \} End of core
