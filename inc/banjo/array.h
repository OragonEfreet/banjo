////////////////////////////////////////////////////////////////////////////////
/// \file
/// Header file for \ref array container type.
////////////////////////////////////////////////////////////////////////////////
/// \defgroup array Array
/// \ingroup algo
///
/// \brief The \ref bj_array type is a sequence container that encapsulates 
///        dynamic C-style arrays.
///        The elements are stored contiguously, allowing access using offsets.
///
///        The storage of the array is expanded as needed.
/// \{
////////////////////////////////////////////////////////////////////////////////
#ifndef BJ_ARRAY_H
#define BJ_ARRAY_H

#include <banjo/api.h>

////////////////////////////////////////////////////////////////////////////////
/// Typedef for the bj_array_t struct
typedef struct bj_array_t bj_array;

////////////////////////////////////////////////////////////////////////////////
/// Creates a new bj_array with a payload size inferred from the type T.
///
/// \param T Type of elements.
/// \return A pointer to the newly created bj_array object.
///
/// \see bj_array_new
#define bj_array_new_t(T) bj_array_new(sizeof(T))

////////////////////////////////////////////////////////////////////////////////
/// Creates a new bj_array with a payload size specified in bytes.
///
/// The function effectively uses \ref bj_array_alloc and \ref bj_array_init.
///
/// \param bytes_payload Size of each element in bytes.
/// \return A pointer to the newly created bj_array object.
BANJO_EXPORT bj_array* bj_array_new(
    size_t bytes_payload
);

////////////////////////////////////////////////////////////////////////////////
/// Deletes a bj_array object and releases associated memory.
///
/// \param p_array Pointer to the bj_array object to delete.
BANJO_EXPORT void bj_array_del(
    bj_array* p_array
);

////////////////////////////////////////////////////////////////////////////////
/// Allocate enough memory to hold an \ref array object
///
/// \return A pointer to the newly allocated memory.
///
/// \par Memory Management
///
/// The memory returned by the function must be freed with \ref bj_free
///
/// \see bj_array_new, bj_array_init, bj_array_reset, bj_array_del
BANJO_EXPORT bj_array* bj_array_alloc(
    void
);

////////////////////////////////////////////////////////////////////////////////
/// Initialize a new array with given size.
///
/// \param p_instance A pointer to an array object.
/// \param bytes_payload Size of each element in bytes.
///
/// \return A pointer to the newly created bj_array object.
BANJO_EXPORT bj_array* bj_array_init(
    bj_array* p_instance, 
    size_t    bytes_payload
);

////////////////////////////////////////////////////////////////////////////////
/// Resets the entire array object, making it suitable for free.
///
/// \param p_array The array object to reset.
BANJO_EXPORT void bj_array_reset(
    bj_array* p_array
);


////////////////////////////////////////////////////////////////////////////////
/// Clears all elements in the array.
///
/// \param array The array object to clear.
/// 
/// If the array is already empty, this function does nothing.
/// To release the internal memory, call \ref bj_array_shrink after clearing.
///
/// \note When called on a null object, this function does nothing.
BANJO_EXPORT void bj_array_clear(
    bj_array* array
);

////////////////////////////////////////////////////////////////////////////////
/// Shrinks the memory allocation to fit the current array length.
///
/// \param array The array object to shrink.
///
/// This function reallocates the memory used by the array to fit its current length.
///
/// \note When called on a null object, this function does nothing.
///       This function invalidates the array data pointer.
BANJO_EXPORT void bj_array_shrink(
    bj_array* array
);

////////////////////////////////////////////////////////////////////////////////
/// Resizes the array to the specified length.
///
/// \param array The array object to resize.
/// \param len   New length of the array.
///
/// If `len == 0`, this function is equivalent to calling \ref bj_array_clear.
///
/// \note When called on a null object, this function does nothing.
///       This function invalidates the array data pointer if reallocation is necessary.
BANJO_EXPORT void bj_array_set_len(
    bj_array* array,
    size_t     len
);

////////////////////////////////////////////////////////////////////////////////
/// Reserves memory for up to `capacity` elements in the array.
///
/// \param array    The array object to reserve memory for.
/// \param capacity Number of elements to reserve space for.
///
/// If `capacity` is smaller than the current capacity, this function does nothing.
/// Otherwise, it reallocates memory to fit the new capacity.
///
/// \note When called on a null object, this function does nothing.
///       This function invalidates the array data pointer if reallocation is performed.
BANJO_EXPORT void bj_array_reserve(
    bj_array* array,
    size_t     capacity
);

////////////////////////////////////////////////////////////////////////////////
/// Appends a value to the end of the array.
///
/// \param array The array object to push the value into.
/// \param value Pointer to the value to append.
///
/// Copies the memory pointed to by `value` into the array using \ref bj_memcpy.
///
/// \note Calling this function may reserve more space in the array, which invalidates the data pointer.
/// \note When called on a null object, this function does nothing.
BANJO_EXPORT void bj_array_push(
    bj_array*   array,
    const void* value
);

////////////////////////////////////////////////////////////////////////////////
/// Removes the last value from the array.
///
/// \param array The array object to remove the last value from.
///
/// This function reduces the array size by one.
///
/// \note When called on a null object, this function does nothing.
BANJO_EXPORT void bj_array_pop(
    bj_array* array
);

////////////////////////////////////////////////////////////////////////////////
/// Retrieves the value stored at the specified index in the array.
///
/// \param array The array object to retrieve the value from.
/// \param at    Index of the element to retrieve.
/// \return Pointer to the value at the specified index.
///
/// \retval 0 if `array` is null.
BANJO_EXPORT void* bj_array_at(
    const bj_array* array,
    size_t           at
);

////////////////////////////////////////////////////////////////////////////////
/// Retrieves a pointer to the underlying data of the array.
///
/// \param array The array object to get the data pointer from.
/// \return Pointer to the underlying data.
///
/// \retval 0 if `array` is null.
BANJO_EXPORT void* bj_array_data(
    const bj_array* array
);

////////////////////////////////////////////////////////////////////////////////
/// Retrieves the number of elements in the array.
///
/// \param array The array object to get the length from.
/// \return Number of elements in the array.
///
/// \retval 0 if `array` is null.
BANJO_EXPORT size_t bj_array_len(
    const bj_array* array
);

////////////////////////////////////////////////////////////////////////////////
/// Retrieves the current capacity of the array.
///
/// \param array The array object to get the capacity from.
/// \return Current capacity of the array.
///
/// \retval 0 if `array` is null.
BANJO_EXPORT size_t bj_array_capacity(
    const bj_array* array
);

#endif
/// \} // End of array group
