#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/memory.h>

#include "rbuffer_t.h"
#include "check.h"

#include <assert.h>
#include <stdio.h>

#define WRAP(i, cap) ((i) % cap)
#define INC(i, n, cap) WRAP((i + n), cap)

bj_rbuffer* bj_rbuffer_new(
    size_t capacity
) {
    bj_rbuffer rbuffer;
    if(bj_rbuffer_init(&rbuffer, capacity) == 0) {
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
    size_t      capacity
) {
    bj_check_or_0(p_instance);
    bj_memzero(p_instance, sizeof(bj_rbuffer));
    bj_rbuffer_reserve(p_instance, capacity);
    return p_instance;
}

void bj_rbuffer_reset(
    bj_rbuffer* p_rbuffer
) {
    bj_check(p_rbuffer);
    bj_memzero(p_rbuffer, sizeof(bj_rbuffer));
}

size_t bj_rbuffer_reserve(
    bj_rbuffer* p_rbuffer,
    size_t      capacity
) {
    bj_check_or_0(p_rbuffer);

    if(capacity > p_rbuffer->capacity) {
        p_rbuffer->capacity = capacity;
    }

    // TODO we're just imagining capacity here

    return p_rbuffer->capacity;
}

bj_bool bj_rbuffer_full(
    const bj_rbuffer* p_rbuffer
) {
    return bj_rbuffer_available(p_rbuffer) == 0;
}

bj_bool bj_rbuffer_empty(
    const bj_rbuffer* p_rbuffer
) {
    return bj_rbuffer_used(p_rbuffer) == 0;
}

size_t bj_rbuffer_used(
    const bj_rbuffer* p_rbuffer
) {
    bj_check_or_0(p_rbuffer);
    bj_check_or_0(p_rbuffer->capacity > 1);
    size_t w   = p_rbuffer->write;
    size_t r   = p_rbuffer->read;
    size_t cap = p_rbuffer->capacity;

    if (w >= r)
        return w - r;
    return cap - r + w;
}

size_t bj_rbuffer_available(
    const bj_rbuffer* p_rbuffer
) {
    bj_check_or_0(p_rbuffer);
    bj_check_or_0(p_rbuffer->capacity > 1);
    return p_rbuffer->capacity - bj_rbuffer_used(p_rbuffer) - 1;
}

void bj_rbuffer_write_overrun(
    bj_rbuffer* p_rbuffer,
    void* p_data,
    size_t n
) {
    bj_check(p_rbuffer);
    bj_check(n);
    (void)p_data; // Ignored for now

    const size_t cap = p_rbuffer->capacity;
    const size_t available = bj_rbuffer_available(p_rbuffer);

    // If we overrun, advance read pointer to discard oldest data
    if (n > available) {
        size_t overrun = n - available;
        p_rbuffer->read = INC(p_rbuffer->read, overrun, cap);
    }

    p_rbuffer->write = INC(p_rbuffer->write, n, cap);
}


