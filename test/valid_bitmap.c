#include "test.h"

#include <banjo/bitmap.h>
#include <banjo/error.h>
#include <banjo/log.h>

TEST_CASE_ARGS(is_valid_bmp, {const char* name;}) {
    const char* folder = BANJO_ASSETS_DIR;
    size_t folder_len = bj_strlen(folder);
    size_t path_len = folder_len + bj_strlen(test_data->name) + 2;
    char* bmp_path = bj_malloc(sizeof(char) * path_len);
    snprintf(bmp_path, path_len, "%s/%s", folder, test_data->name);

    struct bj_error* p_error = 0;
    bj_destroy_bitmap(bj_create_bitmap_from_file(bmp_path, &p_error));
    REQUIRE_NULL(p_error);

    bj_free(bmp_path);
}

TEST_CASE_ARGS(is_corrupt_bmp, {const char* name;enum bj_error_code code;}) {
    const char* folder = BANJO_ASSETS_DIR;
    size_t folder_len = bj_strlen(folder);
    size_t path_len = folder_len + bj_strlen(test_data->name) + 2;
    char* bmp_path = bj_malloc(sizeof(char) * path_len);
    snprintf(bmp_path, path_len, "%s/%s", folder, test_data->name);

    struct bj_error* p_error = 0;
    bj_destroy_bitmap(bj_create_bitmap_from_file(bmp_path, &p_error));

    REQUIRE_VALUE(p_error);
    REQUIRE_EQ(bj_error_code(p_error), (uint32_t)test_data->code);

    bj_clear_error(&p_error);
    
    bj_free(bmp_path);
}

