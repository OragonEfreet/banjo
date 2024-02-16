#pragma once

#include <banjo/api.h>
#include <banjo/memory.h>

typedef usize entity_id;

BJ_DEFINE_HANDLE(BjWorld);

typedef struct BjWorldInfo {
    BjAllocationCallbacks*   pAllocator;
} BjWorldInfo;

BANJO_EXPORT BjWorld bjCreateWorld(
    const BjWorldInfo* pInfo
);

BANJO_EXPORT void bjDestroyWorld(
    BjWorld world
);

// Tableau de component id -> Signature? Has to be sorted
// Archetype -> Tableau de signatures+entity ID qui ont cette signature
// Map EntityID -> Archetype?
// Map Signature->Archetype?







