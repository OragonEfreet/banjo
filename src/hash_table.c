#include <banjo/error.h>
#include <banjo/hash_table.h>
#include <data/hash_table.h>

#include <stdio.h>

#define BUCKET_COUNT 10

typedef struct {
    void*  pKey;
    void*  pValue;
} BjHashTableEntry;

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

void bjInitHashTable(
    const BjHashTableInfo* pInfo,
    BjHashTable                  pInstance
) {
    bjAssert(pInfo != 0);
    bjAssert(pInfo != 0);
    bjAssert(pInfo != 0);

    pInstance->pAllocator  = pInfo->pAllocator;
    pInstance->weak_owning = pInfo->weak_owning;
    pInstance->pfnHash = pInfo->pfnHash ? pInfo->pfnHash : fnv1a_hash;

    bjInitArray(&(BjArrayInfo) {
        .value_size = sizeof(BjForwardList_T),
        .pAllocator = pInfo->pAllocator,
        .capacity      = BUCKET_COUNT,
    }, &pInstance->buckets_array);
}

void bjResetHashTable(
    BjHashTable htable
) {
    bjClearHashTable(htable);
    // TODO
    /* for(usize i = 0 ; i < BUCKET_COUNT ; ++i) { */
    /*     bjResetForwardList(htable->buckets_array.pData + sizeof(BjForwardList_T)*i); */
    /* } */
    bjResetArray(&htable->buckets_array);
}

BjHashTable bjCreateHashTable(
    const BjHashTableInfo* pInfo
) {
    bjAssert(pInfo != 0);
    BjHashTable htable = bjNewStruct(BjHashTable, pInfo->pAllocator);
    bjInitHashTable(pInfo, htable);
    return htable;
}

void bjDestroyHashTable(
    BjHashTable htable
) {
    bjAssert(htable != 0);
    bjResetHashTable(htable);
    bjFree(htable, htable->pAllocator);
}

void bjClearHashTable(
    BjHashTable htable
) {
    // TODO?
    bjAssert(htable != 0);
}

BANJO_EXPORT void bjHashTableSet(
    BjHashTable table,
    const void* pKey,
    const void* pValue
) {
    u32 hash = table->pfnHash(pKey, table->key_size) % BUCKET_COUNT;
    printf("Hash: %u\n", hash);
}
