////////////////////////////////////////////////////////////////////////////////
/// \file
/// Header file for various event types
////////////////////////////////////////////////////////////////////////////////
/// \defgroup events Events
/// \ingroup system
/// \{
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <banjo/api.h>

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

/// List of possible keys on a keyboard.
/// The values are taken from the VK codes in Microsoft API
typedef enum bj_key_t {
    BJ_KEY_UNKNOWN       = 0x00,
    BJ_KEY_SPACE         = 0x20,
    BJ_KEY_APOSTROPHE    = 0xDE,
    BJ_KEY_COMMA         = 0xBC,
    BJ_KEY_MINUS         = 0xBD,
    BJ_KEY_PERIOD        = 0xBE,
    BJ_KEY_SLASH         = 0xBF,
    BJ_KEY_0             = 0x30,
    BJ_KEY_1             = 0x31,
    BJ_KEY_2             = 0x32,
    BJ_KEY_3             = 0x33,
    BJ_KEY_4             = 0x34,
    BJ_KEY_5             = 0x35,
    BJ_KEY_6             = 0x36,
    BJ_KEY_7             = 0x37,
    BJ_KEY_8             = 0x38,
    BJ_KEY_9             = 0x39,
    BJ_KEY_A             = 0x41,
    BJ_KEY_B             = 0x42,
    BJ_KEY_C             = 0x43,
    BJ_KEY_D             = 0x44,
    BJ_KEY_E             = 0x45,
    BJ_KEY_F             = 0x46,
    BJ_KEY_G             = 0x47,
    BJ_KEY_H             = 0x48,
    BJ_KEY_I             = 0x49,
    BJ_KEY_J             = 0x4A,
    BJ_KEY_K             = 0x4B,
    BJ_KEY_L             = 0x4C,
    BJ_KEY_M             = 0x4D,
    BJ_KEY_N             = 0x4E,
    BJ_KEY_O             = 0x4F,
    BJ_KEY_P             = 0x50,
    BJ_KEY_Q             = 0x51,
    BJ_KEY_R             = 0x52,
    BJ_KEY_S             = 0x53,
    BJ_KEY_T             = 0x54,
    BJ_KEY_U             = 0x55,
    BJ_KEY_V             = 0x56,
    BJ_KEY_W             = 0x57,
    BJ_KEY_X             = 0x58,
    BJ_KEY_Y             = 0x59,
    BJ_KEY_Z             = 0x5A,
    BJ_KEY_SEMICOLON     = 0xBA,
    BJ_KEY_EQUAL         = 0xBB,
    BJ_KEY_LEFT_BRACKET  = 0xDB,
    BJ_KEY_BACKSLASH     = 0xDC,
    BJ_KEY_RIGHT_BRACKET = 0xDD,
    BJ_KEY_GRAVE_ACCENT  = 0xC0,
    BJ_KEY_ESCAPE        = 0x1B,
    BJ_KEY_RETURN        = 0x0D,
    BJ_KEY_TAB           = 0x09,
    BJ_KEY_BACKSPACE     = 0x08,
    BJ_KEY_INSERT        = 0x2D,
    BJ_KEY_DELETE        = 0x2E,
    BJ_KEY_LEFT          = 0x25,
    BJ_KEY_UP            = 0x26,
    BJ_KEY_RIGHT         = 0x27,
    BJ_KEY_DOWN          = 0x28,
    BJ_KEY_PRIOR         = 0x21,
    BJ_KEY_NEXT          = 0x22,
    BJ_KEY_HOME          = 0x24,
    BJ_KEY_END           = 0x23,
    BJ_KEY_CAPSLOCK      = 0x14,
    BJ_KEY_SCROLL        = 0x91,
    BJ_KEY_NUMLOCK       = 0x90,
    BJ_KEY_PRINTSCREEN   = 0x2C,
    BJ_KEY_PAUSE         = 0x13,
    BJ_KEY_F1            = 0x70,
    BJ_KEY_F2            = 0x71,
    BJ_KEY_F3            = 0x72,
    BJ_KEY_F4            = 0x73,
    BJ_KEY_F5            = 0x74,
    BJ_KEY_F6            = 0x75,
    BJ_KEY_F7            = 0x76,
    BJ_KEY_F8            = 0x77,
    BJ_KEY_F9            = 0x78,
    BJ_KEY_F10           = 0x79,
    BJ_KEY_F11           = 0x7A,
    BJ_KEY_F12           = 0x7B,
    BJ_KEY_F13           = 0x7C,
    BJ_KEY_F14           = 0x7D,
    BJ_KEY_F15           = 0x7E,
    BJ_KEY_F16           = 0x7F,
    BJ_KEY_F17           = 0x80,
    BJ_KEY_F18           = 0x81,
    BJ_KEY_F19           = 0x82,
    BJ_KEY_F20           = 0x83,
    BJ_KEY_F21           = 0x84,
    BJ_KEY_F22           = 0x85,
    BJ_KEY_F23           = 0x86,
    BJ_KEY_F24           = 0x87,
    BJ_KEY_PAD_0         = 0x60,
    BJ_KEY_PAD_1         = 0x61,
    BJ_KEY_PAD_2         = 0x62,
    BJ_KEY_PAD_3         = 0x63,
    BJ_KEY_PAD_4         = 0x64,
    BJ_KEY_PAD_5         = 0x65,
    BJ_KEY_PAD_6         = 0x66,
    BJ_KEY_PAD_7         = 0x67,
    BJ_KEY_PAD_8         = 0x68,
    BJ_KEY_PAD_9         = 0x69,
    BJ_KEY_PAD_DECIMAL   = 0x6E,
    BJ_KEY_PAD_DIVIDE    = 0x6F,
    BJ_KEY_PAD_MULTIPLY  = 0x6A,
    BJ_KEY_PAD_SUBTRACT  = 0x6D,
    BJ_KEY_PAD_ADD       = 0x6B,
    BJ_KEY_SHIFT_LEFT    = 0xA0,
    BJ_KEY_SHIFT_RIGHT   = 0xA1,
    BJ_KEY_CONTROL_LEFT  = 0xA2,
    BJ_KEY_CONTROL_RIGHT = 0xA3,
    BJ_KEY_ALT_LEFT      = 0xA4,
    BJ_KEY_ALT_RIGHT     = 0xA5,
    BJ_KEY_OS_LEFT       = 0x5B,
    BJ_KEY_OS_RIGHT      = 0x5C,
    BJ_KEY_MENU          = 0x5D,
} bj_key;

BANJO_EXPORT const char* bj_get_key_name(int key);

/// \} //
