////////////////////////////////////////////////////////////////////////////////
/// \file
/// Header file for \ref htable container type.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// \defgroup htable Hash Table
/// \ingroup algo
/// API related to the \ref bj_htable object
///
/// \brief bj_htable is an associative container that maps a key to a value.
///        The elements are stored in an array of linked lists called buckets.
///
/// \{
#pragma once

#include <banjo/api.h>
#include <banjo/array.h>
#include <banjo/memory.h>

/// Function type for hashing keys.
typedef u32 (*bj_hash_fn)(
    const void* p_data, usize size
);

////////////////////////////////////////////////////////////////////////////////
/// Typedef for the bj_htable_t struct
typedef struct bj_htable_t bj_htable;

////////////////////////////////////////////////////////////////////////////////
/// Creates a new bj_htable with the specified sizes for keys and values.
///
/// \param bytes_key   Size of each key in bytes.
/// \param bytes_value Size of each value in bytes.
/// \return A pointer to the newly created bj_htable object.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_htable* bj_htable_new(
    usize bytes_key,
    usize bytes_value
);

////////////////////////////////////////////////////////////////////////////////
/// Creates a new bj_htable with types inferred for keys and values.
///
/// \param K Type of keys.
/// \param V Type of values.
/// \return A pointer to the newly created bj_htable object.
////////////////////////////////////////////////////////////////////////////////
#define bj_htable_new_t(K, V) bj_htable_new(sizeof(K), sizeof(V))

////////////////////////////////////////////////////////////////////////////////
/// Deletes a bj_htable object and releases associated memory.
///
/// \param p_table Pointer to the bj_htable object to delete.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_htable_del(
    bj_htable* p_table
);

////////////////////////////////////////////////////////////////////////////////
/// Inserts a value into the hash table.
///
/// \param table   The table object.
/// \param p_key   The key used to identify the value.
/// \param p_value A pointer to the value to insert.
/// \return A pointer to the inserted value.
///
/// If the key already exists, the associated value is modified to store `p_value` instead.
/// Otherwise, a new entry is created and the (key, value) pair is stored.
///
/// \par Weak / Strong ownership
///
/// When `p_info->weak_owning` is set to true, the container does not allocate any
/// new memory for storing the actual data.
/// Memory allocation is still needed for the container structure itself.
/// In this case, the caller is responsible for the lifetime of the inserted objects (key and value).
///
/// When set to false, the inserted data is copied into the container's internal
/// memory using \ref bj_memcpy.
BANJO_EXPORT void* bj_htable_set(
    bj_htable* table,
    void*       p_key,
    void*       p_value
);

////////////////////////////////////////////////////////////////////////////////
/// Returns the value associated with `p_key`, if it exists.
///
/// \param table     The table object.
/// \param p_key     The search key.
/// \param p_default The value returned if the key is not found.
/// \return A pointer to the found value or `p_default`.
///
/// If `table` does not contain a value for `p_key`, the function returns `p_default`.
///
BANJO_EXPORT void* bj_htable_get(
    const bj_htable* table,
    const void*       p_key,
    void*             p_default
);

////////////////////////////////////////////////////////////////////////////////
/// Returns the number of elements in the table.
///
/// \param table The table object.
/// \return The number of elements in the table.
///
BANJO_EXPORT usize bj_htable_len(
    const bj_htable* table
);

/// \} End of htable group
