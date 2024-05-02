////////////////////////////////////////////////////////////////////////////////
/// \file
/// Header file for \ref htable container type.

////////////////////////////////////////////////////////////////////////////////
/// \defgroup htable Hash Table
/// \ingroup containers
/// API related to the \ref BjHashTable object
///
///  **Generic Name**     | array                 
/// ----------------------|-----------------------
///  Type                 | \ref BjHashTable
///  Info Type            | \ref BjHashTableInfo
///  **Alloc**            | \ref bj_hash_table_alloc 
///  **Create**           | \ref bj_hash_table_new  
///  **Delete**           | \ref bj_hash_table_del  
///  **Reset**            | \ref bj_hash_table_reset  
///
/// \ref BjHashTable is an associative container that maps a _key_ to a _value_.
/// The elements are stored in an array of linked lists called _buckets_.
/// \{
#pragma once

#include <banjo/api.h>
#include <banjo/array.h>
#include <banjo/memory.h>

/// Function type for hashing keys.
typedef u32 (*bjHashFunctionPtr)(
    const void* p_data, usize size
);

/// Info structure used to create a new \ref BjHashTable.
typedef struct BjHashTableInfo {
    usize                  bytes_value;  ///< Size in bytes of each item in the table.
    usize                  bytes_key;    ///< Size in bytes of each key.
    bool                   weak_owning;  ///< _true_ is the table owns the inserted memory.
    bjHashFunctionPtr      fn_hash;      ///< Hash function used for keys.
} BjHashTableInfo;

////////////////////////////////////////////////////////////////////////////////
/// Typedef for the BjHashTable_T struct
typedef struct BjHashTable_T BjHashTable;

#ifdef BJ_NO_OPAQUE
struct BjHashTable_T {
    BjHashTableInfo info;
    BjArray         buckets;
    usize           bytes_entry;
};
#endif

////////////////////////////////////////////////////////////////////////////////
/// Create a new \ref BjHashTable.
///
/// \param p_info       Creation options.
/// \return A handle to a new table.
///
/// \par Memory Management
///
/// The table pointed to by the returned handle **must** be released after use
/// by calling \ref bj_hash_table_del.
///
/// \see bj_array_del
BANJO_EXPORT BjHashTable* bj_hash_table_new(
    const BjHashTableInfo* p_info
);

////////////////////////////////////////////////////////////////////////////////
/// Destroy a table previously created by \ref bj_hash_table_new.
///
/// \param table The instance to destroy.
///
/// \par Memory Management
///
/// The memory allocated for `table` will be freed using the allocator callbacks
/// set by \ref bj_hash_table_new.
///
/// \see bj_hash_table_new
BANJO_EXPORT void bj_hash_table_del(
    BjHashTable* table
);

////////////////////////////////////////////////////////////////////////////////
/// Allocate a new BjHashTable object
///
/// \return An uninitialized hash table object
///
/// \par Memory Management
///
/// The hash table pointed to by the returned handle **must** be released after use
/// by calling \ref bj_free.
///
BANJO_EXPORT BjHashTable* bj_hash_table_alloc(void);

////////////////////////////////////////////////////////////////////////////////
/// Initializes a new \ref BjHashTable.
///
/// \param p_table      The object to initialize
/// \param p_info       Creation options.
///
/// \par Memory Management
///
/// The table pointed to by the returned handle **must** be released after use
/// by calling \ref bj_hash_table_del.
///
/// \see bj_array_del
BANJO_EXPORT void bj_hash_table_init(
    BjHashTable*                 p_table,
    const BjHashTableInfo*       p_info
);

////////////////////////////////////////////////////////////////////////////////
/// Reset a hash table to an invalid state
///
/// \param p_table The Hash Table object.
BANJO_EXPORT void bj_hash_table_reset(
    BjHashTable* p_table
);

////////////////////////////////////////////////////////////////////////////////
/// Clear all data in the hash table.
///
/// \param table The table object.
///
/// If the table is already empty, this function does nothing.
BANJO_EXPORT void bj_hash_table_clear(
    BjHashTable* table
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
BANJO_EXPORT void* bj_hash_table_set(
    BjHashTable* table,
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
BANJO_EXPORT void* bj_hash_table_get(
    const BjHashTable* table,
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
BANJO_EXPORT usize bj_hash_table_len(
    const BjHashTable* table
);

/// \} End of htable group

