////////////////////////////////////////////////////////////////////////////////
/// \file
/// Describe a graphics rendering pipeline using Banjo software renderer

#pragma once

#include <banjo/api.h>
#include <banjo/memory.h>

////////////////////////////////////////////////////////////////////////////////
/// Typedef for the BList_T struct
typedef struct BjGraphicsPipeline_T BjGraphicsPipeline;

#ifdef BJ_NO_OPAQUE
struct BjGraphicsPipeline_T {
    const BjAllocationCallbacks* p_allocator;
};
#endif

typedef struct BjGraphicsPipelineInfo {
    int dummy;
} BjGraphicsPipelineInfo;

BANJO_EXPORT BjGraphicsPipeline* bj_graphics_pipeline_alloc(
    const BjAllocationCallbacks* p_allocator
);

BANJO_EXPORT void bj_graphics_pipeline_init(
    BjGraphicsPipeline*           p_pipeline,
    const BjGraphicsPipelineInfo* p_info
);

BANJO_EXPORT void bj_graphics_pipeline_reset(
    BjGraphicsPipeline* p_list
);

BANJO_EXPORT void bj_graphics_pipeline_del(
    BjGraphicsPipeline* list
);

