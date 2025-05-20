#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/memory.h>

#include "rbuffer_t.h"
#include "check.h"

typedef struct bj_rbucket_t {
    void* buffer;
} bj_rbucket;

bj_rbuffer* bj_rbuffer_new(
    size_t bytes_payload,
    size_t bucket_size
) {
    bj_rbuffer rbuffer;
    if(bj_rbuffer_init(&rbuffer, bytes_payload, bucket_size) == 0) {
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
    size_t      bytes_payload,
    size_t      bucket_size
) {
    bj_check_or_0(p_instance);
    bj_check_or_0(bytes_payload);
    bj_check_or_0(bucket_size);

    bj_memzero(p_instance, sizeof(bj_rbuffer));

    p_instance->item_payload = bytes_payload;
    p_instance->bucket_size  = bucket_size;

    return p_instance;
}

void bj_rbuffer_reset(
    bj_rbuffer* p_rbuffer
) {
    bj_check(p_rbuffer);

    for(size_t b = 0 ; b < p_rbuffer->n_buckets ; ++b) {
        bj_free(p_rbuffer->p_buckets[b].buffer);
    }

    bj_memzero(p_rbuffer, sizeof(bj_rbuffer));
}


