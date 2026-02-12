#include "test.h"
#include <banjo/geometry_2d.h>
#include <banjo/math.h>
#include <banjo/vec.h>

////////////////////////////////////////////////////////////////////////////////
// Line Segment Intersection Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(segments_intersecting_cross) {
  struct bj_vec2 a0 = {BJ_F(0.0), BJ_F(0.0)};
  struct bj_vec2 a1 = {BJ_F(10.0), BJ_F(10.0)};
  struct bj_vec2 b0 = {BJ_F(0.0), BJ_F(10.0)};
  struct bj_vec2 b1 = {BJ_F(10.0), BJ_F(0.0)};

  bj_bool hit = bj_check_segments_hit(a0, a1, b0, b1);
  REQUIRE(hit == BJ_TRUE);
}

TEST_CASE(segments_parallel_no_intersection) {
  struct bj_vec2 a0 = {BJ_F(0.0), BJ_F(0.0)};
  struct bj_vec2 a1 = {BJ_F(10.0), BJ_F(0.0)};
  struct bj_vec2 b0 = {BJ_F(0.0), BJ_F(1.0)};
  struct bj_vec2 b1 = {BJ_F(10.0), BJ_F(1.0)};

  bj_bool hit = bj_check_segments_hit(a0, a1, b0, b1);
  REQUIRE(hit == BJ_FALSE);
}

TEST_CASE(segments_touching_at_endpoint) {
  struct bj_vec2 a0 = {BJ_F(0.0), BJ_F(0.0)};
  struct bj_vec2 a1 = {BJ_F(5.0), BJ_F(5.0)};
  struct bj_vec2 b0 = {BJ_F(5.0), BJ_F(5.0)};
  struct bj_vec2 b1 = {BJ_F(10.0), BJ_F(10.0)};

  bj_bool hit = bj_check_segments_hit(a0, a1, b0, b1);
  REQUIRE(hit == BJ_TRUE);
}

TEST_CASE(segments_separate_no_intersection) {
  struct bj_vec2 a0 = {BJ_F(0.0), BJ_F(0.0)};
  struct bj_vec2 a1 = {BJ_F(1.0), BJ_F(1.0)};
  struct bj_vec2 b0 = {BJ_F(5.0), BJ_F(5.0)};
  struct bj_vec2 b1 = {BJ_F(10.0), BJ_F(10.0)};

  bj_bool hit = bj_check_segments_hit(a0, a1, b0, b1);
  REQUIRE(hit == BJ_FALSE);
}

TEST_CASE(segments_collinear_overlapping) {
  struct bj_vec2 a0 = {BJ_F(0.0), BJ_F(0.0)};
  struct bj_vec2 a1 = {BJ_F(5.0), BJ_F(0.0)};
  struct bj_vec2 b0 = {BJ_F(3.0), BJ_F(0.0)};
  struct bj_vec2 b1 = {BJ_F(8.0), BJ_F(0.0)};

  bj_bool hit = bj_check_segments_hit(a0, a1, b0, b1);
  REQUIRE(hit == BJ_TRUE);
}

TEST_CASE(segments_t_intersection) {
  struct bj_vec2 a0 = {BJ_F(5.0), BJ_F(0.0)};
  struct bj_vec2 a1 = {BJ_F(5.0), BJ_F(10.0)};
  struct bj_vec2 b0 = {BJ_F(0.0), BJ_F(5.0)};
  struct bj_vec2 b1 = {BJ_F(10.0), BJ_F(5.0)};

  bj_bool hit = bj_check_segments_hit(a0, a1, b0, b1);
  REQUIRE(hit == BJ_TRUE);
}

////////////////////////////////////////////////////////////////////////////////
// Circle-Segment Intersection Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(circle_segment_intersecting) {
  struct bj_vec2 c = {BJ_F(5.0), BJ_F(5.0)};
  bj_real r = BJ_F(2.0);
  struct bj_vec2 l0 = {BJ_F(0.0), BJ_F(5.0)};
  struct bj_vec2 l1 = {BJ_F(10.0), BJ_F(5.0)};

  bj_bool hit = bj_check_circle_segment_hit(c, r, l0, l1);
  REQUIRE(hit == BJ_TRUE);
}

