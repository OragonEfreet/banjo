#include "test.h"

#include <array.h>

TEST_CASE(zero_initialization) {

    BjArray array = bj_array_create(&(BjArrayInfo){.value_size=1}, 0);
    REQUIRE_VALUE(array);

    REQUIRE_EQ(array->capacity, 0);
    REQUIRE_EQ(array->count, 0);
    REQUIRE_EQ(array->p_allocator, 0);
    REQUIRE_EQ(array->p_data, 0);

    bj_array_destroy(array);
}

TEST_CASE(init_explicit_capacity) {

    BjArray array = bj_array_create(&(BjArrayInfo){.value_size=1}, 0);
    REQUIRE_VALUE(array);

    bj_array_destroy(array);
}

typedef struct {
    int i;
} value_type;

static value_type values[] = {
    {.i = 42},
    {.i = 420},
};
static usize n_values = sizeof(values) / sizeof(values[0]);

TEST_CASE(insertions) {

    BjArray array = bj_array_create(&(BjArrayInfo){.value_size=sizeof(value_type)}, 0);
    CHECK_VALUE(array);

    CHECK_EQ(array->capacity, 0);
    CHECK_EQ(array->count, 0);
    CHECK_EQ(array->p_allocator, 0);
    CHECK_EQ(array->p_data, 0);

    for(usize i = 0 ; i < n_values ; ++i) {
        bj_array_push(array, &values[i]);
        REQUIRE_EQ(bj_array_count(array), i + 1);
        REQUIRE_EQ(array->count, i + 1);
        
        value_type* got = bj_array_at(array, i);
        REQUIRE_VALUE(got);
        REQUIRE_EQ(got->i, values[i].i);
    }

    bj_array_destroy(array);
}


int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    RUN_TEST(zero_initialization);
    RUN_TEST(init_explicit_capacity);
    RUN_TEST(insertions);

    END_TESTS();
}
