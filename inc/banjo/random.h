////////////////////////////////////////////////////////////////////////////////
/// \file random.h
/// \brief Pseudo-random number generation API.
////////////////////////////////////////////////////////////////////////////////
/// \defgroup random Random
/// \brief Random number generation utilities.
/// \{
////////////////////////////////////////////////////////////////////////////////

#ifndef BJ_RANDOM_H
#define BJ_RANDOM_H

#include <banjo/api.h>
#include <banjo/math.h>

////////////////////////////////////////////////////////////////////////////////
/// \def BJ_RAND_MAX
/// \brief Maximum value returned by bj_rand().
///
/// Matches the stdlib idea of RAND_MAX. Values from bj_rand() are
/// distributed in [0, BJ_RAND_MAX].
////////////////////////////////////////////////////////////////////////////////
#define BJ_RAND_MAX 0x7FFF

////////////////////////////////////////////////////////////////////////////////
/// \brief Seed the standard PRNG.
/// \param seed Seed value. Same seed reproduces the same sequence.
////////////////////////////////////////////////////////////////////////////////
void bj_srand(unsigned int seed);

////////////////////////////////////////////////////////////////////////////////
/// \brief Generate a pseudo-random integer in [0, BJ_RAND_MAX].
///
/// Linear congruential generator (LCG) using:
///   X_{n+1} = (1103515245 * X_n + 12345) mod 2^31
///
/// Returns the high-order bits truncated to BJ_RAND_MAX range.
/// \return Integer in [0, BJ_RAND_MAX].
////////////////////////////////////////////////////////////////////////////////
int bj_rand(void);

////////////////////////////////////////////////////////////////////////////////
/// \brief PCG32 generator state.
///
////////////////////////////////////////////////////////////////////////////////
struct bj_pcg32_t {
    uint64_t state; ///< Current internal state (updated each step).
    uint64_t inc;   ///< Stream selector; odd recommended, 0 allowed.
};
/// \brief Alias for bj_pcg32_t.
typedef struct bj_pcg32_t bj_pcg32;

