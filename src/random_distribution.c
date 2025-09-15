#include <banjo/random.h>

#include <stdint.h>
#include <limits.h>

int32_t bj_uniform_int32_distribution(
    bj_random_u32_fn_t next,
    void*              state,
    int32_t            low,
    int32_t            high
){
    if (low > high) { int32_t tmp = low; low = high; high = tmp; }

    if (low == INT32_MIN && high == INT32_MAX) {
        return (int32_t)next(state);
    }

    uint64_t span64 = (int64_t)high - (int64_t)low + 1LL;
    uint32_t bound  = (uint32_t)span64;

    for (;;) {
        uint32_t r = next(state);
        uint64_t m = (uint64_t)r * (uint64_t)bound;
        uint32_t l = (uint32_t)m;

        if (l < bound) {
            uint32_t t = (uint32_t)(-bound) % bound;
            if (l < t) {
                continue;
            }
        }
        return low + (int32_t)(m >> 32);
    }
}
