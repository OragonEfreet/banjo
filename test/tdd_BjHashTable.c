#include "test.h"

#include <banjo/hash_table.h>
#include <banjo/list.h>

BjHashTable htable;

TEST_CASE(empty_valid_initialization) {
    BjHashTableInfo info = {.bytes_key = 2, .bytes_value = 4};
    bj_hash_table_init(&htable, &info, 0);

    REQUIRE_NULL(htable.p_allocator);
    REQUIRE_NULL(htable.buckets.p_allocator);
    REQUIRE_EQ(htable.buckets.bytes_payload, sizeof(BjList));
    REQUIRE_EQ(htable.buckets.capacity, 10);
    REQUIRE_EQ(htable.buckets.len, 10);
    REQUIRE_VALUE(htable.buckets.p_buffer);
    REQUIRE_EQ(htable.weak_owning, false);
    REQUIRE_EQ(htable.bytes_value, 4);
    REQUIRE_EQ(htable.bytes_key, 2);
    REQUIRE_VALUE(htable.fn_hash);
    REQUIRE_EQ(htable.bytes_entry, 6);

    bj_hash_table_reset(&htable);
}

int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    RUN_TEST(empty_valid_initialization);

    END_TESTS();
}
