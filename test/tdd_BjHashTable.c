#include "test.h"

#include <banjo/hash_table.h>

TEST_CASE_ARGS(zero_initialization, {}) {

    float value = 42.7;

    BjHashTable htable = bjCreateHashTable(&(BjHashTableInfo){
        .value_size=sizeof(u32),
        .key_size=sizeof(float),
    }, 0);
    REQUIRE_VALUE(htable);

    for(u32 key = 0 ; key < 10000 ; ++key) {
        bjHashTableSet(htable, &key, &value);
    }




    /* REQUIRE_EQ(htable->pAllocator, 0); */

    bjDestroyHashTable(htable);
}


int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    RUN_TEST(zero_initialization);
    /* RUN_TEST(custom_allocator); */

    END_TESTS();
}
