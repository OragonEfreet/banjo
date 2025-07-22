////////////////////////////////////////////////////////////////////////////////
/// \file
/// All memory-related functions, including custom allocators.
////////////////////////////////////////////////////////////////////////////////
/// \defgroup memory Memory Management
/// \ingroup core
///
/// \{
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <banjo/api.h>

////////////////////////////////////////////////////////////////////////////////
/// \typedef bj_malloc_fn
/// \brief Memory allocation callback.
///
/// Used in \ref bj_memory_callbacks to set the function used for custom allocations.
///
/// \param p_user_data General purpose context data.
/// \param size Allocation size in bytes requested by the caller.
///
/// \return Pointer to allocated memory block.
////////////////////////////////////////////////////////////////////////////////
typedef void* (*bj_malloc_fn)(
    void* p_user_data,
    size_t size
);

////////////////////////////////////////////////////////////////////////////////
/// \typedef bj_realloc_fn
/// \brief Memory reallocation callback.
///
/// Used in \ref bj_memory_callbacks to set the function used for custom reallocations.
///
/// \param p_user_data General purpose context data.
/// \param p_original Initial object to reallocate.
/// \param size Allocation size in bytes requested by the caller.
///
/// \return Pointer to reallocated memory block.
////////////////////////////////////////////////////////////////////////////////
typedef void* (*bj_realloc_fn)(
    void* p_user_data,
    void* p_original,
    size_t size
);

////////////////////////////////////////////////////////////////////////////////
/// \typedef bj_free_fn
/// \brief Memory free callback.
///
/// Used in \ref bj_memory_callbacks to set the function used for custom deallocations.
///
/// \param p_user_data General purpose context data.
/// \param p_memory Object memory to dispose.
////////////////////////////////////////////////////////////////////////////////
typedef void (*bj_free_fn)(
    void* p_user_data,
    void* p_memory
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Custom allocation callbacks.
///
/// This structure holds function pointers for allocation,
/// reallocation, and deallocation callbacks along with user data.
///
/// These callbacks can be assigned per-object or set globally
/// with \ref bj_memory_set_defaults.
////////////////////////////////////////////////////////////////////////////////
typedef struct bj_memory_callbacks {
    void*         p_user_data;       ///< General purpose context data.
    bj_malloc_fn  fn_allocation;     ///< Allocation function pointer.
    bj_realloc_fn fn_reallocation;   ///< Reallocation function pointer.
    bj_free_fn    fn_free;           ///< Deallocation function pointer.
} bj_memory_callbacks;

////////////////////////////////////////////////////////////////////////////////
/// \brief Allocate `size` bytes of memory.
///
/// \param[in] size Number of bytes to allocate.
///
/// \return Pointer to newly allocated memory block.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void* bj_malloc(
    size_t size
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Allocate `size` bytes of zero-initialized memory.
///
/// The returned memory is set to zero bytes.
///
/// \param[in] size Number of bytes to allocate.
///
/// \return Pointer to newly allocated zeroed memory block.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void* bj_calloc(
    size_t size
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Reallocate a memory block to a new size.
///
/// \param[in] p_memory Pointer to previously allocated memory.
/// \param[in] size Number of bytes to allocate.
///
/// \return Pointer to newly reallocated memory block.
///
/// \note `p_memory` must have been allocated by \ref bj_malloc or \ref bj_realloc.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void* bj_realloc(
    void* p_memory,
    size_t size
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Free a previously allocated memory block.
///
/// \param[in] p_memory Pointer to memory to free.
///
/// \note `p_memory` must have been allocated by \ref bj_malloc or \ref bj_realloc.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_free(
    void* p_memory
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Set the global default memory allocators.
///
/// If `p_allocator` is `NULL`, resets to system defaults (`malloc`, `realloc`, `free`).
///
/// \param[in] p_allocator Pointer to custom allocator callbacks.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_memory_set_defaults(
    const bj_memory_callbacks* p_allocator
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Reset the global default allocators to system defaults.
///
/// Sets allocators back to standard system functions (`malloc`, `realloc`, `free`).
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_memory_unset_defaults(void);

////////////////////////////////////////////////////////////////////////////////
/// \brief Copy `mem_size` bytes from `p_src` to `p_dest`.
///
/// \param[in] p_dest Destination pointer.
/// \param[in] p_src Source pointer.
/// \param[in] mem_size Number of bytes to copy.
///
/// \return Pointer to `p_dest`.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void* bj_memcpy(
    void* p_dest,
    const void* p_src,
    size_t mem_size
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Move `mem_size` bytes from `p_src` to `p_dest`.
///
/// Similar to \ref bj_memcpy but handles overlapping memory regions safely.
///
/// \param[in] p_dest Destination pointer.
/// \param[in] p_src Source pointer.
/// \param[in] mem_size Number of bytes to move.
///
/// \return Pointer to `p_dest`.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void* bj_memmove(
    void* p_dest,
    const void* p_src,
    size_t mem_size
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Compare two memory blocks.
///
/// \param[in] p_block_a Pointer to first memory block.
/// \param[in] p_block_b Pointer to second memory block.
/// \param[in] size Number of bytes to compare.
///
/// \return Zero if equal, negative if a < b, positive if a > b.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT int bj_memcmp(
    const void* p_block_a,
    const void* p_block_b,
    size_t size
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Fill `mem_size` bytes at `p_dest` with `value`.
///
/// \param[in] p_dest Destination pointer.
/// \param[in] value Byte value to fill.
/// \param[in] mem_size Number of bytes to fill.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_memset(
    void* p_dest,
    uint8_t value,
    size_t mem_size
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Zero out `mem_size` bytes at `p_dest`.
///
/// Effectively calls \ref bj_memset with zero.
///
/// \param[in] p_dest Destination pointer.
/// \param[in] mem_size Number of bytes to zero.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_memzero(
    void* p_dest,
    size_t mem_size
);

/// \} // End of memory group
