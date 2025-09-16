////////////////////////////////////////////////////////////////////////////////
/// \example random.c
/// Demonstrates various ways to generate pseudo-randomness
////////////////////////////////////////////////////////////////////////////////
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/random.h>
#include <banjo/time.h>
#include <string.h>

static uint64_t pcg32_u64(bj_pcg32 *g) {
    uint64_t hi = (uint64_t)bj_next_pcg32(g);
    uint64_t lo = (uint64_t)bj_next_pcg32(g);
    return (hi << 32) | lo;
}

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;

    /* std-style functions */
    bj_info("Before srand():");
    for (size_t i = 0; i < 5; ++i) {
        bj_info("\tbj_rand() -> %d", bj_rand());
    }
    bj_srand((unsigned)bj_get_time());
    bj_info("After srand():");
    for (size_t i = 0; i < 5; ++i) {
        bj_info("\tbj_rand() -> %d", bj_rand());
    }

    /* PCG32 engine */
    bj_info("PCG32:");

    /* zero-init stream */
    bj_pcg32 g0 = (bj_pcg32){0};
    bj_info("\tzero-init stream:");
    for (size_t i = 0; i < 3; ++i) {
        bj_info("\t\t%10u", bj_next_pcg32(&g0));
    }

    /* time-seeded stream */
    bj_pcg32 g1 = (bj_pcg32){0};
    bj_seed_pcg32(&g1, (uint64_t)bj_get_time(), 54u);
    bj_info("\tseeded with time, seq=54:");
    for (size_t i = 0; i < 3; ++i) {
        bj_info("\t\t%10u", bj_next_pcg32(&g1));
    }

    /* different independent stream via seq */
    bj_pcg32 g2 = (bj_pcg32){0};
    bj_seed_pcg32(&g2, (uint64_t)bj_get_time(), 55u);
    bj_info("\tseeded with time, seq=55 (independent):");
    for (size_t i = 0; i < 3; ++i) {
        bj_info("\t\t%10u", bj_next_pcg32(&g2));
    }

    /* min/max contract */
    bj_info("\tmin=%u max=%u", bj_min_pcg32(), bj_max_pcg32());

    /* discard/jump-ahead */
    bj_pcg32 gA = (bj_pcg32){0};
    bj_pcg32 gB = (bj_pcg32){0};
    bj_seed_pcg32(&gA, 1234u, 999u);
    bj_seed_pcg32(&gB, 1234u, 999u);
    for (size_t i = 0; i < 10; ++i) (void)bj_next_pcg32(&gA);
    bj_discard_pcg32(&gB, 10);
    bj_info("\tdiscard(10) aligns streams: %u vs %u", bj_next_pcg32(&gA), bj_next_pcg32(&gB));

    /* build a 64-bit value from PCG32 */
    bj_pcg32 g64 = (bj_pcg32){0};
    bj_seed_pcg32(&g64, 0xCAFEBABEULL, 0xDEADULL);
    uint64_t x = pcg32_u64(&g64);
    bj_info("\tu64 from two draws: 0x%016llx", (unsigned long long)x);

    return 0;
}
