#include <banjo/error.h>
#include <banjo/framebuffer.h>
#include <banjo/memory.h>

BANJO_EXPORT bj_framebuffer* bj_famebuffer_init_default(
    bj_framebuffer*   p_framebuffer,
    usize             width,
    usize             height
) {
    bj_memset(p_framebuffer, 0, sizeof(bj_framebuffer));
    p_framebuffer->width = width;
    p_framebuffer->height = height;
    return p_framebuffer;
}

BANJO_EXPORT bj_framebuffer* bj_renderer_reset(
    bj_framebuffer* p_framebuffer
) {
    bj_array_reset(&p_framebuffer->array);
    p_framebuffer->width       = 0;
    p_framebuffer->height      = 0;
    return p_framebuffer;
}

