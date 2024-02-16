#pragma once

#include <banjo/world.h>

typedef struct BjWorld_T {
    BjAllocationCallbacks* pAllocator;
} BjWorld_T;

void bjInitWorld( const BjWorldInfo*, BjWorld);
void bjResetWorld(BjWorld);

