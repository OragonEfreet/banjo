#include <errors.h>
#include <data/world.h>

#include <banjo/array.h>

BjResult bjCreateWorld(
    const BjWorldInfo* pInfo,
    BjWorld*                 pWorld
) {
    bjExpectValue(pInfo, BJ_NULL_CREATE_INFO);
    bjExpectValue(pWorld, BJ_NULL_OUTPUT_HANDLE);

    BjWorld world = bjNewStruct(BjWorld, pInfo->pAllocator);
    world->pAllocator = pInfo->pAllocator;

    *pWorld = world;


    /* BjArray array = {.count = 10}; */
    /* bjInitArray(&array); */

    return 0;
}

BjResult bjInitWorld( const BjWorldInfo* create_info, BjWorld world) {
    return BJ_SUCCESS;
}

BjResult bjResetWorld(BjWorld world) {
    return BJ_SUCCESS;
}

BjResult bjDestroyWorld(
    BjWorld world
) {
    bjExpectValue(world, BJ_NULL_INPUT_HANDLE);
    bjFree(world, world->pAllocator);
    return BJ_SUCCESS;
}
