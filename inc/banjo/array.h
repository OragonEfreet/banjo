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
///  Type                 | \ref BjArray
///  Info Type            | \ref BjArrayInfo
///  **Alloc**            | \ref bj_array_alloc 
///  **Create**           | \ref bj_array_new  
///  **Delete**           | \ref bj_array_del  
///  **Reset**            | \ref bj_array_reset  
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
    usize                        bytes_payload;
    usize                        capacity;
    usize                        len;
    void*                        p_buffer;
};
#endif

////////////////////////////////////////////////////////////////////////////////
/// Info structure used to create a new \ref BjArray.
///
/// When `capacity` > _0_, the array is allocated with `bytes_payload`
/// times the value in bytes.
///
/// When `len` > _0_, the array is allocated and resized to the value.
///
/// When `bytes_paload` is _0_, the object will be nil.
///
typedef struct {
    usize bytes_payload; ///< Size in bytes, of each item in the array.
    usize len;         ///< Number of elements in the array.
    usize capacity;      ///< Number of allocated elements in the array.
} BjArrayInfo;


////////////////////////////////////////////////////////////////////////////////
/// Create a new \ref BjArray.
///
/// \param p_allocator Allocation callbacks, can be _0_.
/// \param p_info      Creation options.
///
/// \return A pointer to an empty array object.
///
/// \par Memory Management
///
/// The array pointed to by the returned handle **must** be released after use
/// by calling \ref bj_array_del.
///
/// When `p_allocator` is not _0_, the given allocator is used though the entire
/// life of the array object.
/// The content of `p_allocator` being copied in memory, the caller doesn't have
/// to retain the pointer after creating the array.
///
/// If `p_info` is zero, the object is initialized nil.
///
/// \see bj_array_del
BANJO_EXPORT BjArray* bj_array_new(
    const BjArrayInfo*           p_info,
    const BjAllocationCallbacks* p_allocator
);


////////////////////////////////////////////////////////////////////////////////
/// Destroy an array previously created by \ref bj_array_new.
///
/// \param array The instance to destroy.
///
/// \par Memory Management
///
/// The memory allocated for `array` will be freed using the allocator callbacks
/// set by \ref bj_array_new.
///
/// \see bj_array_new
BANJO_EXPORT void bj_array_del(
    BjArray* array
);

////////////////////////////////////////////////////////////////////////////////
/// Allocate a new BjArray object
///
/// \param p_allocator Allocation callbacks, can be _0_.
///
/// \return An uninitialized array object
///
/// \par Memory Management
///
/// The array pointed to by the returned handle **must** be released after use
/// by calling \ref bj_free.
///
/// When `p_allocator` is not _0_, the given allocator is used though the entire
/// life of the array object.
/// The content of `p_allocator` being copied in memory, the caller doesn't have
/// to retain the pointer after creating the array.
BANJO_EXPORT BjArray* bj_array_alloc(
    const BjAllocationCallbacks* p_allocator
);

////////////////////////////////////////////////////////////////////////////////
/// Initializes a new \ref BjArray.
///
/// \param p_instance  The array object
/// \param p_info      Creation options.
/// \param p_allocator Allocation callbacks, can be _0_.
///
/// \par Create Info
///
/// When `p_info->capacity` > _0_, the array is allocated with `bytes_payload`
/// times the value in bytes.
///
/// When `p_info->len` > _0_, the array is allocated and resized to the value.
///
/// \par Memory Management
///
/// The array pointed to by the returned handle **must** be released after use
/// by calling \ref bj_array_del.
///
/// When `p_allocator` is not _0_, the given allocator is used though the entire
/// life of the array object.
/// The content of `p_allocator` being copied in memory, the caller doesn't have
/// to retain the pointer after creating the array.
///
/// \par Error Management
///
/// The function fails if `p_instance` is _0_.
///
/// \see bj_array_del
BANJO_EXPORT void bj_array_init(
    BjArray*                     p_instance,
    const BjArrayInfo*           p_info,
    const BjAllocationCallbacks* p_allocator
);

////////////////////////////////////////////////////////////////////////////////
/// Resets a \ref BjArray to a nil state
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
/// internal memory is not released.
/// To effectively free the memory used by the array, call \ref bj_array_shrink
/// after have called this function.
///
/// When called on an  nil object, the function does nothing.
///
/// The function fails is `array` is _0_.
BANJO_EXPORT void bj_array_clear(
    BjArray* array
);

////////////////////////////////////////////////////////////////////////////////
/// Reallocated the used memory to match the array length.
///
/// \param array The array object.
///
/// \ref BjArray usually occupies more space in memory than needed.
/// This is due to array size growth policy that always allocated two times
/// the required space.
/// 
/// This function reallocated the memory used by the array to fit the current 
/// array length.
///
/// When called on an nil object, the function does nothing.
///
/// The function fails is `array` is _0_.
///
/// \note This function effectively invalidates the array data pointer.
BANJO_EXPORT void bj_array_shrink(
    BjArray* array
);

////////////////////////////////////////////////////////////////////////////////
/// Resize the array.
///
/// If needed, the array will reserve up to twice the requested length in memory.
/// 
/// \param array The array object.
/// \param len The number of element in the new array.
///
/// If `len == 0`, this function is the same as calling \ref bj_array_clear.
///
/// When called on an nil, the function does nothing.
///
/// \note This function will invalidate the array data pointer if the resize
/// required a new reallocation.
BANJO_EXPORT void bj_array_set_len(
    BjArray* array,
    usize   len
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
/// When called on an nil, the function does nothing.
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
/// Calling this function effectively grow array len by _1_.
/// The newly added object can be retrieved by calling \ref bj_array_at with
/// an index of `len` - _1_.
///
/// When called on an nil, the function does nothing.
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
///
/// When called on an nil, the function does nothing.
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
/// \retval 0 if `array` is nil.
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
///
/// \retval 0 if `array` is nil.
///
BANJO_EXPORT void* bj_array_data(
    const BjArray* array
);

////////////////////////////////////////////////////////////////////////////////
/// Returns the number of elements in the given array.
///
/// \param array The array object.
///
/// \return a integer indicating the number of elements in the array.
///
/// \retval 0 if `array` is nil.
///
BANJO_EXPORT usize bj_array_len(
    const BjArray* array
);

////////////////////////////////////////////////////////////////////////////////
/// Returns the number of elements the array can hold without reallocating.
///
/// \param array The array object.
///
/// \return a integer indicating the capacity of the array.
///
/// \retval 0 if `array` is nil.
///
BANJO_EXPORT usize bj_array_capacity(
    const BjArray* array
);

/// \} End of array group


