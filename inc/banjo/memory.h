////////////////////////////////////////////////////////////////////////////////
/// \file
/// All memory-related functions, including custom allocators.

////////////////////////////////////////////////////////////////////////////////
/// \defgroup memory Memory Management
/// \ingroup core
///
/// \{
#pragma once

#include <banjo/api.h>

////////////////////////////////////////////////////////////////////////////////
/// Memory allocation callback.
/// Used in \ref bj_memory_callbacks to set the function used for custom allocations.
typedef void* (*bj_malloc_fn)(
    void* p_user_data, //< General purpose context data.
    size_t size         //< Allocation size in bytes requested by the caller.
);

////////////////////////////////////////////////////////////////////////////////
/// Memory allocation callback.
/// Used in \ref bj_memory_callbacks to set the function used for custom reallocations.
typedef void* (*bj_realloc_fn)(
    void* p_user_data, //< General purpose context data.
    void* p_original,  //< Initial object to reallocate.
    size_t size         //< Allocation size in bytes requested by the caller.
);

////////////////////////////////////////////////////////////////////////////////
/// Memory allocation callback.
///
/// Used in \ref bj_memory_callbacks to set the function used for custom deallocations.
typedef void (*bj_free_fn)(
    void* p_user_data, //< General purpose context data.
    void* p_memory     //< Object memory to dispose.
);

////////////////////////////////////////////////////////////////////////////////
/// Custom allocation callbacks.
/// This structure is used to set the function used by the API upon managing memory.
/// Each object created by the API can be set a specific set of callbacks.
/// Also, the custom allocators can be set globally with \ref bj_memory_set_defaults.
typedef struct bj_memory_callbacks {
    void*                      p_user_data;     ///< General purpose context data.
    bj_malloc_fn    fn_allocation;   ///< The allocation function.
    bj_realloc_fn  fn_reallocation; ///< The reallocation function.
    bj_free_fn          fn_free;         ///< The deallocation function.
} bj_memory_callbacks;

////////////////////////////////////////////////////////////////////////////////
/// Allocate `size` bytes of memory and returns a pointer to it.
/// 
/// \param[in] size        The number of bytes to allocate.
///
/// \return The memory address of the newly allocated block.
BANJO_EXPORT void* bj_malloc(
    size_t size
);

////////////////////////////////////////////////////////////////////////////////
/// Reallocate the given area of memory.
/// 
/// \param[in] p_memory    Pointer to the memory area to be reallocated.
/// \param[in] size        The number of bytes to allocate.
///
/// `p_memory` must be previously allocated by \ref bj_malloc or \ref bj_realloc.
///
/// \return The memory address of the newly allocated block.
BANJO_EXPORT void* bj_realloc(
    void*                        p_memory,
    size_t                        size
);

////////////////////////////////////////////////////////////////////////////////
/// Deallocate the given area of memory.
/// 
/// \param[in] p_memory    Pointer to the memory area to be reallocated.
///
/// `p_memory` must be previously allocated by \ref bj_malloc or \ref bj_realloc.
///
/// The latter can have been overriden by \ref bj_memory_set_defaults.
BANJO_EXPORT void bj_free(
    void*                         p_memory
);

////////////////////////////////////////////////////////////////////////////////
/// Set the default allocators.
///
/// When a function requires memory changes,
/// the caller can send callback memory functions.
/// If the given allocator is _0_, Banjo uses global defaults
/// (`malloc`/`realloc`/`free`).
///
/// This function can override the default allocator globally.
/// After calling this function, default allocators (_0_) will be using the
/// callbacks set in parameter.
///
/// If _0_ is passed to this function, Banjo sets back the systems allocators.
///
/// \param[in] p_allocator The new default allocators.
///
/// \see bj_memory_unset_defaults
BANJO_EXPORT void bj_memory_set_defaults(
    const bj_memory_callbacks* p_allocator
);

////////////////////////////////////////////////////////////////////////////////
/// Reset the default allocators.
/// Sets back the global allocators to standard defaults (`malloc`/`realloc`/`free`).
///
/// \see bj_memory_set_defaults
BANJO_EXPORT void bj_memory_unset_defaults(void);

////////////////////////////////////////////////////////////////////////////////
/// Copies `mem_size` bytes from the object pointed to by `p_src` to to object
/// pointed to by `p_dest`.
///
/// \param[in] p_dest   Pointer to the object to copy to.
/// \param[in] p_src    Pointer to the object to copy from.
/// \param[in] mem_size Number of bytes to copy.
///
/// \return `p_dest` pointer.
///
/// The function effectively calls the standard `memcpy` function.
BANJO_EXPORT void* bj_memcpy(
    void*       p_dest,
    const void* p_src,
    size_t       mem_size
);

////////////////////////////////////////////////////////////////////////////////
/// Copies the value `value` into each ot the first `num` bytes of the object
/// pointed to by `p_dest`.
///
/// \param[in] p_dest      Pointer to the object to fill.
/// \param[in] value       fill byte.
/// \param[in] mem_size    number of bytes to fill.
BANJO_EXPORT void bj_memset(
    void* p_dest,
    uint8_t    value,
    size_t mem_size
);

/// \} // End of array memory
