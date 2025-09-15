////////////////////////////////////////////////////////////////////////////////
/// \file random.h
/// \brief Pseudo-random number generation API.
////////////////////////////////////////////////////////////////////////////////

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
/// Matches the stdlib idea of `RAND_MAX`. Values from bj_rand() are
/// distributed in `[0, BJ_RAND_MAX]`. 
////////////////////////////////////////////////////////////////////////////////
#define BJ_RAND_MAX 0x7FFF

////////////////////////////////////////////////////////////////////////////////
/// \brief Seed the standard PRNG.
///
/// Using the same seed reproduces the same sequence.
////////////////////////////////////////////////////////////////////////////////
void bj_srand(unsigned int seed);

////////////////////////////////////////////////////////////////////////////////
/// \brief Generate a pseudo-random integer.
///
/// Mimics the behavior of the C standard library `rand()`.  
/// Implements a linear congruential generator (LCG) with the recurrence:
/// \f[
///   X_{n+1} = (a \cdot X_n + c) \; \bmod \; m
/// \f]
///
/// - Multiplier `a = 1103515245`  
/// - Increment `c = 12345`  
/// - Modulus `m = 2^{31}`  
///
/// The function returns the high-order bits of the state truncated to
/// fit in the range `[0, BJ_RAND_MAX]`.
///
/// \return A pseudo-random integer between 0 and BJ_RAND_MAX inclusive.
////////////////////////////////////////////////////////////////////////////////
int bj_rand(void);

////////////////////////////////////////////////////////////////////////////////
/// \brief PCG32 generator state.
///
/// Zero-initialization is valid and yields a deterministic stream.
/// The structure is plain-old-data: safe for stack allocation, copying,
/// or static storage. The \p inc field selects an independent sequence;
/// it is recommended to use an odd value, though zero is allowed.
///
/// \note The internal layout is part of the public ABI. Do not change
///       without bumping the major version.
////////////////////////////////////////////////////////////////////////////////
struct bj_pcg32_t {
    uint64_t state; ///< Current internal state (updated each step).
    uint64_t inc;   ///< Stream selector; odd recommended, 0 allowed.
};

typedef struct bj_pcg32_t bj_pcg32;

////////////////////////////////////////////////////////////////////////////////
/// \brief Set the generator state from seed and sequence.
/// \param generator Generator pointer (must not be NULL).
/// \param seed      Initial seed value.
/// \param seq       Stream selector (LSB forced to 1 internally).
////////////////////////////////////////////////////////////////////////////////
void bj_pcg32_seed(
    bj_pcg32* generator,
    uint64_t    seed,
    uint64_t    seq
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Advance the generator and return the next 32-bit value.
/// \param generator Generator pointer (must not be NULL).
/// \return Next 32-bit pseudo-random value.
////////////////////////////////////////////////////////////////////////////////
uint32_t bj_pcg32_next(
    bj_pcg32* generator
);


////////////////////////////////////////////////////////////////////////////////
/// \brief Advance the generator state by \p z steps.
/// \param generator Generator pointer (must not be NULL).
/// \param z         Number of steps to skip ahead.
////////////////////////////////////////////////////////////////////////////////
void bj_pcg32_discard(
    bj_pcg32* generator,
    uint64_t    z
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Smallest possible value returned by the generator.
/// \return Always 0.
////////////////////////////////////////////////////////////////////////////////
uint32_t bj_pcg32_min(
    void
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Largest possible value returned by the generator.
/// \return Always 0xFFFFFFFF.
////////////////////////////////////////////////////////////////////////////////
uint32_t bj_pcg32_max(
    void
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Adapter for distribution API (void* state).
///
/// This wrapper allows \ref bj_pcg32 to be used with the generic
/// distribution functions, which expect a function pointer of the form
/// `uint32_t (*)(void* state)`.
///
/// Typical usage:
/// \code
/// bj_pcg32 rng = {0};
/// bj_pcg32_seed(&rng, 123, 456);
/// int v = bj_uniform_int32_distribution(bj_pcg32_generator, &rng, 0, 9);
/// \endcode
///
/// \param state Pointer to bj_pcg32.
/// \return Next 32-bit pseudo-random value.
///
////////////////////////////////////////////////////////////////////////////////
static inline uint32_t bj_pcg32_generator(void* state) {
    return bj_pcg32_next((bj_pcg32*)state);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief RNG callback type for generator-agnostic distributions.
/// \param state Opaque engine state pointer.
/// \return Next 32-bit pseudo-random value.
////////////////////////////////////////////////////////////////////////////////
typedef uint32_t (*bj_random_u32_fn_t)(void* state);

////////////////////////////////////////////////////////////////////////////////
/// \brief Uniform 32-bit integer in [low, high].
/// \param next   RNG callback (e.g., bj_pcg32_generator).
/// \param state  Opaque engine state for \p next.
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
/// \param state  Opaque engine state for \p next.
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
/// \param state  Opaque engine state for \p next.
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

#ifdef BJ_USE_DOUBLE
# define bj_uniform_real_distribution  bj_uniform_double_distribution
#else
# define bj_uniform_real_distribution  bj_uniform_float_distribution
#endif

////////////////////////////////////////////////////////////////////////////////
/// \brief Bernoulli(probability).
/// \param next        RNG callback (e.g., bj_pcg32_generator).
/// \param state       Opaque engine state for \p next.
/// \param probability Probability in [0,1].
/// \return 1 with probability, else 0.
////////////////////////////////////////////////////////////////////////////////
int bj_bernoulli_distribution(
    bj_random_u32_fn_t next,
    void*              state,
    bj_real            probability
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Normal float N(mean, standard_deviation^2).
/// \param next                RNG callback (e.g., bj_pcg32_generator).
/// \param state               Opaque engine state for \p next.
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
/// \param state               Opaque engine state for \p next.
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

#ifdef BJ_USE_DOUBLE
# define bj_normal_real_distribution  bj_normal_double_distribution
typedef struct bj_normald_cache_t bj_normal_cache;
#else
# define bj_normal_real_distribution  bj_normal_float_distribution
typedef struct bj_normalf_cache_t bj_normal_cache;
#endif

#endif
/// \} // end of random group
