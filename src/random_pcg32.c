#include <banjo/random.h>
#include <stdint.h>

#define PCG32_MULT 6364136223846793005ULL

static struct bj_pcg32 pcg32_default = {0};

static inline struct bj_pcg32* pcg32_resolve(struct bj_pcg32* g) {
    return g ? g : &pcg32_default;
}

static inline uint32_t pcg32_step(struct bj_pcg32 *g) {
    uint64_t old = g->state;
    g->state = old * PCG32_MULT + (g->inc | 1ULL);
    uint32_t xorshifted = (uint32_t)(((old >> 18u) ^ old) >> 27u);
    uint32_t rot = (uint32_t)(old >> 59u);
    return (xorshifted >> rot) | (xorshifted << ((32u - rot) & 31u));
}

void bj_seed_pcg32(struct bj_pcg32 *g, uint64_t initstate, uint64_t initseq) {
    struct bj_pcg32* r = pcg32_resolve(g);
    r->state = 0u;
    r->inc   = (initseq << 1u) | 1u;
    (void)pcg32_step(r);
    r->state += initstate;
    (void)pcg32_step(r);
}

uint32_t bj_next_pcg32(struct bj_pcg32 *g) {
    return pcg32_step(pcg32_resolve(g));
}

void bj_discard_pcg32(struct bj_pcg32 *g, uint64_t z) {
    struct bj_pcg32* r = pcg32_resolve(g);
    uint64_t cur_mult = PCG32_MULT;
    uint64_t cur_plus = (r->inc | 1ULL);
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
    r->state = acc_mult * r->state + acc_plus;
}

uint32_t bj_min_pcg32(void) { return 0u; }
uint32_t bj_max_pcg32(void) { return 0xFFFFFFFFu; }
