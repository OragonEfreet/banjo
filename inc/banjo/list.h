////////////////////////////////////////////////////////////////////////////////
/// \file
/// Header file for \ref list container type.

////////////////////////////////////////////////////////////////////////////////
/// \defgroup list List
/// \ingroup containers
/// API related to the \ref BjList object
///
///  **Generic Name**     | list                
/// ----------------------|-----------------------
///  **Opaque Type**      | yes                   
///  **Create Function**  | \ref bj_list_new  
///  **Destroy Function** | \ref bj_list_del 
///
/// \ref BjList is a container that supports constant time insertion and removal
/// from anywhere in the container.
/// It is implemented as a simply linked list.
/// \{
#pragma once

#include <banjo/api.h>
#include <banjo/memory.h>


////////////////////////////////////////////////////////////////////////////////
/// Typedef for the BList_T struct
typedef struct BjList_T BjList;

#ifdef BJ_NO_OPAQUE
struct BjList_T {
    const BjAllocationCallbacks* p_allocator;
    usize                        value_size;
    usize                        entry_size;
    bool                         weak_owning;
    void*                        p_head;
};
#endif

////////////////////////////////////////////////////////////////////////////////
/// Info structure used to create a new \ref BjList.
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
typedef struct BjListInfo {
    usize  value_size;  ///< Size in bytes of each item in the list.
    bool   weak_owning; ///< If _true_, the container doesn't own the stored elements.
} BjListInfo;

////////////////////////////////////////////////////////////////////////////////
/// Create a new \ref BjList.
///
/// \param p_info       Creation options.
/// \param p_allocator  Allocation callbacks, can be _0_.
///
/// \return A handle to a new list.
///
/// \par Memory Management
///
/// The list pointed to by the returned handle **must** be released after use
/// by calling \ref bj_list_del.
///
/// When `p_allocator` is not _0_, the given allocator is used though the entire
/// life of the list object.
/// The content of `p_allocator` being copied in memory, the caller doesn't have
/// to retain the pointer after creating the list.
///
/// \par Weak / Strong ownership
///
/// \see bj_list_del
BANJO_EXPORT BjList* bj_list_new(
    const BjListInfo*     p_info,
    const BjAllocationCallbacks* p_allocator
);

////////////////////////////////////////////////////////////////////////////////
/// Destroy a list previously created by \ref bj_list_new.
///
/// \param list The instance to destroy.
///
/// \par Memory Management
///
/// The memory allocated for `list` will be freed using the allocator callbacks
/// set by \ref bj_list_new.
///
/// \see bj_list_new
BANJO_EXPORT void bj_list_del(
    BjList* list
);

////////////////////////////////////////////////////////////////////////////////
/// Initialize a new \ref BjList.
///
/// \param p_info       Creation options.
/// \param p_allocator  Allocation callbacks, can be _0_.
/// \param p_list       The list object.
///
/// \par Memory Management
///
/// When `p_allocator` is not _0_, the given allocator is used though the entire
/// life of the list object.
/// The content of `p_allocator` being copied in memory, the caller doesn't have
/// to retain the pointer after creating the list.
///
/// \see bj_list_del
BANJO_EXPORT void bj_list_init(
    const BjListInfo*            p_info,
    const BjAllocationCallbacks* p_allocator,
    BjList*                      p_list
);

////////////////////////////////////////////////////////////////////////////////
/// Reset a given list to invalid state
///
/// \param p_list The list object
BANJO_EXPORT void bj_list_reset(
    BjList* p_list
);

////////////////////////////////////////////////////////////////////////////////
/// Clear all data in the list.
///
/// \param list The list object.
///
/// If the list is already empty, this function does nothing.
BANJO_EXPORT void bj_list_clear(
    BjList* list
);

////////////////////////////////////////////////////////////////////////////////
/// Returns the number of elements in the given list.
///
/// \param list The list object.
///
/// \return a integer indicating the number of elements in the list.
BANJO_EXPORT usize bj_list_count(
    BjList* list
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
BANJO_EXPORT void* bj_list_insert(
    BjList* list,
    usize           index,
    void*           p_data
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
BANJO_EXPORT void* bj_list_prepend(
    BjList* list,
    void*           p_data
);

////////////////////////////////////////////////////////////////////////////////
//// Returns the value of the element in the given list.
///
/// \param list  The list object.
/// \param index The position of the element to get.
///
/// \return A pointer to the element.
BANJO_EXPORT void* bj_list_value(
    BjList* list,
    usize           index
);

////////////////////////////////////////////////////////////////////////////////
//// Returns the value of the first element.
///
/// \param list  The list object.
///
/// \return A pointer to the first element.
///
/// This function effectively calls \ref bj_list_value with `index` 0.
BANJO_EXPORT void* bj_list_head(
    BjList* list
);

////////////////////////////////////////////////////////////////////////////////
/// Typedef for the BjListIterator_T structure
typedef struct BjListIterator_T BjListIterator;

#ifdef BJ_NO_OPAQUE
struct BjListIterator_T {
    BjList*  list;
    void**   p_current;
};
#endif


////////////////////////////////////////////////////////////////////////////////
/// Create a forward-only iterator for the given list
///
/// \param list The list object.
///
/// \return A new iterator object
///
/// \par Memory Management
///
/// This function uses the allocator set in the given list for any memory-wise
/// operation performed by this iterator.
BANJO_EXPORT BjListIterator* bj_list_iterator_new(
    BjList* list
);

////////////////////////////////////////////////////////////////////////////////
/// destroy the given list iterator.
///
/// \param iterator The iterator object.
///
/// \par Memory Management
///
/// This function uses the allocator set in the given iterator to destroy the
/// iterator.
BANJO_EXPORT void bj_list_iterator_del(
    BjListIterator* iterator
);

////////////////////////////////////////////////////////////////////////////////
/// Initializes a list iterator
///
/// \param p_list     The list to iterate
/// \param p_iterator The iterator object
void bj_list_iterator_init(
    BjList*         p_list,
    BjListIterator* p_iterator
);

////////////////////////////////////////////////////////////////////////////////
/// Reset a list iterator
///
/// \param p_iterator The iterator object
BANJO_EXPORT void bj_list_iterator_reset(
    BjListIterator* p_iterator
);

////////////////////////////////////////////////////////////////////////////////
/// Return `true` if the iterator has more elements.
///
/// \param iterator The iterator object
/// \return `true` if the iterator has more elements.
BANJO_EXPORT bool bj_list_iterator_has_next(
    BjListIterator* iterator
);

////////////////////////////////////////////////////////////////////////////////
/// Return the next element in the iteration.
///
/// \param iterator The iterator object
/// \return A pointer to the next element in the iteration or _0_ if no new element.
BANJO_EXPORT void* bj_list_iterator_next(
    BjListIterator* iterator
);

/// \} End of list group
