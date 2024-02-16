#include <banjo/error.h>
#include <data/world.h>

#include <banjo/array.h>

BjWorld bjCreateWorld(
    const BjWorldInfo* pInfo
) {
    bjAssert(pInfo != 0);

    BjWorld world = bjNewStruct(BjWorld, pInfo->pAllocator);
    world->pAllocator = pInfo->pAllocator;



    /* BjArray array = {.count = 10}; */
    /* bjInitArray(&array); */

    return world;
}

void bjInitWorld( const BjWorldInfo* create_info, BjWorld world) {
    // ...
}

void bjResetWorld(BjWorld world) {
    // ...
}

void bjDestroyWorld(
    BjWorld world
) {
    bjAssert(world != 0);
    bjFree(world, world->pAllocator);
}
