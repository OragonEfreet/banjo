#include "test.h"

#include <banjo/array.h>

#include <string.h>

BjArray array;

typedef struct {
    short elem0;
    long  elem1;
} payload;
static const usize bytes_payload = sizeof(payload);

TEST_CASE(initialize_with_payload_gives_empty_array) {
    BjArrayInfo info = {.bytes_payload = bytes_payload};
    bj_array_init(&array, &info);

    REQUIRE_EQ(array.info.p_allocator, 0);
    REQUIRE_EQ(array.info.bytes_payload, bytes_payload);
    REQUIRE_EQ(array.info.capacity, 0);
    REQUIRE_EQ(array.info.len, 0);
    REQUIRE_EQ(array.p_buffer, 0);
}

TEST_CASE(nonzero_capacity_allocates_buffer) {
    const usize capacity = 10;

    BjArrayInfo info = {.bytes_payload = bytes_payload, .capacity = capacity};
    bj_array_init(&array, &info);

    REQUIRE_EQ(array.info.p_allocator, 0);
    REQUIRE_EQ(array.info.bytes_payload, bytes_payload);
    REQUIRE_EQ(array.info.capacity, capacity);
    REQUIRE_EQ(array.info.len, 0);
    REQUIRE_VALUE(array.p_buffer);

    bj_array_reset(&array);
};

TEST_CASE(nonzero_len_allocates_buffer) {
    const usize len = 10;

    BjArrayInfo info = {.bytes_payload = bytes_payload, .len = len};
    bj_array_init(&array, &info);

    REQUIRE_EQ(array.info.p_allocator, 0);
    REQUIRE_EQ(array.info.bytes_payload, bytes_payload);
    REQUIRE(array.info.capacity >= len);
    REQUIRE_EQ(array.info.len, len);
    REQUIRE_VALUE(array.p_buffer);
    bj_array_reset(&array);
};

TEST_CASE(len_gt_capacity_expands_capacity) {
    usize len = 10;
    usize capacity = 5;

    BjArrayInfo info = {
        .bytes_payload = bytes_payload,
        .len           = len,
        .capacity      = capacity
    };
    bj_array_init(&array, &info);

    REQUIRE_EQ(array.info.p_allocator, 0);
    REQUIRE_EQ(array.info.bytes_payload, bytes_payload);
    REQUIRE(array.info.capacity >= len);
    REQUIRE_EQ(array.info.len, len);
    REQUIRE_VALUE(array.p_buffer);
    bj_array_reset(&array);
};

TEST_CASE(capacity_gt_len_does_not_expand_capacity) {
    usize len = 5;
    usize capacity = 10;

    BjArrayInfo info = {
        .bytes_payload = bytes_payload,
        .len           = len,
        .capacity      = capacity
    };
    bj_array_init(&array, &info);

    REQUIRE_EQ(array.info.p_allocator, 0);
    REQUIRE_EQ(array.info.bytes_payload, bytes_payload);
    REQUIRE_EQ(array.info.capacity, capacity);
    REQUIRE_EQ(array.info.len, len);
    REQUIRE_VALUE(array.p_buffer);
    bj_array_reset(&array);
};

TEST_CASE(clear_empty_does_nothing) {
    bj_array_init(&array, &(BjArrayInfo) {.bytes_payload = bytes_payload});
    CHECK_EQ(array.info.len, 0);
    bj_array_clear(&array);
    REQUIRE_EQ(array.info.len, 0);
    bj_array_reset(&array);
}

TEST_CASE(clear_nil_does_nothing) {
    bj_array_init(&array, 0);
    CHECK_EMPTY(BjArray, &array);
    bj_array_clear(&array);
    REQUIRE_EMPTY(BjArray, &array);
    bj_array_reset(&array);
}

TEST_CASE(clear_filled_reduces_size_to_zero) {
    bj_array_init(&array, 0);
    bj_array_push(&array, &(payload){.elem0 = 0});
    bj_array_clear(&array);
    REQUIRE_EMPTY(BjArray, &array);
    bj_array_reset(&array);
}

TEST_CASE(clear_filled_does_not_change_capacity) {
    bj_array_init(&array, 0);
    bj_array_push(&array, &(payload){.elem0 = 0});
    usize capacity = bj_array_capacity(&array);
    bj_array_clear(&array);
    REQUIRE_EQ(capacity, bj_array_capacity(&array));
    bj_array_reset(&array);
}

TEST_CASE(shrink_empty_does_nothing) {
    bj_array_init(&array, &(BjArrayInfo) {.bytes_payload = bytes_payload});
    CHECK_EQ(array.info.capacity, 0);
    bj_array_clear(&array);
    REQUIRE_EQ(array.info.capacity, 0);
    bj_array_reset(&array);
}

