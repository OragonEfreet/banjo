////////////////////////////////////////////////////////////////////////////////
/// \example random.c
/// Random number generation with global and per-stream APIs.
///
/// Banjo provides two random number APIs:
/// 1. bj_rand()/bj_srand() - Simple global state (like C stdlib rand/srand)
/// 2. bj_pcg32 - Explicit generator objects with independent streams
///
/// Use the global API for quick prototyping. Use PCG32 when you need:
/// - Multiple independent random streams
/// - Reproducible sequences with explicit seeds
/// - Jump-ahead capability (skipping values efficiently)
/// - Higher quality randomness (PCG32 is a modern, well-tested algorithm)
////////////////////////////////////////////////////////////////////////////////
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/random.h>
#include <banjo/time.h>
#include <string.h>

// Helper: build a 64-bit value by combining two 32-bit draws.
// PCG32 generates 32-bit values, but you can combine them for larger ranges.
static uint64_t pcg32_u64(bj_pcg32 *g) {
    uint64_t hi = (uint64_t)bj_next_pcg32(g);
    uint64_t lo = (uint64_t)bj_next_pcg32(g);
    return (hi << 32) | lo;
}

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;

    // Global API: bj_rand() works like C stdlib rand(). It uses hidden global
    // state, so it's simple but not suitable when you need independent streams.
    bj_info("Before srand():");
    for (size_t i = 0; i < 5; ++i) {
        bj_info("\tbj_rand() -> %d", bj_rand());
    }

    // Seed the global generator. Use current time for non-deterministic results.
    bj_srand((unsigned)bj_get_time());
    bj_info("After srand():");
    for (size_t i = 0; i < 5; ++i) {
        bj_info("\tbj_rand() -> %d", bj_rand());
    }

    // PCG32 API: Each bj_pcg32 is an independent generator with its own state.
    // This allows multiple random streams that don't interfere with each other.
    bj_info("PCG32:");

    // Zero-initialized generators have a default seed. They produce the same
    // sequence every run, which is useful for reproducible testing.
    bj_pcg32 g0 = (bj_pcg32){0};
    bj_info("\tzero-init stream:");
    for (size_t i = 0; i < 3; ++i) {
        bj_info("\t\t%10u", bj_next_pcg32(&g0));
    }

    // Seed with current time for non-deterministic results. The sequence
    // parameter (54) selects which stream to use. Same seed + different sequence
    // = independent random numbers.
    bj_pcg32 g1 = (bj_pcg32){0};
    bj_seed_pcg32(&g1, (uint64_t)bj_get_time(), 54u);
    bj_info("\tseeded with time, seq=54:");
    for (size_t i = 0; i < 3; ++i) {
        bj_info("\t\t%10u", bj_next_pcg32(&g1));
    }

    // Different sequence number creates an independent stream. Even with the
    // same seed, seq=55 produces completely different numbers than seq=54.
    // This is useful for having separate random streams per game entity.
    bj_pcg32 g2 = (bj_pcg32){0};
    bj_seed_pcg32(&g2, (uint64_t)bj_get_time(), 55u);
    bj_info("\tseeded with time, seq=55 (independent):");
    for (size_t i = 0; i < 3; ++i) {
        bj_info("\t\t%10u", bj_next_pcg32(&g2));
    }

    // Query the range of values PCG32 can generate (0 to UINT32_MAX).
    bj_info("\tmin=%u max=%u", bj_min_pcg32(), bj_max_pcg32());

    // Jump-ahead: bj_discard_pcg32() efficiently skips values without generating
    // them. This is useful for synchronizing streams or implementing parallel
    // random number generation.
    bj_pcg32 gA = (bj_pcg32){0};
    bj_pcg32 gB = (bj_pcg32){0};
    bj_seed_pcg32(&gA, 1234u, 999u);
    bj_seed_pcg32(&gB, 1234u, 999u);
    for (size_t i = 0; i < 10; ++i) (void)bj_next_pcg32(&gA);  // Manually advance gA
    bj_discard_pcg32(&gB, 10);  // Efficiently skip 10 values in gB
    bj_info("\tdiscard(10) aligns streams: %u vs %u", bj_next_pcg32(&gA), bj_next_pcg32(&gB));

    // Building larger values: combine multiple 32-bit draws for wider ranges.
    // This is how you'd generate uint64_t random numbers.
    bj_pcg32 g64 = (bj_pcg32){0};
    bj_seed_pcg32(&g64, 0xCAFEBABEULL, 0xDEADULL);
    uint64_t x = pcg32_u64(&g64);
    bj_info("\tu64 from two draws: 0x%016llx", (unsigned long long)x);

    return 0;
}
