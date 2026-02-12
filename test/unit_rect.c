#include "test.h"
#include <banjo/rect.h>

TEST_CASE(rect_intersection_full_overlap) {
  struct bj_rect a = {0, 0, 10, 10};
  struct bj_rect b = {0, 0, 10, 10};
  struct bj_rect res;

  bj_bool intersects = bj_rect_intersection(&a, &b, &res);

  REQUIRE(intersects == BJ_TRUE);
  REQUIRE_EQ(res.x, 0);
  REQUIRE_EQ(res.y, 0);
  REQUIRE_EQ(res.w, 10);
  REQUIRE_EQ(res.h, 10);
}

TEST_CASE(rect_intersection_partial_overlap) {
  struct bj_rect a = {0, 0, 10, 10};
  struct bj_rect b = {5, 5, 10, 10};
  struct bj_rect res;

  bj_bool intersects = bj_rect_intersection(&a, &b, &res);

  REQUIRE(intersects == BJ_TRUE);
  REQUIRE_EQ(res.x, 5);
  REQUIRE_EQ(res.y, 5);
  REQUIRE_EQ(res.w, 5);
  REQUIRE_EQ(res.h, 5);
}

TEST_CASE(rect_intersection_no_overlap) {
  struct bj_rect a = {0, 0, 5, 5};
  struct bj_rect b = {10, 10, 5, 5};

  bj_bool intersects = bj_rect_intersection(&a, &b, NULL);
  REQUIRE(intersects == BJ_FALSE);
}

TEST_CASE(rect_intersection_touching_edges) {
  // Rectangles touching but not overlapping inside
  struct bj_rect a = {0, 0, 5, 5};
  struct bj_rect b = {5, 0, 5, 5};

  // Depending on implementation, touching edges might be considered
  // intersection or not. Usually, in pixel-coord systems, it's [x, x+w), so (5,
  // 0) is OUTSIDE rect A.
  bj_bool intersects = bj_rect_intersection(&a, &b, NULL);
  REQUIRE(intersects == BJ_FALSE);
}

TEST_CASE(rect_intersection_null_result) {
  struct bj_rect a = {0, 0, 10, 10};
  struct bj_rect b = {5, 5, 10, 10};

  bj_bool intersects = bj_rect_intersection(&a, &b, NULL);
  REQUIRE(intersects == BJ_TRUE);
}

int main(int argc, char *argv[]) {
  BEGIN_TESTS(argc, argv);

  RUN_TEST(rect_intersection_full_overlap);
  RUN_TEST(rect_intersection_partial_overlap);
  RUN_TEST(rect_intersection_no_overlap);
  RUN_TEST(rect_intersection_touching_edges);
  RUN_TEST(rect_intersection_null_result);

  END_TESTS();
}