TEST_CASE(shrink_nil_does_nothing) {
    bj_array_init(&array, 0);
    CHECK_EQ(array.info.capacity, 0);
    bj_array_clear(&array);
    REQUIRE_EQ(array.info.capacity, 0);
    bj_array_reset(&array);
}

TEST_CASE(shrink_sets_capacity_to_size) {
    payload p;
    bj_array_init(&array, &(BjArrayInfo) {.bytes_payload = bytes_payload});

    for(usize len = 1 ; len <= 10 ; ++len) {
        bj_array_push(&array, &p);
        CHECK_EQ(array.info.len, len);
        CHECK(array.info.capacity >= len);

        bj_array_shrink(&array);
        usize got_len = bj_array_len(&array);
        usize got_capacity = bj_array_capacity(&array);
        REQUIRE_EQ(got_len, got_capacity);
    }
    bj_array_reset(&array);
}

TEST_CASE(set_greater_len_changes_len_and_capacity) {
    payload p;
    bj_array_init(&array, &(BjArrayInfo) {.bytes_payload = bytes_payload});

    for(usize len = 0 ; len < 10 ; ++len) {
        bj_array_set_len(&array, len);
        REQUIRE_EQ(array.info.len, len);
        REQUIRE(array.info.capacity >= len);
    }
    bj_array_reset(&array);
}

TEST_CASE(set_lower_len_changes_len_but_not_capacity) {
    payload p;
    bj_array_init(&array, &(BjArrayInfo) {.bytes_payload = bytes_payload});

    usize len = 9;
    do {
        bj_array_set_len(&array, len);
        REQUIRE_EQ(array.info.len, len);
        REQUIRE(array.info.capacity >= 9);
    } while(--len > 0);
    bj_array_reset(&array);
}

TEST_CASE(reserve_greater_capacity_growth_buffer) {
    bj_array_init(&array, &(BjArrayInfo) {.bytes_payload = bytes_payload});
    bj_array_reserve(&array, 10);
    REQUIRE(array.info.capacity >= 10);
    REQUIRE_VALUE(array.p_buffer);
    bj_array_reset(&array);
}

TEST_CASE(reserve_smaller_or_equal_capacity_does_nothing) {
    bj_array_init(&array, &(BjArrayInfo) {.bytes_payload = bytes_payload});

    bj_array_reserve(&array, 0);
    REQUIRE_EQ(array.info.capacity, 0);
    REQUIRE_EQ(array.p_buffer, 0);

    bj_array_reserve(&array, 10);
    CHECK(array.info.capacity >= 10);
    CHECK_VALUE(array.p_buffer);

    bj_array_reserve(&array, 5);
    REQUIRE(array.info.capacity >= 10);
    REQUIRE_VALUE(array.p_buffer);

    bj_array_reserve(&array, 0);
    REQUIRE(array.info.capacity >= 10);
    REQUIRE_VALUE(array.p_buffer);

    bj_array_reset(&array);
}

TEST_CASE(push_into_empty_array_creates_new_buffer) {
    payload p;
    bj_array_init(&array, &(BjArrayInfo) {.bytes_payload = bytes_payload});

    CHECK(array.p_buffer == 0);
    CHECK_EQ(array.info.len, 0);
    CHECK_EQ(array.info.capacity, 0);

    bj_array_push(&array, &p);

    REQUIRE(array.p_buffer != 0);
    REQUIRE_EQ(array.info.len, 1);
    REQUIRE(array.info.capacity >= 1);
    
    bj_array_reset(&array);
}

TEST_CASE(push_growth_len_by_1) {
    payload p;
    bj_array_init(&array, &(BjArrayInfo) {.bytes_payload = bytes_payload});

    for(usize i = 1 ; i < 9 ; ++i) {
        bj_array_push(&array, &p);
        REQUIRE_EQ(array.info.len, i);
    }

    bj_array_reset(&array);
}

TEST_CASE(push_growth_capacity_only_if_equals_to_len) {
    payload p;
    bj_array_init(&array, &(BjArrayInfo) {.bytes_payload = bytes_payload});

    for(usize i = 1 ; i < 90 ; ++i) {
        usize before_capacity  = array.info.capacity;
        usize capacity_resized = (before_capacity == array.info.len);

        bj_array_push(&array, &p);

        if(capacity_resized) {
            REQUIRE(array.info.capacity >= before_capacity);
        } else {
            REQUIRE_EQ(array.info.capacity, before_capacity);
        }
    }

    bj_array_reset(&array);
}

