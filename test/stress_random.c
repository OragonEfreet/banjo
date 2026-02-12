#include "test.h"
#include <banjo/random.h>
#include <banjo/time.h>

TEST_CASE(random_pcg32_perf_stress) {
  struct bj_pcg32 rng;
  bj_seed_pcg32(&rng, 12345, 67890);

  const uint32_t count = 1000000; // 1 million numbers
  uint32_t sum = 0;

  // Test performance and basic variety
  for (uint32_t i = 0; i < count; ++i) {
    sum += bj_next_pcg32(&rng);
  }

  // Use the sum to ensure loop isn't optimized away
  REQUIRE(sum != 0);
}

TEST_CASE(random_distribution_stress) {
  struct bj_pcg32 rng;
  bj_seed_pcg32(&rng, 42, 42);

  const int count = 100000;
  int32_t low = -100, high = 100;

  for (int i = 0; i < count; ++i) {
    int32_t val =
        bj_uniform_int32_distribution(bj_pcg32_generator, &rng, low, high);
    REQUIRE(val >= low);
    REQUIRE(val <= high);
  }
}

TEST_CASE(random_normal_stress) {
  struct bj_pcg32 rng;
  bj_seed_pcg32(&rng, 1, 1);

  const int count = 10000;
  for (int i = 0; i < count; ++i) {
    float val =
        bj_normal_float_distribution(bj_pcg32_generator, &rng, 0.0f, 1.0f);
    // Just verify it doesn't crash and returns valid floats
    REQUIRE(val == val); // Not NaN
  }
}

int main(int argc, char *argv[]) {
  BEGIN_TESTS(argc, argv);

  RUN_TEST(random_pcg32_perf_stress);
  RUN_TEST(random_distribution_stress);
  RUN_TEST(random_normal_stress);

  END_TESTS();
}
