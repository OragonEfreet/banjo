////////////////////////////////////////////////////////////////////////////////
/// \file window.h
/// Header file for  bj_window type.
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
#include <banjo/error.h>
#include <banjo/memory.h>

/// Opaque typedef for the window type
struct bj_window;

////////////////////////////////////////////////////////////////////////////////
/// A set of flags describing some properties of a  bj_window.
///
/// These flags can be provided at window creation with \ref bj_bind_window.
/// The may also change during the window lifetime.
/// You can use \ref bj_get_window_flags to query the status of any flag on an
/// active window instance.
////////////////////////////////////////////////////////////////////////////////
enum bj_window_flag {
    BJ_WINDOW_FLAG_NONE       = 0x00, //!< No Flag.
    BJ_WINDOW_FLAG_CLOSE      = 0x01, //!< Window should be closed by the application.
    BJ_WINDOW_FLAG_KEY_REPEAT = 0x02, //!< Key repeat event is enabled (see \ref bj_set_key_callback).
    BJ_WINDOW_FLAG_ALL        = 0xFF, //!< All flags set.
};

////////////////////////////////////////////////////////////////////////////////
/// Create a new struct bj_window with the specified attributes
///
/// \param title  Title of the window
/// \param x      Horizontal position of the window on-screen, expressed in pixels
/// \param y      Vertical position of the window on-screen, expressed in pixels
/// \param width  Width of the window.
/// \param height Height of the window.
/// \param flags  A set of options flags.
/// \param error  Optional pointer to receive error information on failure.
///
/// \return A pointer to the newly created struct bj_window object, or NULL on failure.
///
/// \par Memory Management
///
/// The caller is responsible for releasing the returned  bj_window object
/// with \ref bj_unbind_window.
///
/// \par Error Codes
/// - BJ_ERROR_VIDEO: Window creation failed (display unavailable, etc.)
///
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT struct bj_window* bj_bind_window(
    const char*       title,
    uint16_t          x,
    uint16_t          y,
    uint16_t          width,
    uint16_t          height,
    uint8_t           flags,
    struct bj_error** error
);

////////////////////////////////////////////////////////////////////////////////
/// Deletes a struct bj_window object and releases associated memory.
///
/// \param window Pointer to the window object to delete.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_unbind_window(
    struct bj_window* window
);

////////////////////////////////////////////////////////////////////////////////
/// Flag a given window to be closed.
///
/// Once, flagged, the window is not automatically closed.
/// Instead, call \ref bj_unbind_window.
/// 
/// Note that it is not possible to remove a closed flag once set.
///
/// \param window Pointer to the window object to flag.
///
/// \remark
///
/// This function effectively returns 
/// `bj_get_window_flags(window, BJ_WINDOW_FLAG_CLOSE) > 0`.
///
/// \see bj_should_close_window
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_set_window_should_close(
    struct bj_window* window
);

////////////////////////////////////////////////////////////////////////////////
/// Get the close flag state of a window.
///
/// \param window Pointer to the window object to flag.
/// \return _true_ if the close flag is set, _false_ otherwise.
///
/// If `window` is *0*, the function returns *BJ_TRUE*.
///
/// \see bj_set_window_should_close
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_bool bj_should_close_window(
    struct bj_window* window
);

////////////////////////////////////////////////////////////////////////////////
/// Get window flags.
///
/// This function returns all the flag sets for `window`.
/// `flags` is a filter to only get the flag you are interested into.
/// It can be a single flag, an OR'd combination of multiple flags or even
/// \ref BJ_WINDOW_FLAG_ALL if you want to retrieve them all.
///
/// \param window   The window handler.
/// \param flags      Filter flag set. 
/// \return An OR'd combination of  bj_window_flag filtered by `flags`.
///
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT uint8_t bj_get_window_flags(
    struct bj_window* window,
    uint8_t    flags
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Query the current state of a key for a given window.
///
/// Returns the state of the specified key as either `BJ_RELEASE` or `BJ_PRESS`.
/// If \p window is NULL or if \p key is outside the valid range [0, 0xFE],
/// the function returns `BJ_RELEASE`.
///
/// \param window Pointer to the target window, or NULL
/// \param key      Key code in [0, 0xFE]
/// \return `BJ_PRESS` if the key is currently pressed, `BJ_RELEASE` otherwise
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT int bj_get_key(
    const struct bj_window* window,
    int              key
);

////////////////////////////////////////////////////////////////////////////////
/// Retrieve the size of the window.
///
/// \param window   The window handler
/// \param width      A location to the destination width
/// \param height      A location to the destination height
///
/// \return _1_ on success, _0_ on error.
///
/// \par Behaviour
///
/// The function performs nothing if `window`  is _0_.
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
    const struct bj_window* window,
    int*             width,
    int*             height
);


#endif
/// \} // End of window group

