#include "test.h"
#include "custom_allocators.h"

#include <banjo/array.h>


TEST_CASE(zero_initialization) {
    BjArray array;

    BjResult result = bjCreateArray(&(BjArrayCreateInfo){.elem_size=1}, &array);
    REQUIRE_EQ(result, BJ_SUCCESS);

    REQUIRE_EQ(array->capacity, 0);
    REQUIRE_EQ(array->count, 0);
    REQUIRE_EQ(array->pAllocator, 0);
    REQUIRE_EQ(array->data, 0);

    bjDestroyArray(array);
}

TEST_CASE(custom_allocator) {
    BjAllocationCallbacks allocator = mock_allocators();
    BjArray array;

    BjResult result = bjCreateArray(&(BjArrayCreateInfo){ .pAllocator = &allocator, .elem_size=1 },&array);
    REQUIRE_EQ(result, BJ_SUCCESS);

    REQUIRE(array->pAllocator != 0);

    bjDestroyArray(array);
}

TEST_CASE(init_explicit_capacity) {
    BjArray array;

    BjResult result = bjCreateArray(&(BjArrayCreateInfo){.capacity = 10, .elem_size=1},&array);
    REQUIRE_EQ(result, BJ_SUCCESS);

    REQUIRE(array->capacity >= 10);
    REQUIRE(array->capacity >= array->count);
    REQUIRE(array->count == 0);
    REQUIRE(array->data != 0);

    bjDestroyArray(array);
}

int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    RUN_TEST(zero_initialization);
    RUN_TEST(custom_allocator);
    RUN_TEST(init_explicit_capacity);

    END_TESTS();
}
