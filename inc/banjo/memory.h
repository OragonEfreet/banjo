////////////////////////////////////////////////////////////////////////////////
/// \file memory.h
/// All memory-related functions, including custom allocators.
////////////////////////////////////////////////////////////////////////////////
/// \defgroup memory Memory Management
///
/// \{
////////////////////////////////////////////////////////////////////////////////
#ifndef BJ_MEMORY_H
#define BJ_MEMORY_H

#include <banjo/api.h>

////////////////////////////////////////////////////////////////////////////////
/// \typedef bj_malloc_fn
/// \brief Memory allocation callback.
///
/// Used in  bj_memory_callbacks to set the function used for custom allocations.
///
/// \param user_data General purpose context data.
/// \param size Allocation size in bytes requested by the caller.
///
/// \return Pointer to allocated memory block.
////////////////////////////////////////////////////////////////////////////////
typedef void* (*bj_malloc_fn)(
    void* user_data,
    size_t size
);

////////////////////////////////////////////////////////////////////////////////
/// \typedef bj_realloc_fn
/// \brief Memory reallocation callback.
///
/// Used in  bj_memory_callbacks to set the function used for custom reallocations.
///
/// \param user_data General purpose context data.
/// \param original Initial object to reallocate.
/// \param size Allocation size in bytes requested by the caller.
///
/// \return Pointer to reallocated memory block.
////////////////////////////////////////////////////////////////////////////////
typedef void* (*bj_realloc_fn)(
    void* user_data,
    void* original,
    size_t size
);

////////////////////////////////////////////////////////////////////////////////
/// \typedef bj_free_fn
/// \brief Memory free callback.
///
/// Used in  bj_memory_callbacks to set the function used for custom deallocations.
///
/// \param user_data General purpose context data.
/// \param memory Object memory to dispose.
////////////////////////////////////////////////////////////////////////////////
typedef void (*bj_free_fn)(
    void* user_data,
    void* memory
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Custom allocation callbacks.
///
/// This structure holds function pointers for allocation,
/// reallocation, and deallocation callbacks along with user data.
///
/// These callbacks can be assigned per-object or set globally
/// with \ref bj_set_memory_defaults.
////////////////////////////////////////////////////////////////////////////////
struct bj_memory_callbacks {
    void*         user_data;       ///< General purpose context data.
    bj_malloc_fn  fn_allocation;     ///< Allocation function pointer.
    bj_realloc_fn fn_reallocation;   ///< Reallocation function pointer.
    bj_free_fn    fn_free;           ///< Deallocation function pointer.
};

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
/// \param[in] memory Pointer to previously allocated memory.
/// \param[in] size Number of bytes to allocate.
///
/// \return Pointer to newly reallocated memory block.
///
/// \note `memory` must have been allocated by \ref bj_malloc or \ref bj_realloc.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void* bj_realloc(
    void* memory,
    size_t size
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Free a previously allocated memory block.
///
/// \param[in] memory Pointer to memory to free.
///
/// \note `memory` must have been allocated by \ref bj_malloc or \ref bj_realloc.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_free(
    void* memory
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Set the global default memory allocators.
///
/// If `allocator` is `NULL`, resets to system defaults (`malloc`, `realloc`, `free`).
///
/// \param[in] allocator Pointer to custom allocator callbacks.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_set_memory_defaults(
    const struct bj_memory_callbacks* allocator
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Reset the global default allocators to system defaults.
///
/// Sets allocators back to standard system functions (`malloc`, `realloc`, `free`).
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_unset_memory_defaults(void);

////////////////////////////////////////////////////////////////////////////////
/// \brief Copy `mem_size` bytes from `src` to `dest`.
///
/// \param[in] dest Destination pointer.
/// \param[in] src Source pointer.
/// \param[in] mem_size Number of bytes to copy.
///
/// \return Pointer to `dest`.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void* bj_memcpy(
    void* dest,
    const void* src,
    size_t mem_size
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Move `mem_size` bytes from `src` to `dest`.
///
/// Similar to \ref bj_memcpy but handles overlapping memory regions safely.
///
/// \param[in] dest Destination pointer.
/// \param[in] src Source pointer.
/// \param[in] mem_size Number of bytes to move.
///
/// \return Pointer to `dest`.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void* bj_memmove(
    void* dest,
    const void* src,
    size_t mem_size
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Compare two memory blocks.
///
/// \param[in] block_a Pointer to first memory block.
/// \param[in] block_b Pointer to second memory block.
/// \param[in] size Number of bytes to compare.
///
/// \return Zero if equal, negative if a < b, positive if a > b.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT int bj_memcmp(
    const void* block_a,
    const void* block_b,
    size_t size
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Fill `mem_size` bytes at `dest` with `value`.
///
/// \param[in] dest Destination pointer.
/// \param[in] value Byte value to fill.
/// \param[in] mem_size Number of bytes to fill.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_memset(
    void* dest,
    uint8_t value,
    size_t mem_size
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Zero out `mem_size` bytes at `dest`.
///
/// Effectively calls \ref bj_memset with zero.
///
/// \param[in] dest Destination pointer.
/// \param[in] mem_size Number of bytes to zero.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_memzero(
    void* dest,
    size_t mem_size
);

#endif
/// \} // End of memory group
