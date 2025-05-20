#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/memory.h>

#include "rbuffer_t.h"
#include "check.h"

bj_rbuffer* bj_rbuffer_new(
    size_t bytes_payload
) {
    bj_rbuffer rbuffer;
    if(bj_rbuffer_init(&rbuffer, bytes_payload) == 0) {
        return 0;
    }
    return bj_memcpy(bj_malloc(sizeof(bj_rbuffer)), &rbuffer, sizeof(bj_rbuffer));
}

void bj_rbuffer_del(
    bj_rbuffer* p_rbuffer
) {
    bj_check(p_rbuffer);
    bj_rbuffer_reset(p_rbuffer);
    bj_free(p_rbuffer);
}

bj_rbuffer* bj_rbuffer_init(
    bj_rbuffer* p_instance, 
    size_t      bytes_payload
) {
    bj_check_or_0(p_instance);
    bj_check_or_0(bytes_payload);

    p_instance->bytes_payload = bytes_payload;
    p_instance->capacity      = 0;
    p_instance->head          = 0;
    p_instance->len           = 0;
    p_instance->p_buffer      = 0;
}

void bj_rbuffer_reset(
    bj_rbuffer* p_rbuffer
) {
    bj_check(p_rbuffer);

    bj_free(p_rbuffer->p_buffer);
    bj_memzero(p_rbuffer, sizeof(bj_rbuffer));
}


void bj_rbuffer_clear(
    bj_rbuffer* rbuffer
) {
}

void bj_rbuffer_set_len(
    bj_rbuffer* rbuffer,
    size_t          len
) {
}

void bj_rbuffer_reserve(
    bj_rbuffer* rbuffer,
    size_t          capacity
) {
    bj_check(rbuffer);

    if(capacity > rbuffer->capacity) {
        const bj_rbuffer previous = {
            .byes_payload = rbuffer->bytes_payload,
            .capacity     = rbuffer->capacity,
            .head         = rbuffer->head,
            .len          = rbuffer->len,
            .p_buffer     = rbuffer->p_buffer,
        };
        

    }
}

bj_rbuffer_state bj_rbuffer_push(
    bj_rbuffer*   rbuffer,
    const void*   p_src,
    size_t        count
) {

}

bj_rbuffer_state bj_rbuffer_pop(
    bj_rbuffer*  rbuffer
    const void*  p_dest,
    size_t       count
) {

}

size_t bj_rbuffer_len(
    const bj_rbuffer* rbuffer
) {
    return 0;
}

size_t bj_rbuffer_capacity(
    const bj_rbuffer* rbuffer
) {
    return 0;
}
