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

struct bj_video_layer;
struct bj_video_layer_data;

/// \brief Define a function used to terminate a video layer.
///
/// This function is called to release all resources associated with the layer.
///
/// \param layer Pointer to the video layer.
/// \param error Output error pointer.
///
/// \see struct bj_video_layer
typedef void (*bj_window_end_fn)(
    struct bj_video_layer* layer,
    struct bj_error**               error
);

/// \brief Define a function used to create a new window.
///
/// \param layer  Pointer to the video layer.
/// \param title    Title of the window.
/// \param x        Initial x position of the window.
/// \param y        Initial y position of the window.
/// \param width    Width of the window in pixels.
/// \param height   Height of the window in pixels.
/// \param flags    Window creation flags.
///
/// \return A pointer to the created window, or NULL on failure.
///
/// \see struct bj_window, struct bj_video_layer
typedef struct bj_window* (*bj_window_create_window_fn)(
    struct bj_video_layer* layer,
    const char*              title,
    uint16_t                 x,
    uint16_t                 y,
    uint16_t                 width,
    uint16_t                 height,
    uint8_t                  flags
);

/// \brief Define a function used to destroy an existing window.
///
/// \param layer  Pointer to the video layer.
/// \param window Window to destroy.
///
/// \see struct bj_window
typedef void (*bj_window_delete_window_fn)(
    struct bj_video_layer* layer,
    struct bj_window*               window
);

/// \brief Define a function used to poll events for all windows.
///
/// \param layer Pointer to the video layer.
///
/// \see bj_dispatch_events
typedef void (*bj_window_poll_events_fn)(
    struct bj_video_layer* layer
);

/// \brief Define a function used to retrieve the size of a window.
///
/// \param layer  Pointer to the video layer.
/// \param window Target window.
/// \param width  Output width in pixels.
/// \param height Output height in pixels.
///
/// \return 1 on success, 0 on failure.
///
/// \see struct bj_window
typedef int (*bj_window_get_size_fn)(
    struct bj_video_layer* layer,
    const struct bj_window*         window,
    int*                     width,
    int*                     height
);

/// \brief Define a function used to create a framebuffer for a window.
///
/// The framebuffer is a pixel buffer that can be written to by the application
/// and later flushed to the screen.
///
/// \param layer  Pointer to the video layer.
/// \param window Target window.
/// \param error  Output error pointer.
///
/// \return A pointer to the new framebuffer, or NULL on error.
///
/// \see struct bj_bitmap, struct bj_window
typedef struct bj_bitmap* (*bj_window_create_framebuffer_fn)(
    struct bj_video_layer* layer,
    const struct bj_window*         window,
    struct bj_error**               error
);

/// \brief Define a function used to present a framebuffer to the window surface.
///
/// This is typically called after rendering to the framebuffer.
///
/// \param layer  Pointer to the video layer.
/// \param window Target window.
///
/// \see struct bj_window, struct bj_bitmap
typedef void (*bj_window_flush_framebuffer_fn)(
    struct bj_video_layer* layer,
    const struct bj_window*         window
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Structure describing a video layer backend.
///
/// This contains all the function pointers that define the backend behavior,
/// as well as backend-specific data.
///
/// \see struct bj_video_layer_create_info
////////////////////////////////////////////////////////////////////////////////
struct bj_video_layer {
    bj_window_end_fn                end;                      ///< Terminate the video layer
    bj_window_create_window_fn      create_window;            ///< Create a window
    bj_window_delete_window_fn      delete_window;            ///< Delete a window
    bj_window_poll_events_fn        poll_events;              ///< Poll window events
    bj_window_get_size_fn           get_window_size;          ///< Retrieve window dimensions
    bj_window_create_framebuffer_fn create_window_framebuffer;///< Create a framebuffer
    bj_window_flush_framebuffer_fn  flush_window_framebuffer; ///< Present framebuffer

    struct bj_video_layer_data*     data;                     ///< Backend-specific data
};

////////////////////////////////////////////////////////////////////////////////
/// \brief Structure used to declare a video backend implementation.
///
/// This is registered during initialization to allow choosing the appropriate
/// video layer at runtime.
///
/// \see struct bj_video_layer
////////////////////////////////////////////////////////////////////////////////
struct bj_video_layer_create_info {
    const char* name;                                ///< Name of the video backend
    struct bj_video_layer* (*create)(struct bj_error** error);   ///< Function to create the backend
};

#endif
/// \} // End of video group
