#include "test.h"

#include <banjo/hash_table.h>

BjHashTable htable;

TEST_CASE(initialize_with_null_info_returns_nil_object) {
    bj_hash_table_init(&htable, 0, 0);
    REQUIRE_EMPTY(BjHashTable, &htable);
}

TEST_CASE(alloc_function_returns_valid_pointer) {
    void* block = bj_hash_table_alloc(0);
    REQUIRE_VALUE(block);
    bj_free(block, 0);
}

TEST_CASE(delete_null_pointer_is_valid_action) {
    bj_hash_table_del(0);
}

TEST_CASE_ARGS(initializes_to_nil, {BjHashTableInfo info;} ) {
    bj_hash_table_init(&htable, &test_data->info, 0);
    REQUIRE_EMPTY(BjHashTable, &htable);
}

int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    RUN_TEST(initialize_with_null_info_returns_nil_object);
    RUN_TEST(alloc_function_returns_valid_pointer);
    RUN_TEST(delete_null_pointer_is_valid_action);
    RUN_TEST_ARGS(initializes_to_nil, {.bytes_value = 0});
    RUN_TEST_ARGS(initializes_to_nil, {.bytes_key = 0});

    END_TESTS();
}
