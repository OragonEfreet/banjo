#include <banjo/pixel.h>

#include "test.h"

TEST_CASE(conversion) {

    uint32_t val = bj_pixel_value(BJ_PIXEL_MODE_RGB565, 0, 0, 0);
    
    REQUIRE_EQ(val, 0);


}

TEST_CASE_ARGS(pixel_mode, {uint32_t enumval;uint32_t expr;}) {
    REQUIRE_EQ(test_data->enumval, test_data->expr);
}

int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    RUN_TEST_ARGS(pixel_mode, .enumval=BJ_PIXEL_MODE_INDEXED_1, .expr=BJ_PIXEL_MODE_MAKE_INDEXED(1));
    RUN_TEST_ARGS(pixel_mode, .enumval=BJ_PIXEL_MODE_INDEXED_4, .expr=BJ_PIXEL_MODE_MAKE_INDEXED(4));
    RUN_TEST_ARGS(pixel_mode, .enumval=BJ_PIXEL_MODE_INDEXED_8, .expr=BJ_PIXEL_MODE_MAKE_INDEXED(8));
    RUN_TEST_ARGS(pixel_mode, .enumval=BJ_PIXEL_MODE_XRGB1555,  .expr=BJ_PIXEL_MODE_MAKE_BITFIELD_16(BJ_PIXEL_LAYOUT_1555, BJ_PIXEL_ORDER_XRGB));
    RUN_TEST_ARGS(pixel_mode, .enumval=BJ_PIXEL_MODE_RGB565,    .expr=BJ_PIXEL_MODE_MAKE_BITFIELD_16(BJ_PIXEL_LAYOUT_565, BJ_PIXEL_ORDER_XRGB));
    RUN_TEST_ARGS(pixel_mode, .enumval=BJ_PIXEL_MODE_XRGB8888,  .expr=BJ_PIXEL_MODE_MAKE_BITFIELD_32(BJ_PIXEL_LAYOUT_8888, BJ_PIXEL_ORDER_XRGB));
    RUN_TEST_ARGS(pixel_mode, .enumval=BJ_PIXEL_MODE_BGR24,     .expr=BJ_PIXEL_MODE_MAKE_BYTES(24, BJ_PIXEL_ORDER_BGR));

    RUN_TEST(conversion);

    END_TESTS();
}
