#include "test.h"
#include <banjo/bitmap.h>
#include <banjo/memory.h>

////////////////////////////////////////////////////////////////////////////////
// Creation Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(bitmap_create_dimensions_match) {
    struct bj_bitmap* bmp = bj_create_bitmap(100, 50, BJ_PIXEL_MODE_XRGB8888, 0);
    REQUIRE_VALUE(bmp);

    REQUIRE_EQ(bj_bitmap_width(bmp), 100);
    REQUIRE_EQ(bj_bitmap_height(bmp), 50);
    REQUIRE_EQ(bj_bitmap_mode(bmp), BJ_PIXEL_MODE_XRGB8888);

    bj_destroy_bitmap(bmp);
}

TEST_CASE(bitmap_create_different_modes) {
    struct bj_bitmap* bmp1 = bj_create_bitmap(10, 10, BJ_PIXEL_MODE_RGB565, 0);
    struct bj_bitmap* bmp2 = bj_create_bitmap(10, 10, BJ_PIXEL_MODE_BGR24, 0);
    struct bj_bitmap* bmp3 = bj_create_bitmap(10, 10, BJ_PIXEL_MODE_XRGB8888, 0);

    REQUIRE_VALUE(bmp1);
    REQUIRE_VALUE(bmp2);
    REQUIRE_VALUE(bmp3);

    REQUIRE_EQ(bj_bitmap_mode(bmp1), BJ_PIXEL_MODE_RGB565);
    REQUIRE_EQ(bj_bitmap_mode(bmp2), BJ_PIXEL_MODE_BGR24);
    REQUIRE_EQ(bj_bitmap_mode(bmp3), BJ_PIXEL_MODE_XRGB8888);

    bj_destroy_bitmap(bmp1);
    bj_destroy_bitmap(bmp2);
    bj_destroy_bitmap(bmp3);
}

TEST_CASE(bitmap_allocate_and_init) {
    struct bj_bitmap* bmp = bj_allocate_bitmap();
    REQUIRE_VALUE(bmp);

    bmp = bj_init_bitmap(bmp, 0, 32, 32, BJ_PIXEL_MODE_XRGB8888, 0);
    REQUIRE_VALUE(bmp);
    REQUIRE_EQ(bj_bitmap_width(bmp), 32);
    REQUIRE_EQ(bj_bitmap_height(bmp), 32);

    bj_reset_bitmap(bmp);
    bj_free(bmp);
}

TEST_CASE(bitmap_create_from_pixels) {
    // Create a small pixel buffer
    uint32_t pixels[4] = {0xFF0000FF, 0x00FF00FF, 0x0000FFFF, 0xFFFFFFFF};

    struct bj_bitmap* bmp = bj_create_bitmap_from_pixels(pixels, 2, 2, BJ_PIXEL_MODE_XRGB8888, 0);
    REQUIRE_VALUE(bmp);

    REQUIRE_EQ(bj_bitmap_width(bmp), 2);
    REQUIRE_EQ(bj_bitmap_height(bmp), 2);

    // Pixels should point to our buffer
    REQUIRE_EQ(bj_bitmap_pixels(bmp), pixels);

    bj_destroy_bitmap(bmp);
    // Note: pixels buffer is NOT freed by destroy since we provided it
}

TEST_CASE(bitmap_create_from_pixels_null_returns_null) {
    struct bj_bitmap* bmp = bj_create_bitmap_from_pixels(0, 10, 10, BJ_PIXEL_MODE_XRGB8888, 0);
    REQUIRE_NULL(bmp);
}

////////////////////////////////////////////////////////////////////////////////
// Copy and Convert Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(bitmap_copy_is_independent) {
    struct bj_bitmap* original = bj_create_bitmap(10, 10, BJ_PIXEL_MODE_XRGB8888, 0);
    REQUIRE_VALUE(original);

    // Set a pixel
    uint32_t color = bj_make_bitmap_pixel(original, 255, 0, 0);
    bj_put_pixel(original, 5, 5, color);

    struct bj_bitmap* copy = bj_copy_bitmap(original);
    REQUIRE_VALUE(copy);

    // Dimensions should match
    REQUIRE_EQ(bj_bitmap_width(copy), bj_bitmap_width(original));
    REQUIRE_EQ(bj_bitmap_height(copy), bj_bitmap_height(original));
    REQUIRE_EQ(bj_bitmap_mode(copy), bj_bitmap_mode(original));

    // Pixel data should be copied
    REQUIRE_EQ(bj_bitmap_pixel(copy, 5, 5), bj_bitmap_pixel(original, 5, 5));

    // But buffers should be independent
    REQUIRE_NEQ(bj_bitmap_pixels(copy), bj_bitmap_pixels(original));

    bj_destroy_bitmap(original);
    bj_destroy_bitmap(copy);
}

