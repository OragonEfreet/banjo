////////////////////////////////////////////////////////////////////////////////
/// \file
/// Encode and manipulate color information

////////////////////////////////////////////////////////////////////////////////
/// \defgroup color Color
/// \ingroup graphics
/// Color manipulation API
///
/// \{
#pragma once

#include <stdint.h>

/// 32-bits integer representing a RGBA Color.
///
/// Colors are encoded into a 32 bits 4-channel integer where each successive byte
/// encode a color component: Red, Green, Blue and Alpha, in that order (from most
/// significant to least significant bit).
/// Each channel is a value in the range 0x00 to 0xFF.
typedef uint32_t BjColor;

/// Contructs a \ref BjColor value by combining the given `r`, `g`, `b` and `a` components.
#define bj_color_rgba(r, g, b, a) (BjColor)((((uint8_t)r & 0xFF) << 24) | (((uint8_t)g & 0xFF) << 16) | (((uint8_t)b & 0xFF) << 8) | (uint8_t)a)

/// Contructs a \ref BjColor value by combining the given `r`, `g` and `b` component. The alpha component is defaulted to _0xFF_.
#define bj_color_rgb(r, g, b) bj_color_rgba(r, g, b, 0xFF)

/// Get the red component of the given BjColor value.
#define bj_red(rgba) (uint8_t)((rgba >> 24) & 0xFF)

/// Get the green component of the given BjColor value.
#define bj_green(rgba) (uint8_t)((rgba >> 16) & 0xFF)

/// Get the blue component of the given BjColor value.
#define bj_blue(rgba) (uint8_t)((rgba >> 8) & 0xFF)

/// Get the alpha component of the given BjColor value.
#define bj_alpha(rgba) (uint8_t)(rgba & 0xFF)

