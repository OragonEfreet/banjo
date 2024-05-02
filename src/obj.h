#pragma once

#define BJ_IMPL_ALLOC(T, t)                                                    \
Bj ## T* bj_ ## t ## _alloc(                                                   \
    const BjAllocationCallbacks* p_allocator                                   \
) {                                                                            \
    return bj_malloc(sizeof(Bj ## T));                                         \
}

#define BJ_IMPL_NEW(T, t)                                                      \
Bj ## T* bj_ ## t ## _new(const Bj ## T ## Info* p_info) {                     \
    Bj ## T* obj = bj_ ## t ## _alloc(p_info->p_allocator);                    \
    bj_ ## t ## _init(obj, p_info);                                            \
    return obj;                                                                \
}

#define BJ_IMPL_DEL(T, t)                                                      \
void bj_ ## t ## _del(                                                         \
    Bj ## T* obj                                                               \
) {                                                                            \
    if(obj != 0) {                                                             \
        bj_ ## t ## _reset(obj);                                               \
    }                                                                          \
    bj_free(obj);                                                              \
}

#define BJ_IMPL_OBJ(T, t)                                                      \
    BJ_IMPL_ALLOC(T, t)                                                        \
    BJ_IMPL_NEW(T, t)                                                          \
    BJ_IMPL_DEL(T, t)

