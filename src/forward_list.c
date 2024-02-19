#include "banjo/forward_list.h"
#include "banjo/memory.h"
#include <banjo/error.h>
#include <data/forward_list.h>

#include <string.h>

/* #include <string.h> */

static const usize p_entry_ptr_size = sizeof(BjForwardListEntry);

#define VALUE_EMBED(list) ((list->weak_owning == false) && (list->value_size <= p_entry_ptr_size))

void bj_forward_list_init(
    const BjForwardListInfo*     p_info,
    const BjAllocationCallbacks* p_allocator,
    BjForwardList                p_instance
) {
    bj_assert(p_info != 0);

    p_instance->p_allocator = p_allocator;
    p_instance->value_size  = p_info->value_size;
    p_instance->weak_owning = p_info->weak_owning;
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

    BjForwardListEntry* entry = list->p_head;
    while(entry != 0) {
        BjForwardListEntry* next = entry->p_next;
        if(VALUE_EMBED(list) == 0 && list->weak_owning == false) {
            bj_free(entry->value, list->p_allocator);
        }
        bj_free(entry, list->p_allocator);
        entry = next;
    }
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
    if(list != 0) {
        BjForwardListEntry* entry = list->p_head;
        while(entry != 0) {
            result += 1;
            entry = entry->p_next;
        }
    }
    return result;
}

void bj_forward_list_insert(
    BjForwardList list,
    usize index,
    void* p_data
) {
    bj_assert(list != 0);

    BjForwardListEntry** source_ptr = &list->p_head;
    for(usize i = 0 ; (i < index) && ((*source_ptr) != 0) ; ++i) {
        source_ptr = &(*source_ptr)->p_next;
    }
    BjForwardListEntry* next_entry = *source_ptr ? (*source_ptr) : 0;

    BjForwardListEntry* new_entry = bj_malloc(sizeof(BjForwardListEntry), list->p_allocator);
    new_entry->p_next = next_entry;
    if(list->weak_owning) {
        new_entry->value = p_data;
    } else {
        if(VALUE_EMBED(list)) {
            bj_memcpy(&new_entry->value, p_data, list->value_size);
        } else {
            new_entry->value = bj_malloc(list->value_size, list->p_allocator);
            bj_memcpy(new_entry->value, p_data, list->value_size);
        }
    }

    *source_ptr = new_entry;
}

void bj_forward_list_prepend(
    BjForwardList list,
    void* p_data
) {
    bj_forward_list_insert(list, 0, p_data);
}

void* bj_forward_list_value(
    BjForwardList list,
    usize index
) {
    BjForwardListEntry* entry = list->p_head;
    usize current_index = 0;
    while(entry != 0) {
        if(current_index++ == index) {
            return VALUE_EMBED(list) ? &entry->value : entry->value;
        }
        entry = entry->p_next;
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
    if(VALUE_EMBED(iterator->list)) {
        return &(iterator->p_current->value);
    }
    return iterator->p_current->value;
}

BANJO_EXPORT bool bj_forward_list_iterator_next(
    BjForwardListIterator iterator
) {
    bj_assert(iterator);
    if(iterator->p_current == 0) {
        return false;
    }
    iterator->p_current = iterator->p_current->p_next;
    return iterator->p_current != 0;
}
