#include <banjo/rect.h>

#include "test.h"

TEST_CASE_ARGS(rect_no_intersect, {bj_rect rect;}) {
    const bj_rect a = {.x = 0, .y = 0, .w = 10, .h = 10};
    bj_rect result = {0};
    bj_bool intersect = bj_rect_intersection(&a, &test_data->rect, &result);
    REQUIRE_EQ(intersect, BJ_FALSE);
    REQUIRE_EQ(result.x, 0);
    REQUIRE_EQ(result.y, 0);
    REQUIRE_EQ(result.w, 0);
    REQUIRE_EQ(result.h, 0);
}

TEST_CASE_ARGS(rect_intersect, {bj_rect rect; bj_rect result;}) {
    const bj_rect a = {.x = 0, .y = 0, .w = 10, .h = 10};
    bj_rect result = test_data->result;
    bj_bool intersect = bj_rect_intersection(&a, &test_data->rect, &result);
    REQUIRE_EQ(intersect, BJ_TRUE);
    REQUIRE_EQ(test_data->result.x, result.x);
    REQUIRE_EQ(test_data->result.y, result.y);
    REQUIRE_EQ(test_data->result.w, result.w);
    REQUIRE_EQ(test_data->result.h, result.h);
}

int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    RUN_TEST_ARGS(rect_intersect,.rect={.x=2, .y=2, .w=8, .h=8},  .result={.x=2,.y=2,.w=8, .h= 8});
    RUN_TEST_ARGS(rect_intersect,.rect={.x=0, .y=0, .w=10,.h= 10},.result={.x=0,.y=0,.w=10,.h= 10});
    RUN_TEST_ARGS(rect_intersect,.rect={.x=2, .y=-2,.w=8, .h=8},  .result={.x=2,.y=0,.w=8, .h = 6});
    RUN_TEST_ARGS(rect_intersect,.rect={.x=8, .y=-2,.w=8, .h= 8}, .result={.x=8,.y=0,.w=2, .h= 6});
    RUN_TEST_ARGS(rect_intersect,.rect={.x=8, .y=2, .w=8, .h= 8}, .result={.x=8,.y=2,.w=2, .h= 8});
    RUN_TEST_ARGS(rect_intersect,.rect={.x=8, .y=8, .w=8, .h= 8}, .result={.x=8,.y=8,.w=2, .h= 2});
    RUN_TEST_ARGS(rect_intersect,.rect={.x=2, .y=8, .w=8, .h= 8}, .result={.x=2,.y=8,.w=8, .h= 2});
    RUN_TEST_ARGS(rect_intersect,.rect={.x=-2,.y=8, .w=8, .h= 8}, .result={.x=0,.y=8,.w=6, .h= 2});
    RUN_TEST_ARGS(rect_intersect,.rect={.x=-2,.y=2, .w=8, .h= 8}, .result={.x=0,.y=2,.w=6, .h= 8});
    RUN_TEST_ARGS(rect_intersect,.rect={.x=-2,.y=-2,.w=8, .h= 8}, .result={.x=0,.y=0,.w=6, .h= 6});
    RUN_TEST_ARGS(rect_no_intersect,.rect={.x=-10,.y=-10,.w=2, .h= 2}, );
    RUN_TEST_ARGS(rect_no_intersect,.rect={.x=-10,.y=-10,.w=0, .h= 0}, );
    RUN_TEST_ARGS(rect_no_intersect,.rect={0}, );
    RUN_TEST_ARGS(rect_no_intersect,.rect={.x=2,.y=2,.w=0, .h= 0}, );

    END_TESTS();
}
