#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/memory.h>

#include "check.h"
#include "htable_t.h"
#include "list_t.h"

#define BUCKET_COUNT 10

// FNV-1a hash function constants
#define FNV_PRIME 0x01000193 // 16777619
#define FNV_OFFSET_BASIS 0x811C9DC5 // 2166136261

// Function to compute the hash of a buffer using FNV-1a
uint32_t fnv1a_hash(const void *data, size_t size) {
    const unsigned char *ptr = (const unsigned char *)data;
    uint32_t hash = FNV_OFFSET_BASIS;

    for (size_t i = 0; i < size; ++i) {
        hash ^= (uint32_t)ptr[i];
        hash *= FNV_PRIME;
    }

    return hash;
}

BANJO_EXPORT bj_htable* bj_htable_new(
    size_t bytes_key,
    size_t bytes_value
) {
    bj_check_or_0(bytes_key);
    bj_check_or_0(bytes_value);

    bj_htable* p_table = bj_malloc(sizeof(bj_htable));
    bj_memset(p_table, 0, sizeof(bj_htable));

    p_table->weak_owning = BJ_FALSE;
    /* p_table->fn_hash     = p_info->fn_hash ? p_info->fn_hash : fnv1a_hash; */
    p_table->fn_hash     = fnv1a_hash;
    p_table->bytes_key   = bytes_key;
    p_table->bytes_value = bytes_value;
    p_table->bytes_entry = p_table->weak_owning ? sizeof(void*) * 2 : p_table->bytes_key + p_table->bytes_value;

    bj_array_init(&p_table->buckets, sizeof(bj_list));
    bj_array_set_len(&p_table->buckets, BUCKET_COUNT);

    for(size_t i = 0 ; i < bj_array_len(&p_table->buckets) ; ++i) {
        bj_list* bucket = bj_array_at(&p_table->buckets, i);
        bj_list_init(bucket, p_table->bytes_entry);
    }
    return p_table;
}

void bj_htable_del(
    bj_htable* htable
) {
    bj_check(htable != 0);
    bj_list* bucket = bj_array_data(&htable->buckets);
    for(size_t i = 0 ; i < bj_array_len(&htable->buckets) ; ++i) {
        if(bucket != 0) {
            bj_list_reset(bucket);
        }
        ++bucket;
    }
    bj_array_reset(&htable->buckets);
    bj_free(htable);
}

void* bj_htable_set(
    bj_htable* table,
    void* p_key,
    void* p_value
) {
    uint32_t hash = table->fn_hash(p_key, table->bytes_key) % BUCKET_COUNT;
    bj_list* bucket = bj_array_at(&table->buckets, hash);

    bj_list_iterator it;
    bj_list_iterator_init(bucket, &it);

    while(bj_list_iterator_has_next(&it)) {
        char* key = bj_list_iterator_next(&it);
        if(key != 0) {
            if(bj_memcmp(key, p_key, table->bytes_key) == 0) {
                char* value = key+table->bytes_key;
                bj_memcpy(value, p_value, table->bytes_value);
                return value;
            }
        }
    };
    bj_list_iterator_reset(&it);

    void* new_entry = bj_list_prepend(bucket, 0);
    char* new_key   = new_entry;
    char* new_value = new_key + table->bytes_key;
    bj_memcpy(new_key, p_key, table->bytes_key);
    bj_memcpy(new_value, p_value, table->bytes_value);
    return new_value;
}

void* bj_htable_get(
    const bj_htable* table,
    const void*       p_key,
    void*             p_default
) {
    bj_check_or_0(table);
    bj_check_or_0(p_key);

    uint32_t hash = 0;
    if (table->fn_hash) {
        hash = table->fn_hash(p_key, table->bytes_key) % BUCKET_COUNT;
    }

    bj_list* bucket = bj_array_at(&table->buckets, hash);

    if(bucket != 0) {
        bj_list_iterator it;
        bj_list_iterator_init(bucket, &it);

        while(bj_list_iterator_has_next(&it)) {
            char* key = bj_list_iterator_next(&it);
            if(key != 0) {
                if(bj_memcmp(key, p_key, table->bytes_key) == 0) {
                    return key+table->bytes_key;
                }
            }
        };
        bj_list_iterator_reset(&it);
    }

    return p_default;
}

size_t bj_htable_len(
    const bj_htable* p_table
) {
    bj_check_or_0(p_table);

    size_t len = 0;
    for(size_t i = 0 ; i < bj_array_len(&p_table->buckets) ; ++i) {
        bj_list* bucket = bj_array_at(&p_table->buckets, i);
        len += bj_list_len(bucket);
    }

    return len;
}
