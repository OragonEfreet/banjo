#include "test.h"

#include <banjo/hash_table.h>

TEST_CASE(zero_initialization) {

    BjHashTable* htable = bj_hash_table_new(&(BjHashTableInfo){
        .value_size=sizeof(short),
        .key_size=sizeof(u32),
    }, 0);
    REQUIRE_VALUE(htable);

    bj_hash_table_del(htable);
}

TEST_CASE(set_data) {

    u32 key = 42;
    short value = 17;

    BjHashTable* htable = bj_hash_table_new(&(BjHashTableInfo){
        .value_size=sizeof(short),
        .key_size=sizeof(u32),
    }, 0);
    REQUIRE_VALUE(htable);

    bj_hash_table_set(htable, &key, &value);
    bj_hash_table_set(htable, &key, &value);

    /* REQUIRE_EQ(htable->p_allocator, 0); */

    bj_hash_table_del(htable);
}

TEST_CASE(get_data) {

    u32 key = 42;
    short value0 = 17;
    short value1 = 50;

    BjHashTable* htable = bj_hash_table_new(&(BjHashTableInfo){
        .value_size=sizeof(short),
        .key_size=sizeof(u32),
    }, 0);
    REQUIRE_VALUE(htable);

    bj_hash_table_set(htable, &key, &value0);
    short* got = bj_hash_table_get(htable, &key, 0);
    REQUIRE_EQ(*got, value0);

    bj_hash_table_set(htable, &key, &value1);
    got = bj_hash_table_get(htable, &key, 0);
    REQUIRE_EQ(*got, value1);

    bj_hash_table_del(htable);
}

TEST_CASE(get_data_default) {

    u32 key = 42;
    short fallback = 50;

    BjHashTable* htable = bj_hash_table_new(&(BjHashTableInfo){
        .value_size=sizeof(short),
        .key_size=sizeof(u32),
    }, 0);
    REQUIRE_VALUE(htable);

    short* got = bj_hash_table_get(htable, &key, 0);
    REQUIRE_EQ(got, 0);

    got = bj_hash_table_get(htable, &key, &fallback);
    REQUIRE_EQ(got, &fallback);

    bj_hash_table_del(htable);
}


int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    RUN_TEST(zero_initialization);
    RUN_TEST(set_data);
    RUN_TEST(get_data);
    RUN_TEST(get_data_default);

    END_TESTS();
}
