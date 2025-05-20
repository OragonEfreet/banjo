#include "test.h"

#include "rbuffer_t.h"

#include <string.h>

typedef double payload;

TEST_CASE(initialize_with_payload_gives_empty_rbuffer) {
    bj_rbuffer* p_rbuffer = bj_rbuffer_new_t(payload);
    REQUIRE_EQ(p_rbuffer->bytes_payload, sizeof(payload));
    REQUIRE_EQ(p_rbuffer->capacity, 0);
    REQUIRE_EQ(p_rbuffer->len, 0);
    REQUIRE_EQ(p_rbuffer->p_buffer, 0);
    REQUIRE_EQ(p_rbuffer->head, 0);
    bj_rbuffer_del(p_rbuffer);
}

/* TEST_CASE(clear_filled_reduces_size_to_zero) { */
/*     bj_rbuffer* p_rbuffer = bj_rbuffer_new_t(payload); */
/*     bj_rbuffer_push(p_rbuffer, &(payload){.elem0 = 0}); */
/*     bj_rbuffer_clear(p_rbuffer); */
/*     REQUIRE_EQ(p_rbuffer->len, 0); */
/*     bj_rbuffer_del(p_rbuffer); */
/* } */

/* TEST_CASE(clear_filled_does_not_change_capacity) { */
/*     bj_rbuffer* p_rbuffer = bj_rbuffer_new_t(payload); */
/*     bj_rbuffer_push(p_rbuffer, &(payload){.elem0 = 0}); */
/*     size_t capacity = bj_rbuffer_capacity(p_rbuffer); */
/*     bj_rbuffer_clear(p_rbuffer); */
/*     REQUIRE_EQ(capacity, bj_rbuffer_capacity(p_rbuffer)); */
/*     bj_rbuffer_del(p_rbuffer); */
/* } */

/* TEST_CASE(shrink_empty_does_nothing) { */
/*     bj_rbuffer* p_rbuffer = bj_rbuffer_new_t(payload); */
/*     CHECK_EQ(p_rbuffer->capacity, 0); */
/*     bj_rbuffer_clear(p_rbuffer); */
/*     REQUIRE_EQ(p_rbuffer->capacity, 0); */
/*     bj_rbuffer_del(p_rbuffer); */
/* } */

/* TEST_CASE(shrink_sets_capacity_to_size) { */
/*     payload p; */
/*     bj_rbuffer* p_rbuffer = bj_rbuffer_new_t(payload); */

/*     for(size_t len = 1 ; len <= 10 ; ++len) { */
/*         bj_rbuffer_push(p_rbuffer, &p); */
/*         CHECK_EQ(p_rbuffer->len, len); */
/*         CHECK(p_rbuffer->capacity >= len); */

/*         bj_rbuffer_shrink(p_rbuffer); */
/*         size_t got_len = bj_rbuffer_len(p_rbuffer); */
/*         size_t got_capacity = bj_rbuffer_capacity(p_rbuffer); */
/*         REQUIRE_EQ(got_len, got_capacity); */
/*     } */
/*     bj_rbuffer_del(p_rbuffer); */
/* } */

/* TEST_CASE(set_greater_len_changes_len_and_capacity) { */
/*     bj_rbuffer* p_rbuffer = bj_rbuffer_new_t(payload); */

/*     for(size_t len = 0 ; len < 10 ; ++len) { */
/*         bj_rbuffer_set_len(p_rbuffer, len); */
/*         REQUIRE_EQ(p_rbuffer->len, len); */
/*         REQUIRE(p_rbuffer->capacity >= len); */
/*     } */
/*     bj_rbuffer_del(p_rbuffer); */
/* } */

/* TEST_CASE(set_lower_len_changes_len_but_not_capacity) { */
/*     bj_rbuffer* p_rbuffer = bj_rbuffer_new_t(payload); */

/*     size_t len = 9; */
/*     do { */
/*         bj_rbuffer_set_len(p_rbuffer, len); */
/*         REQUIRE_EQ(p_rbuffer->len, len); */
/*         REQUIRE(p_rbuffer->capacity >= 9); */
/*     } while(--len > 0); */
/*     bj_rbuffer_del(p_rbuffer); */
/* } */

