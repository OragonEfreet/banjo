#include "test.h"

#include <banjo/hash_table.h>

TEST_CASE_ARGS(zero_initialization, {}) {

    float value = 42.7;

    BjHashTable htable = bj_hash_table_create(&(BjHashTableInfo){
        .value_size=sizeof(u32),
        .key_size=sizeof(float),
    }, 0);
    REQUIRE_VALUE(htable);

    for(u32 key = 0 ; key < 10 ; ++key) {
        bj_hash_table_set(htable, &key, &value);
    }

    /* REQUIRE_EQ(htable->p_allocator, 0); */

    bj_hash_table_destroy(htable);
}


int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    RUN_TEST(zero_initialization);
    /* RUN_TEST(custom_allocator); */

    END_TESTS();
}
