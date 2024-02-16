#pragma once

#include <banjo/world.h>

typedef struct BjWorld_T {
    const BjAllocationCallbacks* p_allocator;
} BjWorld_T;

void bjInitWorld( const BjWorldInfo*, BjWorld);
void bjResetWorld(BjWorld);

