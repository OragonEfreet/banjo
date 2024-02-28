#include "banjo/list.h"
#include "banjo/memory.h"
#include <banjo/error.h>

void bj_list_init(
    const BjListInfo*              p_info,
    const BjAllocationCallbacks*   p_allocator,
    BjList*                p_instance
) {
    bj_assert(p_info != 0);

    p_instance->p_allocator = p_allocator;
    p_instance->value_size  = p_info->value_size;
    p_instance->weak_owning = p_info->weak_owning;
    p_instance->entry_size  = p_instance->weak_owning ? sizeof(void*) * 2 : p_info->value_size + sizeof(void*);
    p_instance->p_head = 0;
}

void bj_list_reset(
    BjList* list
) {
    bj_assert(list != 0);
    bj_list_clear(list);
}

BjList* bj_list_new(
    const BjListInfo*     p_info,
    const BjAllocationCallbacks* p_allocator
) {
    bj_assert(p_info != 0);
    BjList* list = bj_malloc(sizeof(struct BjList_T), p_allocator);
    bj_list_init(p_info, p_allocator, list);
    return list;
}

void bj_list_clear(
    BjList* list
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

void bj_list_del(
    BjList* list
) {
    bj_assert(list != 0);
    bj_list_reset(list);
    bj_free(list, list->p_allocator);
}

usize bj_list_count(
    BjList* list
) {
    usize result = 0;

    void** p_next_block = list->p_head;
    while(p_next_block != 0) {
        p_next_block = *p_next_block;
        ++result;
    }

    return result;
}

void* bj_list_insert(
    BjList* list,
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

void* bj_list_prepend(
    BjList* list,
    void* p_data
) {
    return bj_list_insert(list, 0, p_data);
}

void* bj_list_value(
    BjList* list,
    usize index
) {
    void** p_next_block = list->p_head;
    while(p_next_block != 0) {
        if(index-- == 0) {
            void* p_value = ((byte*)p_next_block)+sizeof(void*);
            if(list->weak_owning) {
                return *(void**)p_value;
            }
            return p_value;
        }
        p_next_block = *p_next_block;
    }
    return 0;
}

void* bj_list_head(
    BjList* list
){
    return bj_list_value(list, 0);
}

BANJO_EXPORT BjListIterator* bj_list_iterator_new(
    BjList* list
) {
    bj_assert(list);
    BjListIterator* it = bj_malloc(sizeof(struct BjListIterator_T), list->p_allocator);
    bj_list_iterator_init(list, it);
    return it;
}

BANJO_EXPORT void bj_list_iterator_del(
    BjListIterator* iterator
) {
    const BjAllocationCallbacks* allocator = iterator->list->p_allocator;
    bj_list_iterator_reset(iterator);
    bj_free(iterator, allocator);
}

void bj_list_iterator_init(BjList* list, BjListIterator* iterator) {
    bj_assert(iterator);
    iterator->list        = list;
    iterator->p_current   = &list->p_head;
}

void bj_list_iterator_reset(BjListIterator* iterator) {
    bj_assert(iterator);
    iterator->list      = 0;
    iterator->p_current = 0;
}

BANJO_EXPORT bool bj_list_iterator_has_next(
    BjListIterator* iterator
) {
    bj_assert(iterator);
    return *(iterator->p_current) != 0;
}

BANJO_EXPORT void* bj_list_iterator_next(
    BjListIterator* iterator
) {
    bj_assert(iterator);
    iterator->p_current = *iterator->p_current;
    void* p_value = ((byte*)iterator->p_current)+sizeof(void*);
    if(iterator->list->weak_owning) {
        return *(void**)p_value;
    }
    return p_value;
}
