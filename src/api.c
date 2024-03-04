#define BJ_IMPL_ALLOC(T, t)                                                    \
Bj ## T* bj_ ## t ## _alloc(                                                   \
    const BjAllocationCallbacks* p_allocator                                   \
) {                                                                            \
    return bj_malloc(sizeof(Bj ## T), p_allocator);                            \
}

#define BJ_IMPL_NEW2(T, t)                                                     \
Bj ## T* bj_ ## t ## _new(                                                     \
    const Bj ## T ## Info* p_info,                                             \
    const BjAllocationCallbacks* p_allocator                                   \
) {                                                                            \
    Bj ## T* obj = bj_ ## t ## _alloc(p_allocator);                            \
    bj_ ## t ## _init(obj, p_info, p_allocator);                               \
    return obj;                                                                \
}

#define BJ_IMPL_DEL2(T, t)                                                     \
void bj_ ## t ## _del(                                                         \
    Bj ## T* obj                                                               \
) {                                                                            \
    const BjAllocationCallbacks* allocs = 0;                                   \
    if(obj != 0) {                                                             \
        allocs = obj->p_allocator;                                             \
        bj_ ## t ## _reset(obj);                                               \
    }                                                                          \
    bj_free(obj, allocs);                                                      \
}

#define BJ_IMPL_OBJ2(T, t)                                                     \
    BJ_IMPL_ALLOC(T, t)                                                        \
    BJ_IMPL_NEW2(T, t)                                                         \
    BJ_IMPL_DEL2(T, t)

#include <banjo/array.h>
#include <banjo/hash_table.h>

BJ_IMPL_OBJ2(Array, array)
BJ_IMPL_OBJ2(HashTable, hash_table)
