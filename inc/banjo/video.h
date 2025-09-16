////////////////////////////////////////////////////////////////////////////////
/// \file quat.h
/// Quaternion manipulation API
////////////////////////////////////////////////////////////////////////////////
/// \defgroup video Video
///
/// \brief Manage window creation and pixel-based drawing
///
/// The video API provides an abstraction for creating and managing
/// platform-specific windows and their framebuffers. It supports event polling,
/// framebuffer rendering, and resizing operations.
///
/// Each video backend implements this interface.
///
/// \{
////////////////////////////////////////////////////////////////////////////////
#ifndef BJ_VIDEO_H
#define BJ_VIDEO_H

#include <banjo/error.h>
#include <banjo/window.h>

struct bj_video_layer_t;
struct bj_video_layer_data_t;

/// \brief Define a function used to terminate a video layer.
///
/// This function is called to release all resources associated with the layer.
///
/// \param p_layer Pointer to the video layer.
/// \param p_error Output error pointer.
///
/// \see bj_video_layer
typedef void (*bj_window_end_fn_t)(
    struct bj_video_layer_t* p_layer,
    bj_error**               p_error
);

/// \brief Define a function used to create a new window.
///
/// \param p_layer  Pointer to the video layer.
/// \param title    Title of the window.
/// \param x        Initial x position of the window.
/// \param y        Initial y position of the window.
/// \param width    Width of the window in pixels.
/// \param height   Height of the window in pixels.
/// \param flags    Window creation flags.
///
/// \return A pointer to the created window, or NULL on failure.
///
/// \see bj_window, bj_video_layer
typedef bj_window* (*bj_window_create_window_fn_t)(
    struct bj_video_layer_t* p_layer,
    const char*              title,
    uint16_t                 x,
    uint16_t                 y,
    uint16_t                 width,
    uint16_t                 height,
    uint8_t                  flags
);

/// \brief Define a function used to destroy an existing window.
///
/// \param p_layer  Pointer to the video layer.
/// \param p_window Window to destroy.
///
/// \see bj_window
typedef void (*bj_window_delete_window_fn_t)(
    struct bj_video_layer_t* p_layer,
    bj_window*               p_window
);

/// \brief Define a function used to poll events for all windows.
///
/// \param p_layer Pointer to the video layer.
///
/// \see bj_dispatch_events
typedef void (*bj_window_poll_events_fn_t)(
    struct bj_video_layer_t* p_layer
);

/// \brief Define a function used to retrieve the size of a window.
///
/// \param p_layer  Pointer to the video layer.
/// \param p_window Target window.
/// \param p_width  Output width in pixels.
/// \param p_height Output height in pixels.
///
/// \return 1 on success, 0 on failure.
///
/// \see bj_window
typedef int (*bj_window_get_size_fn_t)(
    struct bj_video_layer_t* p_layer,
    const bj_window*         p_window,
    int*                     p_width,
    int*                     p_height
);

/// \brief Define a function used to create a framebuffer for a window.
///
/// The framebuffer is a pixel buffer that can be written to by the application
/// and later flushed to the screen.
///
/// \param p_layer  Pointer to the video layer.
/// \param p_window Target window.
/// \param p_error  Output error pointer.
///
/// \return A pointer to the new framebuffer, or NULL on error.
///
/// \see bj_bitmap, bj_window
typedef bj_bitmap* (*bj_window_create_framebuffer_fn_t)(
    struct bj_video_layer_t* p_layer,
    const bj_window*         p_window,
    bj_error**               p_error
);

/// \brief Define a function used to present a framebuffer to the window surface.
///
/// This is typically called after rendering to the framebuffer.
///
/// \param p_layer  Pointer to the video layer.
/// \param p_window Target window.
///
/// \see bj_window, bj_bitmap
typedef void (*bj_window_flush_framebuffer_fn_t)(
    struct bj_video_layer_t* p_layer,
    const bj_window*         p_window
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Structure describing a video layer backend.
///
/// This contains all the function pointers that define the backend behavior,
/// as well as backend-specific data.
///
/// \see bj_video_layer_create_info
////////////////////////////////////////////////////////////////////////////////
typedef struct bj_video_layer_t {
    bj_window_end_fn_t                end;                      ///< Terminate the video layer
    bj_window_create_window_fn_t      create_window;            ///< Create a window
    bj_window_delete_window_fn_t      delete_window;            ///< Delete a window
    bj_window_poll_events_fn_t        poll_events;              ///< Poll window events
    bj_window_get_size_fn_t           get_window_size;          ///< Retrieve window dimensions
    bj_window_create_framebuffer_fn_t create_window_framebuffer;///< Create a framebuffer
    bj_window_flush_framebuffer_fn_t  flush_window_framebuffer; ///< Present framebuffer

    struct bj_video_layer_data_t*     data;                     ///< Backend-specific data
} bj_video_layer;

////////////////////////////////////////////////////////////////////////////////
/// \brief Structure used to declare a video backend implementation.
///
/// This is registered during initialization to allow choosing the appropriate
/// video layer at runtime.
///
/// \see bj_video_layer
////////////////////////////////////////////////////////////////////////////////
typedef struct {
    const char* name;                                ///< Name of the video backend
    bj_video_layer* (*create)(bj_error** p_error);   ///< Function to create the backend
} bj_video_layer_create_info;

#endif
/// \} // End of video group
