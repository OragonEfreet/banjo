////////////////////////////////////////////////////////////////////////////////
/// \example rbuffer.c
/// Demonstrates how to use the \ref bj_rbuffer container, the bucket-based
/// ring buffer.
////////////////////////////////////////////////////////////////////////////////
#include <banjo/api.h>
#include <banjo/rbuffer.h>
#include <banjo/log.h>
#include <banjo/memory.h>

#include <stdlib.h>
#include <stdio.h>

#define BUCKET_SIZE 16

static int test_count = 0;

#define REQUIRE(cond) do { \
    ++test_count; \
    if (!(cond)) { \
        fprintf(stderr, "[FAIL] Test %d failed: %s\n", test_count, #cond); \
        exit(1); \
    } \
} while (0)

#define REQUIRE_EQ(a, b) REQUIRE(a == b)
#define REQUIRE_FALSE(COND) REQUIRE(!(COND))


void run_wrapping_tests() {
    bj_rbuffer* buf = bj_rbuffer_new(40);
    size_t cap = bj_rbuffer_capacity(buf);
    REQUIRE(cap > 4);

    // Push/pop alternation to force wrapping
    for (unsigned int i = 0; i < cap * 3; ++i) {
        REQUIRE(bj_rbuffer_push(buf, 0, 1) == BJ_TRUE);
        REQUIRE(bj_rbuffer_ready(buf) == 1);
        REQUIRE(bj_rbuffer_pop(buf, 0, 1) == BJ_TRUE);
        REQUIRE(bj_rbuffer_ready(buf) == 0);
    }

    // Now fill and partially drain multiple times
    for (int i = 0; i < 100; ++i) {
        REQUIRE(bj_rbuffer_push(buf, 0, 3) == BJ_TRUE);
        REQUIRE(bj_rbuffer_ready(buf) >= 3);
        REQUIRE(bj_rbuffer_pop(buf, 0, 2) == BJ_TRUE);
    }

    bj_rbuffer_del(buf);
}

int main(void) {

    run_wrapping_tests();




    return 0;
}




/* void test_null_safety() { */
/*     REQUIRE(bj_rbuffer_ready(NULL) == 0); */
/*     REQUIRE(bj_rbuffer_available(NULL) == 0); */
/*     REQUIRE(bj_rbuffer_push(NULL, 0, 5) == BJ_FALSE); */
/*     REQUIRE(bj_rbuffer_pop(NULL, 0, 5) == BJ_FALSE); */
/*     REQUIRE(bj_rbuffer_reserve(NULL, 1000) == 0); */
/*     bj_rbuffer_reset(NULL); */
/*     REQUIRE(bj_rbuffer_capacity(NULL) == 0); */
/* } */
