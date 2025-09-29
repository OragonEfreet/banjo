////////////////////////////////////////////////////////////////////////////////
/// \file window.h
/// Header file for \ref bj_window type.
////////////////////////////////////////////////////////////////////////////////
/// \defgroup window Windows
/// Window creation and management API.
///
/// This module provides functions to create, destroy, and control
/// application windows. It handles window properties such as position,
/// size, flags, and attached framebuffers. It also provides input
/// queries such as key state inspection.
///
/// Typical usage:
/// - Create a window with \ref bj_bind_window.
/// - Query or modify its flags using \ref bj_get_window_flags and related calls.
/// - Access and update the framebuffer with \ref bj_get_window_framebuffer
///   and \ref bj_update_window_framebuffer.
/// - Release resources with \ref bj_unbind_window.
///
/// Banjo supports windows manipulation for Windows, GNU/Linux and WebAssembly.
///
/// \todo Add support for windows on macOS
/// \todo Add support for windows on Wayland
///
/// \{
////////////////////////////////////////////////////////////////////////////////

#ifndef BJ_WINDOW_H
#define BJ_WINDOW_H

#include <banjo/api.h>
#include <banjo/bitmap.h>
#include <banjo/memory.h>

/// Opaque typedef for the window type
typedef struct bj_window_t bj_window;

////////////////////////////////////////////////////////////////////////////////
/// A set of flags describing some properties of a \ref bj_window.
///
/// These flags can be provided at window creation with \ref bj_bind_window.
/// The may also change during the window lifetime.
/// You can use \ref bj_get_window_flags to query the status of any flag on an
/// active window instance.
////////////////////////////////////////////////////////////////////////////////
typedef enum bj_window_flag_t {
    BJ_WINDOW_FLAG_NONE       = 0x00, //!< No Flag.
    BJ_WINDOW_FLAG_CLOSE      = 0x01, //!< Window should be closed by the application.
    BJ_WINDOW_FLAG_KEY_REPEAT = 0x02, //!< Key repeat event is enabled (see \ref bj_set_key_callback).
    BJ_WINDOW_FLAG_ALL        = 0xFF, //!< All flags set.
} bj_window_flag;

////////////////////////////////////////////////////////////////////////////////
/// Create a new bj_window with the specified attributes
///
/// \param p_title  Title of the window
/// \param x        Horizontal position of the window on-screen, expressed in pixels
/// \param y        Vertical position of the window on-screen, expressed in pixels
/// \param width    Width of the window.
/// \param height   Height of the window.
/// \param flags    A set of options flags.
///
/// \return A pointer to the newly created bj_window object.
///
/// \par Memory Management
///
/// The caller is responsible for releasing the returned \ref bj_window object
/// with \ref bj_unbind_window.
///
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_window* bj_bind_window(
    const char* p_title,
    uint16_t x,
    uint16_t y,
    uint16_t width,
    uint16_t height,
    uint8_t  flags
);

////////////////////////////////////////////////////////////////////////////////
/// Deletes a bj_window object and releases associated memory.
///
/// \param p_window Pointer to the window object to delete.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_unbind_window(
    bj_window* p_window
);

////////////////////////////////////////////////////////////////////////////////
/// Flag a given window to be closed.
///
/// Once, flagged, the window is not automatically closed.
/// Instead, call \ref bj_unbind_window.
/// 
/// Note that it is not possible to remove a closed flag once set.
///
/// \param p_window Pointer to the window object to flag.
///
/// \remark
///
/// This function effectively returns 
/// `bj_get_window_flags(p_window, BJ_WINDOW_FLAG_CLOSE) > 0`.
///
/// \see bj_should_close_window
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_set_window_should_close(
    bj_window* p_window
);

////////////////////////////////////////////////////////////////////////////////
/// Get the close flag state of a window.
///
/// \param p_window Pointer to the window object to flag.
/// \return _true_ if the close flag is set, _false_ otherwise.
///
/// If `p_window` is *0*, the function returns *BJ_TRUE*.
///
/// \see bj_set_window_should_close
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_bool bj_should_close_window(
    bj_window* p_window
);

