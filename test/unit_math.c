#include "test.h"
#include <banjo/math.h>

#define NEAR(a, b) (bj_abs((a) - (b)) < BJ_F(0.0001))

////////////////////////////////////////////////////////////////////////////////
// Clamp Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(math_clamp_within_range) {
    bj_real result = bj_clamp(BJ_F(5.0), BJ_F(0.0), BJ_F(10.0));
    REQUIRE(NEAR(result, BJ_F(5.0)));
}

TEST_CASE(math_clamp_below_min) {
    bj_real result = bj_clamp(BJ_F(-5.0), BJ_F(0.0), BJ_F(10.0));
    REQUIRE(NEAR(result, BJ_F(0.0)));
}

TEST_CASE(math_clamp_above_max) {
    bj_real result = bj_clamp(BJ_F(15.0), BJ_F(0.0), BJ_F(10.0));
    REQUIRE(NEAR(result, BJ_F(10.0)));
}

TEST_CASE(math_clamp_at_boundaries) {
    REQUIRE(NEAR(bj_clamp(BJ_F(0.0), BJ_F(0.0), BJ_F(10.0)), BJ_F(0.0)));
    REQUIRE(NEAR(bj_clamp(BJ_F(10.0), BJ_F(0.0), BJ_F(10.0)), BJ_F(10.0)));
}

////////////////////////////////////////////////////////////////////////////////
// Step Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(math_step_below_edge) {
    bj_real result = bj_step(BJ_F(0.5), BJ_F(0.3));
    REQUIRE(NEAR(result, BJ_FZERO));
}

TEST_CASE(math_step_above_edge) {
    bj_real result = bj_step(BJ_F(0.5), BJ_F(0.7));
    REQUIRE(NEAR(result, BJ_F(1.0)));
}

TEST_CASE(math_step_at_edge) {
    bj_real result = bj_step(BJ_F(0.5), BJ_F(0.5));
    REQUIRE(NEAR(result, BJ_F(1.0)));
}

////////////////////////////////////////////////////////////////////////////////
// Smoothstep Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(math_smoothstep_below_e0) {
    bj_real result = bj_smoothstep(BJ_F(0.0), BJ_F(1.0), BJ_F(-0.5));
    REQUIRE(NEAR(result, BJ_FZERO));
}

TEST_CASE(math_smoothstep_above_e1) {
    bj_real result = bj_smoothstep(BJ_F(0.0), BJ_F(1.0), BJ_F(1.5));
    REQUIRE(NEAR(result, BJ_F(1.0)));
}

TEST_CASE(math_smoothstep_at_midpoint) {
    bj_real result = bj_smoothstep(BJ_F(0.0), BJ_F(1.0), BJ_F(0.5));
    REQUIRE(NEAR(result, BJ_F(0.5)));
}

TEST_CASE(math_smoothstep_at_edges) {
    REQUIRE(NEAR(bj_smoothstep(BJ_F(0.0), BJ_F(1.0), BJ_F(0.0)), BJ_FZERO));
    REQUIRE(NEAR(bj_smoothstep(BJ_F(0.0), BJ_F(1.0), BJ_F(1.0)), BJ_F(1.0)));
}

////////////////////////////////////////////////////////////////////////////////
// Fract Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(math_fract_positive) {
    REQUIRE(NEAR(bj_fract(BJ_F(3.75)), BJ_F(0.75)));
    REQUIRE(NEAR(bj_fract(BJ_F(1.25)), BJ_F(0.25)));
}

TEST_CASE(math_fract_integer) {
    REQUIRE(NEAR(bj_fract(BJ_F(5.0)), BJ_FZERO));
}

TEST_CASE(math_fract_negative) {
    // fract(-0.25) = -0.25 - floor(-0.25) = -0.25 - (-1) = 0.75
    REQUIRE(NEAR(bj_fract(BJ_F(-0.25)), BJ_F(0.75)));
}

