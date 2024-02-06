#pragma once

#include <banjo/core.h>
#include <banjo/memory.h>

typedef usize entity_id;

BJ_DEFINE_HANDLE(BjWorld);

typedef struct {
    BjAllocationCallbacks*   pAllocator;
} BjWorldCreateInfo;

BANJO_EXPORT BjResult bjCreateWorld(
    const BjWorldCreateInfo* pCreateInfo,
    BjWorld*                 pInstance
);

BANJO_EXPORT void bjDestroyWorld(
    BjWorld world
);

// Tableau de component id -> Signature? Has to be sorted
// Archetype -> Tableau de signatures+entity ID qui ont cette signature
// Map EntityID -> Archetype?
// Map Signature->Archetype?







