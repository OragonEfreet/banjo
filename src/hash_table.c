#include "banjo/array.h"
#include "banjo/list.h"
#include "banjo/memory.h"
#include <banjo/error.h>
#include <banjo/hash_table.h>
#include <data/hash_table.h>

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

void bj_hash_table_init(
    const BjHashTableInfo* p_info,
    const BjAllocationCallbacks* p_allocator,
    BjHashTable                  p_instance
) {
    bj_assert(p_info != 0);
    bj_assert(p_info->value_size != 0);
    bj_assert(p_info->key_size != 0);

    p_instance->p_allocator = p_allocator;
    p_instance->weak_owning = p_info->weak_owning;
    p_instance->fn_hash     = p_info->fn_hash ? p_info->fn_hash : fnv1a_hash;
    p_instance->key_size    = p_info->key_size;
    p_instance->value_size  = p_info->value_size;
    p_instance->entry_size  = p_info->weak_owning ? sizeof(void*) * 2 : p_instance->key_size + p_instance->value_size;

    bj_array_init(&(BjArrayInfo) {
        .value_size = sizeof(BjList_T),
        .count      = BUCKET_COUNT,
    }, p_allocator, &p_instance->buckets);

    for(usize i = 0 ; i < bj_array_count(&p_instance->buckets) ; ++i) {
        BjList bucket = bj_array_at(&p_instance->buckets, i);
        bj_list_init(&(BjListInfo) {
            .value_size  = p_instance->entry_size,
            .weak_owning = false,
        }, p_allocator, bucket);
    }
}

void bj_hash_table_reset(
    BjHashTable htable
) {
    bj_hash_table_clear(htable);
    // TODO
    /* for(usize i = 0 ; i < BUCKET_COUNT ; ++i) { */
    /*     bj_list_reset(htable->buckets.p_data + sizeof(BjList_T)*i); */
    /* } */
    bj_array_reset(&htable->buckets);
}

BjHashTable bj_hash_table_create(
    const BjHashTableInfo*       p_info,
    const BjAllocationCallbacks* p_allocator
) {
    bj_assert(p_info != 0);
    BjHashTable htable = bj_new_struct(BjHashTable, p_allocator);
    bj_hash_table_init(p_info, p_allocator, htable);
    return htable;
}

void bj_hash_table_destroy(
    BjHashTable htable
) {
    bj_assert(htable != 0);
    bj_hash_table_reset(htable);
    bj_free(htable, htable->p_allocator);
}

void bj_hash_table_clear(
    BjHashTable htable
) {
    // TODO?
    bj_assert(htable != 0);
}


BANJO_EXPORT void bj_hash_table_set(
    BjHashTable table,
    void* p_key,
    void* p_value
) {
    u32 hash = table->fn_hash(p_key, table->key_size) % BUCKET_COUNT;
    BjList bucket = bj_array_at(&table->buckets, hash);

    BjListIterator_T it;
    bj_list_iterator_init(bucket, &it);

    do {
        byte* key = bj_list_iterator_value(&it);
        if(key != 0) {
            if(memcmp(key, p_key, table->key_size) == 0) {
                byte* value = key+sizeof(table->key_size);
                bj_memcpy(value, p_value, table->value_size);
                return;
            }
        }
    } while(bj_list_iterator_next(&it));
    bj_list_iterator_reset(&it);

    void* new_entry = bj_list_prepend(bucket, 0);
    byte* new_key   = new_entry;
    byte* new_value = new_key + table->key_size;
    bj_memcpy(new_key, p_key, table->key_size);
    bj_memcpy(new_value, p_value, table->value_size);
    /* bj_list_emplace_head(bucket, new_key); */
    /* bj_free(new_entry, table->p_allocator); */
}
