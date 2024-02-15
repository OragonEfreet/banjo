#pragma once

#include <banjo/world.h>

typedef struct BjWorld_T {
    BjAllocationCallbacks* pAllocator;
} BjWorld_T;

BjResult bjInitWorld( const BjWorldInfo*, BjWorld);
BjResult bjResetWorld(BjWorld);

