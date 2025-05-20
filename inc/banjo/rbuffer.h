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

////////////////////////////////////////////////////////////////////////////////
/// Creates a new bj_rbuffer with a payload size inferred from the type T.
///
/// \param T Type of elements.
/// \param bucket_size The size of each bucket in number of items.
/// \return A pointer to the newly created bj_rbuffer object.
///
/// \see bj_rbuffer_new
#define bj_rbuffer_new_t(T, bucket_size) bj_rbuffer_new(sizeof(T), bucket_size)

////////////////////////////////////////////////////////////////////////////////
/// Creates a new bj_rbuffer with a payload size specified in bytes.
///
/// The function effectively uses \ref bj_rbuffer_alloc and \ref bj_rbuffer_init.
///
/// \param bytes_payload Size of each element in bytes.
/// \param bucket_size The size of each bucket in number of items.
///
/// \return A pointer to the newly created bj_rbuffer object.
BANJO_EXPORT bj_rbuffer* bj_rbuffer_new(
    size_t bytes_payload,
    size_t bucket_size
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
/// \param bucket_size The size of each bucket in number of items.
///
/// \return A pointer to the newly created bj_rbuffer object.
BANJO_EXPORT bj_rbuffer* bj_rbuffer_init(
    bj_rbuffer* p_instance, 
    size_t      bytes_payload,
    size_t      bucket_size
);

////////////////////////////////////////////////////////////////////////////////
/// Resets the entire rbuffer object, making it suitable for free.
///
/// \param p_rbuffer The rbuffer object to reset.
BANJO_EXPORT void bj_rbuffer_reset(
    bj_rbuffer* p_rbuffer
);

////////////////////////////////////////////////////////////////////////////////
/// Retrieves the current capacity of the rbuffer.
///
/// \param rbuffer The rbuffer object to get the capacity from.
/// \return Current capacity of the rbuffer.
///
/// The capacity of a ring buffer correspond to the number of items its internal
/// memory has been allocated for.
/// This is always a multiple of the bucket size set at initialization.
///
/// This capacity can be adjusted using \ref bj_rbuffer_reserve.
/// The is also automatically called when the ring buffer is resized using
/// \ref bj_rbuffer_set_len.
///
/// \retval 0 if `rbuffer` is null.
///
/// \see bj_rbuffer_reserve
BANJO_EXPORT size_t bj_rbuffer_capacity(
    const bj_rbuffer* p_rbuffer
);

////////////////////////////////////////////////////////////////////////////////
/// Reserves memory for at least `capacity` elements in the rbuffer.
///
/// \param rbuffer    The rbuffer object to reserve memory for.
/// \param capacity Number of elements to reserve space for.
///
/// \return The actual capacity
///
/// \par Behaviour
///
/// If `capacity` is smaller than the current capacity, this function does nothing.
/// Otherwise, it reallocates memory to fit the new capacity.
///
/// The actual amount of allocated memory may be greater than the required capacity
/// because of the bucket architecture.
///
/// The returned value will always be a multiple of 
/// `bj_rbuffer_bucket_size(p_rbuffer)`.
///
/// \note When called on a null object, this function does nothing.
BANJO_EXPORT size_t bj_rbuffer_reserve(
    bj_rbuffer* rbuffer,
    size_t      capacity
);

/// \} // End of rbuffer group
