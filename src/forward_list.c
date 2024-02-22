#include "banjo/forward_list.h"
#include "banjo/memory.h"
#include <banjo/error.h>
#include <data/forward_list.h>

void bj_forward_list_init(
    const BjForwardListInfo*     p_info,
    const BjAllocationCallbacks* p_allocator,
    BjForwardList                p_instance
) {
    bj_assert(p_info != 0);

    p_instance->p_allocator = p_allocator;
    p_instance->value_size  = p_info->value_size;
    p_instance->weak_owning = p_info->weak_owning;
    p_instance->entry_size  = p_instance->weak_owning ? sizeof(void*) * 2 : p_info->value_size + sizeof(void*);
    p_instance->p_head = 0;
}

void bj_forward_list_reset(
    BjForwardList list
) {
    bj_assert(list != 0);
    bj_forward_list_clear(list);
}

BjForwardList bj_forward_list_create(
    const BjForwardListInfo*     p_info,
    const BjAllocationCallbacks* p_allocator
) {
    bj_assert(p_info != 0);
    BjForwardList list = bj_new_struct(BjForwardList, p_allocator);
    bj_forward_list_init(p_info, p_allocator, list);
    return list;
}

void bj_forward_list_clear(
    BjForwardList list
) {
    bj_assert(list != 0);

    void** p_next_block = list->p_head;
    while(p_next_block != 0) {
        void* to_free = p_next_block;
        p_next_block = *p_next_block;
        bj_free(to_free, list->p_allocator);
    }
    list->p_head = 0;
}

void bj_forward_list_destroy(
    BjForwardList list
) {
    bj_assert(list != 0);
    bj_forward_list_reset(list);
    bj_free(list, list->p_allocator);
}

usize bj_forward_list_count(
    BjForwardList list
) {
    usize result = 0;

    void** p_next_block = list->p_head;
    while(p_next_block != 0) {
        p_next_block = *p_next_block;
        ++result;
    }

    return result;
}

void* bj_forward_list_insert(
    BjForwardList list,
    usize index,
    void* p_data
) {
    bj_assert(list != 0);

    void* p_previous_block = &list->p_head;
    void** p_next_block    = list->p_head;
    for(usize i = 0 ; i < index && (p_next_block != 0) ; ++i) {
        p_previous_block = p_next_block;
        p_next_block     = *p_next_block;
    }

    // p_next_block contains the address of the block that will be on the right
    // of the new element.
    // p_previous_block gets the address of the memory holding the newly current element.

    // We create the new block, its first bytes must contain the address of the next block
    u8* p_block = bj_malloc(list->entry_size, list->p_allocator);
    bj_memcpy(p_block, &p_next_block, sizeof(void*));
    // While in previous block, we put the adress of the current block
    bj_memcpy(p_previous_block, &p_block, sizeof(void*)); 

    void* value = p_block + sizeof(void*);
    if(list->weak_owning) {
        // Weak owning, copy pointer value in buffer value
        bj_memcpy(value, &p_data, sizeof(void*)); 
    } else {
        if(p_data != 0) {
            // Strong owning, copy pointed value into buffer value
            bj_memcpy(value, p_data, list->value_size); 
        }
    }

    return value;
}

void* bj_forward_list_prepend(
    BjForwardList list,
    void* p_data
) {
    return bj_forward_list_insert(list, 0, p_data);
}

void* bj_forward_list_value(
    BjForwardList list,
    usize index
) {
    void** p_next_block = list->p_head;
    while(p_next_block != 0) {
        if(index == 0) {
            return ((byte*)p_next_block)+sizeof(void*);
        }
        p_next_block = *p_next_block;
    }
    return 0;
}

void* bj_forward_list_head(
    BjForwardList list
){
    return bj_forward_list_value(list, 0);
}

void bj_forward_list_iterator_init(const BjForwardList list, BjForwardListIterator iterator) {
    bj_assert(iterator);
    iterator->list        = list;
    iterator->p_current   = list->p_head;
}

void bj_forward_list_iterator_reset(BjForwardListIterator iterator) {
    bj_assert(iterator);
    iterator->list      = 0;
    iterator->p_current = 0;
}

BANJO_EXPORT BjForwardListIterator bj_forward_list_iterator_create(
    const BjForwardList list
) {
    bj_assert(list);
    BjForwardListIterator it = bj_new_struct(BjForwardListIterator, list->p_allocator);
    bj_forward_list_iterator_init(list, it);
    return it;
}

BANJO_EXPORT void bj_forward_list_iterator_destroy(
    BjForwardListIterator iterator
) {
    const BjAllocationCallbacks* allocator = iterator->list->p_allocator;
    bj_forward_list_iterator_reset(iterator);
    bj_free(iterator, allocator);
}

BANJO_EXPORT void* bj_forward_list_iterator_value(
    BjForwardListIterator iterator
) {
    bj_assert(iterator);
    // TODO
    return 0;
    /* return iterator->p_current->value; */
}

BANJO_EXPORT bool bj_forward_list_iterator_next(
    BjForwardListIterator iterator
) {
    bj_assert(iterator);
    if(iterator->p_current == 0) {
        return false;
    }
    // TODO
    /* iterator->p_current = iterator->p_current->p_next; */
    return iterator->p_current != 0;
}
