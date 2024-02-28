////////////////////////////////////////////////////////////////////////////////
/// \file
/// Header file for \ref array container type.

////////////////////////////////////////////////////////////////////////////////
/// \defgroup array Array
/// \ingroup containers
/// API related to the \ref BjArray object
///
///  **Generic Name**     | array                 
/// ----------------------|-----------------------
///  **Opaque Type**      | yes                   
///  **Create Function**  | \ref bj_array_create  
///  **Destroy Function** | \ref bj_array_destroy 
///
/// \ref BjArray is a sequence container that encapsulate dynamic C-Style arrays.
/// The elements are stored contiguously, which means that they can be accessed
/// using offsets.
///
/// The storage of the array is expanded as needed.
/// \{
#pragma once

#include <banjo/api.h>
#include <banjo/memory.h>

////////////////////////////////////////////////////////////////////////////////
/// Typedef for the BjArray_T struct
typedef struct BjArray_T BjArray;

#ifdef BJ_NO_OPAQUE
struct BjArray_T {
    const BjAllocationCallbacks* p_allocator;
    usize                        value_size;
    usize                        capacity;
    usize                        count;
    void*                        p_data;
};
#endif

/// Info structure used to create a new \ref BjArray.
typedef struct {
    usize value_size; ///< Size in bytes, of each item in the array.
    usize count;      ///< Number of elements in the array.
    usize capacity;   ///< Number of allocated elements in the array.
} BjArrayInfo;

////////////////////////////////////////////////////////////////////////////////
/// Create a new \ref BjArray.
///
/// \param p_info      Creation options.
/// \param p_allocator Allocation callbacks, can be _0_.
///
/// \return A handle to a new array.
///
/// \par Create Info
///
/// When `p_info->capacity` > _0_, the array is allocated with `value_size`
/// times the value in bytes.
///
/// When `p_info->count` > _0_, the array is allocated and resized to the value.
///
/// \par Memory Management
///
/// The array pointed to by the returned handle **must** be released after use
/// by calling \ref bj_array_destroy.
///
/// When `p_allocator` is not _0_, the given allocator is used though the entire
/// life of the array object.
/// The content of `p_allocator` being copied in memory, the caller doesn't have
/// to retain the pointer after creating the array.
///
/// \see bj_array_destroy
BANJO_EXPORT BjArray* bj_array_create(
    const BjArrayInfo*           p_info,
    const BjAllocationCallbacks* p_allocator
);

////////////////////////////////////////////////////////////////////////////////
/// Destroy an array previously created by \ref bj_array_create.
///
/// \param array The instance to destroy.
///
/// \par Memory Management
///
/// The memory allocated for `array` will be freed using the allocator callbacks
/// set by \ref bj_array_create.
///
/// \see bj_array_create
BANJO_EXPORT void bj_array_destroy(
    BjArray* array
);

////////////////////////////////////////////////////////////////////////////////
/// Initializes a new \ref BjArray.
///
/// \param p_info      Creation options.
/// \param p_allocator Allocation callbacks, can be _0_.
/// \param p_array     The array object
///
/// \par Create Info
///
/// When `p_info->capacity` > _0_, the array is allocated with `value_size`
/// times the value in bytes.
///
/// When `p_info->count` > _0_, the array is allocated and resized to the value.
///
/// \par Memory Management
///
/// The array pointed to by the returned handle **must** be released after use
/// by calling \ref bj_array_destroy.
///
/// When `p_allocator` is not _0_, the given allocator is used though the entire
/// life of the array object.
/// The content of `p_allocator` being copied in memory, the caller doesn't have
/// to retain the pointer after creating the array.
///
/// \see bj_array_destroy
BANJO_EXPORT void bj_array_init(
    const BjArrayInfo*           p_info,
    const BjAllocationCallbacks* p_allocator,
    BjArray*                     p_array
);

////////////////////////////////////////////////////////////////////////////////
/// Resets a \ref BjArray to an invalid state
///
/// \param p_array The array object
BANJO_EXPORT void bj_array_reset(
    BjArray* p_array
);

////////////////////////////////////////////////////////////////////////////////
/// Clear all data in the array.
///
/// \param array The array object.
///
/// If the array is already empty, this function does nothing.
/// After calling this function, the array is considered as empty, but the
/// internal memory is not release.
/// To effectively free the memory used by the array, call \ref bj_array_shrink
/// after have called this function.
BANJO_EXPORT void bj_array_clear(
    BjArray* array
);

////////////////////////////////////////////////////////////////////////////////
/// Reallocated the used memory to match the array count.
///
/// \param array The array object.
///
/// \ref BjArray usually occupies more space in memory than needed.
/// This is due to array size growth policy that always allocated two times
/// the required space.
/// 
/// This function reallocated the memory used by the array to fit the current 
/// array count.
///
/// \note This function effectively invalidates the array data pointer.
BANJO_EXPORT void bj_array_shrink(
    BjArray* array
);

////////////////////////////////////////////////////////////////////////////////
/// Resize the array.
///
/// If needed, the array will reserve up to twice the requested count in memory.
/// 
/// \param array The array object.
/// \param count The number of element in the new array.
///
/// If `count == 0`, this function is the same as calling \ref bj_array_clear.
///
/// \note This function will invalidate the array data pointer if the resize
/// required a new reallocation.
BANJO_EXPORT void bj_array_set_count(
    BjArray* array,
    usize   count
);

////////////////////////////////////////////////////////////////////////////////
/// Reserve up to `capacity` elements in memory.
///
/// \param array    The array object.
/// \param capacity The number of element to reserve in the new array.
///
/// If `capacity` is smaller than the array current capacity, this function does
/// nothing.
/// Otherwise, the array in-memory is reallocated to fit the new capacity.
///
/// \note This function will invalidate the array data pointer if the reserve
/// if performed.
BANJO_EXPORT void bj_array_reserve(
    BjArray* array,
    usize   capacity
);

////////////////////////////////////////////////////////////////////////////////
/// Pushes a new value in the array.
///
/// \param array The array object.
/// \param value A pointer to the value to push.
///
/// The memory pointed at `value` is copied into the in-memory array using
/// \ref bj_memcpy.
///
/// Calling this function effectively grow array count by _1_.
/// The newly added object can be retrieved by calling \ref bj_array_at with
/// an index of `count` - _1_.
///
/// \note If needed, the function will reserve more space in the array,
/// which invalidates the data pointer.
BANJO_EXPORT void bj_array_push(
    BjArray* array,
    const void* value
);

////////////////////////////////////////////////////////////////////////////////
/// Deletes the last value of the array.
///
/// \param array The array object.
///
/// This function does nothing else than reducing the array size by _1_.
BANJO_EXPORT void bj_array_pop(
    BjArray* array
);

////////////////////////////////////////////////////////////////////////////////
/// Returns the stored value given the index `at`.
///
/// \param array The array object.
/// \param at    Position of the element to return.
///
/// \return A pointer to the value.
///
BANJO_EXPORT void* bj_array_at(
    const BjArray* array,
    usize   at
);

////////////////////////////////////////////////////////////////////////////////
/// Returns a pointer to the underlying data.
///
/// \param array The array object
///
/// \return a pointer to the underlying data.
BANJO_EXPORT void* bj_array_data(
    const BjArray* array
);

////////////////////////////////////////////////////////////////////////////////
/// Returns the number of elements in the given array.
///
/// \param array The array object.
///
/// \return a integer indicating the number of elements in the array.
BANJO_EXPORT usize bj_array_count(
    const BjArray* array
);

/// \} End of array group


