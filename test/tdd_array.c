#include "test.h"
#include "array_t.h"

typedef struct {
    short elem0;
    long  elem1;
} payload;

TEST_CASE(initialize_with_payload_gives_empty_array) {
    bj_array* p_array = bj_array_new_t(payload);
    REQUIRE_EQ(p_array->bytes_payload, sizeof(payload));
    REQUIRE_EQ(p_array->capacity, 0);
    REQUIRE_EQ(p_array->len, 0);
    REQUIRE_EQ(p_array->p_buffer, 0);
    bj_array_del(p_array);
}

TEST_CASE(clear_empty_does_nothing) {
    bj_array* p_array = bj_array_new_t(payload);
    CHECK_EQ(p_array->len, 0);
    bj_array_clear(p_array);
    REQUIRE_EQ(p_array->len, 0);
    bj_array_del(p_array);
}

TEST_CASE(clear_filled_reduces_size_to_zero) {
    bj_array* p_array = bj_array_new_t(payload);
    bj_array_push(p_array, &(payload){.elem0 = 0});
    bj_array_clear(p_array);
    REQUIRE_EQ(p_array->len, 0);
    bj_array_del(p_array);
}

TEST_CASE(clear_filled_does_not_change_capacity) {
    bj_array* p_array = bj_array_new_t(payload);
    bj_array_push(p_array, &(payload){.elem0 = 0});
    size_t capacity = bj_array_capacity(p_array);
    bj_array_clear(p_array);
    REQUIRE_EQ(capacity, bj_array_capacity(p_array));
    bj_array_del(p_array);
}

TEST_CASE(shrink_empty_does_nothing) {
    bj_array* p_array = bj_array_new_t(payload);
    CHECK_EQ(p_array->capacity, 0);
    bj_array_clear(p_array);
    REQUIRE_EQ(p_array->capacity, 0);
    bj_array_del(p_array);
}


TEST_CASE(shrink_sets_capacity_to_size) {
    payload p;
    bj_array* p_array = bj_array_new_t(payload);

    for(size_t len = 1 ; len <= 10 ; ++len) {
        bj_array_push(p_array, &p);
        CHECK_EQ(p_array->len, len);
        CHECK(p_array->capacity >= len);

        bj_array_shrink(p_array);
        size_t got_len = bj_array_len(p_array);
        size_t got_capacity = bj_array_capacity(p_array);
        REQUIRE_EQ(got_len, got_capacity);
    }
    bj_array_del(p_array);
}

TEST_CASE(set_greater_len_changes_len_and_capacity) {
    bj_array* p_array = bj_array_new_t(payload);

    for(size_t len = 0 ; len < 10 ; ++len) {
        bj_array_set_len(p_array, len);
        REQUIRE_EQ(p_array->len, len);
        REQUIRE(p_array->capacity >= len);
    }
    bj_array_del(p_array);
}

TEST_CASE(set_lower_len_changes_len_but_not_capacity) {
    bj_array* p_array = bj_array_new_t(payload);

    size_t len = 9;
    do {
        bj_array_set_len(p_array, len);
        REQUIRE_EQ(p_array->len, len);
        REQUIRE(p_array->capacity >= 9);
    } while(--len > 0);
    bj_array_del(p_array);
}

TEST_CASE(reserve_greater_capacity_growth_buffer) {
    bj_array* p_array = bj_array_new_t(payload);
    bj_array_reserve(p_array, 10);
    REQUIRE(p_array->capacity >= 10);
    REQUIRE_VALUE(p_array->p_buffer);
    bj_array_del(p_array);
}

TEST_CASE(reserve_smaller_or_equal_capacity_does_nothing) {
    bj_array* p_array = bj_array_new_t(payload);

    bj_array_reserve(p_array, 0);
    REQUIRE_EQ(p_array->capacity, 0);
    REQUIRE_EQ(p_array->p_buffer, 0);

    bj_array_reserve(p_array, 10);
    CHECK(p_array->capacity >= 10);
    CHECK_VALUE(p_array->p_buffer);

    bj_array_reserve(p_array, 5);
    REQUIRE(p_array->capacity >= 10);
    REQUIRE_VALUE(p_array->p_buffer);

    bj_array_reserve(p_array, 0);
    REQUIRE(p_array->capacity >= 10);
    REQUIRE_VALUE(p_array->p_buffer);

    bj_array_del(p_array);
}

TEST_CASE(push_into_empty_array_creates_new_buffer) {
    payload p;
    bj_array* p_array = bj_array_new_t(payload);

    CHECK(p_array->p_buffer == 0);
    CHECK_EQ(p_array->len, 0);
    CHECK_EQ(p_array->capacity, 0);

    bj_array_push(p_array, &p);

    REQUIRE(p_array->p_buffer != 0);
    REQUIRE_EQ(p_array->len, 1);
    REQUIRE(p_array->capacity >= 1);
    
    bj_array_del(p_array);
}

