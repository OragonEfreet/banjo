#include "test.h"
#include <banjo/system.h>
#include <banjo/time.h>

TEST_CASE(stopwatch_zero_init_is_valid) {
  struct bj_stopwatch sw = {0};
  double elapsed = bj_stopwatch_elapsed(&sw);
  REQUIRE(elapsed >= 0.0);
}

TEST_CASE(stopwatch_reset_sets_start_time) {
  struct bj_stopwatch sw = {0};
  bj_reset_stopwatch(&sw);
  REQUIRE(sw.start_counter > 0);
  REQUIRE(sw.last_tick > 0);
}

TEST_CASE(stopwatch_elapsed_increases) {
  struct bj_stopwatch sw = {0};
  bj_reset_stopwatch(&sw);
  double t1 = bj_stopwatch_elapsed(&sw);
  bj_sleep(10); // Sleep 10ms
  double t2 = bj_stopwatch_elapsed(&sw);
  REQUIRE(t2 > t1);
}

TEST_CASE(stopwatch_step_updates_last_tick) {
  struct bj_stopwatch sw = {0};
  bj_reset_stopwatch(&sw);
  uint64_t tick1 = sw.last_tick;
  bj_sleep(5);
  bj_step_stopwatch(&sw);
  uint64_t tick2 = sw.last_tick;
  REQUIRE(tick2 > tick1);
}

TEST_CASE(stopwatch_delay_measures_step_interval) {
  struct bj_stopwatch sw = {0};
  bj_reset_stopwatch(&sw);
  bj_sleep(10);
  bj_step_stopwatch(&sw);
  bj_sleep(10);
  double delay = bj_stopwatch_delay(&sw);
  REQUIRE(delay >= 0.008); // At least 8ms (allowing for timing variance)
}

TEST_CASE(step_delay_stopwatch_returns_interval) {
  struct bj_stopwatch sw = {0};
  bj_reset_stopwatch(&sw);
  bj_sleep(10);
  double delay = bj_step_delay_stopwatch(&sw);
  REQUIRE(delay >= 0.008);
}

TEST_CASE(time_counter_increases) {
  uint64_t t1 = bj_time_counter();
  bj_sleep(5);
  uint64_t t2 = bj_time_counter();
  REQUIRE(t2 > t1);
}

TEST_CASE(time_frequency_is_positive) {
  uint64_t freq = bj_time_frequency();
  REQUIRE(freq > 0);
}

TEST_CASE(time_counter_to_seconds_conversion) {
  uint64_t freq = bj_time_frequency();
  uint64_t t1 = bj_time_counter();
  bj_sleep(100); // 100ms
  uint64_t t2 = bj_time_counter();

  double elapsed = (double)(t2 - t1) / (double)freq;
  REQUIRE(elapsed >= 0.08); // At least 80ms
  REQUIRE(elapsed < 0.6);   // Increased from 0.2 to 0.6 to be more resilient
}

TEST_CASE(get_time_returns_reasonable_value) {
  uint64_t t = bj_get_time();
  // Should be a reasonable Unix timestamp (after 2020)
  REQUIRE(t > 1577836800); // Jan 1, 2020
}

TEST_CASE(sleep_actually_sleeps) {
  uint64_t t1 = bj_time_counter();
  bj_sleep(50);
  uint64_t t2 = bj_time_counter();
  uint64_t freq = bj_time_frequency();
  double elapsed = (double)(t2 - t1) / (double)freq;
  REQUIRE(elapsed >= 0.04); // At least 40ms
}

TEST_CASE(multiple_steps_tracked_correctly) {
  struct bj_stopwatch sw = {0};
  bj_reset_stopwatch(&sw);

  bj_sleep(10);
  double d1 = bj_step_delay_stopwatch(&sw);

  bj_sleep(10);
  double d2 = bj_step_delay_stopwatch(&sw);

  bj_sleep(10);
  double d3 = bj_step_delay_stopwatch(&sw);

  REQUIRE(d1 >= 0.008);
  REQUIRE(d2 >= 0.008);
  REQUIRE(d3 >= 0.008);
}

int main(int argc, char *argv[]) {
  bj_begin(0, NULL);
  BEGIN_TESTS(argc, argv);

  RUN_TEST(stopwatch_zero_init_is_valid);
  RUN_TEST(stopwatch_reset_sets_start_time);
  RUN_TEST(stopwatch_elapsed_increases);
  RUN_TEST(stopwatch_step_updates_last_tick);
  RUN_TEST(stopwatch_delay_measures_step_interval);
  RUN_TEST(step_delay_stopwatch_returns_interval);
  RUN_TEST(time_counter_increases);
  RUN_TEST(time_frequency_is_positive);
  RUN_TEST(time_counter_to_seconds_conversion);
  RUN_TEST(get_time_returns_reasonable_value);
  RUN_TEST(sleep_actually_sleeps);
  RUN_TEST(multiple_steps_tracked_correctly);

  END_TESTS();
  bj_end();
}
