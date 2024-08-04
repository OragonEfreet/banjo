#include "test.h"

#include "oldbmp_t.h"

#include <string.h>

TEST_CASE(init_with_0_width_returns_0) {
    bj_oldbmp* p_bmp = bj_oldbmp_new(0, 10);
    REQUIRE_NULL(p_bmp);
}

TEST_CASE(init_with_0_height_returns_0) {
    bj_oldbmp* p_bmp = bj_oldbmp_new(10, 0);
    REQUIRE_NULL(p_bmp);
}

TEST_CASE(init_with_non_null_size_creates_a_buffer) {
    bj_oldbmp* p_bmp = bj_oldbmp_new(10, 12);
    REQUIRE_VALUE(p_bmp->buffer);
    REQUIRE_EQ(p_bmp->width, 10);
    REQUIRE_EQ(p_bmp->height, 12);
    bj_oldbmp_del(p_bmp);
}

TEST_CASE(default_init_sets_the_clear_color_to_black) {
    bj_oldbmp* p_bmp = bj_oldbmp_new(10, 10);
    REQUIRE_EQ(p_bmp->clear_color, BJ_COLOR_BLACK);
    bj_oldbmp_del(p_bmp);
}

TEST_CASE(all_pixels_of_a_cleared_oldbmp_have_the_clear_color) {
    bj_oldbmp* p_bmp = bj_oldbmp_new(4, 4);

    bj_oldbmp_set_clear_color(p_bmp, BJ_COLOR_AQUAMARINE);
    bj_oldbmp_clear(p_bmp);

    for(usize p = 0 ; p < 16 ; ++p) {
        REQUIRE_EQ(p_bmp->buffer[p], BJ_COLOR_AQUAMARINE);
    }

    bj_oldbmp_del(p_bmp);
}

TEST_CASE(changing_clear_color_updates_the_clear_color_field) {
    bj_oldbmp* p_bmp = bj_oldbmp_new(1, 1);
    bj_oldbmp_set_clear_color(p_bmp, BJ_COLOR_AQUAMARINE);
    REQUIRE_EQ(p_bmp->clear_color, BJ_COLOR_AQUAMARINE);
    bj_oldbmp_del(p_bmp);
}

TEST_CASE(the_data_can_be_retrieved) {
    bj_oldbmp* p_bmp = bj_oldbmp_new(10, 10);
    bj_color* data = bj_oldbmp_data(p_bmp);
    REQUIRE_EQ(data, p_bmp->buffer);
    bj_oldbmp_del(p_bmp);
}

TEST_CASE_ARGS(draw_lines, {bj_pixel a; bj_pixel b; bj_color buf[25];}) {
    bj_oldbmp* p_bmp = bj_oldbmp_new(5, 5);

    bj_oldbmp_clear(p_bmp);
    bj_oldbmp_draw_line(p_bmp, test_data->a, test_data->b, BJ_COLOR_WHITE);
    int differences = memcmp(test_data->buf, p_bmp->buffer, sizeof(bj_color) * 25);
    REQUIRE_EQ(differences, 0);

    bj_oldbmp_del(p_bmp);
}

TEST_CASE_ARGS(draw_triangles, {bj_pixel a; bj_pixel b; bj_pixel c;bj_color buf[25];}) {
    bj_oldbmp* p_bmp = bj_oldbmp_new(5, 5);

    bj_oldbmp_clear(p_bmp);
    bj_oldbmp_draw_triangle(p_bmp, test_data->a, test_data->b, test_data->c, BJ_COLOR_WHITE);
    int differences = memcmp(test_data->buf, p_bmp->buffer, sizeof(bj_color) * 25);
    REQUIRE_EQ(differences, 0);

    bj_oldbmp_del(p_bmp);
}


// Used for making some test call faster
/* static void buffer_content(bj_oldbmp* bmp) { */
/*     bj_color* c = bj_oldbmp_data(bmp); */
/*     for(usize p = 0 ; p < bmp->width * bmp->height ; ++p) { */
/*         printf("%c,", *(c++) == bmp->clear_color ? 'B' : 'F'); */
/*     } */
/* } */

/* static void write_line_run(usize x0, usize y0, usize x1, usize y1)  { */
/*     bj_oldbmp* bmp = bj_new(oldbmp, default, 5, 5); */
/*     bj_oldbmp_clear(bmp); */
/*     bj_oldbmp_draw_line(bmp, (bj_pixel){x0, y0}, (bj_pixel){x1, y1}, BJ_COLOR_WHITE); */
/*     printf("RUN_TEST_ARGS(draw_lines, .a = {%ld, %ld}, .b = {%ld, %ld}, .buf = {", x0, y0, x1, y1); */ 
/*     buffer_content(bmp); */
/*     printf("});\n"); */ 
/*     bj_del(oldbmp, bmp); */
/* } */

