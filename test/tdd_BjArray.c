#include "test.h"

#include <banjo/array.h>

TEST_CASE(zero_initialization) {

    BjArray* array = bj_array_new(&(BjArrayInfo){.bytes_payload=1}, 0);
    REQUIRE_VALUE(array);

    REQUIRE_EQ(array->capacity, 0);
    REQUIRE_EQ(array->count, 0);
    REQUIRE_EQ(array->p_allocator, 0);
    REQUIRE_EQ(array->p_buffer, 0);

    bj_array_del(array);
}

TEST_CASE(initialize_with_size_allocates_memory) {
    BjArray* array = bj_array_new(&(BjArrayInfo){.bytes_payload=1, .count = 10}, 0);
    REQUIRE_EQ(array->count, 10);
    REQUIRE_VALUE(array->p_buffer);
    bj_array_del(array);
}

TEST_CASE(initialize_with_size_set_shrink_capacity) {
    BjArray* array = bj_array_new(&(BjArrayInfo){.bytes_payload=1, .count = 10}, 0);
    REQUIRE_EQ(array->count, array->capacity);
    bj_array_del(array);
}

TEST_CASE(init_explicit_capacity) {

    BjArray* array = bj_array_new(&(BjArrayInfo){.bytes_payload=1}, 0);
    REQUIRE_VALUE(array);

    bj_array_del(array);
}

typedef struct {
    int i;
} value_type;

static value_type values[] = {
    {.i = 42},
    {.i = 420},
};
static usize n_values = sizeof(values) / sizeof(values[0]);

TEST_CASE(each_insertion_growth_count_by_1) {
    BjArray* array = bj_array_new(&(BjArrayInfo){.bytes_payload=sizeof(value_type)}, 0);

    for(usize i = 0 ; i < n_values ; ++i) {
        bj_array_push(array, &values[i]);
        REQUIRE_EQ(bj_array_count(array), i + 1);
        REQUIRE_EQ(array->count, i + 1);
    }

    bj_array_del(array);
}

TEST_CASE(inserted_value_retrieved_using_last_index) {
    BjArray* array = bj_array_new(&(BjArrayInfo){.bytes_payload=sizeof(value_type)}, 0);

    for(usize i = 0 ; i < n_values ; ++i) {
        bj_array_push(array, &values[i]);

        value_type* got = bj_array_at(array, i);
        REQUIRE_VALUE(got);
        REQUIRE_EQ(got->i, values[i].i);
    }

    bj_array_del(array);
}

TEST_CASE(capacity_grows_twice_count) {
    BjArray* array = bj_array_new(&(BjArrayInfo){.bytes_payload=sizeof(value_type)}, 0);

    for(usize i = 0 ; i < n_values ; ++i) {
        bj_array_push(array, &values[i]);
        usize count = bj_array_count(array);
        REQUIRE_EQ(array->capacity, count * 2);
    }

    bj_array_del(array);
}

TEST_CASE(capacity_is_not_updated_when_big_enough) {
    BjArray* array = bj_array_new(&(BjArrayInfo){.bytes_payload=sizeof(value_type)}, 0);
    usize expected_capacity = n_values * 2;
    bj_array_reserve(array, expected_capacity);

    for(usize i = 0 ; i < n_values ; ++i) {
        bj_array_push(array, &values[i]);
        REQUIRE_EQ(array->capacity, expected_capacity);
    }

    bj_array_del(array);
}

TEST_CASE(asking_smaller_capacity_does_nothing) {
    BjArray* array = bj_array_new(&(BjArrayInfo){.bytes_payload=sizeof(value_type)}, 0);
    bj_array_reserve(array, 10);
    bj_array_reserve(array, 5);
    REQUIRE_EQ(array->capacity, 10);
    bj_array_del(array);
}

TEST_CASE(clear_array_set_count_to_0) {
    BjArray* array = bj_array_new(&(BjArrayInfo){.bytes_payload=sizeof(value_type)}, 0);
    usize expected_capacity = n_values * 2;
    bj_array_reserve(array, expected_capacity);

    for(usize i = 0 ; i < n_values ; ++i) {
        bj_array_push(array, &values[i]);
    }
    bj_array_clear(array);
    REQUIRE_EQ(array->count, 0);
    usize count = bj_array_count(array);
    REQUIRE_EQ(count, 0);

    bj_array_del(array);
}

TEST_CASE(clear_and_shrink_array_clears_memory) {
    BjArray* array = bj_array_new(&(BjArrayInfo){.bytes_payload=sizeof(value_type)}, 0);
    usize expected_capacity = n_values * 2;
    bj_array_reserve(array, expected_capacity);

    for(usize i = 0 ; i < n_values ; ++i) {
        bj_array_push(array, &values[i]);
    }
    bj_array_clear(array);
    bj_array_shrink(array);
    REQUIRE_EQ(array->capacity, 0);
    REQUIRE_EQ(array->p_buffer, 0);

    bj_array_del(array);
}

TEST_CASE(shrink_sets_capacity_to_size) {
    BjArray* array = bj_array_new(&(BjArrayInfo){.bytes_payload=sizeof(value_type)}, 0);

    for(usize i = 0 ; i < n_values ; ++i) {
        bj_array_push(array, &values[i]);
        bj_array_shrink(array);
        REQUIRE_EQ(array->capacity, array->count);
    }
    bj_array_del(array);
}

int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    RUN_TEST(zero_initialization);
    RUN_TEST(initialize_with_size_allocates_memory);
    RUN_TEST(initialize_with_size_set_shrink_capacity);
    RUN_TEST(init_explicit_capacity);
    RUN_TEST(each_insertion_growth_count_by_1);
    RUN_TEST(inserted_value_retrieved_using_last_index);
    RUN_TEST(capacity_grows_twice_count);
    RUN_TEST(capacity_is_not_updated_when_big_enough);
    RUN_TEST(asking_smaller_capacity_does_nothing);
    RUN_TEST(clear_array_set_count_to_0);
    RUN_TEST(clear_and_shrink_array_clears_memory);
    RUN_TEST(shrink_sets_capacity_to_size);

    END_TESTS();
}
