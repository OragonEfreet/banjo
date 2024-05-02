#include "banjo/array.h"
#include "banjo/list.h"
#include "banjo/memory.h"
#include <banjo/error.h>
#include <banjo/hash_table.h>

#include <string.h>

BjHashTable* bj_hash_table_new(const BjHashTableInfo* p_info) {
    BjHashTable* obj = bj_hash_table_alloc();
    bj_hash_table_init(obj, p_info);
    return obj;
}

void bj_hash_table_del(BjHashTable* obj) {
    if(obj != 0) {
        bj_hash_table_reset(obj);
    }
    bj_free(obj);
}

BjHashTable* bj_hash_table_alloc(void) {
    return bj_malloc(sizeof(BjHashTable));
}

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
    BjHashTable*              p_instance,
    const BjHashTableInfo* p_info
) {
    bj_memset(p_instance, 0, sizeof(BjHashTable));
    if(p_info == 0 || p_info->bytes_key == 0 || p_info->bytes_value == 0) {
        return;
    }

    p_instance->info.weak_owning = p_info->weak_owning;
    p_instance->info.fn_hash     = p_info->fn_hash ? p_info->fn_hash : fnv1a_hash;
    p_instance->info.bytes_key   = p_info->bytes_key;
    p_instance->info.bytes_value = p_info->bytes_value;
    p_instance->bytes_entry = p_info->weak_owning ? sizeof(void*) * 2 : p_instance->info.bytes_key + p_instance->info.bytes_value;

    bj_array_init(&p_instance->buckets, 
        &(BjArrayInfo) {
            .bytes_payload = sizeof(BjList),
            .len           = BUCKET_COUNT,
        }
    );

    for(usize i = 0 ; i < bj_array_len(&p_instance->buckets) ; ++i) {
        BjList* bucket = bj_array_at(&p_instance->buckets, i);
        bj_list_init(bucket, &(BjListInfo) {
            .bytes_payload  = p_instance->bytes_entry,
            .weak_owning   = false,
        });
    }
}

void bj_hash_table_reset(
    BjHashTable* htable
) {
    bj_hash_table_clear(htable);
    BjList* bucket = bj_array_data(&htable->buckets);
    for(usize i = 0 ; i < bj_array_len(&htable->buckets) ; ++i) {
        if(bucket != 0) {
            bj_list_reset(bucket);
        }
        ++bucket;
    }
    bj_array_reset(&htable->buckets);
}


void bj_hash_table_clear(
    BjHashTable* htable
) {
    // TODO?
    bj_assert(htable != 0);
}


void* bj_hash_table_set(
    BjHashTable* table,
    void* p_key,
    void* p_value
) {
    u32 hash = table->info.fn_hash(p_key, table->info.bytes_key) % BUCKET_COUNT;
    BjList* bucket = bj_array_at(&table->buckets, hash);

    BjListIterator it;
    bj_list_iterator_init(bucket, &it);

    while(bj_list_iterator_has_next(&it)) {
        byte* key = bj_list_iterator_next(&it);
        if(key != 0) {
            if(memcmp(key, p_key, table->info.bytes_key) == 0) {
                byte* value = key+table->info.bytes_key;
                bj_memcpy(value, p_value, table->info.bytes_value);
                return value;
            }
        }
    };
    bj_list_iterator_reset(&it);

    void* new_entry = bj_list_prepend(bucket, 0);
    byte* new_key   = new_entry;
    byte* new_value = new_key + table->info.bytes_key;
    bj_memcpy(new_key, p_key, table->info.bytes_key);
    bj_memcpy(new_value, p_value, table->info.bytes_value);
    return new_value;
}

void* bj_hash_table_get(
    const BjHashTable* table,
    const void*       p_key,
    void*             p_default
) {
    u32 hash = 0;
    if (table->info.fn_hash) {
        hash = table->info.fn_hash(p_key, table->info.bytes_key) % BUCKET_COUNT;
    }

    BjList* bucket = bj_array_at(&table->buckets, hash);

    if(bucket != 0) {
        BjListIterator it;
        bj_list_iterator_init(bucket, &it);

        while(bj_list_iterator_has_next(&it)) {
            byte* key = bj_list_iterator_next(&it);
            if(key != 0) {
                if(memcmp(key, p_key, table->info.bytes_key) == 0) {
                    return key+table->info.bytes_key;
                }
            }
        };
        bj_list_iterator_reset(&it);
    }

    return p_default;
}

usize bj_hash_table_len(
    const BjHashTable* p_table
) {
    usize len = 0;

    for(usize i = 0 ; i < bj_array_len(&p_table->buckets) ; ++i) {
        BjList* bucket = bj_array_at(&p_table->buckets, i);
        len += bj_list_len(bucket);
    }

    return len;
}
