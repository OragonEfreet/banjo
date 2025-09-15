// random.c
#include <banjo/random.h>

static uint64_t rand_state = 1;

void bj_srand(unsigned int seed) {
    rand_state = ((uint64_t)seed << 1) | 1u;
}

int bj_rand(void) {
    rand_state = rand_state * 6364136223846793005ULL + 1ULL;
    return (int)(((uint64_t)(rand_state >> 49)) & (uint64_t)BJ_RAND_MAX);
}

#define PCG32_MULT 6364136223846793005ULL

static inline uint32_t pcg32_step(bj_pcg32_t *g) {
    uint64_t old = g->state;
    g->state = old * PCG32_MULT + (g->inc | 1ULL);
    uint32_t xorshifted = (uint32_t)(((old >> 18u) ^ old) >> 27u);
    uint32_t rot = (uint32_t)(old >> 59u);
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31u));
}

uint32_t bj_pcg32(
    bj_pcg32_t *g
) {
    return pcg32_step(g);
}

void bj_pcg32_discard(bj_pcg32_t *g, uint64_t z) {
    uint64_t cur_mult = PCG32_MULT;
    uint64_t cur_plus = (g->inc | 1ULL);
    uint64_t acc_mult = 1u;
    uint64_t acc_plus = 0u;

    while (z > 0) {
        if (z & 1u) {
            acc_mult = acc_mult * cur_mult;
            acc_plus = acc_plus * cur_mult + cur_plus;
        }
        cur_plus = (cur_mult + 1) * cur_plus;
        cur_mult = cur_mult * cur_mult;
        z >>= 1u;
    }
    g->state = acc_mult * g->state + acc_plus;
}

uint32_t bj_pcg32_min(
    void
) {
    return 0;
}

uint32_t bj_pcg32_max(
    void
) {
    return 0xFFFFFFFF;
}
