#include "test.h"

#include <banjo/hash_table.h>

TEST_CASE_ARGS(zero_initialization, {}) {

    u32 key = 97;
    float value = 42.7;

    BjHashTable htable = bjCreateHashTable(&(BjHashTableInfo){
        .value_size=sizeof(u32),
        .key_size=sizeof(float),
    });
    REQUIRE_VALUE(htable);

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
