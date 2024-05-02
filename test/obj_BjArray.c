#include "test.h"

#include <banjo/array.h>

BjArray array;

TEST_CASE(initialize_with_null_info_returns_nil_object) {
    bj_array_init(&array, 0);
    REQUIRE_EMPTY(BjArray, &array);
}

TEST_CASE(alloc_function_returns_valid_pointer) {
    void* block = bj_array_alloc();
    REQUIRE_VALUE(block);
    bj_free(block);
}

TEST_CASE(delete_null_pointer_is_valid_action) {
    bj_array_del(0);
}

TEST_CASE_ARGS(initializes_to_nil, {BjArrayInfo info;} ) {
    bj_array_init(&array, &test_data->info);
    REQUIRE_EMPTY(BjArray, &array);
}

int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    RUN_TEST(initialize_with_null_info_returns_nil_object);
    RUN_TEST(alloc_function_returns_valid_pointer);
    RUN_TEST(delete_null_pointer_is_valid_action);
    RUN_TEST_ARGS(initializes_to_nil, {.bytes_payload = 0});

    END_TESTS();
}
