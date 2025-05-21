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
/// Creates a new bj_rbuffer with a payload size specified in bytes.
///
/// The function effectively uses \ref bj_rbuffer_alloc and \ref bj_rbuffer_init.
///
/// \param capacity The initial buffer capacity
///
/// \par Behaviour
///
/// \return A pointer to the newly created bj_rbuffer object.
BANJO_EXPORT bj_rbuffer* bj_rbuffer_new(
    size_t capacity
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
/// \param capacity   The initial capacity
///
/// \return A pointer to the newly created bj_rbuffer object.
BANJO_EXPORT bj_rbuffer* bj_rbuffer_init(
    bj_rbuffer* p_instance, 
    size_t      capacity
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
/// The capacity of a ring buffer correspond to the number of bytes its internal
/// memory has been allocated for.
/// This is always a multiple of the bucket size set at initialization.
///
/// This capacity can be adjusted using \ref bj_rbuffer_reserve.
///
/// \retval 0 if `rbuffer` is null.
///
/// \see bj_rbuffer_reserve
BANJO_EXPORT size_t bj_rbuffer_capacity(
    const bj_rbuffer* p_rbuffer
);

////////////////////////////////////////////////////////////////////////////////
/// Reserves memory for at least `capacity` bytes in the rbuffer.
///
/// \param rbuffer    The rbuffer object to reserve memory for.
/// \param capacity   Number of bytes to reserve space for.
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
/// \note When called on a null object, this function does nothing.
BANJO_EXPORT size_t bj_rbuffer_reserve(
    bj_rbuffer* r_buffer,
    size_t      capacity
);


////////////////////////////////////////////////////////////////////////////////
/// Get the number of bytes already pushed in to the buffer and ready for 
/// pop()
///
/// \param p_rbuffer    The rbuffer object to reserve memory for.
///
/// \return A number of bytes.
///
/// \par Behaviour
///
/// The returned value can be considered as the maximum number to use with
/// \ref bj_buffer_available at the moment of calling this function.
///
/// \note When called on a null object, this function does nothing.
BANJO_EXPORT size_t bj_rbuffer_ready(
    const bj_rbuffer* p_rbuffer
);

////////////////////////////////////////////////////////////////////////////////
/// Get the maximum number of bytes that can be pushed into the buffer at the
/// moment of calling this function.
///
/// \param p_rbuffer    The rbuffer object to reserve memory for.
///
/// \return A number of bytes.
///
/// \par Behaviour
///
/// Pushing more that the value returned would result in a buffer overrun.
///
/// \note When called on a null object, this function does nothing.
BANJO_EXPORT size_t bj_rbuffer_available(
    const bj_rbuffer* p_rbuffer
);

////////////////////////////////////////////////////////////////////////////////
/// Push `n` bytes into the buffer, copying the same amount of bytes from
/// `p_data`.
///
/// \param p_rbuffer The rbuffer object to reserve memory for.
/// \param p_data    A source buffer
/// \param n         Number of bytes to push and copy from `p_data`.
///
/// \return `BJ_FALSE` in case of overrun, `BJ_TRUE` otherwise.
///
/// \par Behaviour
///
/// Pushing more that the available bytes will result in an overrun.
/// In this case, the entire operation is dropped and the buffer is left unchanged.
///
/// `p_data` can be _0_.
/// In this case, the ring buffer is updated correctly but the content is left
/// uninitialized.
///
/// \note When called on a null object, this function does nothing.
BANJO_EXPORT bj_bool bj_rbuffer_push(
    bj_rbuffer* p_rbuffer,
    void*       p_data,
    size_t      n
);

////////////////////////////////////////////////////////////////////////////////
/// Takes `n` bytes from the buffer, copying the same amount of bytes into
/// `p_data`.
///
/// \param p_rbuffer The rbuffer object to reserve memory for.
/// \param p_data    A destination buffer
/// \param n         Number of bytes to pull and copy into `p_data`.
///
/// \return `BJ_FALSE` in case of overrun, `BJ_TRUE` otherwise.
///
/// \par Behaviour
///
/// Trying to get more that the available bytes will result in an underrun.
/// In this case, the entire operation is dropped and the object is left unchanged.
///
/// `p_data` an be _0_.
/// In this case, the ring buffer is updated correctly, but the content is not
/// copied.
///
/// \note When called on a null object, this function does nothing.
BANJO_EXPORT bj_bool bj_rbuffer_pop(
    bj_rbuffer* p_rbuffer,
    void*       p_data,
    size_t      n
);

BANJO_EXPORT void bj_rbuffer_debug(
    const bj_rbuffer* p_rbuffer,
    const char* desc
);




/// \} // End of rbuffer group
