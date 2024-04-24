#pragma once

#include <banjo/api.h>
#include <banjo/memory.h>

typedef struct BjFramebuffer_T BjFramebuffer;

#ifdef BJ_NO_OPAQUE
#include <banjo/array.h>
struct BjFramebuffer_T {
    const BjAllocationCallbacks* p_allocator;
    struct BjArray_T array;
    usize width;
    usize height;
};
#endif

typedef struct BjFramebufferInfo {
    usize width;
    usize height;
} BjFramebufferInfo;

BANJO_EXPORT BjFramebuffer* bj_renderer_alloc(
    const BjAllocationCallbacks* p_allocator
);

BANJO_EXPORT void bj_renderer_init(
    BjFramebuffer*           p_renderer,
    const BjFramebufferInfo* p_info,
    const BjAllocationCallbacks*  p_allocator
);

BANJO_EXPORT void bj_renderer_reset(
    BjFramebuffer* p_list
);

BANJO_EXPORT void bj_renderer_del(
    BjFramebuffer* list
);


