#include "test.h"

#include "rbuffer_t.h"

#include <string.h>

#define BUCKET_SIZE 16


/* TEST_CASE(zero_ready_available_after_new) { */
/*     bj_rbuffer* buf = bj_rbuffer_new(4); */
/*     REQUIRE_VALUE(buf); */
/*     REQUIRE_EQ(bj_rbuffer_ready(buf), 0); */
/*     REQUIRE_EQ(bj_rbuffer_available(buf), bj_rbuffer_capacity(buf)); */
/*     bj_rbuffer_del(buf); */
/* } */

/* TEST_CASE(push_increases_ready) { */
/*     bj_rbuffer* buf = bj_rbuffer_new(20); */
/*     size_t avail = bj_rbuffer_available(buf); */
/*     REQUIRE(bj_rbuffer_push(buf, 0, 10)); */
/*     REQUIRE_EQ(bj_rbuffer_ready(buf), 10); */
/*     REQUIRE_EQ(bj_rbuffer_available(buf), avail - 10); */
/*     bj_rbuffer_del(buf); */
/* } */

/* TEST_CASE(pop_decreases_ready) { */
/*     bj_rbuffer* buf = bj_rbuffer_new(20); */
/*     bj_rbuffer_push(buf, 0, 10); */
/*     size_t before = bj_rbuffer_ready(buf); */
/*     REQUIRE(bj_rbuffer_pop(buf, 0, 4)); */
/*     REQUIRE_EQ(bj_rbuffer_ready(buf), before - 4); */
/*     bj_rbuffer_del(buf); */
/* } */

/* TEST_CASE(overrun_fails) { */
/*     bj_rbuffer* buf = bj_rbuffer_new(1); */
/*     size_t cap = bj_rbuffer_capacity(buf); */
/*     REQUIRE_FALSE(bj_rbuffer_push(buf, 0, cap + 1)); */
/*     bj_rbuffer_del(buf); */
/* } */

/* TEST_CASE(underrun_fails) { */
/*     bj_rbuffer* buf = bj_rbuffer_new(1); */
/*     REQUIRE_FALSE(bj_rbuffer_pop(buf, 0, 1)); */
/*     bj_rbuffer_del(buf); */
/* } */

/* TEST_CASE(reset_clears_ready) { */
/*     bj_rbuffer* buf = bj_rbuffer_new(20); */
/*     bj_rbuffer_push(buf, 0, 8); */
/*     REQUIRE(bj_rbuffer_ready(buf) == 8); */
/*     bj_rbuffer_reset(buf); */
/*     REQUIRE(bj_rbuffer_ready(buf) == 0); */
/*     bj_rbuffer_del(buf); */
/* } */

int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    /* RUN_TEST(zero_ready_available_after_new); */
    /* RUN_TEST(push_increases_ready); */
    /* RUN_TEST(pop_decreases_ready); */
    /* RUN_TEST(overrun_fails); */
    /* RUN_TEST(underrun_fails); */
    /* RUN_TEST(reset_clears_ready); */

    END_TESTS();
}

