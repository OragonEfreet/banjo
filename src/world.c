#include <banjo/array.h>
#include <banjo/error.h>
#include <banjo/world.h>

BjWorld* bj_world_create(
    const BjWorldInfo*           p_info,
    const BjAllocationCallbacks* p_allocator
) {
    bj_assert(p_info != 0);

    BjWorld* world = bj_malloc(sizeof(struct BjWorld_T), p_allocator);
    world->p_allocator = p_allocator;
    /* BjArray array = {.count = 10}; */
    /* p_array_init(&array); */

    return world;
}

void bjInitWorld( const BjWorldInfo* create_info, BjWorld* world) {
    // ...
}

void bjResetWorld(BjWorld* world) {
    // ...
}

void bj_world_destroy(
    BjWorld* world
) {
    bj_assert(world != 0);
    bj_free(world, world->p_allocator);
}
// Tableau de component id -> Signature? Has to be sorted
// Archetype -> Tableau de signatures+entity ID qui ont cette signature
// Map EntityID -> Archetype?
// Map Signature->Archetype?
