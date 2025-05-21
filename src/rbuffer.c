#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/memory.h>

#include "rbuffer_t.h"
#include "check.h"

#include <assert.h>
#include <stdio.h>

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
    size_t      bucket_size
) {
    bj_check_or_0(p_instance);
    bj_check_or_0(bucket_size);
    bj_memzero(p_instance, sizeof(bj_rbuffer));
    bj_rbuffer_reserve(p_instance, bucket_size);
    return p_instance;
}

void bj_rbuffer_reset(
    bj_rbuffer* p_rbuffer
) {
    bj_check(p_rbuffer);

    bj_memzero(p_rbuffer, sizeof(bj_rbuffer));
}

/* static size_t bj_rbuffer_required_buckets( */
/*     const bj_rbuffer* p_rbuffer, */
/*     size_t            capacity */
/* ) { */
/*     bj_check_or_0(p_rbuffer); */
/*     bj_check_or_0(capacity); */

/*     return (capacity + p_rbuffer->bucket_size - 1) / p_rbuffer->bucket_size; */
/* } */

size_t bj_rbuffer_capacity(
    const bj_rbuffer* p_rbuffer
) {
    bj_check_or_0(p_rbuffer);
    /* return p_rbuffer->bucket_size * p_rbuffer->n_buckets; */
    return p_rbuffer->_capacity;
}


size_t bj_rbuffer_reserve(
    bj_rbuffer* p_rbuffer,
    size_t      capacity
) {
    bj_check_or_0(p_rbuffer);

    if(capacity > p_rbuffer->_capacity) {
        p_rbuffer->_capacity = capacity;
    }

    /* const size_t required_buckets = bj_rbuffer_required_buckets(p_rbuffer, capacity); */

    /* if(required_buckets > p_rbuffer->n_buckets) { */
    /*     if(p_rbuffer->buckets == 0) { */
    /*         assert(p_rbuffer->n_buckets == 0); */
    /*         p_rbuffer->buckets = bj_malloc(sizeof(bj_rbucket) * required_buckets); */
    /*     } else { */
    /*         p_rbuffer->buckets = bj_realloc(p_rbuffer->buckets, sizeof(bj_rbucket) * required_buckets); */
    /*     } */

    /*     for(size_t b = p_rbuffer->n_buckets ; b < required_buckets ; ++b) { */
    /*         p_rbuffer->buckets[b].data = bj_malloc(p_rbuffer->bucket_size); */
    /*     } */
    /*     p_rbuffer->n_buckets = required_buckets; */
    /* } */

    return bj_rbuffer_capacity(p_rbuffer);
}

BANJO_EXPORT size_t bj_rbuffer_ready(
    const bj_rbuffer* p_rbuffer
) {
    bj_check_or_0(p_rbuffer);
    return 0; // TODO
}

size_t bj_rbuffer_available(
    const bj_rbuffer* p_rbuffer
) {
    bj_check_or_0(p_rbuffer);
    return 0; // TODO
}

bj_bool bj_rbuffer_push(
    bj_rbuffer* p_rbuffer,
    void*       p_data,
    size_t      n
) {
    bj_check_or_0(p_rbuffer);
    /* bj_check_or_0(p_data); */
    bj_check_or_0(n);
    (void)p_data;

    return BJ_FALSE; // TODO
}

bj_bool bj_rbuffer_pop(
    bj_rbuffer* p_rbuffer,
    void*       p_data,
    size_t      n
) {
    bj_check_or_0(p_rbuffer);
    /* bj_check_or_0(p_data); */
    bj_check_or_0(n);
    (void)p_data;

    bj_check_or_0(p_rbuffer);
    /* bj_check_or_0(p_data); */
    bj_check_or_0(n);
    (void)p_data;

    return BJ_FALSE; // TODO
}

BANJO_EXPORT void bj_rbuffer_debug(
    const bj_rbuffer* p_rbuffer,
    const char* desc
) {
    bj_check(p_rbuffer);
    printf("%s: head(%ld) tail(%ld) avail(%ld) ready(%ld) cap(%ld)\n",
        desc,
        p_rbuffer->head, p_rbuffer->tail, 
        bj_rbuffer_available(p_rbuffer),
        bj_rbuffer_ready(p_rbuffer),
        bj_rbuffer_capacity(p_rbuffer)
    );
}
