////////////////////////////////////////////////////////////////////////////////
/// \file
/// Header file for various event types
#pragma once

////////////////////////////////////////////////////////////////////////////////
/// Event action type enumeration.
///
/// Used in key and button events to identify the action type.
////////////////////////////////////////////////////////////////////////////////
typedef enum bj_event_action_t {
    BJ_RELEASE, ///!< The button/key is released
    BJ_PRESS,   ///!< The button/key is pressed
    BJ_REPEAT,  ///!< The button/key is kept being pressed
} bj_event_action;


////////////////////////////////////////////////////////////////////////////////
/// Button identifiers

/// First button
#define BJ_BUTTON_1      1
/// Second button
#define BJ_BUTTON_2      2
/// Third button
#define BJ_BUTTON_3      3
/// Fourth button
#define BJ_BUTTON_4      4
/// Fifth button
#define BJ_BUTTON_5      5
/// Left mouse button
#define BJ_BUTTON_LEFT   BJ_BUTTON_1
/// Right mouse button
#define BJ_BUTTON_RIGHT  BJ_BUTTON_3
/// Middle mouse button
#define BJ_BUTTON_MIDDLE BJ_BUTTON_2
/// Scroll up mouse button
#define BJ_BUTTON_UP     BJ_BUTTON_4
/// Scroll down mouse button
#define BJ_BUTTON_DOWN   BJ_BUTTON_5