/* TEST_CASE(reserve_greater_capacity_growth_buffer) { */
/*     bj_rbuffer* p_rbuffer = bj_rbuffer_new_t(payload); */
/*     bj_rbuffer_reserve(p_rbuffer, 10); */
/*     REQUIRE(p_rbuffer->capacity >= 10); */
/*     REQUIRE_VALUE(p_rbuffer->p_buffer); */
/*     bj_rbuffer_del(p_rbuffer); */
/* } */

/* TEST_CASE(reserve_smaller_or_equal_capacity_does_nothing) { */
/*     bj_rbuffer* p_rbuffer = bj_rbuffer_new_t(payload); */

/*     bj_rbuffer_reserve(p_rbuffer, 0); */
/*     REQUIRE_EQ(p_rbuffer->capacity, 0); */
/*     REQUIRE_EQ(p_rbuffer->p_buffer, 0); */

/*     bj_rbuffer_reserve(p_rbuffer, 10); */
/*     CHECK(p_rbuffer->capacity >= 10); */
/*     CHECK_VALUE(p_rbuffer->p_buffer); */

/*     bj_rbuffer_reserve(p_rbuffer, 5); */
/*     REQUIRE(p_rbuffer->capacity >= 10); */
/*     REQUIRE_VALUE(p_rbuffer->p_buffer); */

/*     bj_rbuffer_reserve(p_rbuffer, 0); */
/*     REQUIRE(p_rbuffer->capacity >= 10); */
/*     REQUIRE_VALUE(p_rbuffer->p_buffer); */

/*     bj_rbuffer_del(p_rbuffer); */
/* } */

/* TEST_CASE(push_into_empty_rbuffer_creates_new_buffer) { */
/*     payload p; */
/*     bj_rbuffer* p_rbuffer = bj_rbuffer_new_t(payload); */

/*     CHECK(p_rbuffer->p_buffer == 0); */
/*     CHECK_EQ(p_rbuffer->len, 0); */
/*     CHECK_EQ(p_rbuffer->capacity, 0); */

/*     bj_rbuffer_push(p_rbuffer, &p); */

/*     REQUIRE(p_rbuffer->p_buffer != 0); */
/*     REQUIRE_EQ(p_rbuffer->len, 1); */
/*     REQUIRE(p_rbuffer->capacity >= 1); */
    
/*     bj_rbuffer_del(p_rbuffer); */
/* } */

/* TEST_CASE(push_growth_len_by_1) { */
/*     payload p; */
/*     bj_rbuffer* p_rbuffer = bj_rbuffer_new_t(payload); */

/*     for(size_t i = 1 ; i < 9 ; ++i) { */
/*         bj_rbuffer_push(p_rbuffer, &p); */
/*         REQUIRE_EQ(p_rbuffer->len, i); */
/*     } */

/*     bj_rbuffer_del(p_rbuffer); */
/* } */

/* TEST_CASE(push_growth_capacity_only_if_equals_to_len) { */
/*     payload p; */
/*     bj_rbuffer* p_rbuffer = bj_rbuffer_new_t(payload); */

/*     for(size_t i = 1 ; i < 90 ; ++i) { */
/*         size_t before_capacity  = p_rbuffer->capacity; */
/*         size_t capacity_resized = (before_capacity == p_rbuffer->len); */

/*         bj_rbuffer_push(p_rbuffer, &p); */

/*         if(capacity_resized) { */
/*             REQUIRE(p_rbuffer->capacity >= before_capacity); */
/*         } else { */
/*             REQUIRE_EQ(p_rbuffer->capacity, before_capacity); */
/*         } */
/*     } */

/*     bj_rbuffer_del(p_rbuffer); */
/* } */

/* TEST_CASE(pop_empty_does_nothing) { */
/*     bj_rbuffer* p_rbuffer = bj_rbuffer_new_t(payload); */
/*     bj_rbuffer_pop(p_rbuffer); */
/*     REQUIRE_EQ(p_rbuffer->p_buffer, 0); */
/*     REQUIRE_EQ(p_rbuffer->len, 0); */
/*     REQUIRE_EQ(p_rbuffer->capacity, 0); */
/*     bj_rbuffer_del(p_rbuffer); */
/* } */

