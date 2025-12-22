////////////////////////////////////////////////////////////////////////////////
/// \file renderer.h
/// \brief Rendering backend interface.
////////////////////////////////////////////////////////////////////////////////
/// \defgroup renderer Renderer
///
/// \brief Software rendering backend API.
///
/// This module provides a rendering abstraction that manages framebuffers
/// and presentation to windows. Currently supports software rendering with
/// extensibility for additional backends.
///
/// Typical usage:
/// - Create a renderer with \ref bj_create_renderer.
/// - Configure it for a window using \ref bj_renderer_configure.
/// - Access the framebuffer with \ref bj_get_framebuffer.
/// - Draw to the framebuffer using bitmap operations.
/// - Present the result with \ref bj_present.
/// - Clean up with \ref bj_destroy_renderer.
///
/// \{
////////////////////////////////////////////////////////////////////////////////
#ifndef BJ_RENDERER_H
#define BJ_RENDERER_H

#include <banjo/api.h>

////////////////////////////////////////////////////////////////////////////////
/// \brief Renderer backend type.
///
/// Specifies the type of rendering backend to use.
////////////////////////////////////////////////////////////////////////////////
enum bj_renderer_type {
    BJ_RENDERER_TYPE_SOFTWARE, ///< Software (CPU-based) renderer.
};

/// Opaque renderer handle.
struct bj_bitmap;
/// Opaque renderer handle.
struct bj_renderer;
/// Opaque window handle.
struct bj_window;

////////////////////////////////////////////////////////////////////////////////
/// \brief Create a new renderer instance.
///
/// Allocates and initializes a renderer of the specified type.
///
/// \param type The type of renderer to create.
///
/// \return Pointer to the newly created renderer, or NULL on failure.
///
/// \par Memory Management
///
/// The caller is responsible for destroying the renderer with
/// \ref bj_destroy_renderer.
///
/// \see bj_destroy_renderer
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT struct bj_renderer* bj_create_renderer(
    enum bj_renderer_type type
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Destroy a renderer and free associated resources.
///
/// Releases all resources associated with the renderer.
///
/// \param renderer Pointer to the renderer to destroy.
///
/// \see bj_create_renderer
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_destroy_renderer(
    struct bj_renderer* renderer
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Configure a renderer for a specific window.
///
/// Associates the renderer with a window and sets up the framebuffer
/// to match the window dimensions and properties.
///
/// \param renderer Pointer to the renderer to configure.
/// \param window   Pointer to the target window.
///
/// \par Behavior
///
/// This function should be called before rendering operations to ensure
/// the framebuffer matches the window's current size and format.
///
/// \see bj_get_framebuffer
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_renderer_configure(
    struct bj_renderer* renderer,
    struct bj_window* window
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Get the renderer's framebuffer.
///
/// Returns a pointer to the framebuffer bitmap that can be drawn to.
/// All drawing operations should target this bitmap.
///
/// \param renderer Pointer to the renderer.
///
/// \return Pointer to the framebuffer bitmap, or NULL if not configured.
///
/// \par Memory Management
///
/// The returned bitmap is owned by the renderer. Do not destroy it manually.
///
/// \see bj_renderer_configure, bj_present
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT struct bj_bitmap* bj_get_framebuffer(
    struct bj_renderer* renderer
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Present the framebuffer to a window.
///
/// Copies the contents of the renderer's framebuffer to the specified
/// window, making the rendered content visible.
///
/// \param renderer Pointer to the renderer.
/// \param window   Pointer to the target window.
///
/// \par Behavior
///
/// This function should be called after all drawing operations are complete
/// to display the final result on the window.
///
/// \see bj_get_framebuffer
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_present(
    struct bj_renderer* renderer,
    struct bj_window*   window
);


#endif
/// \} // End of renderer group