TEST_CASE(push_growth_len_by_1) {
    payload p;
    bj_array* p_array = bj_array_new_t(payload);

    for(size_t i = 1 ; i < 9 ; ++i) {
        bj_array_push(p_array, &p);
        REQUIRE_EQ(p_array->len, i);
    }

    bj_array_del(p_array);
}

TEST_CASE(push_growth_capacity_only_if_equals_to_len) {
    payload p;
    bj_array* p_array = bj_array_new_t(payload);

    for(size_t i = 1 ; i < 90 ; ++i) {
        size_t before_capacity  = p_array->capacity;
        size_t capacity_resized = (before_capacity == p_array->len);

        bj_array_push(p_array, &p);

        if(capacity_resized) {
            REQUIRE(p_array->capacity >= before_capacity);
        } else {
            REQUIRE_EQ(p_array->capacity, before_capacity);
        }
    }

    bj_array_del(p_array);
}

TEST_CASE(pop_empty_does_nothing) {
    bj_array* p_array = bj_array_new_t(payload);
    bj_array_pop(p_array);
    REQUIRE_EQ(p_array->p_buffer, 0);
    REQUIRE_EQ(p_array->len, 0);
    REQUIRE_EQ(p_array->capacity, 0);
    bj_array_del(p_array);
}

TEST_CASE(pop_nonempty_reduces_len_but_not_capacity_nor_pointer) {
    bj_array* p_array = bj_array_new_t(payload);

    size_t len = 10;
    bj_array_set_len(p_array, len);
    void* buffer  = p_array->p_buffer;
    size_t capacity = p_array->capacity;

    do {
        bj_array_pop(p_array);
        REQUIRE_EQ(p_array->len, --len);
        REQUIRE_EQ(p_array->capacity, capacity);
        REQUIRE_EQ(p_array->p_buffer, buffer);
    } while(len > 0);
    bj_array_del(p_array);
}

TEST_CASE(at_0_returns_0) {
    REQUIRE_EQ(bj_array_at(0, 0), 0);
    REQUIRE_EQ(bj_array_at(0, 1), 0);
    REQUIRE_EQ(bj_array_at(0, 2), 0);
    REQUIRE_EQ(bj_array_at(0, 3), 0);
    REQUIRE_EQ(bj_array_at(0, 4), 0);
}

TEST_CASE(at_empty_returns_0) {
    bj_array* p_array = bj_array_new_t(payload);
    REQUIRE_EQ(bj_array_at(p_array, 0), 0);
    REQUIRE_EQ(bj_array_at(p_array, 1), 0);
    REQUIRE_EQ(bj_array_at(p_array, 2), 0);
    REQUIRE_EQ(bj_array_at(p_array, 3), 0);
    REQUIRE_EQ(bj_array_at(p_array, 4), 0);
    bj_array_del(p_array);
}

TEST_CASE(at_nonempty_returns_indexed_value) {
    bj_array* p_array = bj_array_new_t(payload);

    for(short i = 0 ; i < 10 ; ++i) {

        payload p = {.elem0 = i * 2, .elem1 = -((long)i)};
        bj_array_push(p_array, &p);

        payload* got = bj_array_at(p_array, i);

        REQUIRE_EQ(p.elem0, got->elem0);
        REQUIRE_EQ(p.elem1, got->elem1);

        int diff = bj_memcmp(&p, got, sizeof(payload));
        REQUIRE_EQ(diff, 0);
    }

    bj_array_del(p_array);
}

int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    RUN_TEST(initialize_with_payload_gives_empty_array);
    RUN_TEST(clear_empty_does_nothing);
    RUN_TEST(clear_filled_reduces_size_to_zero);
    RUN_TEST(clear_filled_does_not_change_capacity);
    RUN_TEST(shrink_empty_does_nothing);
    RUN_TEST(shrink_sets_capacity_to_size);
    RUN_TEST(set_greater_len_changes_len_and_capacity);
    RUN_TEST(set_lower_len_changes_len_but_not_capacity);
    RUN_TEST(reserve_greater_capacity_growth_buffer);
    RUN_TEST(reserve_smaller_or_equal_capacity_does_nothing);
    RUN_TEST(push_into_empty_array_creates_new_buffer);
    RUN_TEST(push_growth_len_by_1);
    RUN_TEST(push_growth_capacity_only_if_equals_to_len);
    RUN_TEST(pop_empty_does_nothing);
    RUN_TEST(pop_nonempty_reduces_len_but_not_capacity_nor_pointer);
    RUN_TEST(at_0_returns_0);
    RUN_TEST(at_empty_returns_0);
    RUN_TEST(at_nonempty_returns_indexed_value);

    END_TESTS();
}

