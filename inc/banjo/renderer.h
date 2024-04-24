#pragma once

#include <banjo/api.h>
#include <banjo/memory.h>

typedef struct BjRenderer_T BjRenderer;

#ifdef BJ_NO_OPAQUE
struct BjRenderer_T {
    const BjAllocationCallbacks* p_allocator;
};
#endif

typedef struct BjRendererInfo {
    int dummy;
} BjRendererInfo;

BANJO_EXPORT BjRenderer* bj_renderer_alloc(
    const BjAllocationCallbacks* p_allocator
);

BANJO_EXPORT void bj_renderer_init(
    BjRenderer*           p_renderer,
    const BjRendererInfo* p_info,
    const BjAllocationCallbacks*  p_allocator
);

BANJO_EXPORT void bj_renderer_reset(
    BjRenderer* p_list
);

BANJO_EXPORT void bj_renderer_del(
    BjRenderer* list
);