////////////////////////////////////////////////////////////////////////////////
// Mod Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(math_mod_positive) {
    REQUIRE(NEAR(bj_mod(BJ_F(7.0), BJ_F(3.0)), BJ_F(1.0)));
    REQUIRE(NEAR(bj_mod(BJ_F(6.0), BJ_F(3.0)), BJ_FZERO));
}

TEST_CASE(math_mod_negative_dividend) {
    // bj_mod should return non-negative result
    bj_real result = bj_mod(BJ_F(-1.0), BJ_F(3.0));
    REQUIRE(result >= BJ_FZERO);
    REQUIRE(NEAR(result, BJ_F(2.0)));
}

////////////////////////////////////////////////////////////////////////////////
// Absolute Epsilon Comparison Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(math_real_eq_identical) {
    REQUIRE(bj_real_eq(BJ_F(1.0), BJ_F(1.0)));
}

TEST_CASE(math_real_eq_within_epsilon) {
    bj_real a = BJ_F(1.0);
    bj_real b = a + BJ_EPSILON * BJ_F(0.5);
    REQUIRE(bj_real_eq(a, b));
}

TEST_CASE(math_real_neq_different) {
    REQUIRE(bj_real_neq(BJ_F(1.0), BJ_F(2.0)));
}

TEST_CASE(math_real_lt_basic) {
    REQUIRE(bj_real_lt(BJ_F(1.0), BJ_F(2.0)));
    REQUIRE_FALSE(bj_real_lt(BJ_F(2.0), BJ_F(1.0)));
}

TEST_CASE(math_real_gt_basic) {
    REQUIRE(bj_real_gt(BJ_F(2.0), BJ_F(1.0)));
    REQUIRE_FALSE(bj_real_gt(BJ_F(1.0), BJ_F(2.0)));
}

TEST_CASE(math_real_lte_basic) {
    REQUIRE(bj_real_lte(BJ_F(1.0), BJ_F(2.0)));
    REQUIRE(bj_real_lte(BJ_F(1.0), BJ_F(1.0)));
}

TEST_CASE(math_real_gte_basic) {
    REQUIRE(bj_real_gte(BJ_F(2.0), BJ_F(1.0)));
    REQUIRE(bj_real_gte(BJ_F(1.0), BJ_F(1.0)));
}

TEST_CASE(math_real_cmp_three_way) {
    REQUIRE_EQ(bj_real_cmp(BJ_F(1.0), BJ_F(2.0)), -1);
    REQUIRE_EQ(bj_real_cmp(BJ_F(2.0), BJ_F(1.0)), 1);
    REQUIRE_EQ(bj_real_cmp(BJ_F(1.0), BJ_F(1.0)), 0);
}

////////////////////////////////////////////////////////////////////////////////
// Relative Epsilon Comparison Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(math_real_eq_rel_identical) {
    REQUIRE(bj_real_eq_rel(BJ_F(1000.0), BJ_F(1000.0)));
}

TEST_CASE(math_real_eq_rel_scales_with_magnitude) {
    // For large values, relative epsilon allows more absolute difference
    bj_real big = BJ_F(1000000.0);
    bj_real tiny_diff = big + BJ_EPSILON * big * BJ_F(0.5);
    REQUIRE(bj_real_eq_rel(big, tiny_diff));
}

TEST_CASE(math_real_cmp_rel_three_way) {
    REQUIRE_EQ(bj_real_cmp_rel(BJ_F(1.0), BJ_F(2.0)), -1);
    REQUIRE_EQ(bj_real_cmp_rel(BJ_F(2.0), BJ_F(1.0)), 1);
    REQUIRE_EQ(bj_real_cmp_rel(BJ_F(1.0), BJ_F(1.0)), 0);
}

////////////////////////////////////////////////////////////////////////////////
// Zero Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(math_real_is_zero_exact) {
    REQUIRE(bj_real_is_zero(BJ_FZERO));
}

TEST_CASE(math_real_is_zero_within_epsilon) {
    REQUIRE(bj_real_is_zero(BJ_EPSILON * BJ_F(0.5)));
}

TEST_CASE(math_real_is_zero_false_for_nonzero) {
    REQUIRE_FALSE(bj_real_is_zero(BJ_F(1.0)));
}

