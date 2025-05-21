#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/memory.h>

#include "rbuffer_t.h"
#include "check.h"

#include <assert.h>

typedef struct bj_rbucket_t {
    void* data;
} bj_rbucket;

bj_rbuffer* bj_rbuffer_new(
    size_t bucket_size
) {
    bj_rbuffer rbuffer;
    if(bj_rbuffer_init(&rbuffer, bucket_size) == 0) {
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
    p_instance->bucket_size  = bucket_size;
    return p_instance;
}

void bj_rbuffer_reset(
    bj_rbuffer* p_rbuffer
) {
    bj_check(p_rbuffer);

    for(size_t b = 0 ; b < p_rbuffer->n_buckets ; ++b) {
        bj_free(p_rbuffer->buckets[b].data);
    }
    bj_free(p_rbuffer->buckets);

    bj_memzero(p_rbuffer, sizeof(bj_rbuffer));
}

size_t bj_rbuffer_bucket_size(
    const bj_rbuffer* p_rbuffer
) {
    bj_check_or_0(p_rbuffer);
    return p_rbuffer->bucket_size;
}

static size_t bj_rbuffer_required_buckets(
    const bj_rbuffer* p_rbuffer,
    size_t            capacity
) {
    bj_check_or_0(p_rbuffer);
    bj_check_or_0(capacity);

    return (capacity + p_rbuffer->bucket_size - 1) / p_rbuffer->bucket_size;
}

size_t bj_rbuffer_capacity(
    const bj_rbuffer* p_rbuffer
) {
    bj_check_or_0(p_rbuffer);
    return p_rbuffer->bucket_size * p_rbuffer->n_buckets;
}


size_t bj_rbuffer_reserve(
    bj_rbuffer* p_rbuffer,
    size_t      capacity
) {
    bj_check_or_0(p_rbuffer);

    const size_t required_buckets = bj_rbuffer_required_buckets(p_rbuffer, capacity);

    if(required_buckets > p_rbuffer->n_buckets) {
        if(p_rbuffer->buckets == 0) {
            assert(p_rbuffer->n_buckets == 0);
            p_rbuffer->buckets = bj_malloc(sizeof(bj_rbucket) * required_buckets);
        } else {
            p_rbuffer->buckets = bj_realloc(p_rbuffer->buckets, sizeof(bj_rbucket) * required_buckets);
        }

        for(size_t b = p_rbuffer->n_buckets ; b < required_buckets ; ++b) {
            p_rbuffer->buckets[b].data = bj_malloc(p_rbuffer->bucket_size);
        }
        p_rbuffer->n_buckets = required_buckets;
    }

    return bj_rbuffer_capacity(p_rbuffer);
}

int bj_rbuffer_push(
    bj_rbuffer* p_rbuffer,
    void*       p_data,
    size_t      n
) {
    bj_check_or_0(p_rbuffer);
    bj_check_or_0(p_data);
    bj_check_or_0(n);

    p_rbuffer->tail += n;

    return 0;
}

int bj_rbuffer_pop(
    bj_rbuffer* p_rbuffer,
    void**      p_data,
    size_t      n
) {
    bj_check_or_0(p_rbuffer);
    bj_check_or_0(p_data);
    bj_check_or_0(n);

    p_rbuffer->head += n;

    return 0;
}

