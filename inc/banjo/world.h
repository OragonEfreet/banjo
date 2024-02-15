#pragma once

#include <banjo/api.h>
#include <banjo/memory.h>

typedef usize entity_id;

BJ_DEFINE_HANDLE(BjWorld);

typedef struct BjWorldInfo {
    BjAllocationCallbacks*   pAllocator;
} BjWorldInfo;

BANJO_EXPORT BjResult bjCreateWorld(
    const BjWorldInfo* pInfo,
    BjWorld*                 pInstance
);

BANJO_EXPORT BjResult bjDestroyWorld(
    BjWorld world
);

// Tableau de component id -> Signature? Has to be sorted
// Archetype -> Tableau de signatures+entity ID qui ont cette signature
// Map EntityID -> Archetype?
// Map Signature->Archetype?







