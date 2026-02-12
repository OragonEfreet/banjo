#include "test.h"
#include <banjo/bitmap.h>
#include <banjo/draw.h>
#include <banjo/pixel.h>
#include <banjo/vec.h>

TEST_CASE(draw_line_horizontal) {
  // Create a 10x10 bitmap, stride=0 for auto-compute
  struct bj_bitmap *bmp = bj_create_bitmap(10, 10, BJ_PIXEL_MODE_XRGB8888, 0);
  REQUIRE(bmp != NULL);

  uint32_t color = 0xFFFFFFFF; // White
  bj_draw_line(bmp, 0, 0, 9, 0, color);

  // Check first and last pixel of the line using public API
  uint32_t p0 = bj_bitmap_pixel(bmp, 0, 0);
  uint32_t p9 = bj_bitmap_pixel(bmp, 9, 0);

  REQUIRE_EQ(p0, color);
  REQUIRE_EQ(p9, color);

  bj_destroy_bitmap(bmp);
}

TEST_CASE(draw_filled_rectangle_full) {
  struct bj_bitmap *bmp = bj_create_bitmap(10, 10, BJ_PIXEL_MODE_XRGB8888, 0);
  REQUIRE(bmp != NULL);

  struct bj_rect area = {0, 0, 10, 10};
  uint32_t color = 0xFF00FF00; // Green
  bj_draw_filled_rectangle(bmp, &area, color);

  // Check points inside using API
  REQUIRE_EQ(bj_bitmap_pixel(bmp, 0, 0), color);
  REQUIRE_EQ(bj_bitmap_pixel(bmp, 5, 5), color);
  REQUIRE_EQ(bj_bitmap_pixel(bmp, 9, 9), color);

  bj_destroy_bitmap(bmp);
}

TEST_CASE(draw_circle_basic) {
  struct bj_bitmap *bmp = bj_create_bitmap(20, 20, BJ_PIXEL_MODE_XRGB8888, 0);
  REQUIRE(bmp != NULL);

  uint32_t color = 0xFFFF0000; // Red
  bj_draw_circle(bmp, 10, 10, 5, color);

  // Centre should still be empty (black=0)
  REQUIRE_EQ(bj_bitmap_pixel(bmp, 10, 10), 0);

  // Points on the circle should be red
  // At r=5: (15, 10), (5, 10), (10, 15), (10, 5)
  REQUIRE_EQ(bj_bitmap_pixel(bmp, 15, 10), color);
  REQUIRE_EQ(bj_bitmap_pixel(bmp, 5, 10), color);
  REQUIRE_EQ(bj_bitmap_pixel(bmp, 10, 15), color);
  REQUIRE_EQ(bj_bitmap_pixel(bmp, 10, 5), color);

  bj_destroy_bitmap(bmp);
}

TEST_CASE(draw_polyline_loop) {
  struct bj_bitmap *bmp = bj_create_bitmap(10, 10, BJ_PIXEL_MODE_XRGB8888, 0);
  REQUIRE(bmp != NULL);

  int x[] = {2, 7, 7, 2};
  int y[] = {2, 2, 7, 7};
  uint32_t color = 0xFF0000FF; // Blue

  bj_draw_polyline(bmp, 4, x, y, BJ_TRUE, color);

  // Check vertices
  REQUIRE_EQ(bj_bitmap_pixel(bmp, 2, 2), color);
  REQUIRE_EQ(bj_bitmap_pixel(bmp, 7, 2), color);
  REQUIRE_EQ(bj_bitmap_pixel(bmp, 7, 7), color);
  REQUIRE_EQ(bj_bitmap_pixel(bmp, 2, 7), color);

  bj_destroy_bitmap(bmp);
}

int main(int argc, char *argv[]) {
  BEGIN_TESTS(argc, argv);

  RUN_TEST(draw_line_horizontal);
  RUN_TEST(draw_filled_rectangle_full);
  RUN_TEST(draw_circle_basic);
  RUN_TEST(draw_polyline_loop);

  END_TESTS();
}
