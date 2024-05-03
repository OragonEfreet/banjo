#include "banjo/array.h"
#include "banjo/list.h"
#include "banjo/memory.h"
#include <banjo/error.h>
#include <banjo/htable.h>

#include <string.h>

#define BUCKET_COUNT 10

// FNV-1a hash function constants
#define FNV_PRIME 0x01000193 // 16777619
#define FNV_OFFSET_BASIS 0x811C9DC5 // 2166136261

// Function to compute the hash of a buffer using FNV-1a
u32 fnv1a_hash(const void *data, size_t size) {
    const unsigned char *ptr = (const unsigned char *)data;
    uint32_t hash = FNV_OFFSET_BASIS;

    for (size_t i = 0; i < size; ++i) {
        hash ^= (uint32_t)ptr[i];
        hash *= FNV_PRIME;
    }

    return hash;
}

bj_htable* bj_htable_init_default(
    bj_htable*                 p_instance,
    usize bytes_key,
    usize bytes_value
) {
    bj_memset(p_instance, 0, sizeof(bj_htable));
    if(bytes_key == 0 || bytes_value == 0) {
        return p_instance;
    }

    p_instance->weak_owning = false;
    /* p_instance->fn_hash     = p_info->fn_hash ? p_info->fn_hash : fnv1a_hash; */
    p_instance->fn_hash     = fnv1a_hash;
    p_instance->bytes_key   = bytes_key;
    p_instance->bytes_value = bytes_value;
    p_instance->bytes_entry = p_instance->weak_owning ? sizeof(void*) * 2 : p_instance->bytes_key + p_instance->bytes_value;

    bj_array_init_with_capacity_t(&p_instance->buckets, bj_list, BUCKET_COUNT);
    bj_array_set_len(&p_instance->buckets, BUCKET_COUNT);

    for(usize i = 0 ; i < bj_array_len(&p_instance->buckets) ; ++i) {
        bj_list* bucket = bj_array_at(&p_instance->buckets, i);
        bj_list_init_default(bucket, p_instance->bytes_entry);
    }
    return p_instance;
}

bj_htable* bj_htable_reset(
    bj_htable* htable
) {
    bj_htable_clear(htable);
    bj_list* bucket = bj_array_data(&htable->buckets);
    for(usize i = 0 ; i < bj_array_len(&htable->buckets) ; ++i) {
        if(bucket != 0) {
            bj_list_reset(bucket);
        }
        ++bucket;
    }
    bj_array_reset(&htable->buckets);
    return htable;
}


void bj_htable_clear(
    bj_htable* htable
) {
    // TODO?
    bj_assert(htable != 0);
}


void* bj_htable_set(
    bj_htable* table,
    void* p_key,
    void* p_value
) {
    u32 hash = table->fn_hash(p_key, table->bytes_key) % BUCKET_COUNT;
    bj_list* bucket = bj_array_at(&table->buckets, hash);

    bj_list_iterator it;
    bj_list_iterator_init(bucket, &it);

    while(bj_list_iterator_has_next(&it)) {
        byte* key = bj_list_iterator_next(&it);
        if(key != 0) {
            if(memcmp(key, p_key, table->bytes_key) == 0) {
                byte* value = key+table->bytes_key;
                bj_memcpy(value, p_value, table->bytes_value);
                return value;
            }
        }
    };
    bj_list_iterator_reset(&it);

    void* new_entry = bj_list_prepend(bucket, 0);
    byte* new_key   = new_entry;
    byte* new_value = new_key + table->bytes_key;
    bj_memcpy(new_key, p_key, table->bytes_key);
    bj_memcpy(new_value, p_value, table->bytes_value);
    return new_value;
}

void* bj_htable_get(
    const bj_htable* table,
    const void*       p_key,
    void*             p_default
) {
    u32 hash = 0;
    if (table->fn_hash) {
        hash = table->fn_hash(p_key, table->bytes_key) % BUCKET_COUNT;
    }

    bj_list* bucket = bj_array_at(&table->buckets, hash);

    if(bucket != 0) {
        bj_list_iterator it;
        bj_list_iterator_init(bucket, &it);

        while(bj_list_iterator_has_next(&it)) {
            byte* key = bj_list_iterator_next(&it);
            if(key != 0) {
                if(memcmp(key, p_key, table->bytes_key) == 0) {
                    return key+table->bytes_key;
                }
            }
        };
        bj_list_iterator_reset(&it);
    }

    return p_default;
}

usize bj_htable_len(
    const bj_htable* p_table
) {
    usize len = 0;

    for(usize i = 0 ; i < bj_array_len(&p_table->buckets) ; ++i) {
        bj_list* bucket = bj_array_at(&p_table->buckets, i);
        len += bj_list_len(bucket);
    }

    return len;
}
