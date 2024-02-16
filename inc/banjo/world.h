#pragma once

#include <banjo/api.h>
#include <banjo/memory.h>

typedef usize entity_id;

BJ_DEFINE_HANDLE(BjWorld);

typedef struct BjWorldInfo {
    int rfu;
} BjWorldInfo;

BANJO_EXPORT BjWorld bj_world_create(
    const BjWorldInfo*           p_info,
    const BjAllocationCallbacks* p_allocator
);

BANJO_EXPORT void bj_world_destroy(
    BjWorld world
);

// Tableau de component id -> Signature? Has to be sorted
// Archetype -> Tableau de signatures+entity ID qui ont cette signature
// Map EntityID -> Archetype?
// Map Signature->Archetype?







