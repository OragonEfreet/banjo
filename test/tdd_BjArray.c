#include "test.h"

#include <array.h>

TEST_CASE_ARGS(zero_initialization, {}) {

    BjArray array = bjCreateArray(&(BjArrayInfo){.value_size=1}, 0);
    REQUIRE_VALUE(array);

    REQUIRE_EQ(array->capacity, 0);
    REQUIRE_EQ(array->count, 0);
    REQUIRE_EQ(array->pAllocator, 0);
    REQUIRE_EQ(array->pData, 0);

    bjDestroyArray(array);
}

TEST_CASE_ARGS(init_explicit_capacity, {}) {

    BjArray array = bjCreateArray(&(BjArrayInfo){.value_size=1}, 0);
    REQUIRE_VALUE(array);

    bjDestroyArray(array);
}

int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    RUN_TEST(zero_initialization);
    RUN_TEST(init_explicit_capacity);

    END_TESTS();
}
