#include "test.h"
#include <banjo/pixel.h>

TEST_CASE(pixel_mode_encoding_decoding) {
  // Test that the macros correctly pack and unpack values
  enum bj_pixel_mode mode = BJ_PIXEL_MODE_MAKE(
      32, BJ_PIXEL_TYPE_BITFIELD, BJ_PIXEL_LAYOUT_8888, BJ_PIXEL_ORDER_RGBA);

  REQUIRE_EQ(BJ_PIXEL_GET_BPP(mode), 32);
  REQUIRE_EQ(BJ_PIXEL_GET_TYPE(mode), BJ_PIXEL_TYPE_BITFIELD);
  REQUIRE_EQ(BJ_PIXEL_GET_LAYOUT(mode), BJ_PIXEL_LAYOUT_8888);
  REQUIRE_EQ(BJ_PIXEL_GET_ORDER(mode), BJ_PIXEL_ORDER_RGBA);
}

TEST_CASE(pixel_mode_indexed) {
  enum bj_pixel_mode mode = BJ_PIXEL_MODE_MAKE_INDEXED(8);
  REQUIRE_EQ(BJ_PIXEL_GET_BPP(mode), 8);
  REQUIRE_EQ(BJ_PIXEL_GET_TYPE(mode), BJ_PIXEL_TYPE_INDEX);
}

TEST_CASE(pixel_value_roundtrip_rgba8888) {
  enum bj_pixel_mode mode = BJ_PIXEL_MODE_XRGB8888;
  uint8_t r = 0xAA, g = 0xBB, b = 0xCC;
  uint32_t val = bj_get_pixel_value(mode, r, g, b);

  uint8_t out_r, out_g, out_b;
  bj_make_pixel_rgb(mode, val, &out_r, &out_g, &out_b);

  REQUIRE_EQ(out_r, r);
  REQUIRE_EQ(out_g, g);
  REQUIRE_EQ(out_b, b);
}

TEST_CASE(pixel_value_roundtrip_rgb565) {
  enum bj_pixel_mode mode = BJ_PIXEL_MODE_RGB565;
  // Use values that fit in 5 and 6 bits exactly to avoid rounding issues in the
  // test
  uint8_t r = 0xF8; // 11111000
  uint8_t g = 0xFC; // 11111100
  uint8_t b = 0xF8; // 11111000

  uint32_t val = bj_get_pixel_value(mode, r, g, b);

  uint8_t out_r, out_g, out_b;
  bj_make_pixel_rgb(mode, val, &out_r, &out_g, &out_b);

  // We expect some precision loss, but it should be close
  REQUIRE(out_r >= 0xF0);
  REQUIRE(out_g >= 0xF8);
  REQUIRE(out_b >= 0xF0);
}

TEST_CASE(compute_pixel_mode_xrgb8888) {
  // Typical 32-bit XRGB masks
  uint32_t r_mask = 0x00FF0000;
  uint32_t g_mask = 0x0000FF00;
  uint32_t b_mask = 0x000000FF;

  int mode = bj_compute_pixel_mode(32, r_mask, g_mask, b_mask);
  REQUIRE_EQ((enum bj_pixel_mode)mode, BJ_PIXEL_MODE_XRGB8888);
}

TEST_CASE(bitmap_stride_calculation) {
  // 32bpp: 10 pixels * 4 bytes = 40 bytes. Already 4-byte aligned.
  REQUIRE_EQ(bj_compute_bitmap_stride(10, BJ_PIXEL_MODE_XRGB8888), 40);

  // 24bpp: 10 pixels * 3 bytes = 30 bytes. Aligned to 4 = 32 bytes.
  REQUIRE_EQ(bj_compute_bitmap_stride(10, BJ_PIXEL_MODE_BGR24), 32);

  // 16bpp: 5 pixels * 2 bytes = 10 bytes. Aligned to 4 = 12 bytes.
  REQUIRE_EQ(bj_compute_bitmap_stride(5, BJ_PIXEL_MODE_RGB565), 12);
}

int main(int argc, char *argv[]) {
  BEGIN_TESTS(argc, argv);

  RUN_TEST(pixel_mode_encoding_decoding);
  RUN_TEST(pixel_mode_indexed);
  RUN_TEST(pixel_value_roundtrip_rgba8888);
  RUN_TEST(pixel_value_roundtrip_rgb565);
  RUN_TEST(compute_pixel_mode_xrgb8888);
  RUN_TEST(bitmap_stride_calculation);

  END_TESTS();
}
