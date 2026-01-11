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
struct bj_build_info {
    const char* name;                ///< API name (see BJ_NAME).
    const char* variant;             ///< API name variant (see BJ_NAME_VARIANT).
    uint32_t    version;             ///< Packed API version (see BJ_VERSION).
    const char* compiler_name;       ///< Compiler name string.
    int         compiler_version;    ///< Compiler version number.
    bj_bool     debug;               ///< Non-zero if built with debug info.
    bj_bool     backend_alsa;        ///< Built with ALSA audio.
    bj_bool     backend_cocoa;       ///< Built with Cocoa/macOS support.
    bj_bool     backend_emscripten;  ///< Built with Emscripten support.
    bj_bool     backend_mme;         ///< Built with Windows MME audio.
    bj_bool     backend_win32;       ///< Built with Win32 window support.
    bj_bool     backend_x11;         ///< Built with X11 window support.
    bj_bool     checks_abort;        ///< Checks abort execution on failure.
    bj_bool     checks_log;          ///< Checks log failures.
    bj_bool     fastmath;            ///< Built with fast-math optimizations.
    bj_bool     log_color;           ///< Colored log output enabled.
    bj_bool     pedantic;            ///< Extra runtime checks enabled.
};

////////////////////////////////////////////////////////////////////////////////
/// \brief Get runtime build information for the loaded Banjo binaries.
///
/// The returned pointer is owned by the library and remains valid for the
/// lifetime of the process. The content is immutable.
///
/// \return Pointer to a struct bj_build_info structure describing the runtime build.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT const struct bj_build_info* bj_build_information(void);

////////////////////////////////////////////////////////////////////////////////
/// \name Convenience typedefs for public API types
///
/// By default, Banjo provides typedefs that allow writing type names without
/// the `struct` or `enum` keywords (e.g., `bj_vec2` instead of `struct bj_vec2`).
///
/// To disable these typedefs and use explicit struct/enum keywords everywhere,
/// define `BJ_NO_TYPEDEF` before including Banjo headers.
///
////////////////////////////////////////////////////////////////////////////////
/// \{
#ifndef BJ_NO_TYPEDEF

typedef enum bj_audio_format bj_audio_format;
typedef enum bj_blit_op bj_blit_op;
typedef enum bj_callback_result bj_callback_result;
typedef enum bj_error_code bj_error_code;
typedef enum bj_event_action bj_event_action;
typedef enum bj_event_type bj_event_type;
typedef enum bj_key bj_key;
typedef enum bj_log_level bj_log_level;
typedef enum bj_pixel_mode bj_pixel_mode;
typedef enum bj_renderer_type bj_renderer_type;
typedef enum bj_seek_origin bj_seek_origin;
typedef enum bj_shader_flag bj_shader_flag;
typedef enum bj_window_flag bj_window_flag;
typedef struct bj_angular_2d bj_angular_2d;
typedef struct bj_cli bj_cli;
typedef struct bj_cli_argument bj_cli_argument;
typedef struct bj_audio_device bj_audio_device;
typedef struct bj_audio_play_note_data bj_audio_play_note_data;
typedef struct bj_audio_properties bj_audio_properties;
typedef struct bj_bitmap bj_bitmap;
typedef struct bj_build_info bj_build_info;
typedef struct bj_button_event bj_button_event;
typedef struct bj_cursor_event bj_cursor_event;
typedef struct bj_enter_event bj_enter_event;
typedef struct bj_error bj_error;
typedef struct bj_event bj_event;
typedef struct bj_key_event bj_key_event;
typedef struct bj_mat3x2 bj_mat3x2;
typedef struct bj_mat3x3 bj_mat3;
typedef struct bj_mat3x3 bj_mat3x3;
typedef struct bj_mat4x3 bj_mat4x3;
typedef struct bj_mat4x4 bj_mat4;
typedef struct bj_mat4x4 bj_mat4x4;
typedef struct bj_memory_callbacks bj_memory_callbacks;
typedef struct bj_particle_2d bj_particle_2d;
typedef struct bj_pcg32 bj_pcg32;
typedef struct bj_rect bj_rect;
typedef struct bj_renderer bj_renderer;
typedef struct bj_rigid_body_2d bj_rigid_body_2d;
typedef struct bj_stopwatch bj_stopwatch;
typedef struct bj_stream bj_stream;
typedef struct bj_vec2 bj_vec2;
typedef struct bj_vec3 bj_vec3;
typedef struct bj_vec4 bj_quat;
typedef struct bj_vec4 bj_vec4;
typedef struct bj_window bj_window;

#endif /* BJ_NO_TYPEDEF */
/// \}

#endif /* BJ_API_H */
/// \} End of core
