////////////////////////////////////////////////////////////////////////////////
/// \file
/// All memory-related functions, including custom allocators.

////////////////////////////////////////////////////////////////////////////////
/// \defgroup memory Memory Management 
/// \ingroup core 
/// Memory allocation and manipulation facilities
/// \{
#pragma once

#include <banjo/api.h>

////////////////////////////////////////////////////////////////////////////////
/// Memory allocation callback.
/// Used in \ref bj_memory_callbacks to set the function used for custom allocations.
typedef void* (*bj_malloc_fn)(
    void* p_user_data, //< General purpose context data.
    usize size         //< Allocation size in bytes requested by the caller.
);

////////////////////////////////////////////////////////////////////////////////
/// Memory allocation callback.
/// Used in \ref bj_memory_callbacks to set the function used for custom reallocations.
typedef void* (*bj_realloc_fn)(
    void* p_user_data, //< General purpose context data.
    void* p_original,  //< Initial object to reallocate.
    usize size         //< Allocation size in bytes requested by the caller.
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
    usize size
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
    usize                        size
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
/// When a function requires memory changes, (such as \ref bj_new),
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
    usize       mem_size
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
    u8    value,
    usize mem_size
);

/// \brief Convenience macro for creating a new object of any type.
///
/// This macro effectively calls \ref bj_malloc and any corresponding 
/// initialization function, given it exists.
///
/// \param typ      The name of the object to create, for example, _array_.
/// \param initlzr  The right and side of any `bj_??_init_()` function.
///
/// \par Example
///
/// The following line:
///
///     bj_new(array, with_capacity, sizeof(int), 10)
///
/// will expand to:
///
///     bj_array_init_with_capacity(bj_malloc(sizeof(bj_array), sizeof(int), 10))
///
/// \par Memory
///
/// The allocation being done using \ref bj_malloc, the memory can be managed
/// using the custom allocators (see \ref bj_memory_callbacks).
///
/// The returned pointer must be reset, then freed.
/// For this, call \ref bj_del.
#define bj_new(typ, initlzr, ...) bj_ ## typ ## _init_ ## initlzr(bj_malloc(sizeof(bj_ ## typ)), __VA_ARGS__)


/// \brief Convenience macro for deleting an object.
///
/// Thie macro effectively calls the reset function of the given type, then \ref bj_free.
///
/// \param typ      The name of the object to create, for example, _array_.
/// \param ptr      The object to delete
///
/// \par Example
///
/// The following line:
///
///     bj_del(array, an_array)
///
/// will expand to:
///
///     bj_free(bj_array_reset(an_array))
///
/// \par Memory
///
/// The deallocation being done using \ref bj_free, the memory can be managed
/// using the custom allocators (see \ref bj_memory_callbacks).
#define bj_del(typ, ptr) bj_free(bj_ ## typ ## _reset(ptr))


////////////////////////////////////////////////////////////////////////////////
/// \def bj_with(typ, var, initlzr, ...)
///
/// \brief Scoped resource management macro for Banjo objects.
///
/// This macro simplifies resource management for Banjo objects by encapsulating
/// their initialization and disposal within a scoped block. It initializes a
/// Banjo object using the specified initializer and optional arguments, and
/// automatically disposes of it at the end of the block.
///
/// \param typ     The type of the Banjo object.
/// \param var     The name of the variable representing the Banjo object.
/// \param initlzr The initializer for the Banjo object.
/// \param ...     Optional additional arguments required by the initializer.
///
/// Usage:
/// \code{.c}
/// bj_with(array, arr, default_t, int) {
///     // Use 'arr' within this block
/// }
/// // 'arr' is automatically disposed after the block
/// \endcode
///
/// \warning Early return
/// This macro does not perform auto-free on early return.
/// If you ever return from inside the block, you need to manually dispose the
/// object again:
/// \code{.c}
/// bj_with(array, arr, default_t, int) {
///     bj_del(array, arr);
///     return;
/// }
/// \endcode
///
#define bj_with(typ, var, initlzr, ...) \
    for(bj_ ## typ* var = bj_new(typ, initlzr, __VA_ARGS__); \
    !!(var); \
    bj_del(typ, var), var=0)

/// \} End of memory
