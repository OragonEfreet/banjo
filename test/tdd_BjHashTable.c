#include "test.h"

#include <banjo/hash_table.h>

TEST_CASE_ARGS(zero_initialization, {}) {
    BjHashTable htable;

    u32 key = 97;
    float value = 42.7;

    BjResult result = bjCreateHashTable(&(BjHashTableInfo){
        .value_size=sizeof(u32),
        .key_size=sizeof(float),
    }, &htable);
    REQUIRE_EQ(result, BJ_SUCCESS);

    bjHashTableSet(htable, &key, &value);




    /* REQUIRE_EQ(htable->pAllocator, 0); */

    bjDestroyHashTable(htable);
}


int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    RUN_TEST(zero_initialization);
    /* RUN_TEST(custom_allocator); */

    END_TESTS();
}
