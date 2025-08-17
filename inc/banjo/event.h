////////////////////////////////////////////////////////////////////////////////
/// \defgroup event Event
/// \ingroup core
///
/// \brief Handle input events such as keyboard, mouse and focus
///
/// The event system provides a simple callback interface for handling
/// user interactions with windows, including cursor movement, button presses,
/// keyboard input, and window enter/leave detection.
///
/// Events are dispatched via polling with \ref bj_dispatch_events.
/// \{
////////////////////////////////////////////////////////////////////////////////

#ifndef BJ_EVENT_H
#define BJ_EVENT_H

#include <banjo/window.h>

// TODO Align buttons and keys
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

////////////////////////////////////////////////////////////////////////////////
/// List of possible keys on a keyboard.
///
/// The values are directly taken from the [VK codes in Microsoft API]
/// (https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes)
/// This also includes the annotated documentation of each key.
///
/// Even if all values from the abovementioned page are present in this enum,
/// not all are used by Banjo and some are never returned by key events.
///
/// Values for 0-9 and A-Z keys directly directly correspond to their ASCII
/// equivalent.
///
////////////////////////////////////////////////////////////////////////////////
typedef enum bj_key_t {
    BJ_KEY_UNKNOWN             = 0x00,  //!< No Button
    BJ_KEY_LBUTTON             = 0x01,  //!< Left mouse button
    BJ_KEY_RBUTTON             = 0x02,  //!< Right mouse button
    BJ_KEY_CANCEL              = 0x03,  //!< Control-break processing
    BJ_KEY_MBUTTON             = 0x04,  //!< Middle mouse button
    BJ_KEY_XBUTTON1            = 0x05,  //!< X1 mouse button
    BJ_KEY_XBUTTON2            = 0x06,  //!< X2 mouse button
    // 0x07 	Reserved
    BJ_KEY_BACK                = 0x08,  //!< Backspace key
    BJ_KEY_TAB                 = 0x09,  //!< Tab key
    // 0x0A-0B 	Reserved
    BJ_KEY_CLEAR               = 0x0C,  //!< Clear key
    BJ_KEY_RETURN              = 0x0D,  //!< Enter key
    // 0x0E-0F 	Unassigned
    BJ_KEY_SHIFT               = 0x10,  //!< Shift key
    BJ_KEY_CONTROL             = 0x11,  //!< Ctrl key
    BJ_KEY_MENU                = 0x12,  //!< Alt key
    BJ_KEY_PAUSE               = 0x13,  //!< Pause key
    BJ_KEY_CAPITAL             = 0x14,  //!< Caps lock key
    BJ_KEY_KANA                = 0x15,  //!< IME Kana mode
    BJ_KEY_IME_ON              = 0x16,  //!< IME On
    BJ_KEY_JUNJA               = 0x17,  //!< IME Junja mode
    BJ_KEY_FINAL               = 0x18,  //!< IME final mode
    BJ_KEY_KANJI               = 0x19,  //!< IME Hanja mode
    BJ_KEY_IME_OFF             = 0x1A,  //!< IME Off
    BJ_KEY_ESCAPE              = 0x1B,  //!< Esc key
    BJ_KEY_CONVERT             = 0x1C,  //!< IME convert
    BJ_KEY_NONCONVERT          = 0x1D,  //!< IME nonconvert
    BJ_KEY_ACCEPT              = 0x1E,  //!< IME accept
    BJ_KEY_MODECHANGE          = 0x1F,  //!< IME mode change request
    BJ_KEY_SPACE               = 0x20,  //!< Spacebar key
    BJ_KEY_PRIOR               = 0x21,  //!< Page up key
    BJ_KEY_NEXT                = 0x22,  //!< Page down key
    BJ_KEY_END                 = 0x23,  //!< End key
    BJ_KEY_HOME                = 0x24,  //!< Home key
    BJ_KEY_LEFT                = 0x25,  //!< Left arrow key
    BJ_KEY_UP                  = 0x26,  //!< Up arrow key
    BJ_KEY_RIGHT               = 0x27,  //!< Right arrow key
    BJ_KEY_DOWN                = 0x28,  //!< Down arrow key
    BJ_KEY_SELECT              = 0x29,  //!< Select key
    BJ_KEY_PRINT               = 0x2A,  //!< Print key
    BJ_KEY_EXECUTE             = 0x2B,  //!< Execute key
    BJ_KEY_SNAPSHOT            = 0x2C,  //!< Print screen key
    BJ_KEY_INSERT              = 0x2D,  //!< Insert key
    BJ_KEY_DELETE              = 0x2E,  //!< Delete key
    BJ_KEY_HELP                = 0x2F,  //!< Help key
    BJ_KEY_0                   = 0x30,  //!< 0 key
    BJ_KEY_1                   = 0x31,  //!< 1 key
    BJ_KEY_2                   = 0x32,  //!< 2 key
    BJ_KEY_3                   = 0x33,  //!< 3 key
    BJ_KEY_4                   = 0x34,  //!< 4 key
    BJ_KEY_5                   = 0x35,  //!< 5 key
    BJ_KEY_6                   = 0x36,  //!< 6 key
    BJ_KEY_7                   = 0x37,  //!< 7 key
    BJ_KEY_8                   = 0x38,  //!< 8 key
    BJ_KEY_9                   = 0x39,  //!< 9 key
    // 0x3A-40 	Undefined
    BJ_KEY_A                   = 0x41,  //!< A key
    BJ_KEY_B                   = 0x42,  //!< B key
    BJ_KEY_C                   = 0x43,  //!< C key
    BJ_KEY_D                   = 0x44,  //!< D key
    BJ_KEY_E                   = 0x45,  //!< E key
    BJ_KEY_F                   = 0x46,  //!< F key
    BJ_KEY_G                   = 0x47,  //!< G key
    BJ_KEY_H                   = 0x48,  //!< H key
    BJ_KEY_I                   = 0x49,  //!< I key
    BJ_KEY_J                   = 0x4A,  //!< J key
    BJ_KEY_K                   = 0x4B,  //!< K key
    BJ_KEY_L                   = 0x4C,  //!< L key
    BJ_KEY_M                   = 0x4D,  //!< M key
    BJ_KEY_N                   = 0x4E,  //!< N key
    BJ_KEY_O                   = 0x4F,  //!< O key
    BJ_KEY_P                   = 0x50,  //!< P key
    BJ_KEY_Q                   = 0x51,  //!< Q key
    BJ_KEY_R                   = 0x52,  //!< R key
    BJ_KEY_S                   = 0x53,  //!< S key
    BJ_KEY_T                   = 0x54,  //!< T key
    BJ_KEY_U                   = 0x55,  //!< U key
    BJ_KEY_V                   = 0x56,  //!< V key
    BJ_KEY_W                   = 0x57,  //!< W key
    BJ_KEY_X                   = 0x58,  //!< X key
    BJ_KEY_Y                   = 0x59,  //!< Y key
    BJ_KEY_Z                   = 0x5A,  //!< Z key
    BJ_KEY_LWIN                = 0x5B,  //!< Left Windows logo key
    BJ_KEY_RWIN                = 0x5C,  //!< Right Windows logo key
    BJ_KEY_APPS                = 0x5D,  //!< Application key
    // 0x5E 	Reserved
    BJ_KEY_SLEEP               = 0x5F,  //!< Computer Sleep key
    BJ_KEY_NUMPAD0             = 0x60,  //!< Numeric keypad 0 key
    BJ_KEY_NUMPAD1             = 0x61,  //!< Numeric keypad 1 key
    BJ_KEY_NUMPAD2             = 0x62,  //!< Numeric keypad 2 key
    BJ_KEY_NUMPAD3             = 0x63,  //!< Numeric keypad 3 key
    BJ_KEY_NUMPAD4             = 0x64,  //!< Numeric keypad 4 key
    BJ_KEY_NUMPAD5             = 0x65,  //!< Numeric keypad 5 key
    BJ_KEY_NUMPAD6             = 0x66,  //!< Numeric keypad 6 key
    BJ_KEY_NUMPAD7             = 0x67,  //!< Numeric keypad 7 key
    BJ_KEY_NUMPAD8             = 0x68,  //!< Numeric keypad 8 key
    BJ_KEY_NUMPAD9             = 0x69,  //!< Numeric keypad 9 key
    BJ_KEY_MULTIPLY            = 0x6A,  //!< Multiply key
    BJ_KEY_ADD                 = 0x6B,  //!< Add key
    BJ_KEY_SEPARATOR           = 0x6C,  //!< Separator key
    BJ_KEY_SUBTRACT            = 0x6D,  //!< Subtract key
    BJ_KEY_DECIMAL             = 0x6E,  //!< Decimal key
    BJ_KEY_DIVIDE              = 0x6F,  //!< Divide key
    BJ_KEY_F1                  = 0x70,  //!< F1 key
    BJ_KEY_F2                  = 0x71,  //!< F2 key
    BJ_KEY_F3                  = 0x72,  //!< F3 key
    BJ_KEY_F4                  = 0x73,  //!< F4 key
    BJ_KEY_F5                  = 0x74,  //!< F5 key
    BJ_KEY_F6                  = 0x75,  //!< F6 key
    BJ_KEY_F7                  = 0x76,  //!< F7 key
    BJ_KEY_F8                  = 0x77,  //!< F8 key
    BJ_KEY_F9                  = 0x78,  //!< F9 key
    BJ_KEY_F10                 = 0x79,  //!< F10 key
    BJ_KEY_F11                 = 0x7A,  //!< F11 key
    BJ_KEY_F12                 = 0x7B,  //!< F12 key
    BJ_KEY_F13                 = 0x7C,  //!< F13 key
    BJ_KEY_F14                 = 0x7D,  //!< F14 key
    BJ_KEY_F15                 = 0x7E,  //!< F15 key
    BJ_KEY_F16                 = 0x7F,  //!< F16 key
    BJ_KEY_F17                 = 0x80,  //!< F17 key
    BJ_KEY_F18                 = 0x81,  //!< F18 key
    BJ_KEY_F19                 = 0x82,  //!< F19 key
    BJ_KEY_F20                 = 0x83,  //!< F20 key
    BJ_KEY_F21                 = 0x84,  //!< F21 key
    BJ_KEY_F22                 = 0x85,  //!< F22 key
    BJ_KEY_F23                 = 0x86,  //!< F23 key
    BJ_KEY_F24                 = 0x87,  //!< F24 key
    // 0x88-8F 	Reserved
    BJ_KEY_NUMLOCK             = 0x90,  //!< Num lock key
    BJ_KEY_SCROLL              = 0x91,  //!< Scroll lock key
    // 0x92-96 	OEM specific
    // 0x97-9F 	Unassigned
    BJ_KEY_LSHIFT              = 0xA0,  //!< Left Shift key
    BJ_KEY_RSHIFT              = 0xA1,  //!< Right Shift key
    BJ_KEY_LCONTROL            = 0xA2,  //!< Left Ctrl key
    BJ_KEY_RCONTROL            = 0xA3,  //!< Right Ctrl key
    BJ_KEY_LMENU               = 0xA4,  //!< Left Alt key
    BJ_KEY_RMENU               = 0xA5,  //!< Right Alt key
    BJ_KEY_BROWSER_BACK        = 0xA6,  //!< Browser Back key
    BJ_KEY_BROWSER_FORWARD     = 0xA7,  //!< Browser Forward key
    BJ_KEY_BROWSER_REFRESH     = 0xA8,  //!< Browser Refresh key
    BJ_KEY_BROWSER_STOP        = 0xA9,  //!< Browser Stop key
    BJ_KEY_BROWSER_SEARCH      = 0xAA,  //!< Browser Search key
    BJ_KEY_BROWSER_FAVORITES   = 0xAB,  //!< Browser Favorites key
    BJ_KEY_BROWSER_HOME        = 0xAC,  //!< Browser Start and Home key
    BJ_KEY_VOLUME_MUTE         = 0xAD,  //!< Volume Mute key
    BJ_KEY_VOLUME_DOWN         = 0xAE,  //!< Volume Down key
    BJ_KEY_VOLUME_UP           = 0xAF,  //!< Volume Up key
    BJ_KEY_MEDIA_NEXT_TRACK    = 0xB0,  //!< Next Track key
    BJ_KEY_MEDIA_PREV_TRACK    = 0xB1,  //!< Previous Track key
    BJ_KEY_MEDIA_STOP          = 0xB2,  //!< Stop Media key
    BJ_KEY_MEDIA_PLAY_PAUSE    = 0xB3,  //!< Play/Pause Media key
    BJ_KEY_LAUNCH_MAIL         = 0xB4,  //!< Start Mail key
    BJ_KEY_LAUNCH_MEDIA_SELECT = 0xB5,  //!< Select Media key
    BJ_KEY_LAUNCH_APP1         = 0xB6,  //!< Start Application 1 key
    BJ_KEY_LAUNCH_APP2         = 0xB7,  //!< Start Application 2 key
    // 0xB8-B9 	Reserved
    BJ_KEY_OEM_1               = 0xBA,  //!< Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the ;: key
    BJ_KEY_OEM_PLUS            = 0xBB,  //!< For any country/region, the + key
    BJ_KEY_OEM_COMMA           = 0xBC,  //!< For any country/region, the , key
    BJ_KEY_OEM_MINUS           = 0xBD,  //!< For any country/region, the - key
    BJ_KEY_OEM_PERIOD          = 0xBE,  //!< For any country/region, the . key
    BJ_KEY_OEM_2               = 0xBF,  //!< Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the /? key
    BJ_KEY_OEM_3               = 0xC0,  //!< Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the `~ key
    // 0xC1-DA 	Reserved
    BJ_KEY_OEM_4               = 0xDB,  //!< Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the [{ key
    BJ_KEY_OEM_5               = 0xDC,  //!< Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the \\| key
    BJ_KEY_OEM_6               = 0xDD,  //!< Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the ]} key
    BJ_KEY_OEM_7               = 0xDE,  //!< Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '" key
    BJ_KEY_OEM_8               = 0xDF,  //!< Used for miscellaneous characters; it can vary by keyboard.
    // 0xE0 	Reserved
    // 0xE1 	OEM specific
    BJ_KEY_OEM_102             = 0xE2,  //!< The <> keys on the US standard keyboard, or the \\| key on the non-US 102-key keyboard
    // 0xE3-E4 	OEM specific
    BJ_KEY_PROCESSKEY          = 0xE5,  //!< IME PROCESS key
    // 0xE6 	OEM specific
    BJ_KEY_PACKET              = 0xE7,  //!< Used to pass Unicode characters as if they were keystrokes. The VK_PACKET key is the low word of a 32-bit Virtual Key value used for non-keyboard input methods. For more information, see Remark in KEYBDINPUT, SendInput, WM_KEYDOWN, and WM_KEYUP
    // 0xE8 	Unassigned
    // 0xE9-F5 	OEM specific
    BJ_KEY_ATTN                = 0xF6,  //!< Attn key
    BJ_KEY_CRSEL               = 0xF7,  //!< CrSel key
    BJ_KEY_EXSEL               = 0xF8,  //!< ExSel key
    BJ_KEY_EREOF               = 0xF9,  //!< Erase EOF key
    BJ_KEY_PLAY                = 0xFA,  //!< Play key
    BJ_KEY_ZOOM                = 0xFB,  //!< Zoom key
    BJ_KEY_NONAME              = 0xFC,  //!< Reserved
    BJ_KEY_PA1                 = 0xFD,  //!< PA1 key
    BJ_KEY_OEM_CLEAR           = 0xFE,  //!< Clear key
} bj_key;