int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);


    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/8bpp-colorsimportant-large.bmp", .code = BJ_ERROR_INVALID_FORMAT);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/8bpp-colorsimportant-negative.bmp", .code = BJ_ERROR_INVALID_FORMAT);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/8bpp-colorsused-large.bmp", .code = BJ_ERROR_INVALID_FORMAT);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/8bpp-colorsused-negative.bmp", .code = BJ_ERROR_INVALID_FORMAT);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/bitdepth-large.bmp", .code = BJ_ERROR_INCORRECT_VALUE);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/bitdepth-odd.bmp", .code = BJ_ERROR_INCORRECT_VALUE);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/bitdepth-zero.bmp", .code = BJ_ERROR_INCORRECT_VALUE);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/colormasks-cropped.bmp", .code = BJ_ERROR_INVALID_FORMAT);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/colormasks-missing.bmp", .code = BJ_ERROR_INVALID_FORMAT);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/compression-bad-rle4-for-8bpp.bmp", .code = BJ_ERROR_INCORRECT_VALUE);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/compression-bad-rle8-for-4bpp.bmp", .code = BJ_ERROR_INCORRECT_VALUE);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/compression-unknown.bmp", .code = BJ_ERROR_INVALID_FORMAT);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/emptyfile.bmp", .code = BJ_ERROR_INVALID_FORMAT);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/fileinfoheader-cropped.bmp", .code = BJ_ERROR_INVALID_FORMAT);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/height-zero.bmp", .code = BJ_ERROR_INCORRECT_VALUE);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/infoheader-cropped.bmp", .code = BJ_ERROR_INVALID_FORMAT);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/infoheader-missing.bmp", .code = BJ_ERROR_INVALID_FORMAT);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/infoheadersize-large.bmp", .code = BJ_ERROR_INVALID_FORMAT);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/infoheadersize-small.bmp", .code = BJ_ERROR_INCORRECT_VALUE);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/infoheadersize-zero.bmp", .code = BJ_ERROR_INCORRECT_VALUE);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/magicnumber-bad.bmp", .code = BJ_ERROR_INCORRECT_VALUE);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/magicnumber-cropped.bmp", .code = BJ_ERROR_INVALID_FORMAT);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/offbits-large.bmp", .code = BJ_ERROR_INVALID_FORMAT);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/offbits-negative.bmp", .code = BJ_ERROR_INVALID_FORMAT);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/offbits-zero.bmp", .code = BJ_ERROR_INVALID_FORMAT);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/palette-cropped.bmp", .code = BJ_ERROR_INVALID_FORMAT);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/palette-missing.bmp", .code = BJ_ERROR_INVALID_FORMAT);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/palette-too-big.bmp", .code = BJ_ERROR_INVALID_FORMAT);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/rle4-no-end-of-line-marker.bmp", .code = BJ_ERROR_INVALID_FORMAT);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/rle8-deltaleavesimage.bmp", .code = BJ_ERROR_INVALID_FORMAT);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/rle8-no-end-of-line-marker.bmp", .code = BJ_ERROR_INVALID_FORMAT);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/width-negative.bmp", .code = BJ_ERROR_INCORRECT_VALUE);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/width-times-height-overflow.bmp", .code = BJ_ERROR_INCORRECT_VALUE);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/width-zero.bmp", .code = BJ_ERROR_INCORRECT_VALUE);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/1bpp-no-palette.bmp", .code = BJ_ERROR_INVALID_FORMAT);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/1bpp-pixeldata-cropped.bmp", .code = BJ_ERROR_INVALID_FORMAT);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/24bpp-pixeldata-cropped.bmp", .code = BJ_ERROR_INVALID_FORMAT);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/32bpp-pixeldata-cropped.bmp", .code = BJ_ERROR_INVALID_FORMAT);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/4bpp-no-palette.bmp", .code = BJ_ERROR_INVALID_FORMAT);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/4bpp-pixeldata-cropped.bmp", .code = BJ_ERROR_INVALID_FORMAT);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/555-pixeldata-cropped.bmp", .code = BJ_ERROR_INVALID_FORMAT);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/8bpp-no-palette.bmp", .code = BJ_ERROR_INVALID_FORMAT);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/8bpp-pixeldata-cropped.bmp", .code = BJ_ERROR_INVALID_FORMAT);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/pixeldata-missing.bmp", .code = BJ_ERROR_INVALID_FORMAT);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/rle4-absolute-cropped.bmp", .code = BJ_ERROR_INVALID_FORMAT);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/rle4-delta-cropped.bmp", .code = BJ_ERROR_INVALID_FORMAT);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/rle4-runlength-cropped.bmp", .code = BJ_ERROR_INVALID_FORMAT);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/rle8-absolute-cropped.bmp", .code = BJ_ERROR_INVALID_FORMAT);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/rle8-delta-cropped.bmp", .code = BJ_ERROR_INVALID_FORMAT);
    RUN_TEST_ARGS(is_corrupt_bmp, .name = "/bmp/test/corrupt/rle8-runlength-cropped.bmp", .code = BJ_ERROR_INVALID_FORMAT);
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/ spaces in  filename.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/1bpp-1x1.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/1bpp-320x240-color.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/1bpp-320x240-overlappingcolor.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/1bpp-320x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/1bpp-321x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/1bpp-322x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/1bpp-323x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/1bpp-324x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/1bpp-325x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/1bpp-326x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/1bpp-327x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/1bpp-328x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/1bpp-329x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/1bpp-330x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/1bpp-331x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/1bpp-332x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/1bpp-333x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/1bpp-334x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/1bpp-335x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/1bpp-topdown-320x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/24bpp-1x1.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/24bpp-320x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/24bpp-321x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/24bpp-322x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/24bpp-323x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/24bpp-imagesize-zero.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/24bpp-topdown-320x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/32bpp-101110-320x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/32bpp-1x1.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/32bpp-320x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/32bpp-888-optimalpalette-320x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/32bpp-optimalpalette-320x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/32bpp-topdown-320x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/4bpp-1x1.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/4bpp-320x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/4bpp-321x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/4bpp-322x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/4bpp-323x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/4bpp-324x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/4bpp-325x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/4bpp-326x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/4bpp-327x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/4bpp-topdown-320x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/555-1x1.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/555-320x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/555-321x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/565-1x1.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/565-320x240-topdown.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/565-320x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/565-321x240-topdown.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/565-321x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/565-322x240-topdown.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/565-322x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/8bpp-1x1.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/8bpp-1x64000.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/8bpp-320x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/8bpp-321x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/8bpp-322x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/8bpp-323x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/8bpp-colorsimportant-two.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/8bpp-colorsused-zero.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/8bpp-topdown-320x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/misleadingextension.jpg");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/nofileextension");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/rle4-absolute-320x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/rle4-alternate-320x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/rle4-delta-320x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/rle4-encoded-320x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/rle8-64000x1.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/rle8-absolute-320x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/rle8-blank-160x120.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/rle8-delta-320x240.bmp");
    RUN_TEST_ARGS(is_valid_bmp, .name = "/bmp/test/valid/rle8-encoded-320x240.bmp");

    END_TESTS();
}


