#include "test.h"

#include <banjo/array.h>

BjArray array;

typedef struct {
    short elem0;
    long  elem1;
} payload;
static const usize bytes_payload = sizeof(payload);

TEST_CASE(zero_initialization) {
    bj_array_init(&array, 0, 0);
    REQUIRE_EMPTY(BjArray, &array);
}

TEST_CASE(alloc) {
    void* block = bj_array_alloc(0);
    REQUIRE_VALUE(block);
    bj_free(block, 0);
}

TEST_CASE(del_0_is_ok) {
    bj_array_del(0);
}

TEST_CASE(invalid_byte_size_is_nil) {
    BjArrayInfo info = {.bytes_payload = 0};
    bj_array_init(&array, &info, 0);
    REQUIRE_EMPTY(BjArray, &array);
}

TEST_CASE(empty_initialization) {
    BjArrayInfo info = {.bytes_payload = bytes_payload};
    bj_array_init(&array, &info, 0);

    REQUIRE_EQ(array.p_allocator, 0);
    REQUIRE_EQ(array.bytes_payload, bytes_payload);
    REQUIRE_EQ(array.capacity, 0);
    REQUIRE_EQ(array.len, 0);
    REQUIRE_EQ(array.p_buffer, 0);
}

TEST_CASE_ARGS(init_with_capacity_allocates_buffer, {usize capacity;}) {

    BjArrayInfo info = {.bytes_payload = bytes_payload, .capacity = 10};
    bj_array_init(&array, &info, 0);

    REQUIRE_EQ(array.p_allocator, 0);
    REQUIRE_EQ(array.bytes_payload, bytes_payload);
    REQUIRE_EQ(array.capacity, test_data->capacity);
    REQUIRE_EQ(array.len, 0);
    REQUIRE_VALUE(array.p_buffer);

    bj_array_reset(&array);
};

TEST_CASE_ARGS(init_with_len_allocates_buffer, {usize len;}) {

    BjArrayInfo info = {.bytes_payload = bytes_payload, .len = test_data->len};
    bj_array_init(&array, &info, 0);

    REQUIRE_EQ(array.p_allocator, 0);
    REQUIRE_EQ(array.bytes_payload, bytes_payload);
    REQUIRE(array.capacity >= test_data->len);
    REQUIRE_EQ(array.len, test_data->len);
    REQUIRE_VALUE(array.p_buffer);
    bj_array_reset(&array);
};

TEST_CASE_ARGS(init_with_len_gt_capacity, {usize len; usize capacity;}) {

    BjArrayInfo info = {
        .bytes_payload = bytes_payload,
        .len         = test_data->len,
        .capacity      = test_data->capacity
    };
    bj_array_init(&array, &info, 0);

    REQUIRE_EQ(array.p_allocator, 0);
    REQUIRE_EQ(array.bytes_payload, bytes_payload);
    REQUIRE(array.capacity >= test_data->len);
    REQUIRE_EQ(array.len, test_data->len);
    REQUIRE_VALUE(array.p_buffer);
    bj_array_reset(&array);
};

TEST_CASE_ARGS(init_with_len_lt_capacity, {usize len; usize capacity;}) {

    BjArrayInfo info = {
        .bytes_payload = bytes_payload,
        .len           = test_data->len,
        .capacity      = test_data->capacity
    };
    bj_array_init(&array, &info, 0);

    REQUIRE_EQ(array.p_allocator, 0);
    REQUIRE_EQ(array.bytes_payload, bytes_payload);
    REQUIRE_EQ(array.capacity, test_data->capacity);
    REQUIRE_EQ(array.len, test_data->len);
    REQUIRE_VALUE(array.p_buffer);
    bj_array_reset(&array);
};

TEST_CASE(empty) {
    bj_array_init(&array, 0, 0);
    void* block = 0;

    bj_array_clear(&array);                      REQUIRE_EMPTY(BjArray, &array);
    bj_array_shrink(&array);                     REQUIRE_EMPTY(BjArray, &array);
    bj_array_set_len(&array, 10);              REQUIRE_EMPTY(BjArray, &array);
    bj_array_reserve(&array, 10);                REQUIRE_EMPTY(BjArray, &array);
    bj_array_push(&array, &(payload){.elem0=0}); REQUIRE_EMPTY(BjArray, &array);
    bj_array_pop(&array);                        REQUIRE_EMPTY(BjArray, &array);
    block = bj_array_at(&array, 10);             REQUIRE_EMPTY(BjArray, &array); REQUIRE_NULL(block);
    block = bj_array_data(&array);               REQUIRE_EMPTY(BjArray, &array); REQUIRE_NULL(block);
    usize len = bj_array_len(&array);        REQUIRE_EMPTY(BjArray, &array); REQUIRE_EQ(len, 0);
}

int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    RUN_TEST(zero_initialization);
    RUN_TEST(alloc);
    RUN_TEST(del_0_is_ok);
    RUN_TEST(invalid_byte_size_is_nil);
    RUN_TEST(empty_initialization);
    RUN_TEST_ARGS(init_with_capacity_allocates_buffer, .capacity = 10);
    RUN_TEST_ARGS(init_with_len_allocates_buffer, .len = 10);
    RUN_TEST_ARGS(init_with_len_gt_capacity, .len = 10, .capacity = 5);
    RUN_TEST_ARGS(init_with_len_lt_capacity, .len = 5, .capacity = 10);
    RUN_TEST(empty);

    END_TESTS();
}
