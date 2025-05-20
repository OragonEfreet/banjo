////////////////////////////////////////////////////////////////////////////////
/// \file
/// Header file for \ref rbuffer container type.
////////////////////////////////////////////////////////////////////////////////
/// \defgroup rbuffer Array
/// \ingroup algo
///
/// \{
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <banjo/api.h>

////////////////////////////////////////////////////////////////////////////////
/// Typedef for the bj_rbuffer_t struct
typedef struct bj_rbuffer_t bj_rbuffer;

typedef enum bj_rbuffer_state_t {
    BJ_RBUFFER_OK,
    BJ_RBUFFER_UNDERRUN,
    BJ_RBUFFER_OVERRUN,
} bj_rbuffer_state;

////////////////////////////////////////////////////////////////////////////////
/// Creates a new bj_rbuffer with a payload size inferred from the type T.
///
/// \param T Type of elements.
/// \return A pointer to the newly created bj_rbuffer object.
///
/// \see bj_rbuffer_new
#define bj_rbuffer_new_t(T) bj_rbuffer_new(sizeof(T))

////////////////////////////////////////////////////////////////////////////////
/// Creates a new bj_rbuffer with a payload size specified in bytes.
///
/// The function effectively uses \ref bj_rbuffer_alloc and \ref bj_rbuffer_init.
///
/// \param bytes_payload Size of each element in bytes.
/// \return A pointer to the newly created bj_rbuffer object.
BANJO_EXPORT bj_rbuffer* bj_rbuffer_new(
    size_t bytes_payload
);

////////////////////////////////////////////////////////////////////////////////
/// Deletes a bj_rbuffer object and releases associated memory.
///
/// \param p_rbuffer Pointer to the bj_rbuffer object to delete.
BANJO_EXPORT void bj_rbuffer_del(
    bj_rbuffer* p_rbuffer
);

////////////////////////////////////////////////////////////////////////////////
/// Initialize a new rbuffer with given size.
///
/// \param p_instance A pointer to an rbuffer object.
/// \param bytes_payload Size of each element in bytes.
///
/// \return A pointer to the newly created bj_rbuffer object.
BANJO_EXPORT bj_rbuffer* bj_rbuffer_init(
    bj_rbuffer* p_instance, 
    size_t          bytes_payload
);

////////////////////////////////////////////////////////////////////////////////
/// Resets the entire rbuffer object, making it suitable for free.
///
/// \param p_rbuffer The rbuffer object to reset.
BANJO_EXPORT void bj_rbuffer_reset(
    bj_rbuffer* p_rbuffer
);


////////////////////////////////////////////////////////////////////////////////
/// Clears all elements in the rbuffer.
///
/// \param rbuffer The rbuffer object to clear.
/// 
/// If the rbuffer is already empty, this function does nothing.
///
/// \note When called on a null object, this function does nothing.
BANJO_EXPORT void bj_rbuffer_clear(
    bj_rbuffer* rbuffer
);

////////////////////////////////////////////////////////////////////////////////
/// Resizes the rbuffer to the specified length.
///
/// \param rbuffer The rbuffer object to resize.
/// \param len   New length of the rbuffer.
///
/// If `len == 0`, this function is equivalent to calling \ref bj_rbuffer_clear.
///
/// \note When called on a null object, this function does nothing.
BANJO_EXPORT void bj_rbuffer_set_len(
    bj_rbuffer* rbuffer,
    size_t          len
);

////////////////////////////////////////////////////////////////////////////////
/// Reserves memory for up to `capacity` elements in the rbuffer.
///
/// \param rbuffer    The rbuffer object to reserve memory for.
/// \param capacity Number of elements to reserve space for.
///
/// If `capacity` is smaller than the current capacity, this function does nothing.
/// Otherwise, it reallocates memory to fit the new capacity.
///
/// \note When called on a null object, this function does nothing.
///       This function invalidates the rbuffer data pointer if reallocation is performed.
BANJO_EXPORT void bj_rbuffer_reserve(
    bj_rbuffer* rbuffer,
    size_t          capacity
);

BANJO_EXPORT bj_rbuffer_state bj_rbuffer_push(
    bj_rbuffer*   rbuffer,
    const void*   p_src,
    size_t        count
);

BANJO_EXPORT bj_rbuffer_state bj_rbuffer_pop(
    bj_rbuffer*  rbuffer
    const void*  p_dest,
    size_t       count
);

////////////////////////////////////////////////////////////////////////////////
/// Retrieves the number of elements in the rbuffer.
///
/// \param rbuffer The rbuffer object to get the length from.
/// \return Number of elements in the rbuffer.
///
/// \retval 0 if `rbuffer` is null.
BANJO_EXPORT size_t bj_rbuffer_len(
    const bj_rbuffer* rbuffer
);

////////////////////////////////////////////////////////////////////////////////
/// Retrieves the current capacity of the rbuffer.
///
/// \param rbuffer The rbuffer object to get the capacity from.
/// \return Current capacity of the rbuffer.
///
/// \retval 0 if `rbuffer` is null.
BANJO_EXPORT size_t bj_rbuffer_capacity(
    const bj_rbuffer* rbuffer
);

/// \} // End of rbuffer group