TEST_CASE(bitmap_copy_null_returns_null) {
    struct bj_bitmap* copy = bj_copy_bitmap(0);
    REQUIRE_NULL(copy);
}

/* TEST_CASE(bitmap_convert_preserves_dimensions) { */
/*     struct bj_bitmap* original = bj_create_bitmap(20, 15, BJ_PIXEL_MODE_XRGB8888, 0); */
/*     REQUIRE_VALUE(original); */

/*     struct bj_bitmap* converted = bj_convert_bitmap(original, BJ_PIXEL_MODE_RGB565); */
/*     REQUIRE_VALUE(converted); */

/*     REQUIRE_EQ(bj_bitmap_width(converted), 20); */
/*     REQUIRE_EQ(bj_bitmap_height(converted), 15); */
/*     REQUIRE_EQ(bj_bitmap_mode(converted), BJ_PIXEL_MODE_RGB565); */

/*     bj_destroy_bitmap(original); */
/*     bj_destroy_bitmap(converted); */
/* } */

TEST_CASE(bitmap_convert_same_mode_copies) {
    struct bj_bitmap* original = bj_create_bitmap(10, 10, BJ_PIXEL_MODE_XRGB8888, 0);
    REQUIRE_VALUE(original);

    struct bj_bitmap* converted = bj_convert_bitmap(original, BJ_PIXEL_MODE_XRGB8888);
    REQUIRE_VALUE(converted);

    // Should be a copy
    REQUIRE_NEQ(converted, original);
    REQUIRE_NEQ(bj_bitmap_pixels(converted), bj_bitmap_pixels(original));

    bj_destroy_bitmap(original);
    bj_destroy_bitmap(converted);
}

////////////////////////////////////////////////////////////////////////////////
// Pixel Access Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(bitmap_put_pixel_get_pixel_roundtrip) {
    struct bj_bitmap* bmp = bj_create_bitmap(10, 10, BJ_PIXEL_MODE_XRGB8888, 0);
    REQUIRE_VALUE(bmp);

    uint32_t red = bj_make_bitmap_pixel(bmp, 255, 0, 0);
    uint32_t green = bj_make_bitmap_pixel(bmp, 0, 255, 0);
    uint32_t blue = bj_make_bitmap_pixel(bmp, 0, 0, 255);

    bj_put_pixel(bmp, 0, 0, red);
    bj_put_pixel(bmp, 1, 1, green);
    bj_put_pixel(bmp, 2, 2, blue);

    REQUIRE_EQ(bj_bitmap_pixel(bmp, 0, 0), red);
    REQUIRE_EQ(bj_bitmap_pixel(bmp, 1, 1), green);
    REQUIRE_EQ(bj_bitmap_pixel(bmp, 2, 2), blue);

    bj_destroy_bitmap(bmp);
}

TEST_CASE(bitmap_make_rgb_extracts_components) {
    struct bj_bitmap* bmp = bj_create_bitmap(10, 10, BJ_PIXEL_MODE_XRGB8888, 0);
    REQUIRE_VALUE(bmp);

    uint32_t color = bj_make_bitmap_pixel(bmp, 100, 150, 200);
    bj_put_pixel(bmp, 5, 5, color);

    uint8_t r, g, b;
    bj_make_bitmap_rgb(bmp, 5, 5, &r, &g, &b);

    REQUIRE_EQ(r, 100);
    REQUIRE_EQ(g, 150);
    REQUIRE_EQ(b, 200);

    bj_destroy_bitmap(bmp);
}

TEST_CASE(bitmap_clear_sets_all_pixels) {
    struct bj_bitmap* bmp = bj_create_bitmap(10, 10, BJ_PIXEL_MODE_XRGB8888, 0);
    REQUIRE_VALUE(bmp);

    // Set clear color
    uint32_t red = bj_make_bitmap_pixel(bmp, 255, 0, 0);
    bj_set_bitmap_color(bmp, red, BJ_BITMAP_CLEAR_COLOR);

    // Clear
    bj_clear_bitmap(bmp);

    // Check several pixels
    REQUIRE_EQ(bj_bitmap_pixel(bmp, 0, 0), red);
    REQUIRE_EQ(bj_bitmap_pixel(bmp, 5, 5), red);
    REQUIRE_EQ(bj_bitmap_pixel(bmp, 9, 9), red);

    bj_destroy_bitmap(bmp);
}

