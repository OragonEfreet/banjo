#include <core/errors.h>
#include <core/memory.h>
#include <ecs/world.h>

#include <banjo/array.h>

BjResult bjCreateWorld(
    const BjWorldCreateInfo* pCreateInfo,
    BjWorld*                 pWorld
) {
    bjExpectValue(pCreateInfo, BJ_NULL_CREATE_INFO);
    bjExpectValue(pWorld, BJ_NULL_OUTPUT_HANDLE);

    BjWorld world = bjNew(BjWorld, pCreateInfo->pAllocator);
    world->pAllocator = pCreateInfo->pAllocator;

    *pWorld = world;


    BjArray array = {.count = 10};
    bjInitArray(&array);






    return 0;
}

void bjDestroyWorld(
    BjWorld world
) {
    bjExpectValue(world, BJ_NULL_INPUT_HANDLE);
    bjFree(world, world->pAllocator);
}