TEST_CASE(circle_segment_no_intersection) {
  struct bj_vec2 c = {BJ_F(5.0), BJ_F(5.0)};
  bj_real r = BJ_F(1.0);
  struct bj_vec2 l0 = {BJ_F(0.0), BJ_F(0.0)};
  struct bj_vec2 l1 = {BJ_F(10.0), BJ_F(0.0)};

  bj_bool hit = bj_check_circle_segment_hit(c, r, l0, l1);
  REQUIRE(hit == BJ_FALSE);
}

TEST_CASE(circle_segment_tangent) {
  struct bj_vec2 c = {BJ_F(0.0), BJ_F(2.0)};
  bj_real r = BJ_F(2.0);
  struct bj_vec2 l0 = {BJ_F(-5.0), BJ_F(0.0)};
  struct bj_vec2 l1 = {BJ_F(5.0), BJ_F(0.0)};

  bj_bool hit = bj_check_circle_segment_hit(c, r, l0, l1);
  REQUIRE(hit == BJ_TRUE);
}

TEST_CASE(circle_contains_segment_endpoint) {
  struct bj_vec2 c = {BJ_F(0.0), BJ_F(0.0)};
  bj_real r = BJ_F(5.0);
  struct bj_vec2 l0 = {BJ_F(0.0), BJ_F(0.0)};
  struct bj_vec2 l1 = {BJ_F(10.0), BJ_F(0.0)};

  bj_bool hit = bj_check_circle_segment_hit(c, r, l0, l1);
  REQUIRE(hit == BJ_TRUE);
}

TEST_CASE(circle_segment_passes_through) {
  struct bj_vec2 c = {BJ_F(5.0), BJ_F(5.0)};
  bj_real r = BJ_F(3.0);
  struct bj_vec2 l0 = {BJ_F(5.0), BJ_F(0.0)};
  struct bj_vec2 l1 = {BJ_F(5.0), BJ_F(10.0)};

  bj_bool hit = bj_check_circle_segment_hit(c, r, l0, l1);
  REQUIRE(hit == BJ_TRUE);
}

TEST_CASE(circle_zero_radius_point_on_segment) {
  struct bj_vec2 c = {BJ_F(5.0), BJ_F(5.0)};
  bj_real r = BJ_FZERO;
  struct bj_vec2 l0 = {BJ_F(0.0), BJ_F(5.0)};
  struct bj_vec2 l1 = {BJ_F(10.0), BJ_F(5.0)};

  bj_bool hit = bj_check_circle_segment_hit(c, r, l0, l1);
  REQUIRE(hit == BJ_TRUE);
}

TEST_CASE(circle_zero_radius_point_off_segment) {
  struct bj_vec2 c = {BJ_F(5.0), BJ_F(6.0)};
  bj_real r = BJ_FZERO;
  struct bj_vec2 l0 = {BJ_F(0.0), BJ_F(5.0)};
  struct bj_vec2 l1 = {BJ_F(10.0), BJ_F(5.0)};

  bj_bool hit = bj_check_circle_segment_hit(c, r, l0, l1);
  REQUIRE(hit == BJ_FALSE);
}

TEST_CASE(circle_segment_near_miss) {
  struct bj_vec2 c = {BJ_F(0.0), BJ_F(0.0)};
  bj_real r = BJ_F(1.0);
  struct bj_vec2 l0 = {BJ_F(2.0), BJ_F(2.0)};
  struct bj_vec2 l1 = {BJ_F(10.0), BJ_F(2.0)};

  bj_bool hit = bj_check_circle_segment_hit(c, r, l0, l1);
  REQUIRE(hit == BJ_FALSE);
}

int main(int argc, char *argv[]) {
  BEGIN_TESTS(argc, argv);

  // Segment intersection tests
  RUN_TEST(segments_intersecting_cross);
  RUN_TEST(segments_parallel_no_intersection);
  RUN_TEST(segments_touching_at_endpoint);
  RUN_TEST(segments_separate_no_intersection);
  RUN_TEST(segments_collinear_overlapping);
  RUN_TEST(segments_t_intersection);

  // Circle-segment intersection tests
  RUN_TEST(circle_segment_intersecting);
  RUN_TEST(circle_segment_no_intersection);
  RUN_TEST(circle_segment_tangent);
  RUN_TEST(circle_contains_segment_endpoint);
  RUN_TEST(circle_segment_passes_through);
  RUN_TEST(circle_zero_radius_point_on_segment);
  RUN_TEST(circle_zero_radius_point_off_segment);
  RUN_TEST(circle_segment_near_miss);

  END_TESTS();
}