////////////////////////////////////////////////////////////////////////////////
// Color Key Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(bitmap_colorkey_set_and_enable) {
    struct bj_bitmap* bmp = bj_create_bitmap(10, 10, BJ_PIXEL_MODE_XRGB8888, 0);
    REQUIRE_VALUE(bmp);

    uint32_t magenta = bj_make_bitmap_pixel(bmp, 255, 0, 255);

    // Setting colorkey should auto-enable it
    bj_set_bitmap_color(bmp, magenta, BJ_BITMAP_COLORKEY);

    // We can disable it
    bj_enable_colorkey(bmp, BJ_FALSE);

    // And re-enable
    bj_enable_colorkey(bmp, BJ_TRUE);

    bj_destroy_bitmap(bmp);
    REQUIRE(1); // No crash
}

TEST_CASE(bitmap_set_both_colors_at_once) {
    struct bj_bitmap* bmp = bj_create_bitmap(10, 10, BJ_PIXEL_MODE_XRGB8888, 0);
    REQUIRE_VALUE(bmp);

    uint32_t black = bj_make_bitmap_pixel(bmp, 0, 0, 0);

    // Set both clear and colorkey at once
    bj_set_bitmap_color(bmp, black, BJ_BITMAP_CLEAR_COLOR | BJ_BITMAP_COLORKEY);

    bj_destroy_bitmap(bmp);
    REQUIRE(1);
}

////////////////////////////////////////////////////////////////////////////////
// Stride Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(bitmap_stride_minimum_computed) {
    // For 32bpp, stride should be at least width * 4
    struct bj_bitmap* bmp = bj_create_bitmap(10, 10, BJ_PIXEL_MODE_XRGB8888, 0);
    REQUIRE_VALUE(bmp);

    REQUIRE(bj_bitmap_stride(bmp) >= 10 * 4);

    bj_destroy_bitmap(bmp);
}

TEST_CASE(bitmap_stride_custom_accepted) {
    // Request a larger stride
    struct bj_bitmap* bmp = bj_create_bitmap(10, 10, BJ_PIXEL_MODE_XRGB8888, 100);
    REQUIRE_VALUE(bmp);

    // Stride should be at least 100 (or whatever we requested)
    REQUIRE(bj_bitmap_stride(bmp) >= 100);

    bj_destroy_bitmap(bmp);
}

////////////////////////////////////////////////////////////////////////////////
// Blit Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(blit_same_size_copies) {
    struct bj_bitmap* src = bj_create_bitmap(10, 10, BJ_PIXEL_MODE_XRGB8888, 0);
    struct bj_bitmap* dst = bj_create_bitmap(10, 10, BJ_PIXEL_MODE_XRGB8888, 0);
    REQUIRE_VALUE(src);
    REQUIRE_VALUE(dst);

    // Fill source with red
    uint32_t red = bj_make_bitmap_pixel(src, 255, 0, 0);
    bj_set_bitmap_color(src, red, BJ_BITMAP_CLEAR_COLOR);
    bj_clear_bitmap(src);

    // Blit entire source to destination
    bj_bool result = bj_blit(src, 0, dst, 0, BJ_BLIT_OP_COPY);
    REQUIRE_EQ(result, BJ_TRUE);

    // Check destination has the color
    REQUIRE_EQ(bj_bitmap_pixel(dst, 5, 5), red);

    bj_destroy_bitmap(src);
    bj_destroy_bitmap(dst);
}

TEST_CASE(blit_partial_area) {
    struct bj_bitmap* src = bj_create_bitmap(20, 20, BJ_PIXEL_MODE_XRGB8888, 0);
    struct bj_bitmap* dst = bj_create_bitmap(20, 20, BJ_PIXEL_MODE_XRGB8888, 0);
    REQUIRE_VALUE(src);
    REQUIRE_VALUE(dst);

    // Fill source with blue
    uint32_t blue = bj_make_bitmap_pixel(src, 0, 0, 255);
    bj_set_bitmap_color(src, blue, BJ_BITMAP_CLEAR_COLOR);
    bj_clear_bitmap(src);

    // Clear destination with black
    uint32_t black = bj_make_bitmap_pixel(dst, 0, 0, 0);
    bj_set_bitmap_color(dst, black, BJ_BITMAP_CLEAR_COLOR);
    bj_clear_bitmap(dst);

    // Blit a 5x5 region from (5,5) in src to (10,10) in dst
    struct bj_rect src_area = {5, 5, 5, 5};
    struct bj_rect dst_area = {10, 10, 5, 5};

    bj_bool result = bj_blit(src, &src_area, dst, &dst_area, BJ_BLIT_OP_COPY);
    REQUIRE_EQ(result, BJ_TRUE);

    // Check destination: (10,10) should be blue, (0,0) should be black
    REQUIRE_EQ(bj_bitmap_pixel(dst, 10, 10), blue);
    REQUIRE_EQ(bj_bitmap_pixel(dst, 0, 0), black);

    bj_destroy_bitmap(src);
    bj_destroy_bitmap(dst);
}

