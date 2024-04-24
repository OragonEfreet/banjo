#include <banjo/error.h>
#include <banjo/framebuffer.h>
#include <banjo/memory.h>

void bj_framebuffer_init(
    BjFramebuffer*                p_renderer,
    const BjFramebufferInfo*      p_info,
    const BjAllocationCallbacks*  p_allocator
) {
    bj_memset(p_renderer, 0, sizeof(BjArray));
    if(p_info != 0) {
        p_renderer->p_allocator = p_allocator;
    }

}

void bj_framebuffer_reset(
    BjFramebuffer* p_renderer
) {
    bj_array_reset(&p_renderer->array);
    p_renderer->width       = 0;
    p_renderer->height      = 0;
    p_renderer->p_allocator = 0;
}


