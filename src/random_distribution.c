#include <banjo/math.h>
#include <banjo/random.h>

#include <stdint.h>
#include <limits.h>

int32_t bj_uniform_int32_distribution(
    bj_random_u32_fn_t next,
    void*              state,
    int32_t            low,
    int32_t            high
){
    if (low > high) { int32_t t = low; low = high; high = t; }
    if (low == INT32_MIN && high == INT32_MAX) return (int32_t)next(state);

    uint64_t span64 = (int64_t)high - (int64_t)low + 1LL;
    uint32_t bound  = (uint32_t)span64;

    if ((bound & (bound - 1u)) == 0u) { /* power of two */
        return low + (int32_t)(next(state) & (bound - 1u));
    }

    for (;;) {
        uint32_t r = next(state);
        uint64_t m = (uint64_t)r * (uint64_t)bound;
        uint32_t l = (uint32_t)m;
        if (l < bound) {
            uint32_t t = (uint32_t)(-bound) % bound;
            if (l < t) continue;
        }
        return low + (int32_t)(m >> 32);
    }
}

float bj_uniform_float_distribution(
    bj_random_u32_fn_t next,
    void*              state,
    float              low,
    float              high
){
    if (low > high) { float t = low; low = high; high = t; }
    if (low == high) return low;

    uint32_t r = next(state);
    uint32_t v = r >> 8;                       /* top 24 bits */
    float u = (float)v * (1.0f / 16777216.0f); /* 2^-24 */
    return low + (high - low) * u;
}

double bj_uniform_double_distribution(
    bj_random_u32_fn_t next,
    void*              state,
    double             low,
    double             high
){
    if (low > high) { double t = low; low = high; high = t; }
    if (low == high) return low;

    uint64_t u64 = ((uint64_t)next(state) << 32) | (uint64_t)next(state);
    uint64_t v   = u64 >> 11;                            /* top 53 bits */
    double u     = (double)v * (1.0 / 9007199254740992.0); /* 2^-53 */
    return low + (high - low) * u;
}

int bj_bernoulli_distribution(
    bj_random_u32_fn_t next,
    void*              state,
    bj_real            probability
){
    if (probability <= BJ_F(0.0)) {
        return 0;
    }
    if (probability >= BJ_F(1.0)) {
        return 1;
    }

    bj_real u = bj_uniform_real_distribution(next, state, BJ_F(0.0), BJ_F(1.0));
    return (u < probability) ? 1 : 0;
}