// Some Keycode aliases
#define BJ_KEY_APOSTROPHE    BJ_KEY_OEM_7      //!< Alias for \ref BJ_KEY_OEM_7.
#define BJ_KEY_BACKSLASH     BJ_KEY_OEM_5      //!< Alias for \ref BJ_KEY_OEM_5.
#define BJ_KEY_BACKSPACE     BJ_KEY_BACK       //!< Alias for \ref BJ_KEY_BACK.
#define BJ_KEY_CAPSLOCK      BJ_KEY_CAPITAL    //!< Alias for \ref BJ_KEY_CAPITAL.
#define BJ_KEY_COMMA         BJ_KEY_OEM_COMMA  //!< Alias for \ref BJ_KEY_OEM_COMMA.
#define BJ_KEY_ENTER         BJ_KEY_RETURN     //!< Alias for \ref BJ_KEY_RETURN.
#define BJ_KEY_GRAVE_ACCENT  BJ_KEY_OEM_3      //!< Alias for \ref BJ_KEY_OEM_3.
#define BJ_KEY_HANGUL        BJ_KEY_KANA       //!< Alias for \ref BJ_KEY_KANA.
#define BJ_KEY_HANJA         BJ_KEY_KANJI      //!< Alias for \ref BJ_KEY_KANJI.
#define BJ_KEY_LEFT_ALT      BJ_KEY_LMENU      //!< Alias for \ref BJ_KEY_LMENU.
#define BJ_KEY_LEFT_BRACKET  BJ_KEY_OEM_4      //!< Alias for \ref BJ_KEY_OEM_4.
#define BJ_KEY_LEFT_CONTROL  BJ_KEY_LCONTROL   //!< Alias for \ref BJ_KEY_LCONTROL.
#define BJ_KEY_LEFT_OS       BJ_KEY_LWIN       //!< Alias for \ref BJ_KEY_LWIN.
#define BJ_KEY_LEFT_SHIT     BJ_KEY_LSHIFT     //!< Alias for \ref BJ_KEY_LSHIFT.
#define BJ_KEY_MINUS         BJ_KEY_OEM_MINUS  //!< Alias for \ref BJ_KEY_OEM_MINUS.
#define BJ_KEY_PAGE_DOWN     BJ_KEY_NEXT       //!< Alias for \ref BJ_KEY_NEXT.
#define BJ_KEY_PAGE_UP       BJ_KEY_PRIOR      //!< Alias for \ref BJ_KEY_PRIOR.
#define BJ_KEY_PERIOD        BJ_KEY_OEM_PERIOD //!< Alias for \ref BJ_KEY_OEM_PERIOD.
#define BJ_KEY_PLUS          BJ_KEY_OEM_PLUS   //!< Alias for \ref BJ_KEY_OEM_PLUS.
#define BJ_KEY_PRINT_SCREEN  BJ_KEY_SNAPSHOT   //!< Alias for \ref BJ_KEY_SNAPSHOT.
#define BJ_KEY_RIGHT_ALT     BJ_KEY_RMENU      //!< Alias for \ref BJ_KEY_RMENU.
#define BJ_KEY_RIGHT_BRACKET BJ_KEY_OEM_6      //!< Alias for \ref BJ_KEY_OEM_6.
#define BJ_KEY_RIGHT_CONTROL BJ_KEY_RCONTROL   //!< Alias for \ref BJ_KEY_RCONTROL.
#define BJ_KEY_RIGHT_OS      BJ_KEY_RWIN       //!< Alias for \ref BJ_KEY_RWIN.
#define BJ_KEY_RIGHT_SHIFT   BJ_KEY_RSHIFT     //!< Alias for \ref BJ_KEY_RSHIFT.
#define BJ_KEY_SEMICOLON     BJ_KEY_OEM_1      //!< Alias for \ref BJ_KEY_OEM_1.
#define BJ_KEY_SLASH         BJ_KEY_OEM_2      //!< Alias for \ref BJ_KEY_OEM_2.

