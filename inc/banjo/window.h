////////////////////////////////////////////////////////////////////////////////
/// \file
/// Header file for \ref bj_window type.
////////////////////////////////////////////////////////////////////////////////
/// \defgroup window Windows
/// \ingroup system
/// \{
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <banjo/api.h>
#include <banjo/bitmap.h>
#include <banjo/memory.h>

/// Opaque typedef for the window type
typedef struct bj_window_t bj_window;

////////////////////////////////////////////////////////////////////////////////
/// A set of flags describing some properties of a \ref bj_window.
///
/// These flags can be provided at window creation with \ref bj_window_new.
/// The may also change during the window lifetime.
/// You can use \ref bj_window_get_flags to query the status of any flag on an
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
/// with \ref bj_window_del.
///
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_window* bj_window_new(
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
BANJO_EXPORT void bj_window_del(
    bj_window* p_window
);

////////////////////////////////////////////////////////////////////////////////
/// Flag a given window to be closed.
///
/// Once, flagged, the window is not automatically closed.
/// Instead, call \ref bj_window_del.
/// 
/// Note that it is not possible to remove a closed flag once set.
///
/// \param p_window Pointer to the window object to flag.
///
/// \remark
///
/// This function effectively returns 
/// `bj_window_get_flags(p_window, BJ_WINDOW_FLAG_CLOSE) > 0`.
///
/// \see bj_window_should_close
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_window_set_should_close(
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
/// \see bj_window_set_should_close
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_bool bj_window_should_close(
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
BANJO_EXPORT uint8_t bj_window_get_flags(
    bj_window* p_window,
    uint8_t    flags
);

////////////////////////////////////////////////////////////////////////////////
/// Return the framebuffer attached to the window.
///
/// The framebuffer is an instance of \ref bj_bitmap attached (and owned) by
/// the window.
/// If necessary, \ref bj_window_get_flags will create (or recreate) the
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
BANJO_EXPORT bj_bitmap* bj_window_get_framebuffer(
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
BANJO_EXPORT int bj_window_get_size(
    const bj_window* p_window,
    int* width,
    int* height
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
/// \see bj_window_get_framebuffer
///
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_window_update_framebuffer(
    bj_window* p_window
);

/// \} // End of window group
