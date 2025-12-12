#include <banjo/math.h>
#include <banjo/random.h>

#include <stdint.h>
#include <limits.h>

int32_t bj_uniform_int32_distribution(
    bj_random_u32_fn next,
    void*              state,
    int32_t            low,
    int32_t            high
){
    if (low > high) { int32_t t = low; low = high; high = t; }
    if (low == INT32_MIN && high == INT32_MAX) return (int32_t)next(state);

    uint64_t span64 = (int64_t)high - (int64_t)low + 1LL;
    uint32_t bound  = (uint32_t)span64;

    if ((bound & (bound - 1u)) == 0u) {
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
    bj_random_u32_fn next,
    void*              state,
    float              low,
    float              high
){
    if (low > high) { float t = low; low = high; high = t; }
    if (low == high) return low;

    uint32_t r = next(state);
    uint32_t v = r >> 8;                       // top 24 bits
    float u = (float)v * (1.0f / 16777216.0f); // 2^-24
    return low + (high - low) * u;
}

double bj_uniform_double_distribution(
    bj_random_u32_fn next,
    void*              state,
    double             low,
    double             high
){
    if (low > high) { double t = low; low = high; high = t; }
    if (low == high) return low;

    uint64_t u64 = ((uint64_t)next(state) << 32) | (uint64_t)next(state);
    uint64_t v   = u64 >> 11;                            // top 53 bits
    double u     = (double)v * (1.0 / 9007199254740992.0); // 2^-53
    return low + (high - low) * u;
}

long double bj_uniform_long_double_distribution(
    bj_random_u32_fn next,
    void*              state,
    long double        low,
    long double        high
){
    if (low > high) { long double t = low; low = high; high = t; }
    if (low == high) return low;

    /* 64 uniform bits → [0,1) with 2^-64 resolution */
    uint64_t u64 = ((uint64_t)next(state) << 32) | (uint64_t)next(state);
    long double u = (long double)u64 * (1.0L / 18446744073709551616.0L); /* 2^-64 */

    return low + (high - low) * u;
}

int bj_bernoulli_distribution(
    bj_random_u32_fn next,
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

float bj_normal_float_distribution(
    bj_random_u32_fn next,
    void*              state,
    float              mean,
    float              standard_deviation
){
    if (standard_deviation <= 0.0f) return mean;

    float u1 = bj_uniform_float_distribution(next, state, 0.0f, 1.0f);
    float u2 = bj_uniform_float_distribution(next, state, 0.0f, 1.0f);

    float r     = bj_sqrtf(-2.0f * bj_log(1.0f - u1));
    float theta = BJ_TAU_F * u2;

    float z0 = r * bj_cosf(theta);
    return mean + standard_deviation * z0;
}

double bj_normal_double_distribution(
    bj_random_u32_fn next,
    void*              state,
    double             mean,
    double             standard_deviation
){
    if (standard_deviation <= 0.0) return mean;

    double u1 = bj_uniform_double_distribution(next, state, 0.0, 1.0);
    double u2 = bj_uniform_double_distribution(next, state, 0.0, 1.0);

    double r     = bj_sqrt(-2.0 * bj_log(1.0 - u1));
    double theta = BJ_TAU_D * u2;

    double z0 = r * bj_cos(theta);
    return mean + standard_deviation * z0;
}

long double bj_normal_long_double_distribution(
    bj_random_u32_fn next,
    void*              state,
    long double        mean,
    long double        standard_deviation
){
    if (standard_deviation <= 0.0L) return mean;

    long double u1 = bj_uniform_long_double_distribution(next, state, 0.0L, 1.0L);
    long double u2 = bj_uniform_long_double_distribution(next, state, 0.0L, 1.0L);

    /* Box–Muller: use 1-u1 to avoid logl(0) */
    long double r     = sqrtl(-2.0L * logl(1.0L - u1));
    long double theta = 6.28318530717958647692528676655900576L * u2; /* 2π */

    long double z0 = r * cosl(theta); /* standard normal */
    return mean + standard_deviation * z0;
}

float bj_normal_float_minmax_distribution(
    bj_random_u32_fn next,
    void*              state,
    float              min,
    float              max
) {
    const float mu    = 0.5f * (min + max);
    const float sigma = (max - min) / 6.0f;
    return bj_normal_float_distribution(next, state, mu, sigma);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Normal from min/max convenience (μ ≈ (min+max)/2, σ ≈ (max-min)/6).
///
/// Interprets min ≈ μ − 3σ and max ≈ μ + 3σ, then calls the typed normal
/// distribution. Samples are not guaranteed to lie inside [min, max].
///
/// \param next   RNG callback (e.g., bj_pcg32_generator).
/// \param state  Opaque engine state for next.
/// \param min    Approximate lower limit (treated as μ − 3σ).
/// \param max    Approximate upper limit (treated as μ + 3σ).
/// \return One sample from N(μ, σ²) with μ,σ derived from min/max.
////////////////////////////////////////////////////////////////////////////////
double bj_normal_double_minmax_distribution(
    bj_random_u32_fn next,
    void*              state,
    double             min,
    double             max
) {
    const double mu    = 0.5 * (min + max);
    const double sigma = (max - min) / 6.0;
    return bj_normal_double_distribution(next, state, mu, sigma);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Normal from min/max convenience (μ ≈ (min+max)/2, σ ≈ (max-min)/6).
///
/// Interprets min ≈ μ − 3σ and max ≈ μ + 3σ, then calls the typed normal
/// distribution. Samples are not guaranteed to lie inside [min, max].
///
/// \param next   RNG callback (e.g., bj_pcg32_generator).
/// \param state  Opaque engine state for next.
/// \param min    Approximate lower limit (treated as μ − 3σ).
/// \param max    Approximate upper limit (treated as μ + 3σ).
/// \return One sample from N(μ, σ²) with μ,σ derived from min/max.
////////////////////////////////////////////////////////////////////////////////
long double bj_normal_long_double_minmax_distribution(
    bj_random_u32_fn next,
    void*              state,
    long double        min,
    long double        max
) {
    const long double mu    = 0.5L * (min + max);
    const long double sigma = (max - min) / 6.0L;
    return bj_normal_long_double_distribution(next, state, mu, sigma);
}

