#include "test.h"

#include <banjo/list.h>

#include "htable_t.h"
#include "list_t.h"

typedef struct {
    short elem0;
    long  elem1;
} value_t;

typedef int key_t;

TEST_CASE(empty_valid_initialization) {
    bj_htable* htable = bj_htable_new_t(key_t, value_t);
    REQUIRE_EQ(htable->buckets.bytes_payload, sizeof(bj_list));
    REQUIRE_EQ(htable->buckets.len, 10);
    REQUIRE(htable->buckets.capacity >= 10);
    REQUIRE_VALUE(htable->buckets.p_buffer);
    REQUIRE_EQ(htable->weak_owning, false);
    REQUIRE_EQ(htable->bytes_key, sizeof(key_t));
    REQUIRE_EQ(htable->bytes_value, sizeof(value_t));
    REQUIRE_VALUE(htable->fn_hash);
    REQUIRE_EQ(htable->bytes_entry, sizeof(value_t) + sizeof(key_t));
    bj_htable_del(htable);
}

TEST_CASE(init_with_zero_bytes_value_gives_0) {
    bj_htable* htable = bj_htable_new(sizeof(key_t), 0);
    REQUIRE_NULL(htable);
    bj_htable_del(htable);
}

TEST_CASE(init_with_zero_bytes_key_gives_0) {
    bj_htable* htable = bj_htable_new(0, sizeof(value_t));
    REQUIRE_NULL(htable);
    bj_htable_del(htable);
}

TEST_CASE(init_with_no_hash_gives_default_hash) {
    bj_htable* htable = bj_htable_new_t(key_t, value_t);
    REQUIRE_VALUE(htable->fn_hash);
    bj_htable_del(htable);
}


TEST_CASE(len_0_returns_0) {
    REQUIRE_EQ(bj_htable_len(0), 0);
}

TEST_CASE(len_empty_returns_0) {
    bj_htable* htable = bj_htable_new_t(key_t, value_t);
    REQUIRE_EQ(bj_htable_len(htable), 0);
    bj_htable_del(htable);
}

TEST_CASE(set_a_value_with_new_key_growth_len_by_one) {
    value_t value;

    bj_htable* htable = bj_htable_new_t(key_t, value_t);

    for(key_t count = 1 ; count < 9 ; ++count) {
        bj_htable_set(htable, &count, &value);
        REQUIRE_EQ(bj_htable_len(htable), (size_t)count);
    }
    bj_htable_del(htable);
}

TEST_CASE(set_a_value_with_existing_key_does_not_change_len) {
    key_t key;
    value_t value;

    bj_htable* htable = bj_htable_new_t(key_t, value_t);

    bj_htable_set(htable, &key, &value);
    CHECK_EQ(bj_htable_len(htable), 1);
    bj_htable_set(htable, &key, &value);
    REQUIRE_EQ(bj_htable_len(htable), 1);
    bj_htable_del(htable);
}

TEST_CASE(set_a_value_with_new_key_returns_new_address) {
    key_t key = 0;
    value_t value;

    bj_htable* htable = bj_htable_new_t(key_t, value_t);

    const void* data = bj_htable_set(htable, &key, &value);

    for(key_t count = 1 ; count < 9 ; ++count) {
        const void* new_data = bj_htable_set(htable, &count, &value);
        REQUIRE(data != new_data);
    }
    bj_htable_del(htable);
}

TEST_CASE(set_a_value_with_existing_key_returns_same_address) {
    key_t key;
    value_t value;

    bj_htable* htable = bj_htable_new_t(key_t, value_t);

    const void* data = bj_htable_set(htable, &key, &value);
    const void* new_data = bj_htable_set(htable, &key, &value);
    REQUIRE_EQ(data, new_data);
    bj_htable_del(htable);
}

TEST_CASE(get_from_0_returns_0) {
    for(key_t count = 0 ; count < 10 ; ++count) {
        const void* got = bj_htable_get(0, &count, 0);
        REQUIRE_NULL(got);
    }
}

TEST_CASE(get_empty_returns_0) {
    bj_htable* htable = bj_htable_new_t(key_t, value_t);
    for(key_t count = 0 ; count < 10 ; ++count) {
        const void* got = bj_htable_get(htable, &count, 0);
        REQUIRE_NULL(got);
    }
    bj_htable_del(htable);
}

TEST_CASE(get_existing_key_returns_associated_value) {
    key_t key;
    value_t value;

    bj_htable* htable = bj_htable_new_t(key_t, value_t);
    
    const void* data = bj_htable_set(htable, &key, &value);
    const void* got = bj_htable_get(htable, &key, 0);
    REQUIRE_EQ(data, got);

    bj_htable_del(htable);
}

TEST_CASE(get_nonexisting_key_returns_0) {
    key_t existing_key = 0;
    key_t nonexisting_key = 1;;
    value_t value;

    bj_htable* htable = bj_htable_new_t(key_t, value_t);
    
    bj_htable_set(htable, &existing_key, &value);

    const void* got = bj_htable_get(htable, &nonexisting_key, 0);
    REQUIRE_NULL(got);

    bj_htable_del(htable);
}

int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    RUN_TEST(empty_valid_initialization);
    RUN_TEST(init_with_zero_bytes_value_gives_0);
    RUN_TEST(init_with_zero_bytes_key_gives_0);
    RUN_TEST(init_with_no_hash_gives_default_hash);
    RUN_TEST(len_0_returns_0);
    RUN_TEST(len_empty_returns_0);
    RUN_TEST(set_a_value_with_new_key_growth_len_by_one);
    RUN_TEST(set_a_value_with_existing_key_does_not_change_len);
    RUN_TEST(set_a_value_with_existing_key_returns_same_address);
    RUN_TEST(set_a_value_with_new_key_returns_new_address);
    RUN_TEST(get_from_0_returns_0);
    RUN_TEST(get_empty_returns_0);
    RUN_TEST(get_existing_key_returns_associated_value);
    RUN_TEST(get_nonexisting_key_returns_0);

    END_TESTS();
}
