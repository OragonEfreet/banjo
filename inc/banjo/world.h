/// \file
/// \brief Main structure for Entity-Component-System object.
#pragma once

#include <banjo/api.h>
#include <banjo/memory.h>

typedef struct BjWorld_T BjWorld;

#ifdef BJ_NO_OPAQUE
struct BjWorld_T {
    const BjAllocationCallbacks* p_allocator;
};
#endif

/// Creation info for a new instance of BjWorld.
/// \see BjWorld
typedef struct BjWorldInfo {
    int rfu; //!< Unused
} BjWorldInfo;

/// Create a new instance of \ref BjWorld.
///
/// The caller owns the returned object.
/// To free it properly, use \ref bj_world_destroy.
///
/// \return A new \ref BjWorld.
BANJO_EXPORT BjWorld* bj_world_create(
    const BjWorldInfo*           p_info,        ///< The creation configuration.
    const BjAllocationCallbacks* p_allocator    ///< Custom allocator. Can be _0_.
);

/// Destroy an instance of \ref BjWorld.
BANJO_EXPORT void bj_world_destroy(
    BjWorld* world
);

void bjInitWorld(const BjWorldInfo*, BjWorld*);
void bjResetWorld(BjWorld*);






