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
#include <banjo/event.h>
#include <banjo/memory.h>

/// Opaque typedef for the window type
typedef struct bj_window_t bj_window;

////////////////////////////////////////////////////////////////////////////////
/// Create a new bj_window with the specified attributes
///
/// \param p_title  Title of the window
/// \param x  Horizontal position of the window on-screen, expressed in pixels
/// \param y  Vertical position of the window on-screen, expressed in pixels
/// \param width  Width of the window.
/// \param height Height of the window.
///
/// \return A pointer to the newly created bj_window object.
///
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_window* bj_window_new(
    const char* p_title,
    uint16_t x,
    uint16_t y,
    uint16_t width,
    uint16_t height
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
/// \see bj_window_set_should_close
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bool bj_window_should_close(
    bj_window* p_window
);

////////////////////////////////////////////////////////////////////////////////
/// Callback type for functions called when the mouse cursor enters a window.
///
/// \param p_window Window handle
/// \param enter    _true_ if the cursor enters the window, _false_ otherwise.
/// \param x        The horizontal position of the cursor
/// \param y        The vertical position of the cursor
///
/// \see bj_window_set_enter_event
////////////////////////////////////////////////////////////////////////////////
typedef void(* bj_window_enter_event_t)(bj_window* p_window, bool enter, int x, int y);

////////////////////////////////////////////////////////////////////////////////
/// Callback type for functions called when the mouse cursor position changes.
///
/// \param p_window Window handle
/// \param x        The horizontal position of the cursor
/// \param y        The vertical position of the cursor
///
/// \see bj_window_set_cursor_event
////////////////////////////////////////////////////////////////////////////////
typedef void(* bj_window_cursor_event_t)(bj_window* p_window, int x, int y);

////////////////////////////////////////////////////////////////////////////////
/// Callback type for functions called when a mouse button is pressed or released.
///
/// \param p_window Window handle
/// \param action   Sets if the button is pressed or released
/// \param x        The horizontal position of the cursor
/// \param y        The vertical position of the cursor
///
/// \see bj_window_set_button_event
////////////////////////////////////////////////////////////////////////////////
typedef void(* bj_window_button_event_t)(bj_window* p_window, int, bj_event_action action, int x, int y);

////////////////////////////////////////////////////////////////////////////////
/// Callback type for functions called when a keyboard key is pressed or released.
///
/// \param p_window Window handle
/// \param action   Sets if the button is pressed or released
/// \param keycode  Key code
///
/// \see bj_window_set_key_event
////////////////////////////////////////////////////////////////////////////////
typedef void(* bj_window_key_event_t)(bj_window* p_window, bj_event_action action, unsigned int keycode);


////////////////////////////////////////////////////////////////////////////////
/// Set the callback for cursor events.
///
/// The provided callback is called each time a cursor event is raised by the
/// system.
/// If an event was already set previously, it is returned to the caller.
/// The event set is used until a new callback function is set.
///
/// \param p_window   The window handler
/// \param p_callback The callback function
/// \return *0* or the previously set callback function if any.
///
/// \see bj_window_cursor_event_t
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_window_cursor_event_t bj_window_set_cursor_event(
    bj_window*                 p_window,
    bj_window_cursor_event_t   p_callback
);


////////////////////////////////////////////////////////////////////////////////
/// Set the callback for button events.
///
/// The providded callback is called each time a button event is raised by the
/// system.
/// If an event was already set previously, it is returned to the caller.
/// The vent set is user until a new callback function is set.
///
/// \param p_window   The window handler
/// \param p_callback The callback function
/// \return *0* or the previously set callback function if any.
///
/// \see bj_window_button_event_t
///
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_window_button_event_t bj_window_set_button_event(
    bj_window*                 p_window,
    bj_window_button_event_t   p_callback
);


////////////////////////////////////////////////////////////////////////////////
/// Set the callback for key events.
///
/// The providded callback is called each time a key event is raised by the
/// system.
/// If an event was already set previously, it is returned to the caller.
/// The vent set is user until a new callback function is set.
///
/// \param p_window   The window handler
/// \param p_callback The callback function
/// \return *0* or the previously set callback function if any.
///
/// \see bj_window_key_event_t
///
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_window_key_event_t bj_window_set_key_event(
    bj_window*              p_window,
    bj_window_key_event_t   p_callback
);


////////////////////////////////////////////////////////////////////////////////
/// Set the callback for enter events.
///
/// The providded callback is called each time an enter event is raised by the
/// system.
/// If an event was already set previously, it is returned to the caller.
/// The vent set is user until a new callback function is set.
///
/// \param p_window   The window handler
/// \param p_callback The callback function
/// \return *0* or the previously set callback function if any.
///
/// \see bj_window_enter_event_t
///
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_window_enter_event_t bj_window_set_enter_event(
    bj_window*                p_window,
    bj_window_enter_event_t   p_callback
);

////////////////////////////////////////////////////////////////////////////////
/// Polls all pending events and dispatch them to callbacks.
///
/// All events received by the system will be processed and sent to the event
/// callbacks.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_poll_events(
    void
);

/// \} // End of window group