TEST_CASE(math_real_snap_zero_snaps) {
    bj_real tiny = BJ_EPSILON * BJ_F(0.1);
    REQUIRE(NEAR(bj_real_snap_zero(tiny), BJ_FZERO));
}

TEST_CASE(math_real_snap_zero_preserves_nonzero) {
    bj_real val = BJ_F(1.0);
    REQUIRE(NEAR(bj_real_snap_zero(val), val));
}

TEST_CASE(math_real_snorm_safe_divides) {
    bj_real result = bj_real_snorm_safe(BJ_F(10.0), BJ_F(2.0));
    REQUIRE(NEAR(result, BJ_F(5.0)));
}

TEST_CASE(math_real_snorm_safe_zero_denominator) {
    bj_real result = bj_real_snorm_safe(BJ_F(10.0), BJ_FZERO);
    REQUIRE(NEAR(result, BJ_FZERO));
}

////////////////////////////////////////////////////////////////////////////////
// Constants Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(math_pi_is_approximately_3_14) {
    REQUIRE(BJ_PI > BJ_F(3.14));
    REQUIRE(BJ_PI < BJ_F(3.15));
}

TEST_CASE(math_tau_is_2_pi) {
    REQUIRE(NEAR(BJ_TAU, BJ_F(2.0) * BJ_PI));
}

TEST_CASE(math_epsilon_is_positive) {
    REQUIRE(BJ_EPSILON > BJ_FZERO);
}

TEST_CASE(math_fzero_is_zero) {
    REQUIRE(NEAR(BJ_FZERO, BJ_F(0.0)));
}

////////////////////////////////////////////////////////////////////////////////
// Trigonometry Wrapper Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(math_sin_cos_identity) {
    // sin^2 + cos^2 = 1
    bj_real angle = BJ_F(0.7);
    bj_real s = bj_sin(angle);
    bj_real c = bj_cos(angle);
    REQUIRE(NEAR(s * s + c * c, BJ_F(1.0)));
}

TEST_CASE(math_sqrt_basic) {
    REQUIRE(NEAR(bj_sqrt(BJ_F(4.0)), BJ_F(2.0)));
    REQUIRE(NEAR(bj_sqrt(BJ_F(9.0)), BJ_F(3.0)));
}

TEST_CASE(math_abs_basic) {
    REQUIRE(NEAR(bj_abs(BJ_F(-5.0)), BJ_F(5.0)));
    REQUIRE(NEAR(bj_abs(BJ_F(5.0)), BJ_F(5.0)));
}

TEST_CASE(math_floor_basic) {
    REQUIRE(NEAR(bj_floor(BJ_F(3.7)), BJ_F(3.0)));
    REQUIRE(NEAR(bj_floor(BJ_F(-3.7)), BJ_F(-4.0)));
}

TEST_CASE(math_round_basic) {
    REQUIRE(NEAR(bj_round(BJ_F(3.4)), BJ_F(3.0)));
    REQUIRE(NEAR(bj_round(BJ_F(3.6)), BJ_F(4.0)));
}

TEST_CASE(sin_of_zero_is_zero) { REQUIRE(NEAR(bj_sin(BJ_FZERO), BJ_FZERO)); }

TEST_CASE(cos_of_zero_is_one) { REQUIRE(NEAR(bj_cos(BJ_FZERO), BJ_F(1.0))); }

TEST_CASE(sin_of_pi_over_2_is_one) {
  REQUIRE(NEAR(bj_sin(BJ_PI / BJ_F(2.0)), BJ_F(1.0)));
}

TEST_CASE(cos_of_pi_is_minus_one) { REQUIRE(NEAR(bj_cos(BJ_PI), BJ_F(-1.0))); }

TEST_CASE(tan_of_pi_over_4_is_one) {
  REQUIRE(NEAR(bj_tan(BJ_PI / BJ_F(4.0)), BJ_F(1.0)));
}

