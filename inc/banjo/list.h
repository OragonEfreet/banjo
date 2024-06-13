////////////////////////////////////////////////////////////////////////////////
/// \file
/// Header file for \ref list container type.

////////////////////////////////////////////////////////////////////////////////
/// \defgroup list List
/// \ingroup containers
/// API related to the \ref bj_list object
///
/// \ref bj_list is a container that supports constant time insertion and removal
/// from anywhere in the container.
/// It is implemented as a simply linked list.
/// \{
#pragma once

#include <banjo/api.h>
#include <banjo/memory.h>

////////////////////////////////////////////////////////////////////////////////
/// Typedef for the BList_t struct
typedef struct bj_list_t bj_list;

/// \brief The internal data structure for the \ref bj_list type.
struct bj_list_t {
    usize  bytes_payload;  ///< Size in bytes of each item in the list.
    bool   weak_owning;    ///< If _true_, the container doesn't own the stored elements.
    usize  bytes_entry;    ///< Size of each entry in the list (data+metadata).
    void*  p_head;         ///< Pointer to the first entry in the list.
};

////////////////////////////////////////////////////////////////////////////////
/// Initialize a new \ref bj_list.
///
/// \param p_list         The list object.
/// \param bytes_payload  The size in bytes of each value.
///
/// \return `p_list`
///
/// \see bj_list_del
BANJO_EXPORT bj_list* bj_list_init_default(
    bj_list* p_list,
    usize   bytes_payload
);

////////////////////////////////////////////////////////////////////////////////
/// Default initializer for \ref bj_list.
///
/// \param p_list  The list object.
/// \param T       Value type
///
/// \return `p_list`
///
/// The function expands to a call to \ref bj_list_init_default, using
/// `T` for the `bytes_payload` parameter.
///
/// \see bj_list_del
#define bj_list_init_default_t(p_list, T) bj_list_init_default(p_list, sizeof(T))

////////////////////////////////////////////////////////////////////////////////
/// Reset a given list to invalid state
///
/// \param p_list The list object
///
/// \return `p_list`
BANJO_EXPORT bj_list* bj_list_reset(
    bj_list* p_list
);

////////////////////////////////////////////////////////////////////////////////
/// Clear all data in the list.
///
/// \param list The list object.
///
/// If the list is already empty, this function does nothing.
BANJO_EXPORT void bj_list_clear(
    bj_list* list
);

////////////////////////////////////////////////////////////////////////////////
/// Returns the number of elements in the given list.
///
/// \param list The list object.
///
/// \return a integer indicating the number of elements in the list.
BANJO_EXPORT usize bj_list_len(
    bj_list* list
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
    bj_list* list,
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
    bj_list* list,
    void*           p_data
);

////////////////////////////////////////////////////////////////////////////////
//// Returns the value of the element in the given list.
///
/// \param list  The list object.
/// \param index The position of the element to get.
///
/// \return A pointer to the element.
BANJO_EXPORT void* bj_list_at(
    bj_list* list,
    usize           index
);

////////////////////////////////////////////////////////////////////////////////
//// Returns the value of the first element.
///
/// \param list  The list object.
///
/// \return A pointer to the first element.
///
/// This function effectively calls \ref bj_list_at with `index` 0.
BANJO_EXPORT void* bj_list_head(
    bj_list* list
);

////////////////////////////////////////////////////////////////////////////////
/// Typedef for the bj_list_iterator_t structure
typedef struct bj_list_iterator_t bj_list_iterator;

/// Ignore
struct bj_list_iterator_t {
    bj_list*  list;
    void**   p_current;
};


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
BANJO_EXPORT bj_list_iterator* bj_list_iterator_new(
    bj_list* list
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
    bj_list_iterator* iterator
);

////////////////////////////////////////////////////////////////////////////////
/// Initializes a list iterator
///
/// \param p_list     The list to iterate
/// \param p_iterator The iterator object
void bj_list_iterator_init(
    bj_list*         p_list,
    bj_list_iterator* p_iterator
);

////////////////////////////////////////////////////////////////////////////////
/// Reset a list iterator
///
/// \param p_iterator The iterator object
BANJO_EXPORT void bj_list_iterator_reset(
    bj_list_iterator* p_iterator
);

////////////////////////////////////////////////////////////////////////////////
/// Return `true` if the iterator has more elements.
///
/// \param iterator The iterator object
/// \return `true` if the iterator has more elements.
BANJO_EXPORT bool bj_list_iterator_has_next(
    bj_list_iterator* iterator
);

////////////////////////////////////////////////////////////////////////////////
/// Return the next element in the iteration.
///
/// \param iterator The iterator object
/// \return A pointer to the next element in the iteration or _0_ if no new element.
BANJO_EXPORT void* bj_list_iterator_next(
    bj_list_iterator* iterator
);

/// \} End of list group
