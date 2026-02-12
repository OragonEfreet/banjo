#include "test.h"
#include <banjo/random.h>

#define SAMPLE_SIZE 1000
#define CHI_SQUARE_BINS 10

////////////////////////////////////////////////////////////////////////////////
// Statistical Property Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(rand_distribution_uniformity) {
  int bins[CHI_SQUARE_BINS] = {0};
  bj_srand(12345);

  // Generate samples and count into bins
  for (int i = 0; i < SAMPLE_SIZE; ++i) {
    int val = bj_rand();
    int bin = (val * CHI_SQUARE_BINS) / (BJ_RAND_MAX + 1);
    if (bin >= 0 && bin < CHI_SQUARE_BINS) {
      bins[bin]++;
    }
  }

  // Check that each bin has a reasonable count
  // Expected count per bin: SAMPLE_SIZE / CHI_SQUARE_BINS = 100
  // Allow 50-150 (50% deviation)
  int expected = SAMPLE_SIZE / CHI_SQUARE_BINS;
  for (int i = 0; i < CHI_SQUARE_BINS; ++i) {
    REQUIRE(bins[i] > expected / 2);
    REQUIRE(bins[i] < expected * 2);
  }
}

TEST_CASE(pcg32_distribution_uniformity) {
  int bins[CHI_SQUARE_BINS] = {0};
  struct bj_pcg32 gen = {0};
  bj_seed_pcg32(&gen, 12345, 67890);

  for (int i = 0; i < SAMPLE_SIZE; ++i) {
    uint32_t val = bj_next_pcg32(&gen);
    int bin = (int)((val / (double)0xFFFFFFFFu) * CHI_SQUARE_BINS);
    if (bin >= CHI_SQUARE_BINS)
      bin = CHI_SQUARE_BINS - 1;
    bins[bin]++;
  }

  int expected = SAMPLE_SIZE / CHI_SQUARE_BINS;
  for (int i = 0; i < CHI_SQUARE_BINS; ++i) {
    REQUIRE(bins[i] > expected / 2);
    REQUIRE(bins[i] < expected * 2);
  }
}

TEST_CASE(rand_mean_near_midpoint) {
  bj_srand(99999);
  double sum = 0.0;

  for (int i = 0; i < SAMPLE_SIZE; ++i) {
    sum += bj_rand();
  }

  double mean = sum / SAMPLE_SIZE;
  double expected_mean = BJ_RAND_MAX / 2.0;

  // Mean should be within 10% of expected
  REQUIRE(mean > expected_mean * 0.9);
  REQUIRE(mean < expected_mean * 1.1);
}

TEST_CASE(pcg32_mean_near_midpoint) {
  struct bj_pcg32 gen = {0};
  bj_seed_pcg32(&gen, 99999, 11111);
  double sum = 0.0;

  for (int i = 0; i < SAMPLE_SIZE; ++i) {
    sum += bj_next_pcg32(&gen);
  }

  double mean = sum / SAMPLE_SIZE;
  double expected_mean = 0xFFFFFFFFu / 2.0;

  REQUIRE(mean > expected_mean * 0.9);
  REQUIRE(mean < expected_mean * 1.1);
}

TEST_CASE(rand_no_obvious_correlation) {
  bj_srand(55555);
  int prev = bj_rand();
  int same_parity_count = 0;

  // Check that consecutive values don't have obvious patterns
  for (int i = 0; i < 100; ++i) {
    int curr = bj_rand();
    if ((prev % 2) == (curr % 2)) {
      same_parity_count++;
    }
    prev = curr;
  }

  // Should be roughly 50% same parity (allow 30-70%)
  REQUIRE(same_parity_count > 30);
  REQUIRE(same_parity_count < 70);
}

TEST_CASE(pcg32_no_obvious_correlation) {
  struct bj_pcg32 gen = {0};
  bj_seed_pcg32(&gen, 55555, 22222);
  uint32_t prev = bj_next_pcg32(&gen);
  int same_parity_count = 0;

  for (int i = 0; i < 100; ++i) {
    uint32_t curr = bj_next_pcg32(&gen);
    if ((prev % 2) == (curr % 2)) {
      same_parity_count++;
    }
    prev = curr;
  }

  REQUIRE(same_parity_count > 30);
  REQUIRE(same_parity_count < 70);
}

TEST_CASE(rand_different_seeds_different_sequences) {
  bj_srand(1);
  int seq1[10];
  for (int i = 0; i < 10; ++i)
    seq1[i] = bj_rand();

  bj_srand(2);
  int seq2[10];
  for (int i = 0; i < 10; ++i)
    seq2[i] = bj_rand();

  // At least some values should differ
  int diff_count = 0;
  for (int i = 0; i < 10; ++i) {
    if (seq1[i] != seq2[i])
      diff_count++;
  }

  REQUIRE(diff_count > 0);
}

TEST_CASE(pcg32_different_seeds_different_sequences) {
  struct bj_pcg32 gen1 = {0}, gen2 = {0};
  bj_seed_pcg32(&gen1, 1, 1);
  bj_seed_pcg32(&gen2, 2, 2);

  int diff_count = 0;
  for (int i = 0; i < 10; ++i) {
    if (bj_next_pcg32(&gen1) != bj_next_pcg32(&gen2)) {
      diff_count++;
    }
  }

  REQUIRE(diff_count > 0);
}

TEST_CASE(rand_range_coverage) {
  bj_srand(77777);
  int min_val = BJ_RAND_MAX;
  int max_val = 0;

  for (int i = 0; i < SAMPLE_SIZE; ++i) {
    int val = bj_rand();
    if (val < min_val)
      min_val = val;
    if (val > max_val)
      max_val = val;
  }

  // Should cover a significant portion of the range
  REQUIRE(min_val < BJ_RAND_MAX / 10);
  REQUIRE(max_val > BJ_RAND_MAX * 9 / 10);
}

TEST_CASE(pcg32_range_coverage) {
  struct bj_pcg32 gen = {0};
  bj_seed_pcg32(&gen, 77777, 33333);
  uint32_t min_val = 0xFFFFFFFFu;
  uint32_t max_val = 0;

  for (int i = 0; i < SAMPLE_SIZE; ++i) {
    uint32_t val = bj_next_pcg32(&gen);
    if (val < min_val)
      min_val = val;
    if (val > max_val)
      max_val = val;
  }

  REQUIRE(min_val < 0xFFFFFFFFu / 10);
  REQUIRE(max_val > 0xFFFFFFFFu / 10 * 9);
}

int main(int argc, char *argv[]) {
  BEGIN_TESTS(argc, argv);

  RUN_TEST(rand_distribution_uniformity);
  RUN_TEST(pcg32_distribution_uniformity);
  RUN_TEST(rand_mean_near_midpoint);
  RUN_TEST(pcg32_mean_near_midpoint);
  RUN_TEST(rand_no_obvious_correlation);
  RUN_TEST(pcg32_no_obvious_correlation);
  RUN_TEST(rand_different_seeds_different_sequences);
  RUN_TEST(pcg32_different_seeds_different_sequences);
  RUN_TEST(rand_range_coverage);
  RUN_TEST(pcg32_range_coverage);

  END_TESTS();
}
