////////////////////////////////////////////////////////////////////////////////
/// \file
/// Header file for \ref flist container type.

////////////////////////////////////////////////////////////////////////////////
/// \defgroup flist Forward List
/// \ingroup containers
/// API related to the \ref BjForwardList object
///
///  **Generic Name**     | forward list                 
/// ----------------------|-----------------------
///  **Opaque Type**      | yes                   
///  **Create Function**  | \ref bj_forward_list_create  
///  **Destroy Function** | \ref bj_forward_list_destroy 
///
/// \ref BjForwardList is a container that supports constant time insertion and removal
/// from anywhere in the container.
/// It is implemented as a simply linked list.
/// \{
#pragma once

#include <banjo/api.h>
#include <banjo/memory.h>

/// Handle to an forward list object.
BJ_DEFINE_HANDLE(BjForwardList);

/// Handle to an forward list iterator.
BJ_DEFINE_HANDLE(BjForwardListIterator);

/// Info structure used to create a new \ref BjForwardList.
typedef struct BjForwardListInfo {
    usize  value_size;  ///< Size in bytes of each item in the list.
    bool   weak_owning; ///< If _true_, the container doesn't own the stored elements.
} BjForwardListInfo;

////////////////////////////////////////////////////////////////////////////////
/// Create a new \ref BjForwardList.
///
/// \param p_info       Creation options.
/// \param p_allocator  Allocation callbacks, can be _0_.
///
/// \return A handle to a new list.
///
/// \par Memory Management
///
/// The list pointed to by the returned handle **must** be released after use
/// by calling \ref bj_forward_list_destroy.
///
/// When `p_allocator` is not _0_, the given allocator is used though the entire
/// life of the list object.
/// The content of `p_allocator` being copied in memory, the caller doesn't have
/// to retain the pointer after creating the list.
///
/// \par Weak / Strong ownership
///
/// When `p_info->weak_owning` is set to _true_, the container does not allocate any
/// new memory for storing the actual data.
/// Memory allocation is still needed for the container structure itself.
/// In this case, the caller is responsible for the lifetime of the inserted objects.
///
/// When set to _false_, the inserted data is copied into the container's internal
/// memory using \ref bj_memcpy.
///
/// \see bj_forward_list_destroy
BANJO_EXPORT BjForwardList bj_forward_list_create(
    const BjForwardListInfo*     p_info,
    const BjAllocationCallbacks* p_allocator
);

////////////////////////////////////////////////////////////////////////////////
/// Destroy a list previously created by \ref bj_forward_list_create.
///
/// \param list The instance to destroy.
///
/// \par Memory Management
///
/// The memory allocated for `list` will be freed using the allocator callbacks
/// set by \ref bj_forward_list_create.
///
/// \see bj_forward_list_create
BANJO_EXPORT void bj_forward_list_destroy(
    BjForwardList list
);

////////////////////////////////////////////////////////////////////////////////
/// Clear all data in the list.
///
/// \param list The list object.
///
/// If the list is already empty, this function does nothing.
BANJO_EXPORT void bj_forward_list_clear(
    BjForwardList list
);

////////////////////////////////////////////////////////////////////////////////
/// Returns the number of elements in the given list.
///
/// \param list The list object.
///
/// \return a integer indicating the number of elements in the list.
BANJO_EXPORT usize bj_forward_list_count(
    BjForwardList list
);

////////////////////////////////////////////////////////////////////////////////
/// Insert a new element at `pos` and return a pointer to it.
///
/// \param list   The list object.
/// \param index  The position of the new element in the list.
/// \param p_data A pointer to the memory of the data to insert.
///
/// \return A pointer to the inserted value.
///
/// The newly inserted element is located at `index`.
/// All elements previously located at any position starting from `index` have
/// their position shifted by _1_.
///
/// \par Weak/Strong ownership
///
/// In weak ownership, the return value is equal to `p_data`.
///
/// In strong ownership, the return value points to a newly allocated block.
/// In this case, if `p_data` != _0_, the allocated block is initialized with
/// the content pointed by `p_data` (using \ref bj_memcpy).
/// Otherwise, the block is left uninitialized.
BANJO_EXPORT void* bj_forward_list_insert(
    BjForwardList list,
    usize         index,
    void*         p_data
);

////////////////////////////////////////////////////////////////////////////////
/// Insert a new element at position _0_ and return a pointer to it.
///
/// \param list   The list object.
/// \param p_data A pointer to the memory of the data to insert.
///
/// \return A pointer to the inserted value.
///
/// The newly inserted element is located at index _0_.
/// All elements previously located at any position starting from `index` have
/// their position shifted by _1_.
///
/// \par Weak/Strong ownership
///
/// In weak ownership, the return value is equal to `p_data`.
///
/// In strong ownership, the return value points to a newly allocated block.
/// In this case, if `p_data` != _0_, the allocated block is initialized with
/// the content pointed by `p_data` (using \ref bj_memcpy).
/// Otherwise, the block is left uninitialized.
BANJO_EXPORT void* bj_forward_list_prepend(
    BjForwardList list,
    void*         p_data
);

////////////////////////////////////////////////////////////////////////////////
//// Returns the value of the element in the given list.
///
/// \param list  The list object.
/// \param index The position of the element to get.
///
/// \return A pointer to the element.
BANJO_EXPORT void* bj_forward_list_value(
    BjForwardList list,
    usize         index
);

////////////////////////////////////////////////////////////////////////////////
//// Returns the value of the first element.
///
/// \param list  The list object.
///
/// \return A pointer to the first element.
///
/// This function effectively calls \ref bj_forward_list_value with `index` 0.
BANJO_EXPORT void* bj_forward_list_head(
    BjForwardList list
);


/// Pouet
/// \return nothing
BANJO_EXPORT BjForwardListIterator bj_forward_list_iterator_create(
    const BjForwardList list ///< Pouet
);

/// Pouet
BANJO_EXPORT void bj_forward_list_iterator_destroy(
    BjForwardListIterator iterator ///< Pouet
);

/// Pouet
/// \return nothing
BANJO_EXPORT void* bj_forward_list_iterator_value(
    BjForwardListIterator iterator ///< Pouet
);

/// Pouet
/// \return pouet
BANJO_EXPORT bool bj_forward_list_iterator_next(
    BjForwardListIterator iterator ///< Pouet
);
/// \} End of flist group
