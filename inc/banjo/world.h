/// \brief Main structure for Entity-Component-System object.
#pragma once

#include <banjo/api.h>
#include <banjo/memory.h>

/// Creation info for a new instance of BjWorld.
/// \see BjWorld
typedef struct BjWorldInfo {
    int rfu; //!< Unused
} BjWorldInfo;

////////////////////////////////////////////////////////////////////////////////
/// Typedef for the BjWorld_t struct
typedef struct BjWorld_t BjWorld;

#ifdef BJ_NO_OPAQUE
struct BjWorld_t {
    const bj_memory_callbacks* p_allocator;
    BjWorldInfo                  info;
};
#endif

/// Create a new instance of \ref BjWorld.
///
/// The caller owns the returned object.
/// To free it properly, use \ref bj_world_del.
///
/// \return A new \ref BjWorld.
BANJO_EXPORT BjWorld* bj_world_new(
    const BjWorldInfo*           p_info,        ///< The creation configuration.
    const bj_memory_callbacks* p_allocator    ///< Custom allocator. Can be _0_.
);

/// Destroy an instance of \ref BjWorld.
/// \param world The world object
BANJO_EXPORT void bj_world_del(
    BjWorld* world
);

/// Initializes a new instance of \ref BjWorld.
///
/// The caller owns the returned object.
/// To free it properly, use \ref bj_world_del.
///
/// \param p_info  The info object
/// \param p_world The world object
///
BANJO_EXPORT void bjInitWorld(
    const BjWorldInfo* p_info,
    BjWorld* p_world
);

/// Reset a given world to invalid state
/// \param p_world The world object
BANJO_EXPORT void bjResetWorld(BjWorld* p_world);







