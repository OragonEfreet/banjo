////////////////////////////////////////////////////////////////////////////////
/// \file
/// Header file for \ref array container type.

////////////////////////////////////////////////////////////////////////////////
/// \defgroup array Array
/// \ingroup containers
///
/// \ref bj_array is a sequence container that encapsulate dynamic C-Style arrays.
/// The elements are stored contiguously, which means that they can be accessed
/// using offsets.
///
/// The storage of the array is expanded as needed.
/// \{
#pragma once

#include <banjo/api.h>

typedef struct bj_array_t bj_array; ///< Typedef for the bj_array_t struct

/// \brief The internal data structure for the \ref bj_array type.
struct bj_array_t {
    usize bytes_payload; ///< Size in bytes, of each item in the array.
    usize len;           ///< Number of elements in the array.
    usize capacity;      ///< Number of allocated elements in the array.
    void* p_buffer;      ///< Data buffer
};

////////////////////////////////////////////////////////////////////////////////
/// Initializes a new \ref bj_array.
///
/// \param p_instance     The array object
/// \param bytes_payload  The size in byte of each element in the array.
///
/// This creates and empty array with initial capacity to _0_.
///
/// \par Error Management
///
/// If `p_instance` is _0_, the function does nothing.
/// 
/// If `bytes_payload` is _0_, `p_instance` is set to \nil.
///
/// \return `p_instance`.
///
/// \see bj_array_del
BANJO_EXPORT bj_array* bj_array_init_default(
    bj_array* p_instance,
    usize     bytes_payload
);

////////////////////////////////////////////////////////////////////////////////
/// Initializes a new \ref bj_array.
///
/// \param p_instance   The array object
/// \param T            Value type
///
/// This creates and empty array with initial capacity to _0_.
///
/// The function expands to a call to \ref bj_array_init_default, using
/// `T` for the `bytes_payload` parameter.
///
/// \par Error Management
///
/// If `p_instance` is _0_, the function does nothing.
/// 
/// If `bytes_payload` is _0_, `p_instance` is set to \nil.
///
/// \return `p_instance`.
///
/// \see bj_array_del
#define bj_array_init_default_t(p_instance, T) bj_array_init_default(p_instance, sizeof(T))

////////////////////////////////////////////////////////////////////////////////
/// Initializes a new \ref bj_array byt setting its initial capacity.
///
/// \param p_instance     The array object
/// \param bytes_payload  The size in byte of each element in the array.
/// \param capacity       The requested initial capacity.
///
/// The internal memory will effectively be allocated to contain at least
/// `capacity` times `bytes_payload` bytes.
/// The array itself is still empty at initialization.
///
/// \par Error Management
///
/// If `p_instance` is _0_, the function does nothing.
/// 
/// If `bytes_payload` is _0_, `p_instance` is set to \nil.
///
/// \return `p_instance`.
///
/// \see bj_array_del
BANJO_EXPORT bj_array* bj_array_init_with_capacity(
    bj_array* p_instance,
    usize     bytes_payload,
    usize     capacity
);

////////////////////////////////////////////////////////////////////////////////
/// Initializes a new \ref bj_array.
///
/// \param p_instance   The array object
/// \param T            Value type
/// \param capacity     The requested initial capacity.
///
/// This creates and empty array with initial capacity to _0_.
///
/// The function expands to a call to \ref bj_array_init_with_capacity, 
/// using `T` for the `bytes_payload` parameter.
///
/// \par Error Management
///
/// If `p_instance` is _0_, the function does nothing.
/// 
/// If `bytes_payload` is _0_, `p_instance` is set to \nil.
///
/// \return `p_instance`.
///
/// \see bj_array_del
#define bj_array_init_with_capacity_t(p_instance, T, capacity) bj_array_init_with_capacity(p_instance, sizeof(T), capacity)

////////////////////////////////////////////////////////////////////////////////
/// Resets a \ref bj_array to a nil state
///
/// \param p_array The array object
///
/// \return `p_array`
BANJO_EXPORT bj_array* bj_array_reset(
    bj_array* p_array
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
    bj_array* array
);

////////////////////////////////////////////////////////////////////////////////
/// Reallocated the used memory to match the array length.
///
/// \param array The array object.
///
/// \ref bj_array usually occupies more space in memory than needed.
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
    bj_array* array
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
    bj_array* array,
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
    bj_array* array,
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
    bj_array* array,
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
    bj_array* array
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
    const bj_array* array,
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
    const bj_array* array
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
    const bj_array* array
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
    const bj_array* array
);

/// \} End of array group


