#include "test.h"

#include <banjo/hash_table.h>
#include <banjo/list.h>

BjHashTable htable;

TEST_CASE(zero_initialization) {
    bj_hash_table_init(&htable, 0, 0);
    REQUIRE_EMPTY(BjHashTable, &htable);
}

TEST_CASE(alloc) {
    void* block = bj_hash_table_alloc(0);
    REQUIRE_VALUE(block);
    bj_free(block, 0);
}

TEST_CASE(del_0_is_ok) {
    bj_hash_table_del(0);
}

TEST_CASE(invalid_value_size_is_nil) {
    BjHashTableInfo info = {.bytes_value = 0};
    bj_hash_table_init(&htable, &info, 0);
    REQUIRE_EMPTY(BjHashTable, &htable);
}

TEST_CASE(invalid_key_size_is_nil) {
    BjHashTableInfo info = {.bytes_key = 0};
    bj_hash_table_init(&htable, &info, 0);
    REQUIRE_EMPTY(BjHashTable, &htable);
}

TEST_CASE(empty_valid_initialization) {
    BjHashTableInfo info = {.bytes_key = 2, .bytes_value = 4};
    bj_hash_table_init(&htable, &info, 0);

    REQUIRE_NULL(htable.p_allocator);
    REQUIRE_NULL(htable.buckets.p_allocator);
    REQUIRE_EQ(htable.buckets.bytes_payload, sizeof(BjList));
    REQUIRE_EQ(htable.buckets.capacity, 10);
    REQUIRE_EQ(htable.buckets.count, 10);
    REQUIRE_VALUE(htable.buckets.p_buffer);
    REQUIRE_EQ(htable.weak_owning, false);
    REQUIRE_EQ(htable.bytes_value, 4);
    REQUIRE_EQ(htable.bytes_key, 2);
    REQUIRE_VALUE(htable.fn_hash);
    REQUIRE_EQ(htable.bytes_entry, 6);
}

int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    RUN_TEST(zero_initialization);
    RUN_TEST(alloc);
    RUN_TEST(del_0_is_ok);
    RUN_TEST(invalid_value_size_is_nil);
    RUN_TEST(invalid_key_size_is_nil);
    RUN_TEST(empty_valid_initialization);

    END_TESTS();
}