////////////////////////////////////////////////////////////////////////////////
/// \brief Get the string name of a key.
///
/// Return a string representing the key code. The returned string omits
/// the `BJ_KEY_` prefix.
///
/// \param key The \ref bj_key value.
/// \return A null-terminated string owned by Banjo.
///
/// \see bj_key
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT const char* bj_get_key_name(int key);

////////////////////////////////////////////////////////////////////////////////
/// \brief Define event action types for keys or mouse buttons.
////////////////////////////////////////////////////////////////////////////////
typedef enum bj_event_action_t {
    BJ_RELEASE, //!< The key or button was released
    BJ_PRESS,   //!< The key or button was pressed
    BJ_REPEAT,  //!< The key is being held (repeats)
} bj_event_action;

////////////////////////////////////////////////////////////////////////////////
/// \brief Represent a mouse enter or leave event.
////////////////////////////////////////////////////////////////////////////////
typedef struct bj_enter_event_t {
    int     x;     ///< Cursor x position
    int     y;     ///< Cursor y position
    bj_bool enter; ///< BJ_TRUE if entering window, BJ_FALSE if leaving
} bj_enter_event;

////////////////////////////////////////////////////////////////////////////////
/// \brief Represent a mouse cursor movement event.
////////////////////////////////////////////////////////////////////////////////
typedef struct bj_cursor_event_t {
    int x; ///< Cursor x position
    int y; ///< Cursor y position
} bj_cursor_event;

