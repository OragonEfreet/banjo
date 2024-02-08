#include <errors.h>
#include <data/world.h>

#include <banjo/array.h>

BjResult bjCreateWorld(
    const BjWorldCreateInfo* pCreateInfo,
    BjWorld*                 pWorld
) {
    bjExpectValue(pCreateInfo, BJ_NULL_CREATE_INFO);
    bjExpectValue(pWorld, BJ_NULL_OUTPUT_HANDLE);

    BjWorld world = bjNewStruct(BjWorld, pCreateInfo->pAllocator);
    world->pAllocator = pCreateInfo->pAllocator;

    *pWorld = world;


    /* BjArray array = {.count = 10}; */
    /* bjInitArray(&array); */

    return 0;
}

BjResult bjInitWorld( const BjWorldCreateInfo* create_info, BjWorld world) {
    return BJ_SUCCESS;
}

BjResult bjResetWorld(BjWorld world) {
    return BJ_SUCCESS;
}

void bjDestroyWorld(
    BjWorld world
) {
    bjExpectValue(world, BJ_NULL_INPUT_HANDLE);
    bjFree(world, world->pAllocator);
}
