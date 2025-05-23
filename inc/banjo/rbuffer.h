////////////////////////////////////////////////////////////////////////////////
/// \file
/// Header file for the \ref rbuffer container type.
////////////////////////////////////////////////////////////////////////////////
/// \defgroup rbuffer Ring Buffer
/// \ingroup algo
///
/// \{
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <banjo/api.h>

////////////////////////////////////////////////////////////////////////////////
/// Forward declaration of the ring buffer structure.
typedef struct bj_rbuffer_t bj_rbuffer;

////////////////////////////////////////////////////////////////////////////////
/// Creates a new \ref bj_rbuffer with a specified capacity in bytes.
///
/// Internally uses \ref bj_rbuffer_alloc and \ref bj_rbuffer_init.
///
/// \param capacity Initial capacity of the buffer, in bytes.
///
/// \return A pointer to the newly allocated \ref bj_rbuffer object.
BANJO_EXPORT bj_rbuffer* bj_rbuffer_new(
    size_t capacity
);

////////////////////////////////////////////////////////////////////////////////
/// Deletes a \ref bj_rbuffer object and frees its memory.
///
/// \param p_rbuffer Pointer to the \ref bj_rbuffer object to delete.
BANJO_EXPORT void bj_rbuffer_del(
    bj_rbuffer* p_rbuffer
);

////////////////////////////////////////////////////////////////////////////////
/// Ensures the ring buffer has space for at least `capacity` bytes.
///
/// \param r_buffer Pointer to the buffer to reallocate.
/// \param capacity Minimum number of bytes the buffer should be able to store.
///
/// \return The actual allocated capacity.
///
/// \details
/// If `capacity` is less than or equal to the current capacity, the function 
/// does nothing. Otherwise, it reallocates internal storage. The resulting 
/// capacity may exceed `capacity` due to alignment or implementation details.
///
/// \note If `r_buffer` is NULL, the function does nothing.
BANJO_EXPORT size_t bj_rbuffer_reserve(
    bj_rbuffer* r_buffer,
    size_t      capacity
);

////////////////////////////////////////////////////////////////////////////////
/// Checks if the buffer is full (i.e., no more bytes can be written).
///
/// \param p_rbuffer Pointer to the buffer.
///
/// \return \ref BJ_TRUE if the buffer is full, \ref BJ_FALSE otherwise.
///
/// \note If `p_rbuffer` is NULL, returns \ref BJ_FALSE.
BANJO_EXPORT bj_bool bj_rbuffer_full(
    const bj_rbuffer* p_rbuffer
);

////////////////////////////////////////////////////////////////////////////////
/// Checks if the buffer is empty (i.e., no data available to read).
///
/// \param p_rbuffer Pointer to the buffer.
///
/// \return \ref BJ_TRUE if the buffer is empty, \ref BJ_FALSE otherwise.
///
/// \note If `p_rbuffer` is NULL, returns \ref BJ_TRUE.
BANJO_EXPORT bj_bool bj_rbuffer_empty(
    const bj_rbuffer* p_rbuffer
);

////////////////////////////////////////////////////////////////////////////////
/// Returns the number of bytes currently stored (readable) in the buffer.
///
/// \param p_rbuffer Pointer to the buffer.
///
/// \return The number of used bytes.
///
/// \note Returns 0 if `p_rbuffer` is NULL or has zero capacity.
BANJO_EXPORT size_t bj_rbuffer_used(
    const bj_rbuffer* p_rbuffer
);

////////////////////////////////////////////////////////////////////////////////
/// Returns the number of bytes available for writing.
///
/// \param p_rbuffer Pointer to the buffer.
///
/// \return The number of available bytes.
///
/// \note Returns 0 if `p_rbuffer` is NULL or has zero capacity.
BANJO_EXPORT size_t bj_rbuffer_available(
    const bj_rbuffer* p_rbuffer
);

////////////////////////////////////////////////////////////////////////////////
/// Writes `n` bytes to the buffer, potentially overwriting old data.
///
/// \param p_rbuffer Pointer to the buffer to write into.
/// \param p_data    Source buffer to write from (can be NULL).
/// \param n         Number of bytes to write.
///
/// \details
/// If `p_data` is NULL, memory is left uninitialized but the write index is 
/// still advanced.
///
/// If `n` exceeds available space, the oldest data is discarded (overrun) and 
/// the read index is advanced accordingly to maintain consistency.
///
/// \note If `p_rbuffer` is NULL or `n` is zero, the function has no effect.
///
/// \see bj_rbuffer_available, bj_rbuffer_used, bj_rbuffer_empty
BANJO_EXPORT void bj_rbuffer_write_overrun(
    bj_rbuffer* p_rbuffer,
    void*       p_data,
    size_t      n
);

/// \} // End of rbuffer group