TEST_CASE(trig_identity_sin_squared_plus_cos_squared) {
  bj_real angles[] = {BJ_F(0.0),         BJ_PI / BJ_F(6.0), BJ_PI / BJ_F(4.0),
                      BJ_PI / BJ_F(3.0), BJ_PI / BJ_F(2.0), BJ_PI};

  for (size_t i = 0; i < sizeof(angles) / sizeof(angles[0]); ++i) {
    bj_real s = bj_sin(angles[i]);
    bj_real c = bj_cos(angles[i]);
    bj_real sum = s * s + c * c;
    REQUIRE(NEAR(sum, BJ_F(1.0)));
  }
}

////////////////////////////////////////////////////////////////////////////////
// Clamping
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(clamp_within_range_unchanged) {
  bj_real result = bj_clamp(BJ_F(5.0), BJ_F(0.0), BJ_F(10.0));
  REQUIRE(NEAR(result, BJ_F(5.0)));
}

TEST_CASE(clamp_below_range_returns_min) {
  bj_real result = bj_clamp(BJ_F(-5.0), BJ_F(0.0), BJ_F(10.0));
  REQUIRE(NEAR(result, BJ_F(0.0)));
}

TEST_CASE(clamp_above_range_returns_max) {
  bj_real result = bj_clamp(BJ_F(15.0), BJ_F(0.0), BJ_F(10.0));
  REQUIRE(NEAR(result, BJ_F(10.0)));
}

////////////////////////////////////////////////////////////////////////////////
// Min/Max
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(min_returns_smaller) {
  REQUIRE(NEAR(bj_min(BJ_F(5.0), BJ_F(10.0)), BJ_F(5.0)));
  REQUIRE(NEAR(bj_min(BJ_F(10.0), BJ_F(5.0)), BJ_F(5.0)));
}

TEST_CASE(max_returns_larger) {
  REQUIRE(NEAR(bj_max(BJ_F(5.0), BJ_F(10.0)), BJ_F(10.0)));
  REQUIRE(NEAR(bj_max(BJ_F(10.0), BJ_F(5.0)), BJ_F(10.0)));
}

////////////////////////////////////////////////////////////////////////////////
// Power and Roots
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(sqrt_of_perfect_squares) {
  REQUIRE(NEAR(bj_sqrt(BJ_F(4.0)), BJ_F(2.0)));
  REQUIRE(NEAR(bj_sqrt(BJ_F(9.0)), BJ_F(3.0)));
  REQUIRE(NEAR(bj_sqrt(BJ_F(16.0)), BJ_F(4.0)));
}

TEST_CASE(sqrt_of_one_is_one) { REQUIRE(NEAR(bj_sqrt(BJ_F(1.0)), BJ_F(1.0))); }

TEST_CASE(pow_basic) {
  REQUIRE(NEAR(bj_pow(BJ_F(2.0), BJ_F(3.0)), BJ_F(8.0)));
  REQUIRE(NEAR(bj_pow(BJ_F(3.0), BJ_F(2.0)), BJ_F(9.0)));
}

TEST_CASE(pow_to_zero_is_one) {
  REQUIRE(NEAR(bj_pow(BJ_F(5.0), BJ_FZERO), BJ_F(1.0)));
}

////////////////////////////////////////////////////////////////////////////////
// Absolute Value
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(abs_positive_unchanged) {
  REQUIRE(NEAR(bj_abs(BJ_F(5.0)), BJ_F(5.0)));
}

TEST_CASE(abs_negative_becomes_positive) {
  REQUIRE(NEAR(bj_abs(BJ_F(-5.0)), BJ_F(5.0)));
}

TEST_CASE(abs_zero_is_zero) { REQUIRE(NEAR(bj_abs(BJ_FZERO), BJ_FZERO)); }

////////////////////////////////////////////////////////////////////////////////
// Floor and Round
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(floor_rounds_down) {
  REQUIRE(NEAR(bj_floor(BJ_F(3.7)), BJ_F(3.0)));
  REQUIRE(NEAR(bj_floor(BJ_F(3.2)), BJ_F(3.0)));
}

