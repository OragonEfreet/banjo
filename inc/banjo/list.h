////////////////////////////////////////////////////////////////////////////////
/// \file
/// Header file for \ref list container type.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// \defgroup list List
/// \ingroup algo
/// API related to the \ref bj_list object
///
/// \brief bj_list is a container that supports constant time insertion and removal
///        from anywhere in the container.
///        It is implemented as a singly linked list.
///
/// \{
#pragma once

#include <banjo/api.h>
#include <banjo/memory.h>

////////////////////////////////////////////////////////////////////////////////
/// Typedef for the BList_t struct
typedef struct bj_list_t bj_list;

////////////////////////////////////////////////////////////////////////////////
/// Creates a new bj_list with the specified payload size.
///
/// \param bytes_payload Size of each element's payload in bytes.
/// \return A pointer to the newly created bj_list object.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_list* bj_list_new(
    size_t bytes_payload
);

////////////////////////////////////////////////////////////////////////////////
/// Creates a new bj_list with type inference for elements.
///
/// \param T Type of elements.
/// \return A pointer to the newly created bj_list object.
////////////////////////////////////////////////////////////////////////////////
#define bj_list_new_t(T) bj_list_new(sizeof(T))

////////////////////////////////////////////////////////////////////////////////
/// Deletes a bj_list object and releases associated memory.
///
/// \param p_list Pointer to the bj_list object to delete.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_list_del(
    bj_list* p_list
);

////////////////////////////////////////////////////////////////////////////////
/// Clears all data in the list.
///
/// \param list The list object.
///
/// If the list is already empty, this function does nothing.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_list_clear(
    bj_list* list
);

////////////////////////////////////////////////////////////////////////////////
/// Returns the number of elements in the given list.
///
/// \param list The list object.
/// \return An integer indicating the number of elements in the list.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT size_t bj_list_len(
    bj_list* list
);

////////////////////////////////////////////////////////////////////////////////
/// Inserts a new element at the specified index and returns a pointer to it.
///
/// \param list   The list object.
/// \param index  The position of the new element in the list.
/// \param p_data A pointer to the memory of the data to insert.
/// \return A pointer to the inserted value.
///
/// The newly inserted element is located at `index`.
/// All elements previously located at any position starting from `index` have
/// their positions shifted by 1.
///
/// \par Weak/Strong ownership
///
/// In weak ownership, the return value is equal to `p_data`.
///
/// In strong ownership, the return value points to a newly allocated block.
/// If `p_data` != 0, the allocated block is initialized with the content pointed by
/// `p_data` (using \ref bj_memcpy). Otherwise, the block is left uninitialized.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void* bj_list_insert(
    bj_list* list,
    size_t    index,
    void*    p_data
);

////////////////////////////////////////////////////////////////////////////////
/// Inserts a new element at position 0 and returns a pointer to it.
///
/// \param list   The list object.
/// \param p_data A pointer to the memory of the data to insert.
/// \return A pointer to the inserted value.
///
/// The newly inserted element is located at index 0.
/// All elements previously located at any position starting from 0 have
/// their positions shifted by 1.
///
/// \par Weak/Strong ownership
///
/// In weak ownership, the return value is equal to `p_data`.
///
/// In strong ownership, the return value points to a newly allocated block.
/// If `p_data` != 0, the allocated block is initialized with the content pointed by
/// `p_data` (using \ref bj_memcpy). Otherwise, the block is left uninitialized.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void* bj_list_prepend(
    bj_list* list,
    void*    p_data
);

////////////////////////////////////////////////////////////////////////////////
/// Returns the value of the element at the specified index.
///
/// \param list  The list object.
/// \param index The position of the element to get.
/// \return A pointer to the element.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void* bj_list_at(
    bj_list* list,
    size_t    index
);

////////////////////////////////////////////////////////////////////////////////
/// Returns the value of the first element in the list.
///
/// \param list The list object.
/// \return A pointer to the first element.
///
/// This function effectively calls \ref bj_list_at with `index` 0.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void* bj_list_head(
    bj_list* list
);

////////////////////////////////////////////////////////////////////////////////
/// Typedef for the bj_list_iterator_t structure
typedef struct bj_list_iterator_t bj_list_iterator;

/// Structure representing a list iterator
struct bj_list_iterator_t {
    bj_list* list;
    void**   p_current;
};

////////////////////////////////////////////////////////////////////////////////
/// Creates a forward-only iterator for the given list.
///
/// \param list The list object.
/// \return A new iterator object.
///
/// \par Memory Management
///
/// This function uses the allocator set in the given list for any memory operations
/// performed by this iterator.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_list_iterator* bj_list_iterator_new(
    bj_list* list
);

////////////////////////////////////////////////////////////////////////////////
/// Destroys the given list iterator.
///
/// \param iterator The iterator object.
///
/// \par Memory Management
///
/// This function uses the allocator set in the given iterator to destroy the iterator.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_list_iterator_del(
    bj_list_iterator* iterator
);

////////////////////////////////////////////////////////////////////////////////
/// Initializes a list iterator.
///
/// \param p_list     The list to iterate.
/// \param p_iterator The iterator object.
////////////////////////////////////////////////////////////////////////////////
void bj_list_iterator_init(
    bj_list*         p_list,
    bj_list_iterator* p_iterator
);

////////////////////////////////////////////////////////////////////////////////
/// Resets a list iterator.
///
/// \param p_iterator The iterator object.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_list_iterator_reset(
    bj_list_iterator* p_iterator
);

////////////////////////////////////////////////////////////////////////////////
/// Returns true if the iterator has more elements.
///
/// \param iterator The iterator object.
/// \return true if the iterator has more elements.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bool bj_list_iterator_has_next(
    bj_list_iterator* iterator
);

////////////////////////////////////////////////////////////////////////////////
/// Returns the next element in the iteration.
///
/// \param iterator The iterator object.
/// \return A pointer to the next element in the iteration or 0 if no new element.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void* bj_list_iterator_next(
    bj_list_iterator* iterator
);

/// \} End of list group
