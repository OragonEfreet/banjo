#include "test.h"
#include "mock_memory.h"

#include <banjo/hash_table.h>

TEST_CASE_ARGS(zero_initialization, {}) {
    BjHashTable htable;

    BjResult result = bjCreateHashTable(&(BjHashTableCreateInfo){.elem_size=1}, &htable);
    REQUIRE_EQ(result, BJ_SUCCESS);

    /* REQUIRE_EQ(htable->pAllocator, 0); */

    bjDestroyHashTable(htable);
}

TEST_CASE_ARGS(custom_allocator, {}) {
    BjAllocationCallbacks allocator = mock_allocators(0);
    BjHashTable htable;

    BjResult result = bjCreateHashTable(&(BjHashTableCreateInfo){ .pAllocator = &allocator, .elem_size=1 },&htable);
    REQUIRE_EQ(result, BJ_SUCCESS);

    /* REQUIRE(htable->pAllocator != 0); */

    bjDestroyHashTable(htable);
}

int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    RUN_TEST(zero_initialization);
    RUN_TEST(custom_allocator);

    END_TESTS();
}