////////////////////////////////////////////////////////////////////////////////
/// \brief Represent a mouse button event.
////////////////////////////////////////////////////////////////////////////////
typedef struct bj_button_event_t {
    int             x;       ///< Cursor x position
    int             y;       ///< Cursor y position
    bj_event_action action; ///< Action (press/release)
    int             button; ///< Button identifier (e.g., BJ_BUTTON_LEFT)
} bj_button_event;

////////////////////////////////////////////////////////////////////////////////
/// \brief Represent a keyboard key event.
////////////////////////////////////////////////////////////////////////////////
typedef struct bj_key_event_t {
    bj_key          key;      ///< Key identifier
    int             scancode; ///< Scancode (layout-independent)
    bj_event_action action;   ///< Action (press/release/repeat)
} bj_key_event;

////////////////////////////////////////////////////////////////////////////////
/// \brief Identify the type of a generic event.
////////////////////////////////////////////////////////////////////////////////
typedef enum bj_event_type_t {
    BJ_EVENT_ENTER,  ///< Mouse enter/leave
    BJ_EVENT_CURSOR, ///< Mouse move
    BJ_EVENT_KEY,    ///< Keyboard key
    BJ_EVENT_BUTTON, ///< Mouse button
} bj_event_type;

////////////////////////////////////////////////////////////////////////////////
/// \brief Represent a generic window-related event.
///
/// This union-based structure wraps all types of input events.
////////////////////////////////////////////////////////////////////////////////
typedef struct bj_event_t {
    bj_window*     window; ///< Target window
    bj_event_type  type;   ///< Type of event
    union {
        bj_key_event    key;    ///< Key event data
        bj_button_event button; ///< Button event data
        bj_cursor_event cursor; ///< Cursor event data
        bj_enter_event  enter;  ///< Enter/leave event data
    };
} bj_event;

