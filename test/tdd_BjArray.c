#include "test.h"

#include <array.h>

TEST_CASE_ARGS(zero_initialization, {}) {

    BjArray array = bj_array_create(&(BjArrayInfo){.value_size=1}, 0);
    REQUIRE_VALUE(array);

    REQUIRE_EQ(array->capacity, 0);
    REQUIRE_EQ(array->count, 0);
    REQUIRE_EQ(array->p_allocator, 0);
    REQUIRE_EQ(array->p_data, 0);

    bj_array_destroy(array);
}

TEST_CASE_ARGS(init_explicit_capacity, {}) {

    BjArray array = bj_array_create(&(BjArrayInfo){.value_size=1}, 0);
    REQUIRE_VALUE(array);

    bj_array_destroy(array);
}

int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    RUN_TEST(zero_initialization);
    RUN_TEST(init_explicit_capacity);

    END_TESTS();
}
