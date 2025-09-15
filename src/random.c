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