////////////////////////////////////////////////////////////////////////////////
/// \brief Define the callback type for enter events.
///
/// Called when the cursor enters or exits a window.
/// \see bj_set_enter_callback
////////////////////////////////////////////////////////////////////////////////
typedef void(* bj_enter_callback_fn_t)(bj_window*, const bj_enter_event*, void*);

////////////////////////////////////////////////////////////////////////////////
/// \brief Define the callback type for cursor movement events.
///
/// Called when the cursor moves inside a window.
/// \see bj_set_cursor_callback
////////////////////////////////////////////////////////////////////////////////
typedef void(* bj_cursor_callback_fn_t)(bj_window* p_window, const bj_cursor_event*, void*);

////////////////////////////////////////////////////////////////////////////////
/// \brief Define the callback type for mouse button events.
///
/// Called when a mouse button is pressed or released.
/// \see bj_set_button_callback
////////////////////////////////////////////////////////////////////////////////
typedef void(* bj_button_callback_fn_t)(bj_window* p_window, const bj_button_event*, void*);

////////////////////////////////////////////////////////////////////////////////
/// \brief Define the callback type for keyboard key events.
///
/// Called when a key is pressed, released, or repeated.
/// \see bj_set_key_callback
////////////////////////////////////////////////////////////////////////////////
typedef void(* bj_key_callback_fn_t)(bj_window* p_window, const bj_key_event*, void*);

