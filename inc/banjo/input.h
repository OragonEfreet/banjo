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
#define BJ_KEY_UNKNOWN 0x00

#define BJ_KEY_SPACE 0x20
#define BJ_KEY_APOSTROPHE 0xDE
#define BJ_KEY_COMMA 0xBC
#define BJ_KEY_MINUS 0xBD
#define BJ_KEY_PERIOD 0xBE
#define BJ_KEY_SLASH 0xBF
#define BJ_KEY_0 0x30
#define BJ_KEY_1 0x31
#define BJ_KEY_2 0x32
#define BJ_KEY_3 0x33
#define BJ_KEY_4 0x34
#define BJ_KEY_5 0x35
#define BJ_KEY_6 0x36
#define BJ_KEY_7 0x37
#define BJ_KEY_8 0x38
#define BJ_KEY_9 0x39
#define BJ_KEY_A 0x41
#define BJ_KEY_B 0x42
#define BJ_KEY_C 0x43
#define BJ_KEY_D 0x44
#define BJ_KEY_E 0x45
#define BJ_KEY_F 0x46
#define BJ_KEY_G 0x47
#define BJ_KEY_H 0x48
#define BJ_KEY_I 0x49
#define BJ_KEY_J 0x4A
#define BJ_KEY_K 0x4B
#define BJ_KEY_L 0x4C
#define BJ_KEY_M 0x4D
#define BJ_KEY_N 0x4E
#define BJ_KEY_O 0x4F
#define BJ_KEY_P 0x50
#define BJ_KEY_Q 0x51
#define BJ_KEY_R 0x52
#define BJ_KEY_S 0x53
#define BJ_KEY_T 0x54
#define BJ_KEY_U 0x55
#define BJ_KEY_V 0x56
#define BJ_KEY_W 0x57
#define BJ_KEY_X 0x58
#define BJ_KEY_Y 0x59
#define BJ_KEY_Z 0x5A
#define BJ_KEY_SEMICOLON 0xBA
#define BJ_KEY_EQUAL 0xBB
#define BJ_KEY_LEFT_BRACKET 0xDB
#define BJ_KEY_BACKSLASH 0xDC
#define BJ_KEY_RIGHT_BRACKET 0xDD
#define BJ_KEY_GRAVE_ACCENT 0xC0
#define BJ_KEY_ESCAPE 0x1B
#define BJ_KEY_RETURN 0x0D
#define BJ_KEY_TAB 0x09
#define BJ_KEY_BACKSPACE 0x08
#define BJ_KEY_INSERT 0x2D
#define BJ_KEY_DELETE 0x2E
#define BJ_KEY_LEFT 0x25
#define BJ_KEY_UP 0x26
#define BJ_KEY_RIGHT 0x27
#define BJ_KEY_DOWN 0x28
#define BJ_KEY_PRIOR 0x21
#define BJ_KEY_NEXT 0x22
#define BJ_KEY_HOME 0x24
#define BJ_KEY_END 0x23
#define BJ_KEY_CAPSLOCK 0x14
#define BJ_KEY_SCROLL 0x91
#define BJ_KEY_NUMLOCK 0x90
#define BJ_KEY_PRINTSCREEN 0x2C
#define BJ_KEY_PAUSE 0x13
#define BJ_KEY_F1 0x70
#define BJ_KEY_F2 0x71
#define BJ_KEY_F3 0x72
#define BJ_KEY_F4 0x73
#define BJ_KEY_F5 0x74
#define BJ_KEY_F6 0x75
#define BJ_KEY_F7 0x76
#define BJ_KEY_F8 0x77
#define BJ_KEY_F9 0x78
#define BJ_KEY_F10 0x79
#define BJ_KEY_F11 0x7A
#define BJ_KEY_F12 0x7B
#define BJ_KEY_F13 0x7C
#define BJ_KEY_F14 0x7D
#define BJ_KEY_F15 0x7E
#define BJ_KEY_F16 0x7F
#define BJ_KEY_F17 0x80
#define BJ_KEY_F18 0x81
#define BJ_KEY_F19 0x82
#define BJ_KEY_F20 0x83
#define BJ_KEY_F21 0x84
#define BJ_KEY_F22 0x85
#define BJ_KEY_F23 0x86
#define BJ_KEY_F24 0x87
#define BJ_KEY_PAD_0 0x60
#define BJ_KEY_PAD_1 0x61
#define BJ_KEY_PAD_2 0x62
#define BJ_KEY_PAD_3 0x63
#define BJ_KEY_PAD_4 0x64
#define BJ_KEY_PAD_5 0x65
#define BJ_KEY_PAD_6 0x66
#define BJ_KEY_PAD_7 0x67
#define BJ_KEY_PAD_8 0x68
#define BJ_KEY_PAD_9 0x69
#define BJ_KEY_PAD_DECIMAL 0x6E
#define BJ_KEY_PAD_DIVIDE 0x6F
#define BJ_KEY_PAD_MULTIPLY 0x6A
#define BJ_KEY_PAD_SUBTRACT 0x6D
#define BJ_KEY_PAD_ADD 0x6B
#define BJ_KEY_SHIFT_LEFT 0xA0
#define BJ_KEY_SHIFT_RIGHT 0xA1
#define BJ_KEY_CONTROL_LEFT 0xA2
#define BJ_KEY_CONTROL_RIGHT 0xA3
#define BJ_KEY_ALT_LEFT 0xA4
#define BJ_KEY_ALT_RIGHT 0xA5
#define BJ_KEY_OS_LEFT 0x5B
#define BJ_KEY_OS_RIGHT 0x5C

BANJO_EXPORT const char* bj_get_key_name(int key);

/// \} //