////////////////////////////////////////////////////////////////////////////////
/// Get window flags.
///
/// This function returns all the flag sets for `p_window`.
/// `flags` is a filter to only get the flag you are interested into.
/// It can be a single flag, an OR'd combination of multiple flags or even
/// \ref BJ_WINDOW_FLAG_ALL if you want to retrieve them all.
///
/// \param p_window   The window handler.
/// \param flags      Filter flag set. 
/// \return An OR'd combination of \ref bj_window_flag_t filtered by `flags`.
///
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT uint8_t bj_get_window_flags(
    bj_window* p_window,
    uint8_t    flags
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Query the current state of a key for a given window.
///
/// Returns the state of the specified key as either `BJ_RELEASE` or `BJ_PRESS`.
/// If \p p_window is NULL or if \p key is outside the valid range [0, 0xFE],
/// the function returns `BJ_RELEASE`.
///
/// \param p_window Pointer to the target window, or NULL
/// \param key      Key code in [0, 0xFE]
/// \return `BJ_PRESS` if the key is currently pressed, `BJ_RELEASE` otherwise
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT int bj_get_key(
    const bj_window* p_window,
    int              key
);

////////////////////////////////////////////////////////////////////////////////
/// Return the framebuffer attached to the window.
///
/// The framebuffer is an instance of \ref bj_bitmap attached (and owned) by
/// the window.
/// If necessary, \ref bj_get_window_flags will create (or recreate) the
/// framebuffer object upon calling this function.
/// This can happen when the window is resize, minimized or any even that
/// invalidate the window drawing area.
///
/// TODO: The instance should stay the same and the framebuffer only change
/// internally.
///
/// \param p_window The window handler
/// \param p_error  Optional error location
///
/// \return A pointer to a \ref bj_bitmap attached to the window or _0_ in
///         case of failure.
///
/// \par Behaviour
///
/// The function performs nothing if `p_window`  is _0_.
///
/// \par Memory Management
///
/// The library is responsible for the return \ref bj_bitmap object.
///
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_bitmap* bj_get_window_framebuffer(
    bj_window* p_window,
    bj_error** p_error
);

////////////////////////////////////////////////////////////////////////////////
/// Retrieve the size of the window.
///
/// \param p_window   The window handler
/// \param width      A location to the destination width
/// \param height      A location to the destination height
///
/// \return _1_ on success, _0_ on error.
///
/// \par Behaviour
///
/// The function performs nothing if `p_window`  is _0_.
///
/// `width` and `height` can be _0_ if you are only interested in retrieving
/// one of the values.
///
/// \par Memory Management
///
/// You are responsible for the memory of `width`  and `height`.
///
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT int bj_get_window_size(
    const bj_window* p_window,
    int*             width,
    int*             height
);

////////////////////////////////////////////////////////////////////////////////
/// Copy window's framebuffer onto screen
///
/// \param p_window   The window handler
///
/// Use this function to apply any change made to the framebuffer on the window.
/// 
/// \par Behaviour
///
/// The function performs nothing if `p_window` is _0_ or does not contain any
/// framebuffer.
///
/// \see bj_get_window_framebuffer
///
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_update_window_framebuffer(
    bj_window* p_window
);

#endif
/// \} // End of window group
////////////////////////////////////////////////////////////////////////////////
/// \file window.h
/// Header file for \ref bj_window type.
////////////////////////////////////////////////////////////////////////////////
/// \defgroup window Windows
/// \{
////////////////////////////////////////////////////////////////////////////////

#ifndef BJ_WINDOW_H
#define BJ_WINDOW_H

#include <banjo/api.h>
#include <banjo/bitmap.h>
#include <banjo/memory.h>

/// Opaque typedef for the window type
typedef struct bj_window_t bj_window;

////////////////////////////////////////////////////////////////////////////////
/// A set of flags describing some properties of a \ref bj_window.
///
/// These flags can be provided at window creation with \ref bj_bind_window.
/// The may also change during the window lifetime.
/// You can use \ref bj_get_window_flags to query the status of any flag on an
/// active window instance.
////////////////////////////////////////////////////////////////////////////////
typedef enum bj_window_flag_t {
    BJ_WINDOW_FLAG_NONE       = 0x00, //!< No Flag.
    BJ_WINDOW_FLAG_CLOSE      = 0x01, //!< Window should be closed by the application.
    BJ_WINDOW_FLAG_KEY_REPEAT = 0x02, //!< Key repeat event is enabled (see \ref bj_set_key_callback).
    BJ_WINDOW_FLAG_ALL        = 0xFF, //!< All flags set.
} bj_window_flag;

////////////////////////////////////////////////////////////////////////////////
/// Create a new bj_window with the specified attributes
///
/// \param p_title  Title of the window
/// \param x        Horizontal position of the window on-screen, expressed in pixels
/// \param y        Vertical position of the window on-screen, expressed in pixels
/// \param width    Width of the window.
/// \param height   Height of the window.
/// \param flags    A set of options flags.
///
/// \return A pointer to the newly created bj_window object.
///
/// \par Memory Management
///
/// The caller is responsible for releasing the returned \ref bj_window object
/// with \ref bj_unbind_window.
///
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_window* bj_bind_window(
    const char* p_title,
    uint16_t x,
    uint16_t y,
    uint16_t width,
    uint16_t height,
    uint8_t  flags
);

////////////////////////////////////////////////////////////////////////////////
/// Deletes a bj_window object and releases associated memory.
///
/// \param p_window Pointer to the window object to delete.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_unbind_window(
    bj_window* p_window
);