////////////////////////////////////////////////////////////////////////////////
/// \brief Set the global callback for cursor events.
///
/// \param p_callback The new callback function
/// \param p_user_data Location to user-provided data.
/// \return The previously set callback, or NULL if none
///
/// \see bj_cursor_callback_fn_t
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_cursor_callback_fn_t bj_set_cursor_callback(
    bj_cursor_callback_fn_t p_callback,
    void*                   p_user_data
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Set the global callback for mouse button events.
///
/// \param p_callback The new callback function
/// \param p_user_data Location to user-provided data.
/// \return The previously set callback, or NULL if none
///
/// \see bj_button_callback_fn_t
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_button_callback_fn_t bj_set_button_callback(
    bj_button_callback_fn_t p_callback,
    void*                   p_user_data
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Set the global callback for keyboard key events.
///
/// \param p_callback The new callback function
/// \param p_user_data Location to user-provided data.
/// \return The previously set callback, or NULL if none
///
/// \par Key Repeat
/// If the window has the \ref BJ_WINDOW_FLAG_KEY_REPEAT flag set,
/// \ref BJ_REPEAT actions will be sent when holding keys.
///
/// \see bj_key_callback_fn_t
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_key_callback_fn_t bj_set_key_callback(
    bj_key_callback_fn_t p_callback,
    void*                p_user_data
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Set the global callback for mouse enter/leave events.
///
/// \param p_callback The new callback function
/// \param p_user_data Location to user-provided data.
/// \return The previously set callback, or NULL if none
///
/// \see bj_enter_callback_fn_t
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_enter_callback_fn_t bj_set_enter_callback(
    bj_enter_callback_fn_t p_callback,
    void*                  p_user_data
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Handle the ESC key to close a window.
///
/// This utility callback closes the window when \ref BJ_KEY_ESCAPE is pressed.
/// Can be passed directly to \ref bj_set_key_callback.
///
/// \see bj_key_callback_fn_t
/// \see bj_set_key_callback
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_close_on_escape(bj_window*, const bj_key_event*, void* p_user_data);

////////////////////////////////////////////////////////////////////////////////
/// \brief Push a custom event to the internal event queue.
///
/// Typically used by the platform backend to post low-level events
/// into the event processing system.
///
/// \param e Pointer to the event to push.
///
/// \note The event is copied internally and queued for later dispatch.
///
void bj_push_event(const bj_event* e);

////////////////////////////////////////////////////////////////////////////////
/// \brief Push a keyboard event into the event system.
///
/// This function creates and queues a keyboard event reflecting
/// a key press, release, or repeat.
///
/// \param p_window Pointer to the window receiving the event.
/// \param action   The key event action (press, release, repeat).
/// \param key      The key code that triggered the event.
/// \param scancode The platform-specific scancode of the key.
void bj_push_key_event(bj_window* p_window, bj_event_action action, bj_key key, int scancode);

////////////////////////////////////////////////////////////////////////////////
/// \brief Push a cursor movement event.
///
/// Indicates the cursor has moved within the specified window.
///
/// \param p_window Pointer to the window receiving the event.
/// \param x        The new x-coordinate of the cursor.
/// \param y        The new y-coordinate of the cursor.
void bj_push_cursor_event(bj_window* p_window, int x, int y);

////////////////////////////////////////////////////////////////////////////////
/// \brief Push a mouse button event.
///
/// Represents a mouse button press or release within a window.
///
/// \param p_window Pointer to the window receiving the event.
/// \param button   The mouse button identifier.
/// \param action   The action performed (press or release).
/// \param x        The x-coordinate of the cursor at the event.
/// \param y        The y-coordinate of the cursor at the event.
void bj_push_button_event(bj_window* p_window, int button, bj_event_action action, int x, int y);

////////////////////////////////////////////////////////////////////////////////
/// \brief Push an enter or leave window event.
///
/// Indicates whether the cursor entered or left the specified window.
///
/// \param p_window Pointer to the window receiving the event.
/// \param enter    `BJ_TRUE` if cursor entered the window, `BJ_FALSE` if left.
/// \param x        The x-coordinate of the cursor at the event.
/// \param y        The y-coordinate of the cursor at the event.
void bj_push_enter_event(bj_window* p_window, bj_bool enter, int x, int y);

////////////////////////////////////////////////////////////////////////////////
/// \brief Poll and dispatch all pending events.
///
/// This function checks for new input events and invokes the appropriate
/// registered callbacks.
///
/// \see bj_set_cursor_callback
/// \see bj_set_button_callback
/// \see bj_set_key_callback
/// \see bj_set_enter_callback
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_dispatch_events(void);

BANJO_EXPORT bj_bool bj_poll_events(bj_event* p_event);

BANJO_EXPORT void bj_dispatch_event(const bj_event* p_event);


#endif
/// \} // end of event group
