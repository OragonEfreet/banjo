#pragma once

#define BJ_IMPL_NEW(T, t)                                                      \
Bj ## T* bj_ ## t ## _new(                                                     \
    const Bj ## T ## Info*       p_info,                                       \
    const BjAllocationCallbacks* p_allocator                                   \
) {                                                                            \
    bj_assert(p_info != 0);                                                    \
    Bj ## T* obj = bj_malloc(                                                  \
        sizeof(struct Bj ## T ## _T), p_allocator                              \
    );                                                                         \
    bj_ ## t ## _init(p_info, p_allocator, obj);                               \
    return obj;                                                                \
}

#define BJ_IMPL_DEL(T, t)                                                      \
void bj_ ## t ## _del(                                                         \
    Bj ## T* obj                                                               \
) {                                                                            \
    bj_assert(obj != 0);                                                       \
    bj_ ## t ## _reset(obj);                                                   \
    bj_free(obj, obj->p_allocator);                                            \
}

#define BJ_IMPL_OBJ(T, t)                                                      \
    BJ_IMPL_NEW(T, t)                                                          \
    BJ_IMPL_DEL(T, t)


#define BJ_IMPL_NEW_(T)                                                        \
bj_ ## T* bj_ ## T ## _new(                                                    \
    const bj_ ## T ## _info*       p_info,                                     \
    const BjAllocationCallbacks* p_allocator                                   \
) {                                                                            \
    bj_assert(p_info != 0);                                                    \
    bj_ ## T* obj = bj_malloc(                                                  \
        sizeof(struct bj_ ## T ## _t), p_allocator                              \
    );                                                                         \
    bj_ ## T ## _init(p_info, p_allocator, obj);                               \
    return obj;                                                                \
}

#define BJ_IMPL_DEL_(T)                                                        \
void bj_ ## T ## _del(                                                         \
    bj_ ## T* obj                                                              \
) {                                                                            \
    bj_assert(obj != 0);                                                       \
    bj_ ## T ## _reset(obj);                                                   \
    bj_free(obj, obj->p_allocator);                                            \
}

#define BJ_IMPL_OBJ_(t)                                                        \
    BJ_IMPL_NEW_(t)                                                            \
    BJ_IMPL_DEL_(t)
