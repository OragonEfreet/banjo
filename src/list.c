#include <banjo/error.h>
#include <banjo/list.h>
#include <banjo/memory.h>

bj_list* bj_list_init_default_with_size(
    bj_list* p_instance,
    usize    bytes_payload
) {
    bj_memset(p_instance, 0, sizeof(bj_list));
    if(bytes_payload > 0) {
        p_instance->bytes_payload = bytes_payload;
        p_instance->weak_owning   = false;
        p_instance->bytes_entry   = p_instance->weak_owning ? sizeof(void*) * 2 : bytes_payload + sizeof(void*);
        p_instance->p_head        = 0;
    }
    return p_instance;
}

bj_list* bj_list_reset(
    bj_list* list
) {
    bj_assert(list != 0);
    bj_list_clear(list);
    bj_memset(list, 0, sizeof(bj_list));
    return list; 
}

void bj_list_clear(
    bj_list* list
) {
    bj_assert(list != 0);

    void** p_next_block = list->p_head;
    while(p_next_block != 0) {
        void* to_free = p_next_block;
        p_next_block = *p_next_block;
        bj_free(to_free);
    }
    list->p_head = 0;
}


usize bj_list_len(
    bj_list* list
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
    bj_list* list,
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
    u8* p_block = bj_malloc(list->bytes_entry);
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
            bj_memcpy(value, p_data, list->bytes_payload); 
        }
    }

    return value;
}

void* bj_list_prepend(
    bj_list* list,
    void* p_data
) {
    return bj_list_insert(list, 0, p_data);
}

void* bj_list_at(
    bj_list* list,
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
    bj_list* list
){
    return bj_list_at(list, 0);
}

bj_list_iterator* bj_list_iterator_new(
    bj_list* list
) {
    bj_assert(list);
    bj_list_iterator* it = bj_malloc(sizeof(struct bj_list_iterator_t));
    bj_list_iterator_init(list, it);
    return it;
}

void bj_list_iterator_del(
    bj_list_iterator* iterator
) {
    bj_list_iterator_reset(iterator);
    bj_free(iterator);
}

void bj_list_iterator_init(bj_list* list, bj_list_iterator* iterator) {
    bj_assert(iterator);
    iterator->list        = list;
    iterator->p_current   = &list->p_head;
}

void bj_list_iterator_reset(bj_list_iterator* iterator) {
    bj_assert(iterator);
    iterator->list      = 0;
    iterator->p_current = 0;
}

bool bj_list_iterator_has_next(
    bj_list_iterator* iterator
) {
    bj_assert(iterator);
    return *(iterator->p_current) != 0;
}

void* bj_list_iterator_next(
    bj_list_iterator* iterator
) {
    bj_assert(iterator);
    iterator->p_current = *iterator->p_current;
    void* p_value = ((byte*)iterator->p_current)+sizeof(void*);
    if(iterator->list->weak_owning) {
        return *(void**)p_value;
    }
    return p_value;
}
