////////////////////////////////////////////////////////////////////////////////
/// \file
/// Header file for \ref bj_window type.

#pragma once

#include <banjo/api.h>
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


