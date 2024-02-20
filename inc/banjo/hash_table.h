////////////////////////////////////////////////////////////////////////////////
/// \file
/// Header file for \ref htable container type.

////////////////////////////////////////////////////////////////////////////////
/// \defgroup htable Hash Table
/// \ingroup containers
/// API related to the \ref BjHashTable object
///
///  **Generic Name**     | hash table                 
/// ----------------------|-----------------------
///  **Opaque Type**      | yes                   
///  **Create Function**  | \ref bj_hash_table_create  
///  **Destroy Function** | \ref bj_hash_table_destroy 
///
/// \ref BjHashTable is an associative container that maps a _key_ to an _value_.
/// The elements are stored in an array of linked lists called _buckets_.
/// \{
#pragma once

#include <banjo/api.h>
#include <banjo/memory.h>

/// Handle to an hash table object.
BJ_DEFINE_HANDLE(BjHashTable);

/// Function type for hashing keys.
typedef u32 (*bjHashFunctionPtr)(
    const void* p_data, usize size
);

/// Info structure used to create a new \ref BjHashTable.
typedef struct BjHashTableInfo {
    usize                  value_size;  ///< Size in bytes of each item in the table.
    usize                  key_size;    ///< Size in bytes of each key.
    bool                   weak_owning; ///< _true_ is the table owns the inserted memory.
    bjHashFunctionPtr      fn_hash;     ///< Hash function used for keys.
} BjHashTableInfo;

////////////////////////////////////////////////////////////////////////////////
/// Create a new \ref BjHashTable.
///
/// \param p_info       Creation options.
/// \param p_allocator  Allocation callbacks, can be _0_.
///
/// \return A handle to a new table.
///
/// \par Memory Management
///
/// The table pointed to by the returned handle **must** be released after use
/// by calling \ref bj_hash_table_destroy.
///
/// When `p_allocator` is not _0_, the given allocator is used though the entire
/// life of the table object.
/// The content of `p_allocator` being copied in memory, the caller doesn't have
/// to retain the pointer after creating the table.
///
/// \see bj_array_destroy
BANJO_EXPORT BjHashTable bj_hash_table_create(
    const BjHashTableInfo*       p_info,
    const BjAllocationCallbacks* p_allocator
);

////////////////////////////////////////////////////////////////////////////////
/// Destroy a table previously created by \ref bj_hash_table_create.
///
/// \param table The instance to destroy.
///
/// \par Memory Management
///
/// The memory allocated for `table` will be freed using the allocator callbacks
/// set by \ref bj_hash_table_create.
///
/// \see bj_hash_table_create
BANJO_EXPORT void bj_hash_table_destroy(
    BjHashTable table
);

////////////////////////////////////////////////////////////////////////////////
/// Clear all data in the hash table.
///
/// \param table The table object.
///
/// If the table is already empty, this function does nothing.
BANJO_EXPORT void bj_hash_table_clear(
    BjHashTable table
);

////////////////////////////////////////////////////////////////////////////////
/// Insert a value in the hash table.
///
/// \param table The table object.
/// \param key   The key used to identify the value.
/// \param value A pointer to the value to insert.
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
BANJO_EXPORT void bj_hash_table_set(
    BjHashTable table,
    void*       key,
    void*       value
);

/// \} End of htable group

