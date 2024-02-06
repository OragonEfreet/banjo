#include "test.h"
#include "custom_allocators.h"

#include <banjo/array.h>


TEST_CASE(zero_initialization) {
    BjArray array = {};

    CHECK_EQ(array.capacity, 0);
    CHECK_EQ(array.count, 0);
    CHECK_EQ(array.pAllocator, 0);
    CHECK_EQ(array.data, 0);

    BjResult result = bjInitArray(&array);
    REQUIRE_EQ(result, BJ_SUCCESS);

    REQUIRE_EQ(array.capacity, 0);
    REQUIRE_EQ(array.count, 0);
    REQUIRE_EQ(array.pAllocator, 0);
    REQUIRE_EQ(array.data, 0);
}

TEST_CASE(custom_allocator) {
    BjAllocationCallbacks allocator = mock_allocators();
    BjArray array = { .pAllocator = &allocator };

    CHECK(array.pAllocator != 0);

    BjResult result = bjInitArray(&array);
    REQUIRE_EQ(result, BJ_SUCCESS);

    REQUIRE(array.pAllocator != 0);
}

TEST_CASE(init_explicit_capacity) {
    BjAllocationCallbacks allocator = mock_allocators();
    BjArray array = {.capacity = 10};

    BjResult result = bjInitArray(&array);
    REQUIRE_EQ(result, BJ_SUCCESS);

    REQUIRE(array.capacity > 0);
    REQUIRE(array.capacity >= array.count);
    REQUIRE(array.count == 0);
    REQUIRE(array.data != 0);
}

int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    RUN_TEST(zero_initialization);
    RUN_TEST(custom_allocator);
    RUN_TEST(init_explicit_capacity);

    END_TESTS();
}
