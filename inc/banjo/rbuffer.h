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

/// \} // End of rbuffer group
