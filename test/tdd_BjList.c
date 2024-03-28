#include "banjo/list.h"
#include "test.h"

#include <banjo/list.h>

#include <string.h>

BjList list;

typedef struct {
    short elem0;
    long  elem1;
} payload;
static const usize bytes_payload = sizeof(payload);

TEST_CASE(initialize_with_payload_gives_empty_list) {
    BjListInfo info = {.bytes_payload = bytes_payload};
    bj_list_init(&list, &info, 0);

    REQUIRE_EQ(list.p_allocator, 0);
    REQUIRE_EQ(list.bytes_payload, bytes_payload);
    REQUIRE(list.bytes_entry > bytes_payload);
    REQUIRE_EQ(list.weak_owning, false);
    REQUIRE_EQ(list.p_head, 0);
}

TEST_CASE(clear_nil_does_nothing) {
    bj_list_init(&list, 0, 0);
    bj_list_clear(&list);
    REQUIRE_EMPTY(BjList, &list);
}

TEST_CASE(clear_empty_does_nothing) {
    BjListInfo info = {.bytes_payload = bytes_payload};
    bj_list_init(&list, &info, 0);
    bj_list_clear(&list);

    REQUIRE_EQ(list.p_allocator, 0);
    REQUIRE_EQ(list.bytes_payload, bytes_payload);
    REQUIRE(list.bytes_entry > bytes_payload);
    REQUIRE_EQ(list.weak_owning, false);
    REQUIRE_EQ(list.p_head, 0);
}

TEST_CASE(len_nil_returns_0) {
    bj_list_init(&list, 0, 0);
    REQUIRE_EQ(bj_list_len(&list), 0);
}

TEST_CASE(len_empty_returns_0) {
    bj_list_init(&list, &(BjListInfo){.bytes_payload = bytes_payload}, 0);
    REQUIRE_EQ(bj_list_len(&list), 0);
}

TEST_CASE(len_returns_number_of_elements) {
    payload p;
    bj_list_init(&list, &(BjListInfo){.bytes_payload = bytes_payload}, 0);

    for(usize i = 1 ; i < 10 ; ++i) {
        bj_list_prepend(&list, &p);
        REQUIRE_EQ(bj_list_len(&list), i);
    }
    bj_list_reset(&list);
}

TEST_CASE_ARGS(insert_to_n_makes_item_available_at_index_n, {usize n;}) {
    payload filler = {.elem0 = -1, .elem1 = -1};
    payload data = {.elem0 = 42, .elem1 = 513};

    bj_list_init(&list, &(BjListInfo){.bytes_payload = bytes_payload}, 0);

    usize initial_total = 10 + (test_data->n * 2);
    for(usize i = 0 ; i < initial_total ; ++i) {
        bj_list_prepend(&list, &filler);
    }
    CHECK_EQ(bj_list_len(&list), initial_total);

    bj_list_insert(&list, test_data->n, &data);
    REQUIRE_EQ(bj_list_len(&list), initial_total + 1);

    const payload* got = bj_list_at(&list, test_data->n);

    REQUIRE_EQ(got->elem0, data.elem0);
    REQUIRE_EQ(got->elem1, data.elem1);
    int diff = memcmp(got, &data, bytes_payload);
    REQUIRE_EQ(diff, 0);

    bj_list_reset(&list);
}

int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    RUN_TEST(initialize_with_payload_gives_empty_list);
    RUN_TEST(clear_nil_does_nothing);
    RUN_TEST(clear_empty_does_nothing);
    RUN_TEST(len_nil_returns_0);
    RUN_TEST(len_empty_returns_0);
    RUN_TEST(len_returns_number_of_elements);
    RUN_TEST_ARGS(insert_to_n_makes_item_available_at_index_n, .n=0);
    RUN_TEST_ARGS(insert_to_n_makes_item_available_at_index_n, .n=10);

    END_TESTS();
}