#define BJ_COLOR_BLACK               RGB(0x00, 0x00, 0x00) //!< Black color.
#define BJ_COLOR_WHITE               RGB(0xFF, 0xFF, 0xFF) //!< White color.
#define BJ_COLOR_RED                 RGB(0xFF, 0x00, 0x00) //!< Red color.
#define BJ_COLOR_GREEN               RGB(0x00, 0xFF, 0x00) //!< Green color.
#define BJ_COLOR_BLUE                RGB(0x00, 0x00, 0xFF) //!< Blue color.
#define BJ_COLOR_YELLOW              RGB(0xFF, 0xFF, 0x00) //!< Yellow color.
#define BJ_COLOR_CYAN                RGB(0x00, 0xFF, 0xFF) //!< Cyan color.
#define BJ_COLOR_MAGENTA             RGB(0xFF, 0x00, 0xFF) //!< Magenta color.
#define BJ_COLOR_GRAY                RGB(0x80, 0x80, 0x80) //!< Gray color.
#define BJ_COLOR_LIGHT_GRAY          RGB(0xC0, 0xC0, 0xC0) //!< Light gray color.
#define BJ_COLOR_DARK_GRAY           RGB(0x40, 0x40, 0x40) //!< Dark gray color.
#define BJ_COLOR_MAROON              RGB(0x80, 0x00, 0x00) //!< Maroon color.
#define BJ_COLOR_DARK_GREEN          RGB(0x00, 0x80, 0x00) //!< Dark green color.
#define BJ_COLOR_NAVY                RGB(0x00, 0x00, 0x80) //!< Navy color.
#define BJ_COLOR_OLIVE               RGB(0x80, 0x80, 0x00) //!< Olive color.
#define BJ_COLOR_TEAL                RGB(0x00, 0x80, 0x80) //!< Teal color.
#define BJ_COLOR_PURPLE              RGB(0x80, 0x00, 0x80) //!< Purple color.
#define BJ_COLOR_SILVER              RGB(0xC0, 0xC0, 0xC0) //!< Silver color.
#define BJ_COLOR_LIGHT_RED           RGB(0xFF, 0x66, 0x66) //!< Light red color.
#define BJ_COLOR_LIGHT_GREEN         RGB(0x66, 0xFF, 0x66) //!< Light green color.
#define BJ_COLOR_LIGHT_BLUE          RGB(0x66, 0x66, 0xFF) //!< Light blue color.
#define BJ_COLOR_LIGHT_YELLOW        RGB(0xFF, 0xFF, 0x66) //!< Light yellow color.
#define BJ_COLOR_LIGHT_CYAN          RGB(0x66, 0xFF, 0xFF) //!< Light cyan color.
#define BJ_COLOR_LIGHT_MAGENTA       RGB(0xFF, 0x66, 0xFF) //!< Light magenta color.
#define BJ_COLOR_DARK_RED            RGB(0x80, 0x00, 0x00) //!< Dark red color.
#define BJ_COLOR_DARK_GREEN          RGB(0x00, 0x80, 0x00) //!< Dark green color.
#define BJ_COLOR_DARK_BLUE           RGB(0x00, 0x00, 0x80) //!< Dark blue color.
#define BJ_COLOR_DARK_YELLOW         RGB(0x80, 0x80, 0x00) //!< Dark yellow color.
#define BJ_COLOR_DARK_CYAN           RGB(0x00, 0x80, 0x80) //!< Dark cyan color.
#define BJ_COLOR_DARK_MAGENTA        RGB(0x80, 0x00, 0x80) //!< Dark magenta color.
#define BJ_COLOR_LIGHT_ORANGE        RGB(0xFF, 0xCC, 0x99) //!< Light orange color.
#define BJ_COLOR_DARK_ORANGE         RGB(0xCC, 0x66, 0x00) //!< Dark orange color.
#define BJ_COLOR_GOLD                RGB(0xFF, 0xD7, 0x00) //!< Gold color.
#define BJ_COLOR_SALMON              RGB(0xFA, 0x80, 0x72) //!< Salmon color.
#define BJ_COLOR_SKY_BLUE            RGB(0x87, 0xCE, 0xEB) //!< Sky blue color.
#define BJ_COLOR_LIME                RGB(0x00, 0xFF, 0x00) //!< Lime color.
#define BJ_COLOR_INDIGO              RGB(0x4B, 0x00, 0x82) //!< Indigo color.
#define BJ_COLOR_BEIGE               RGB(0xF5, 0xF5, 0xDC) //!< Beige color.
#define BJ_COLOR_TURQUOISE           RGB(0x40, 0xE0, 0xD0) //!< Turquoise color.
#define BJ_COLOR_AQUAMARINE          RGB(0x7F, 0xFF, 0xD4) //!< Aquamarine color.
#define BJ_COLOR_CORAL               RGB(0xFF, 0x7F, 0x50) //!< Coral color.
#define BJ_COLOR_CRIMSON             RGB(0xDC, 0x14, 0x3C) //!< Crimson color.
#define BJ_COLOR_DARK_SLATE_GRAY     RGB(0x2F, 0x4F, 0x4F) //!< Dark slate gray color.
#define BJ_COLOR_DIM_GRAY            RGB(0x69, 0x69, 0x69) //!< Dim gray color.
#define BJ_COLOR_FIREBRICK           RGB(0xB2, 0x22, 0x22) //!< Firebrick color.
#define BJ_COLOR_FOREST_GREEN        RGB(0x22, 0x8B, 0x22) //!< Forest green color.
#define BJ_COLOR_INDIAN_RED          RGB(0xCD, 0x5C, 0x5C) //!< Indian red color.
#define BJ_COLOR_KHAKI               RGB(0xF0, 0xE6, 0x8C) //!< Khaki color.
#define BJ_COLOR_MEDIUM_AQUAMARINE   RGB(0x66, 0xCD, 0xAA) //!< Medium aquamarine color.
#define BJ_COLOR_MEDIUM_ORCHID       RGB(0xBA, 0x55, 0xD3) //!< Medium orchid color.
#define BJ_COLOR_MEDIUM_PURPLE       RGB(0x93, 0x71, 0xDB) //!< Medium purple color.
#define BJ_COLOR_MEDIUM_SEA_GREEN    RGB(0x3C, 0xB3, 0x71) //!< Medium sea green color.
#define BJ_COLOR_MEDIUM_SLATE_BLUE   RGB(0x7B, 0x68, 0xEE) //!< Medium slate blue color.
#define BJ_COLOR_MEDIUM_SPRING_GREEN RGB(0x00, 0xFA, 0x9A) //!< Medium spring green color.
#define BJ_COLOR_MEDIUM_TURQUOISE    RGB(0x48, 0xD1, 0xCC) //!< Medium turquoise color.
#define BJ_COLOR_MEDIUM_VIOLET_RED   RGB(0xC7, 0x15, 0x85) //!< Medium violet red color.
#define BJ_COLOR_MIDNIGHT_BLUE       RGB(0x19, 0x19, 0x70) //!< Midnight blue color.
#define BJ_COLOR_OLIVE_DRAB          RGB(0x6B, 0x8E, 0x23) //!< Olive drab color.
#define BJ_COLOR_ORANGE              RGB(0xFF, 0xA5, 0x00) //!< Orange color.
#define BJ_COLOR_ORCHID              RGB(0xDA, 0x70, 0xD6) //!< Orchid color.
#define BJ_COLOR_PALE_GREEN          RGB(0x98, 0xFB, 0x98) //!< Pale green color.
#define BJ_COLOR_PALE_TURQUOISE      RGB(0xAF, 0xEE, 0xEE) //!< Pale turquoise color.
#define BJ_COLOR_PALE_VIOLET_RED     RGB(0xDB, 0x70, 0x93) //!< Pale violet red color.
#define BJ_COLOR_PERU                RGB(0xCD, 0x85, 0x3F) //!< Peru color.
#define BJ_COLOR_ROYAL_BLUE          RGB(0x41, 0x69, 0xE1) //!< Royal blue color.
#define BJ_COLOR_SADDLE_BROWN        RGB(0x8B, 0x45, 0x13) //!< Saddle brown color.
#define BJ_COLOR_SEA_GREEN           RGB(0x2E, 0x8B, 0x57) //!< Sea green color.
#define BJ_COLOR_SIENNA              RGB(0xA0, 0x52, 0x2D) //!< Sienna color.
#define BJ_COLOR_SLATE_BLUE          RGB(0x6A, 0x5A, 0xCD) //!< Slate blue color.
#define BJ_COLOR_SLATE_GRAY          RGB(0x70, 0x80, 0x90) //!< Slate gray color.
#define BJ_COLOR_SPRING_GREEN        RGB(0x00, 0xFF, 0x7F) //!< Spring green color.
#define BJ_COLOR_STEEL_BLUE          RGB(0x46, 0x82, 0xB4) //!< Steel blue color.
#define BJ_COLOR_TAN                 RGB(0xD2, 0xB4, 0x8C) //!< Tan color.
#define BJ_COLOR_THISTLE             RGB(0xD8, 0xBF, 0xD8) //!< Thistle color.
#define BJ_COLOR_TOMATO              RGB(0xFF, 0x63, 0x47) //!< Tomato color.
#define BJ_COLOR_VIOLET              RGB(0xEE, 0x82, 0xEE) //!< Violet color.
#define BJ_COLOR_WHEAT               RGB(0xF5, 0xDE, 0xB3) //!< Wheat color.
#define BJ_COLOR_YELLOW_GREEN        RGB(0x9A, 0xCD, 0x32) //!< Yellow green color.

/// \} End of color group
