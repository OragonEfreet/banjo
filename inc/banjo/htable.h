////////////////////////////////////////////////////////////////////////////////
/// \file
/// Header file for \ref htable container type.

////////////////////////////////////////////////////////////////////////////////
/// \defgroup htable Hash Table
/// \ingroup containers
/// API related to the \ref bj_htable object
///
/// \ref bj_htable is an associative container that maps a _key_ to a _value_.
/// The elements are stored in an array of linked lists called _buckets_.
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

/// \brief The internal data structure for the \ref bj_htable type.
struct bj_htable_t {
    usize             bytes_value;  ///< Size in bytes of each item in the table.
    usize             bytes_key;    ///< Size in bytes of each key.
    bool              weak_owning;  ///< _true_ is the table owns the inserted memory.
    bj_hash_fn        fn_hash;      ///< Hash function used for keys
    bj_array          buckets;      ///< Internal data buffer
    usize             bytes_entry;  ///< Size of an element (key+value+metadata)
};

////////////////////////////////////////////////////////////////////////////////
/// Initializes a new \ref bj_htable.
///
/// \param p_table      The object to initialize
/// \param bytes_value  The size in bytes of a value
/// \param bytes_key    The size in bytes of a key
///
/// \return `p_table`
///
/// \par Memory Management
///
/// The table pointed to by the returned handle **must** be released after use
/// by calling \ref bj_htable_reset.
///
/// \see bj_array_del
BANJO_EXPORT bj_htable* bj_htable_init_default(
    bj_htable*                 p_table,
    usize bytes_key,
    usize bytes_value
);

////////////////////////////////////////////////////////////////////////////////
/// Initializes a new \ref bj_htable.
///
/// \param p_table      The object to initialize
/// \param K            Key type
/// \param V            Value type
///
/// \return `p_table`
///
/// This function expands to a call to \ref bj_htable_init_default,
/// using `K` anv `V` for respectively `bytes_key` and `bytes_value`.
///
/// \par Memory Management
///
/// The table pointed to by the returned handle **must** be released after use
/// by calling \ref bj_htable_reset.
///
/// \see bj_array_del
#define bj_htable_init_default_t(p_table, K, V) bj_htable_init_default(p_table, sizeof(K), sizeof(V))

////////////////////////////////////////////////////////////////////////////////
/// Reset a hash table to an invalid state
///
/// \param p_table The Hash Table object.
///
/// \return `p_table`
BANJO_EXPORT bj_htable* bj_htable_reset(
    bj_htable* p_table
);

////////////////////////////////////////////////////////////////////////////////
/// Clear all data in the hash table.
///
/// \param table The table object.
///
/// If the table is already empty, this function does nothing.
BANJO_EXPORT void bj_htable_clear(
    bj_htable* table
);

////////////////////////////////////////////////////////////////////////////////
/// Insert a value in the hash table.
///
/// \param table   The table object.
/// \param p_key   The key used to identify the value.
/// \param p_value A pointer to the value to insert.
///
/// \return A pionter to the inserted value.
///
/// The the key already exists, the associated value is modified to stored
/// `value` instead.
/// Otherwise, a new entry is created and the `key`/`value` couple is stored.
///
/// \par Weak / Strong ownership
///
/// When `p_info->weak_owning` is set to _true_, the container does not allocate any
/// new memory for storing the actual data.
/// Memory allocation is still needed for the container structure itself.
/// In this case, the caller is responsible for the lifetime of the inserted objects (key and value).
///
/// When set to _false_, the inserted data is copied into the container's internal
/// memory using \ref bj_memcpy.
BANJO_EXPORT void* bj_htable_set(
    bj_htable* table,
    void*       p_key,
    void*       p_value
);

////////////////////////////////////////////////////////////////////////////////
/// Return the value associated to `p_key`, if existing.
///
/// \param table      The table object
/// \param p_key      The search key
/// \param p_default  The value returned if the key if not found
///
/// \return A pointer to the found value or `p_default`.
///
/// If `table` holds no value at `p_key`, the function returns `p_default`.
///
BANJO_EXPORT void* bj_htable_get(
    const bj_htable* table,
    const void*       p_key,
    void*             p_default
);

////////////////////////////////////////////////////////////////////////////////
/// Return the number of elements in the table
///
/// \param table      The table object
///
/// \return The number of elements in the table
///
BANJO_EXPORT usize bj_htable_len(
    const bj_htable* table
);

/// \} End of htable group

