#include <banjo/hash_table.h>
#include <data/hash_table.h>
#include <errors.h>

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

BjResult bjInitHashTable(
    const BjHashTableInfo* pInfo,
    BjHashTable                  pInstance
) {
    bjExpectValue(pInfo, BJ_NULL_CREATE_INFO);
    bjExpectValue(pInfo->value_size, BJ_INVALID_PARAMETER);
    bjExpectValue(pInfo->key_size, BJ_INVALID_PARAMETER);

    pInstance->pAllocator  = pInfo->pAllocator;
    pInstance->weak_owning = pInfo->weak_owning;
    pInstance->pfnHash = pInfo->pfnHash ? pInfo->pfnHash : fnv1a_hash;

    bjInitArray(&(BjArrayInfo) {
        .value_size = sizeof(BjForwardList_T),
        .pAllocator = pInfo->pAllocator,
        .capacity      = BUCKET_COUNT,
    }, &pInstance->buckets_array);

    return BJ_SUCCESS;
}

BjResult bjResetHashTable(
    BjHashTable htable
) {
    BjResult res      = bjClearHashTable(htable);
    // TODO
    /* for(usize i = 0 ; i < BUCKET_COUNT ; ++i) { */
    /*     bjResetForwardList(htable->buckets_array.pData + sizeof(BjForwardList_T)*i); */
    /* } */
    bjResetArray(&htable->buckets_array);
    return res;
}

BjResult bjCreateHashTable(
    const BjHashTableInfo* pInfo,
    BjHashTable* pInstance
) {
    bjExpectValue(pInstance, BJ_NULL_OUTPUT_HANDLE);
    bjExpectValue(pInfo->value_size, BJ_INVALID_PARAMETER);

    BjHashTable htable = bjNewStruct(BjHashTable, pInfo->pAllocator);

    BjResult result = bjInitHashTable(pInfo, htable);
    if(result == BJ_SUCCESS) {
        *pInstance = htable;
    } else {
        bjFree(htable, pInfo->pAllocator);
        return result;
    }

    return BJ_SUCCESS;
}

BjResult bjDestroyHashTable(
    BjHashTable htable
) {
    bjExpectValue(htable, BJ_NULL_PARAMETER);
    BjResult res =  bjResetHashTable(htable);
    bjFree(htable, htable->pAllocator);
    return res;
}

BANJO_EXPORT BjResult bjClearHashTable(
    BjHashTable htable
) {
    bjExpectValue(htable, BJ_NULL_PARAMETER);

    return BJ_SUCCESS;
}

BANJO_EXPORT BjResult bjHashTableSet(
    BjHashTable table,
    const void* pKey,
    const void* pValue
) {
    u32 hash = table->pfnHash(pKey, table->key_size) % BUCKET_COUNT;
    printf("Hash: %u\n", hash);

    return BJ_SUCCESS;
}