////////////////////////////////////////////////////////////////////////////////
/// \brief Set the generator state from seed and sequence.
/// \param generator Generator pointer (must not be NULL).
/// \param seed      Initial seed value.
/// \param seq       Stream selector (LSB forced to 1 internally).
////////////////////////////////////////////////////////////////////////////////
void bj_seed_pcg32(
    bj_pcg32* generator,
    uint64_t  seed,
    uint64_t  seq
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Advance the generator and return the next 32-bit value.
/// \param generator Generator pointer (must not be NULL).
/// \return Next 32-bit pseudo-random value.
////////////////////////////////////////////////////////////////////////////////
uint32_t bj_next_pcg32(
    bj_pcg32* generator
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Advance the generator state by z steps.
/// \param generator Generator pointer (must not be NULL).
/// \param z         Number of steps to skip ahead.
////////////////////////////////////////////////////////////////////////////////
void bj_discard_pcg32(
    bj_pcg32* generator,
    uint64_t  z
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Smallest possible value returned by the generator.
/// \return Always 0.
////////////////////////////////////////////////////////////////////////////////
uint32_t bj_min_pcg32(void);

////////////////////////////////////////////////////////////////////////////////
/// \brief Largest possible value returned by the generator.
/// \return Always 0xFFFFFFFF.
////////////////////////////////////////////////////////////////////////////////
uint32_t bj_max_pcg32(void);

////////////////////////////////////////////////////////////////////////////////
/// \brief Adapter for distribution API (void* state).
///
/// Allows bj_pcg32 to be used with callbacks of type
/// uint32_t (*)(void* state).
///
/// \param state Pointer to bj_pcg32.
/// \return Next 32-bit pseudo-random value.
////////////////////////////////////////////////////////////////////////////////
static inline uint32_t bj_pcg32_generator(void* state) {
    return bj_next_pcg32((bj_pcg32*)state);
}

////////////////////////////////////////////////////////////////////////////////
/// \typedef uint32_t (*bj_random_u32_fn_t)(void* state)
/// \brief RNG callback type for generator-agnostic distributions.
/// \param state Opaque engine state pointer.
/// \return Next 32-bit pseudo-random value.
////////////////////////////////////////////////////////////////////////////////
typedef uint32_t (*bj_random_u32_fn_t)(void* state);

////////////////////////////////////////////////////////////////////////////////
/// \brief Uniform 32-bit integer in [low, high].
/// \param next   RNG callback (e.g., bj_pcg32_generator).
/// \param state  Opaque engine state for next.
/// \param low    Inclusive lower bound.
/// \param high   Inclusive upper bound.
/// \return int32 in [low, high].
////////////////////////////////////////////////////////////////////////////////
int32_t bj_uniform_int32_distribution(
    bj_random_u32_fn_t next,
    void*              state,
    int32_t            low,
    int32_t            high
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Uniform float in [low, high).
/// \param next   RNG callback (e.g., bj_pcg32_generator).
/// \param state  Opaque engine state for next.
/// \param low    Inclusive lower bound.
/// \param high   Exclusive upper bound.
/// \return float in [low, high).
////////////////////////////////////////////////////////////////////////////////
float bj_uniform_float_distribution(
    bj_random_u32_fn_t next,
    void*              state,
    float              low,
    float              high
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Uniform double in [low, high).
/// \param next   RNG callback (e.g., bj_pcg32_generator).
/// \param state  Opaque engine state for next.
/// \param low    Inclusive lower bound.
/// \param high   Exclusive upper bound.
/// \return double in [low, high).
////////////////////////////////////////////////////////////////////////////////
double bj_uniform_double_distribution(
    bj_random_u32_fn_t next,
    void*              state,
    double             low,
    double             high
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Uniform long double in [low, high).
/// \param next   RNG callback (e.g., bj_pcg32_generator).
/// \param state  Opaque engine state for next.
/// \param low    Inclusive lower bound.
/// \param high   Exclusive upper bound.
/// \return long double in [low, high).
////////////////////////////////////////////////////////////////////////////////
long double bj_uniform_long_double_distribution(
    bj_random_u32_fn_t next,
    void*              state,
    long double        low,
    long double        high
);

////////////////////////////////////////////////////////////////////////////////
/// \def bj_uniform_real_distribution
/// \brief Alias to the real-typed uniform distribution for the active precision.
///
/// Maps to:
/// - bj_uniform_long_double_distribution if BJ_API_LONG_DOUBLE
/// - bj_uniform_double_distribution      if BJ_API_FLOAT64
/// - bj_uniform_float_distribution       otherwise
////////////////////////////////////////////////////////////////////////////////
#if defined(BJ_API_LONG_DOUBLE)
    #define bj_uniform_real_distribution  bj_uniform_long_double_distribution
#elif defined(BJ_API_FLOAT64)
    #define bj_uniform_real_distribution  bj_uniform_double_distribution
#else
    #define bj_uniform_real_distribution  bj_uniform_float_distribution
#endif

////////////////////////////////////////////////////////////////////////////////
/// \brief Bernoulli(probability).
/// \param next        RNG callback (e.g., bj_pcg32_generator).
/// \param state       Opaque engine state for next.
/// \param probability Probability in [0,1].
/// \return 1 with the given probability, else 0.
////////////////////////////////////////////////////////////////////////////////
int bj_bernoulli_distribution(
    bj_random_u32_fn_t next,
    void*              state,
    bj_real            probability
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Normal float N(mean, standard_deviation^2).
/// \param next                RNG callback (e.g., bj_pcg32_generator).
/// \param state               Opaque engine state for next.
/// \param mean                Mean.
/// \param standard_deviation  Standard deviation (>= 0).
/// \return float sample.
////////////////////////////////////////////////////////////////////////////////
float bj_normal_float_distribution(
    bj_random_u32_fn_t next,
    void*              state,
    float              mean,
    float              standard_deviation
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Normal double N(mean, standard_deviation^2).
/// \param next                RNG callback (e.g., bj_pcg32_generator).
/// \param state               Opaque engine state for next.
/// \param mean                Mean.
/// \param standard_deviation  Standard deviation (>= 0).
/// \return double sample.
////////////////////////////////////////////////////////////////////////////////
double bj_normal_double_distribution(
    bj_random_u32_fn_t next,
    void*              state,
    double             mean,
    double             standard_deviation
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Normal long double N(mean, standard_deviation^2).
/// \param next                RNG callback (e.g., bj_pcg32_generator).
/// \param state               Opaque engine state for next.
/// \param mean                Mean.
/// \param standard_deviation  Standard deviation (>= 0).
/// \return long double sample.
////////////////////////////////////////////////////////////////////////////////
long double bj_normal_long_double_distribution(
    bj_random_u32_fn_t next,
    void*              state,
    long double        mean,
    long double        standard_deviation
);

////////////////////////////////////////////////////////////////////////////////
/// \def bj_normal_real_distribution
/// \brief Alias to the real-typed normal distribution for the active precision.
///
/// Maps to:
/// - bj_normal_long_double_distribution if BJ_API_LONG_DOUBLE
/// - bj_normal_double_distribution      if BJ_API_FLOAT64
/// - bj_normal_float_distribution       otherwise
////////////////////////////////////////////////////////////////////////////////
#if defined(BJ_API_LONG_DOUBLE)
    #define bj_normal_real_distribution  bj_normal_long_double_distribution
#elif defined(BJ_API_FLOAT64)
    #define bj_normal_real_distribution  bj_normal_double_distribution
#else
    #define bj_normal_real_distribution  bj_normal_float_distribution
#endif

#endif /* BJ_RANDOM_H */
/// \} // end of random group
