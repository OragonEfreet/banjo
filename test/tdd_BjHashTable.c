#include "test.h"

#include <banjo/hash_table.h>

TEST_CASE(zero_initialization) {

    u32 key = 42;
    short value = 17;

    BjHashTable htable = bj_hash_table_create(&(BjHashTableInfo){
        .value_size=sizeof(short),
        .key_size=sizeof(u32),
    }, 0);
    REQUIRE_VALUE(htable);


    bj_hash_table_set(htable, &key, &value);
    bj_hash_table_set(htable, &key, &value);

    /* REQUIRE_EQ(htable->p_allocator, 0); */

    bj_hash_table_destroy(htable);
}


int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    RUN_TEST(zero_initialization);
    /* RUN_TEST(custom_allocator); */

    END_TESTS();
}
