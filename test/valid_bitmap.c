#include "test.h"

#include <banjo/bitmap.h>
#include <banjo/error.h>

static usize valid_count = 0;
static usize invalid_count = 0;

TEST_CASE_ARGS(valid_bitmap, {const char* name;}) {

    const char* folder = BANJO_ASSETS_DIR;
    usize folder_len = strlen(folder);
    char* bmp_path = bj_malloc(sizeof(char) * (folder_len + strlen(test_data->name) + 2));
    sprintf(bmp_path, "%s/%s", folder, test_data->name);

    bj_error* p_error = 0;
    bj_del(bitmap, bj_new(bitmap, from_file, bmp_path, &p_error));

    REQUIRE_NULL(p_error);

    bj_free(bmp_path);
}

TEST_CASE_ARGS(invalid_bitmap, {const char* name; u32 code;}) {

    const char* folder = BANJO_ASSETS_DIR;
    usize folder_len = strlen(folder);
    char* bmp_path = bj_malloc(sizeof(char) * (folder_len + strlen(test_data->name) + 2));
    sprintf(bmp_path, "%s/%s", folder, test_data->name);

    bj_error* p_error = 0;
    bj_del(bitmap, bj_new(bitmap, from_file, bmp_path, &p_error));

    REQUIRE_VALUE(p_error);
    REQUIRE_EQ(p_error->code, test_data->code);

    bj_clear_error(&p_error);
    bj_free(bmp_path);
}

int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);


    RUN_TEST_ARGS(valid_bitmap, .name   = "bmp/all_gray.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name   = "bmp/blackbuck.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name   = "bmp/bmp_24.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name   = "bmp/dots.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name   = "bmp/greenland_grid_velo.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name   = "bmp/lena.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name   = "bmp/snail.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name   = "bmp/test/valid/24bpp-1x1.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name   = "bmp/test/valid/24bpp-320x240.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name   = "bmp/test/valid/24bpp-321x240.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name   = "bmp/test/valid/24bpp-322x240.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name   = "bmp/test/valid/24bpp-323x240.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name   = "bmp/test/valid/24bpp-imagesize-zero.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name   = "bmp/test/valid/8bpp-1x1.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name   = "bmp/test/valid/8bpp-1x64000.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name   = "bmp/test/valid/8bpp-320x240.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name   = "bmp/test/valid/8bpp-321x240.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name   = "bmp/test/valid/8bpp-322x240.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name   = "bmp/test/valid/8bpp-323x240.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name   = "bmp/test/valid/8bpp-colorsimportant-two.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name   = "bmp/test/valid/8bpp-colorsused-zero.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name   = "bmp/test/valid/misleadingextension.jpg");
    RUN_TEST_ARGS(valid_bitmap, .name   = "bmp/test/valid/nofileextension");
    RUN_TEST_ARGS(valid_bitmap, .name   = "bmp/test/valid/ spaces in  filename.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/1bpp-1x1.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/1bpp-320x240.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/1bpp-320x240-color.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/1bpp-320x240-overlappingcolor.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/1bpp-321x240.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/1bpp-322x240.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/1bpp-323x240.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/1bpp-324x240.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/1bpp-325x240.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/1bpp-326x240.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/1bpp-327x240.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/1bpp-328x240.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/1bpp-329x240.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/1bpp-330x240.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/1bpp-331x240.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/1bpp-332x240.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/1bpp-333x240.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/1bpp-334x240.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/1bpp-335x240.bmp");

    RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/4bpp-1x1.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/4bpp-320x240.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/4bpp-321x240.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/4bpp-322x240.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/4bpp-323x240.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/4bpp-324x240.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/4bpp-325x240.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/4bpp-326x240.bmp");
    RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/4bpp-327x240.bmp");

    /* RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/32bpp-101110-320x240.bmp"); */
    /* RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/32bpp-1x1.bmp"); */
    /* RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/32bpp-320x240.bmp"); */
    /* RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/32bpp-888-optimalpalette-320x240.bmp"); */
    /* RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/32bpp-optimalpalette-320x240.bmp"); */
    /* RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/32bpp-topdown-320x240.bmp"); */
    /* RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/555-1x1.bmp"); */
    /* RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/555-320x240.bmp"); */
    /* RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/555-321x240.bmp"); */
    /* RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/565-1x1.bmp"); */
    /* RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/565-320x240.bmp"); */
    /* RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/565-320x240-topdown.bmp"); */
    /* RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/565-321x240.bmp"); */
    /* RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/565-321x240-topdown.bmp"); */
    /* RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/565-322x240.bmp"); */
    /* RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/565-322x240-topdown.bmp"); */
    /* RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/rle4-absolute-320x240.bmp"); */
    /* RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/rle4-alternate-320x240.bmp"); */
    /* RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/rle4-delta-320x240.bmp"); */
    /* RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/rle4-encoded-320x240.bmp"); */
    /* RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/rle8-64000x1.bmp"); */
    /* RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/rle8-absolute-320x240.bmp"); */
    /* RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/rle8-blank-160x120.bmp"); */
    /* RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/rle8-delta-320x240.bmp"); */
    /* RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/rle8-encoded-320x240.bmp"); */

    // Will crash
    /* RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/24bpp-topdown-320x240.bmp"); */
    /* RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/1bpp-topdown-320x240.bmp"); */
    /* /1* RUN_TEST_ARGS(invalid_bitmap, .name = "bmp/test/valid/4bpp-topdown-320x240.bmp", .code = BJ_ERROR_INVALID_FORMAT); *1/ */
    /* RUN_TEST_ARGS(valid_bitmap, .name = "bmp/test/valid/8bpp-topdown-320x240.bmp"); */

    END_TESTS();
}