TEST_CASE(pop_nil_does_nothing) {
    bj_array_init(&array, 0);
    CHECK_EMPTY(BjArray, &array);
    bj_array_pop(&array);
    REQUIRE_EMPTY(BjArray, &array);
}

TEST_CASE(pop_empty_does_nothing) {
    bj_array_init(&array, &(BjArrayInfo) {.bytes_payload = bytes_payload});
    bj_array_pop(&array);
    REQUIRE_EQ(array.p_buffer, 0);
    REQUIRE_EQ(array.info.len, 0);
    REQUIRE_EQ(array.info.capacity, 0);
}

TEST_CASE(pop_nonempty_reduces_len_but_not_capacity_nor_pointer) {
    bj_array_init(&array, &(BjArrayInfo) {.bytes_payload = bytes_payload});

    usize len = 10;
    bj_array_set_len(&array, len);
    void* buffer  = array.p_buffer;
    usize capacity = array.info.capacity;

    do {
        bj_array_pop(&array);
        REQUIRE_EQ(array.info.len, --len);
        REQUIRE_EQ(array.info.capacity, capacity);
        REQUIRE_EQ(array.p_buffer, buffer);
    } while(len > 0);
    bj_array_reset(&array);
}

TEST_CASE(at_nil_returns_0) {
    bj_array_init(&array, 0);

    REQUIRE_EQ(bj_array_at(&array, 0), 0);
    REQUIRE_EQ(bj_array_at(&array, 1), 0);
    REQUIRE_EQ(bj_array_at(&array, 2), 0);
    REQUIRE_EQ(bj_array_at(&array, 3), 0);
    REQUIRE_EQ(bj_array_at(&array, 4), 0);
}

TEST_CASE(at_empty_returns_0) {
    bj_array_init(&array, &(BjArrayInfo) {.bytes_payload = bytes_payload});

    REQUIRE_EQ(bj_array_at(&array, 0), 0);
    REQUIRE_EQ(bj_array_at(&array, 1), 0);
    REQUIRE_EQ(bj_array_at(&array, 2), 0);
    REQUIRE_EQ(bj_array_at(&array, 3), 0);
    REQUIRE_EQ(bj_array_at(&array, 4), 0);
}

TEST_CASE(at_nonempty_returns_indexed_value) {
    bj_array_init(&array, &(BjArrayInfo) {.bytes_payload = bytes_payload});

    for(usize i = 0 ; i < 10 ; ++i) {

        payload p = {.elem0 = i * 2, .elem1 = -i};
        bj_array_push(&array, &p);

        payload* got = bj_array_at(&array, i);

        REQUIRE_EQ(p.elem0, got->elem0);
        REQUIRE_EQ(p.elem1, got->elem1);

        int diff = memcmp(&p, got, bytes_payload);
        REQUIRE_EQ(diff, 0);
    }

    bj_array_reset(&array);
}

int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    RUN_TEST(initialize_with_payload_gives_empty_array);
    RUN_TEST(nonzero_capacity_allocates_buffer);
    RUN_TEST(nonzero_len_allocates_buffer);
    RUN_TEST(len_gt_capacity_expands_capacity);
    RUN_TEST(capacity_gt_len_does_not_expand_capacity);
    RUN_TEST(clear_empty_does_nothing);
    RUN_TEST(clear_nil_does_nothing);
    RUN_TEST(clear_filled_reduces_size_to_zero);
    RUN_TEST(clear_filled_does_not_change_capacity);
    RUN_TEST(shrink_empty_does_nothing);
    RUN_TEST(shrink_nil_does_nothing);
    RUN_TEST(shrink_sets_capacity_to_size);
    RUN_TEST(set_greater_len_changes_len_and_capacity);
    RUN_TEST(set_lower_len_changes_len_but_not_capacity);
    RUN_TEST(reserve_greater_capacity_growth_buffer);
    RUN_TEST(reserve_smaller_or_equal_capacity_does_nothing);
    RUN_TEST(push_into_empty_array_creates_new_buffer);
    RUN_TEST(push_growth_len_by_1);
    RUN_TEST(push_growth_capacity_only_if_equals_to_len);
    RUN_TEST(pop_nil_does_nothing);
    RUN_TEST(pop_empty_does_nothing);
    RUN_TEST(pop_nonempty_reduces_len_but_not_capacity_nor_pointer);
    RUN_TEST(at_nil_returns_0);
    RUN_TEST(at_empty_returns_0);
    RUN_TEST(at_nonempty_returns_indexed_value);

    END_TESTS();
}