TEST_CASE(round_to_nearest) {
  REQUIRE(NEAR(bj_round(BJ_F(3.4)), BJ_F(3.0)));
  REQUIRE(NEAR(bj_round(BJ_F(3.6)), BJ_F(4.0)));
}


int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    // Clamp
    RUN_TEST(math_clamp_within_range);
    RUN_TEST(math_clamp_below_min);
    RUN_TEST(math_clamp_above_max);
    RUN_TEST(math_clamp_at_boundaries);

    // Step
    RUN_TEST(math_step_below_edge);
    RUN_TEST(math_step_above_edge);
    RUN_TEST(math_step_at_edge);

    // Smoothstep
    RUN_TEST(math_smoothstep_below_e0);
    RUN_TEST(math_smoothstep_above_e1);
    RUN_TEST(math_smoothstep_at_midpoint);
    RUN_TEST(math_smoothstep_at_edges);

    // Fract
    RUN_TEST(math_fract_positive);
    RUN_TEST(math_fract_integer);
    RUN_TEST(math_fract_negative);

    // Mod
    RUN_TEST(math_mod_positive);
    RUN_TEST(math_mod_negative_dividend);

    // Absolute epsilon
    RUN_TEST(math_real_eq_identical);
    RUN_TEST(math_real_eq_within_epsilon);
    RUN_TEST(math_real_neq_different);
    RUN_TEST(math_real_lt_basic);
    RUN_TEST(math_real_gt_basic);
    RUN_TEST(math_real_lte_basic);
    RUN_TEST(math_real_gte_basic);
    RUN_TEST(math_real_cmp_three_way);

    // Relative epsilon
    RUN_TEST(math_real_eq_rel_identical);
    RUN_TEST(math_real_eq_rel_scales_with_magnitude);
    RUN_TEST(math_real_cmp_rel_three_way);

    // Zero tests
    RUN_TEST(math_real_is_zero_exact);
    RUN_TEST(math_real_is_zero_within_epsilon);
    RUN_TEST(math_real_is_zero_false_for_nonzero);
    RUN_TEST(math_real_snap_zero_snaps);
    RUN_TEST(math_real_snap_zero_preserves_nonzero);
    RUN_TEST(math_real_snorm_safe_divides);
    RUN_TEST(math_real_snorm_safe_zero_denominator);

    // Constants
    RUN_TEST(math_pi_is_approximately_3_14);
    RUN_TEST(math_tau_is_2_pi);
    RUN_TEST(math_epsilon_is_positive);
    RUN_TEST(math_fzero_is_zero);

    // Trigonometry
    RUN_TEST(math_sin_cos_identity);
    RUN_TEST(math_sqrt_basic);
    RUN_TEST(math_abs_basic);
    RUN_TEST(math_floor_basic);
    RUN_TEST(math_round_basic);

    // Trigonometric tests
    RUN_TEST(sin_of_zero_is_zero);
    RUN_TEST(cos_of_zero_is_one);
    RUN_TEST(sin_of_pi_over_2_is_one);
    RUN_TEST(cos_of_pi_is_minus_one);
    RUN_TEST(tan_of_pi_over_4_is_one);
    RUN_TEST(trig_identity_sin_squared_plus_cos_squared);

    // Clamping
    RUN_TEST(clamp_within_range_unchanged);
    RUN_TEST(clamp_below_range_returns_min);
    RUN_TEST(clamp_above_range_returns_max);

    // Min/Max
    RUN_TEST(min_returns_smaller);
    RUN_TEST(max_returns_larger);

    // Power and roots
    RUN_TEST(sqrt_of_perfect_squares);
    RUN_TEST(sqrt_of_one_is_one);
    RUN_TEST(pow_basic);
    RUN_TEST(pow_to_zero_is_one);

    // Absolute value
    RUN_TEST(abs_positive_unchanged);
    RUN_TEST(abs_negative_becomes_positive);
    RUN_TEST(abs_zero_is_zero);

    // Floor and round
    RUN_TEST(floor_rounds_down);
    RUN_TEST(round_to_nearest);

    END_TESTS();
}
