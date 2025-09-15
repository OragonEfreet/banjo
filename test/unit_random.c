#include "test.h"
#include <banjo/random.h>

TEST_CASE(rand_default_zero_seed_is_deterministic) {
    bj_srand(0u);
    int a0 = bj_rand(), a1 = bj_rand(), a2 = bj_rand();

    bj_srand(0u);
    int b0 = bj_rand(), b1 = bj_rand(), b2 = bj_rand();

    REQUIRE_EQ(a0, b0);
    REQUIRE_EQ(a1, b1);
    REQUIRE_EQ(a2, b2);
}

TEST_CASE(rand_same_seed_same_sequence) {
    bj_srand(123456u);
    int s1[8];
    for (int i = 0; i < 8; ++i) s1[i] = bj_rand();

    bj_srand(123456u);
    for (int i = 0; i < 8; ++i) REQUIRE_EQ(s1[i], bj_rand());
}

TEST_CASE(rand_range_is_0_to_RAND_MAX_inclusive) {
    bj_srand(42u);
    for (int i = 0; i < 1000; ++i) {
        int v = bj_rand();
        REQUIRE(v >= 0);
        REQUIRE(v <= BJ_RAND_MAX);
    }
}

TEST_CASE(pcg32_zero_init_is_valid_and_deterministic) {
    bj_pcg32 g1 = {0};
    bj_pcg32 g2 = {0};

    uint32_t a0 = bj_pcg32_next(&g1);
    uint32_t a1 = bj_pcg32_next(&g1);
    uint32_t b0 = bj_pcg32_next(&g2);
    uint32_t b1 = bj_pcg32_next(&g2);

    REQUIRE_EQ(a0, b0);
    REQUIRE_EQ(a1, b1);
}

TEST_CASE(pcg32_seed_reproducibility) {
    bj_pcg32 g1 = {0}, g2 = {0};
    bj_pcg32_seed(&g1, 0x0123456789ABCDEFULL, 0xDEADBEEFCAFEBABEULL);
    bj_pcg32_seed(&g2, 0x0123456789ABCDEFULL, 0xDEADBEEFCAFEBABEULL);

    for (int i = 0; i < 16; ++i) {
        REQUIRE_EQ(bj_pcg32_next(&g1), bj_pcg32_next(&g2));
    }
}

TEST_CASE(pcg32_min_max_contract) {
    REQUIRE_EQ(bj_pcg32_min(), 0u);
    REQUIRE_EQ(bj_pcg32_max(), 0xFFFFFFFFu);

    bj_pcg32 g = {0};
    for (int i = 0; i < 1000; ++i) {
        uint32_t v = bj_pcg32_next(&g);
        REQUIRE(v >= bj_pcg32_min());
        REQUIRE(v <= bj_pcg32_max());
    }
}

TEST_CASE(pcg32_discard_equivalence) {
    bj_pcg32 g1 = {0}, g2 = {0};
    bj_pcg32_seed(&g1, 1234, 999);
    bj_pcg32_seed(&g2, 1234, 999);

    for (uint64_t i = 0; i < 10; ++i) (void)bj_pcg32_next(&g1);
    bj_pcg32_discard(&g2, 10);

    for (uint64_t i = 0; i < 1000; ++i) (void)bj_pcg32_next(&g1);
    bj_pcg32_discard(&g2, 1000);

    REQUIRE_EQ(bj_pcg32_next(&g1), bj_pcg32_next(&g2));
    REQUIRE_EQ(bj_pcg32_next(&g1), bj_pcg32_next(&g2));
}

TEST_CASE(pcg32_streams_differ_with_different_seq) {
    bj_pcg32 a = {0}, b = {0};
    bj_pcg32_seed(&a, 42, 1);
    bj_pcg32_seed(&b, 42, 2);

    uint32_t a0 = bj_pcg32_next(&a);
    uint32_t b0 = bj_pcg32_next(&b);
    REQUIRE_NEQ(a0, b0);
}

int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    RUN_TEST(rand_default_zero_seed_is_deterministic);
    RUN_TEST(rand_same_seed_same_sequence);
    RUN_TEST(rand_range_is_0_to_RAND_MAX_inclusive);
    RUN_TEST(pcg32_zero_init_is_valid_and_deterministic);
    RUN_TEST(pcg32_seed_reproducibility);
    RUN_TEST(pcg32_min_max_contract);
    RUN_TEST(pcg32_discard_equivalence);
    RUN_TEST(pcg32_streams_differ_with_different_seq);

    END_TESTS();
}