/* static void write_triangle_run(usize x0, usize y0, usize x1, usize y1, usize x2, usize y2) { */
/*     bj_oldbmp* bmp = bj_new(oldbmp, default, 5, 5); */
/*     bj_oldbmp_clear(bmp); */
/*     bj_oldbmp_draw_triangle(bmp, (bj_pixel){x0, y0}, (bj_pixel){x1, y1}, (bj_pixel){x2, y2}, BJ_COLOR_WHITE); */
/*     printf("RUN_TEST_ARGS(draw_triangles, .a = {%ld, %ld}, .b = {%ld, %ld}, .c = {%ld, %ld},  .buf = {", x0, y0, x1, y1, x2, y2); */ 
/*     buffer_content(bmp); */
/*     printf("});\n"); */ 
/*     bj_del(oldbmp, bmp); */
/* } */



#define B BJ_COLOR_BLACK
#define F BJ_COLOR_WHITE

int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    RUN_TEST(init_with_0_width_returns_0);
    RUN_TEST(init_with_0_height_returns_0);
    RUN_TEST(all_pixels_of_a_cleared_oldbmp_have_the_clear_color);
    RUN_TEST(changing_clear_color_updates_the_clear_color_field);
    RUN_TEST(default_init_sets_the_clear_color_to_black);
    RUN_TEST(init_with_non_null_size_creates_a_buffer);
    RUN_TEST(the_data_can_be_retrieved);

    RUN_TEST_ARGS(draw_lines, .a = {0, 0}, .b = {0, 0}, .buf = {F,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,});
    RUN_TEST_ARGS(draw_lines, .a = {0, 0}, .b = {1, 4}, .buf = {F,B,B,B,B,F,B,B,B,B,F,B,B,B,B,B,F,B,B,B,B,F,B,B,B,});
    RUN_TEST_ARGS(draw_lines, .a = {1, 1}, .b = {3, 1}, .buf = {B,B,B,B,B,B,F,F,F,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,});
    RUN_TEST_ARGS(draw_lines, .a = {3, 1}, .b = {1, 1}, .buf = {B,B,B,B,B,B,F,F,F,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,});
    RUN_TEST_ARGS(draw_lines, .a = {3, 1}, .b = {3, 3}, .buf = {B,B,B,B,B,B,B,B,F,B,B,B,B,F,B,B,B,B,F,B,B,B,B,B,B,});
    RUN_TEST_ARGS(draw_lines, .a = {3, 3}, .b = {3, 1}, .buf = {B,B,B,B,B,B,B,B,F,B,B,B,B,F,B,B,B,B,F,B,B,B,B,B,B,});
    RUN_TEST_ARGS(draw_lines, .a = {0, 0}, .b = {4, 4}, .buf = {F,B,B,B,B,B,F,B,B,B,B,B,F,B,B,B,B,B,F,B,B,B,B,B,F,});
    RUN_TEST_ARGS(draw_lines, .a = {4, 4}, .b = {0, 0}, .buf = {F,B,B,B,B,B,F,B,B,B,B,B,F,B,B,B,B,B,F,B,B,B,B,B,F,});
    RUN_TEST_ARGS(draw_lines, .a = {4, 0}, .b = {0, 4}, .buf = {B,B,B,B,F,B,B,B,F,B,B,B,F,B,B,B,F,B,B,B,F,B,B,B,B,});
    RUN_TEST_ARGS(draw_lines, .a = {0, 4}, .b = {4, 0}, .buf = {B,B,B,B,F,B,B,B,F,B,B,B,F,B,B,B,F,B,B,B,F,B,B,B,B,});
    RUN_TEST_ARGS(draw_lines, .a = {0, 0}, .b = {1, 4}, .buf = {F,B,B,B,B,F,B,B,B,B,F,B,B,B,B,B,F,B,B,B,B,F,B,B,B,});
    RUN_TEST_ARGS(draw_lines, .a = {1, 4}, .b = {0, 0}, .buf = {F,B,B,B,B,F,B,B,B,B,B,F,B,B,B,B,F,B,B,B,B,F,B,B,B,});
    RUN_TEST_ARGS(draw_lines, .a = {1, 4}, .b = {0, 2}, .buf = {B,B,B,B,B,B,B,B,B,B,F,B,B,B,B,B,F,B,B,B,B,F,B,B,B,});
    RUN_TEST_ARGS(draw_lines, .a = {0, 2}, .b = {1, 4}, .buf = {B,B,B,B,B,B,B,B,B,B,F,B,B,B,B,F,B,B,B,B,B,F,B,B,B,});
    RUN_TEST_ARGS(draw_lines, .a = {0, 2}, .b = {3, 3}, .buf = {B,B,B,B,B,B,B,B,B,B,F,F,B,B,B,B,B,F,F,B,B,B,B,B,B,});
    RUN_TEST_ARGS(draw_lines, .a = {3, 3}, .b = {0, 2}, .buf = {B,B,B,B,B,B,B,B,B,B,F,F,B,B,B,B,B,F,F,B,B,B,B,B,B,});
    RUN_TEST_ARGS(draw_lines, .a = {3, 3}, .b = {0, 4}, .buf = {B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,F,F,B,F,F,B,B,B,});
    RUN_TEST_ARGS(draw_lines, .a = {0, 4}, .b = {3, 3}, .buf = {B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,F,F,B,F,F,B,B,B,});
    RUN_TEST_ARGS(draw_triangles, .a = {0, 0}, .b = {0, 0}, .c = {0, 0},  .buf = {F,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,});
    RUN_TEST_ARGS(draw_triangles, .a = {0, 2}, .b = {3, 3}, .c = {0, 4},  .buf = {B,B,B,B,B,B,B,B,B,B,F,F,B,B,B,F,B,F,F,B,F,F,B,B,B,});
    RUN_TEST_ARGS(draw_triangles, .a = {0, 2}, .b = {0, 4}, .c = {3, 3},  .buf = {B,B,B,B,B,B,B,B,B,B,F,F,B,B,B,F,B,F,F,B,F,F,B,B,B,});
    RUN_TEST_ARGS(draw_triangles, .a = {3, 3}, .b = {0, 2}, .c = {0, 4},  .buf = {B,B,B,B,B,B,B,B,B,B,F,F,B,B,B,F,B,F,F,B,F,F,B,B,B,});
    RUN_TEST_ARGS(draw_triangles, .a = {3, 3}, .b = {0, 4}, .c = {0, 2},  .buf = {B,B,B,B,B,B,B,B,B,B,F,F,B,B,B,F,B,F,F,B,F,F,B,B,B,});
    RUN_TEST_ARGS(draw_triangles, .a = {0, 4}, .b = {0, 2}, .c = {3, 3},  .buf = {B,B,B,B,B,B,B,B,B,B,F,F,B,B,B,F,B,F,F,B,F,F,B,B,B,});
    RUN_TEST_ARGS(draw_triangles, .a = {0, 4}, .b = {3, 3}, .c = {0, 2},  .buf = {B,B,B,B,B,B,B,B,B,B,F,F,B,B,B,F,B,F,F,B,F,F,B,B,B,});
    RUN_TEST_ARGS(draw_triangles, .a = {1, 1}, .b = {1, 1}, .c = {3, 3},  .buf = {B,B,B,B,B,B,F,B,B,B,B,B,F,B,B,B,B,B,F,B,B,B,B,B,B,});
    RUN_TEST_ARGS(draw_triangles, .a = {1, 1}, .b = {3, 3}, .c = {1, 1},  .buf = {B,B,B,B,B,B,F,B,B,B,B,B,F,B,B,B,B,B,F,B,B,B,B,B,B,});
    RUN_TEST_ARGS(draw_triangles, .a = {1, 1}, .b = {1, 1}, .c = {3, 3},  .buf = {B,B,B,B,B,B,F,B,B,B,B,B,F,B,B,B,B,B,F,B,B,B,B,B,B,});
    RUN_TEST_ARGS(draw_triangles, .a = {1, 1}, .b = {3, 3}, .c = {1, 1},  .buf = {B,B,B,B,B,B,F,B,B,B,B,B,F,B,B,B,B,B,F,B,B,B,B,B,B,});
    RUN_TEST_ARGS(draw_triangles, .a = {3, 3}, .b = {1, 1}, .c = {1, 1},  .buf = {B,B,B,B,B,B,F,B,B,B,B,B,F,B,B,B,B,B,F,B,B,B,B,B,B,});
    RUN_TEST_ARGS(draw_triangles, .a = {3, 3}, .b = {1, 1}, .c = {1, 1},  .buf = {B,B,B,B,B,B,F,B,B,B,B,B,F,B,B,B,B,B,F,B,B,B,B,B,B,});

    END_TESTS();
}