////////////////////////////////////////////////////////////////////////////////
/// Flag a given window to be closed.
///
/// Once, flagged, the window is not automatically closed.
/// Instead, call \ref bj_unbind_window.
/// 
/// Note that it is not possible to remove a closed flag once set.
///
/// \param p_window Pointer to the window object to flag.
///
/// \remark
///
/// This function effectively returns 
/// `bj_get_window_flags(p_window, BJ_WINDOW_FLAG_CLOSE) > 0`.
///
/// \see bj_should_close_window
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_set_window_should_close(
    bj_window* p_window
);

////////////////////////////////////////////////////////////////////////////////
/// Get the close flag state of a window.
///
/// \param p_window Pointer to the window object to flag.
/// \return _true_ if the close flag is set, _false_ otherwise.
///
/// If `p_window` is *0*, the function returns *BJ_TRUE*.
///
/// \see bj_set_window_should_close
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_bool bj_should_close_window(
    bj_window* p_window
);

////////////////////////////////////////////////////////////////////////////////
/// Get window flags.
///
/// This function returns all the flag sets for `p_window`.
/// `flags` is a filter to only get the flag you are interested into.
/// It can be a single flag, an OR'd combination of multiple flags or even
/// \ref BJ_WINDOW_FLAG_ALL if you want to retrieve them all.
///
/// \param p_window   The window handler.
/// \param flags      Filter flag set. 
/// \return An OR'd combination of \ref bj_window_flag_t filtered by `flags`.
///
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT uint8_t bj_get_window_flags(
    bj_window* p_window,
    uint8_t    flags
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Query the current state of a key for a given window.
///
/// Returns the state of the specified key as either `BJ_RELEASE` or `BJ_PRESS`.
/// If \p p_window is NULL or if \p key is outside the valid range [0, 0xFE],
/// the function returns `BJ_RELEASE`.
///
/// \param p_window Pointer to the target window, or NULL
/// \param key      Key code in [0, 0xFE]
/// \return `BJ_PRESS` if the key is currently pressed, `BJ_RELEASE` otherwise
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT int bj_get_key(
    const bj_window* p_window,
    int              key
);

////////////////////////////////////////////////////////////////////////////////
/// Return the framebuffer attached to the window.
///
/// The framebuffer is an instance of \ref bj_bitmap attached (and owned) by
/// the window.
/// If necessary, \ref bj_get_window_flags will create (or recreate) the
/// framebuffer object upon calling this function.
/// This can happen when the window is resize, minimized or any even that
/// invalidate the window drawing area.
///
/// TODO: The instance should stay the same and the framebuffer only change
/// internally.
///
/// \param p_window The window handler
/// \param p_error  Optional error location
///
/// \return A pointer to a \ref bj_bitmap attached to the window or _0_ in
///         case of failure.
///
/// \par Behaviour
///
/// The function performs nothing if `p_window`  is _0_.
///
/// \par Memory Management
///
/// The library is responsible for the return \ref bj_bitmap object.
///
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_bitmap* bj_get_window_framebuffer(
    bj_window* p_window,
    bj_error** p_error
);

////////////////////////////////////////////////////////////////////////////////
/// Retrieve the size of the window.
///
/// \param p_window   The window handler
/// \param width      A location to the destination width
/// \param height      A location to the destination height
///
/// \return _1_ on success, _0_ on error.
///
/// \par Behaviour
///
/// The function performs nothing if `p_window`  is _0_.
///
/// `width` and `height` can be _0_ if you are only interested in retrieving
/// one of the values.
///
/// \par Memory Management
///
/// You are responsible for the memory of `width`  and `height`.
///
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT int bj_get_window_size(
    const bj_window* p_window,
    int*             width,
    int*             height
);

////////////////////////////////////////////////////////////////////////////////
/// Copy window's framebuffer onto screen
///
/// \param p_window   The window handler
///
/// Use this function to apply any change made to the framebuffer on the window.
/// 
/// \par Behaviour
///
/// The function performs nothing if `p_window` is _0_ or does not contain any
/// framebuffer.
///
/// \see bj_get_window_framebuffer
///
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_update_window_framebuffer(
    bj_window* p_window
);

#endif
/// \} // End of window group
