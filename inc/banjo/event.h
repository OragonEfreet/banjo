#pragma once

#include <banjo/window.h>

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
typedef void(* bj_window_enter_event_t)(bj_window* p_window, bj_bool enter, int x, int y);

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
/// \param key      Layout-dependent representation of the pressed key
/// \param scancode Implementation-dependent / Layout-independent representation 
///                 of the key that has been pressed or released.
///
/// \see bj_window_set_key_event
///
////////////////////////////////////////////////////////////////////////////////
typedef void(* bj_window_key_event_t)(bj_window* p_window, bj_event_action action, bj_key key, int scancode);

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
/// \par Behaviour
///
/// Pressing a key on the keyboard will generate a single call event with
/// \ref BJ_PRESS action. Releasing the key generates a single call with 
/// \ref BJ_RELEASE.
/// If the \ref BJ_WINDOW_FLAG_KEY_REPEAT flag is set for a window, holding
/// down the key will continuously call the event function with a 
/// \ref BJ_REPEAT action.
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