/* TEST_CASE(pop_nonempty_reduces_len_but_not_capacity_nor_pointer) { */
/*     bj_rbuffer* p_rbuffer = bj_rbuffer_new_t(payload); */

/*     size_t len = 10; */
/*     bj_rbuffer_set_len(p_rbuffer, len); */
/*     void* buffer  = p_rbuffer->p_buffer; */
/*     size_t capacity = p_rbuffer->capacity; */

/*     do { */
/*         bj_rbuffer_pop(p_rbuffer); */
/*         REQUIRE_EQ(p_rbuffer->len, --len); */
/*         REQUIRE_EQ(p_rbuffer->capacity, capacity); */
/*         REQUIRE_EQ(p_rbuffer->p_buffer, buffer); */
/*     } while(len > 0); */
/*     bj_rbuffer_del(p_rbuffer); */
/* } */

/* TEST_CASE(at_0_returns_0) { */
/*     REQUIRE_EQ(bj_rbuffer_at(0, 0), 0); */
/*     REQUIRE_EQ(bj_rbuffer_at(0, 1), 0); */
/*     REQUIRE_EQ(bj_rbuffer_at(0, 2), 0); */
/*     REQUIRE_EQ(bj_rbuffer_at(0, 3), 0); */
/*     REQUIRE_EQ(bj_rbuffer_at(0, 4), 0); */
/* } */

/* TEST_CASE(at_empty_returns_0) { */
/*     bj_rbuffer* p_rbuffer = bj_rbuffer_new_t(payload); */
/*     REQUIRE_EQ(bj_rbuffer_at(p_rbuffer, 0), 0); */
/*     REQUIRE_EQ(bj_rbuffer_at(p_rbuffer, 1), 0); */
/*     REQUIRE_EQ(bj_rbuffer_at(p_rbuffer, 2), 0); */
/*     REQUIRE_EQ(bj_rbuffer_at(p_rbuffer, 3), 0); */
/*     REQUIRE_EQ(bj_rbuffer_at(p_rbuffer, 4), 0); */
/*     bj_rbuffer_del(p_rbuffer); */
/* } */

/* TEST_CASE(at_nonempty_returns_indexed_value) { */
/*     bj_rbuffer* p_rbuffer = bj_rbuffer_new_t(payload); */

/*     for(short i = 0 ; i < 10 ; ++i) { */

/*         payload p = {.elem0 = i * 2, .elem1 = -((long)i)}; */
/*         bj_rbuffer_push(p_rbuffer, &p); */

/*         payload* got = bj_rbuffer_at(p_rbuffer, i); */

/*         REQUIRE_EQ(p.elem0, got->elem0); */
/*         REQUIRE_EQ(p.elem1, got->elem1); */

/*         int diff = memcmp(&p, got, sizeof(payload)); */
/*         REQUIRE_EQ(diff, 0); */
/*     } */

/*     bj_rbuffer_del(p_rbuffer); */
/* } */

int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    RUN_TEST(initialize_with_payload_gives_empty_rbuffer);
    /* RUN_TEST(clear_empty_does_nothing); */
    /* RUN_TEST(clear_filled_reduces_size_to_zero); */
    /* RUN_TEST(clear_filled_does_not_change_capacity); */
    /* RUN_TEST(shrink_empty_does_nothing); */
    /* RUN_TEST(shrink_sets_capacity_to_size); */
    /* RUN_TEST(set_greater_len_changes_len_and_capacity); */
    /* RUN_TEST(set_lower_len_changes_len_but_not_capacity); */
    /* RUN_TEST(reserve_greater_capacity_growth_buffer); */
    /* RUN_TEST(reserve_smaller_or_equal_capacity_does_nothing); */
    /* RUN_TEST(push_into_empty_rbuffer_creates_new_buffer); */
    /* RUN_TEST(push_growth_len_by_1); */
    /* RUN_TEST(push_growth_capacity_only_if_equals_to_len); */
    /* RUN_TEST(pop_empty_does_nothing); */
    /* RUN_TEST(pop_nonempty_reduces_len_but_not_capacity_nor_pointer); */
    /* RUN_TEST(at_0_returns_0); */
    /* RUN_TEST(at_empty_returns_0); */
    /* RUN_TEST(at_nonempty_returns_indexed_value); */

    END_TESTS();
}