TEST_CASE(blit_returns_false_when_no_overlap) {
    struct bj_bitmap* src = bj_create_bitmap(10, 10, BJ_PIXEL_MODE_XRGB8888, 0);
    struct bj_bitmap* dst = bj_create_bitmap(10, 10, BJ_PIXEL_MODE_XRGB8888, 0);
    REQUIRE_VALUE(src);
    REQUIRE_VALUE(dst);

    // Try to blit completely outside destination
    struct bj_rect dst_area = {100, 100, 10, 10}; // Way outside

    bj_bool result = bj_blit(src, 0, dst, &dst_area, BJ_BLIT_OP_COPY);
    REQUIRE_EQ(result, BJ_FALSE);

    bj_destroy_bitmap(src);
    bj_destroy_bitmap(dst);
}

////////////////////////////////////////////////////////////////////////////////
// Lifecycle Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(bitmap_destroy_null_is_safe) {
    bj_destroy_bitmap(0);
    REQUIRE(1); // No crash
}

TEST_CASE(bitmap_pixels_returns_valid_pointer) {
    struct bj_bitmap* bmp = bj_create_bitmap(10, 10, BJ_PIXEL_MODE_XRGB8888, 0);
    REQUIRE_VALUE(bmp);

    void* pixels = bj_bitmap_pixels(bmp);
    REQUIRE_VALUE(pixels);

    bj_destroy_bitmap(bmp);
}

////////////////////////////////////////////////////////////////////////////////
// File Loading Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(bitmap_from_invalid_file_returns_error) {
    struct bj_error* err = 0;
    struct bj_bitmap* bmp = bj_create_bitmap_from_file("/nonexistent/path/image.bmp", &err);

    REQUIRE_NULL(bmp);
    REQUIRE_VALUE(err);

    bj_clear_error(&err);
}

TEST_CASE(bitmap_from_file_null_error_is_safe) {
    struct bj_bitmap* bmp = bj_create_bitmap_from_file("/nonexistent/path/image.bmp", 0);
    REQUIRE_NULL(bmp);
}

int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    // Creation
    RUN_TEST(bitmap_create_dimensions_match);
    RUN_TEST(bitmap_create_different_modes);
    RUN_TEST(bitmap_allocate_and_init);
    RUN_TEST(bitmap_create_from_pixels);
    RUN_TEST(bitmap_create_from_pixels_null_returns_null);

    // Copy and convert
    RUN_TEST(bitmap_copy_is_independent);
    RUN_TEST(bitmap_copy_null_returns_null);
    /* RUN_TEST(bitmap_convert_preserves_dimensions); */
    RUN_TEST(bitmap_convert_same_mode_copies);

    // Pixel access
    RUN_TEST(bitmap_put_pixel_get_pixel_roundtrip);
    RUN_TEST(bitmap_make_rgb_extracts_components);
    RUN_TEST(bitmap_clear_sets_all_pixels);

    // Color key
    RUN_TEST(bitmap_colorkey_set_and_enable);
    RUN_TEST(bitmap_set_both_colors_at_once);

    // Stride
    RUN_TEST(bitmap_stride_minimum_computed);
    RUN_TEST(bitmap_stride_custom_accepted);

    // Blit
    RUN_TEST(blit_same_size_copies);
    RUN_TEST(blit_partial_area);
    RUN_TEST(blit_returns_false_when_no_overlap);

    // Lifecycle
    RUN_TEST(bitmap_destroy_null_is_safe);
    RUN_TEST(bitmap_pixels_returns_valid_pointer);

    // File loading
    RUN_TEST(bitmap_from_invalid_file_returns_error);
    RUN_TEST(bitmap_from_file_null_error_is_safe);

    END_TESTS();
}
