#include "banjo/list.h"
#include "test.h"

#include <banjo/list.h>

#include <string.h>

bj_list list;

typedef struct {
    short elem0;
    long  elem1;
} payload;
static const usize bytes_payload = sizeof(payload);

TEST_CASE(initialize_with_no_payload_gives_nil) {
    bj_list_init_default(&list, 0);
    REQUIRE_EMPTY(bj_list, &list);
}

TEST_CASE(initialize_with_payload_gives_empty_list) {
    bj_list_init_default(&list, bytes_payload);

    REQUIRE_EQ(list.bytes_payload, bytes_payload);
    REQUIRE(list.bytes_entry > bytes_payload);
    REQUIRE_EQ(list.weak_owning, false);
    REQUIRE_EQ(list.p_head, 0);
}

TEST_CASE(clear_nil_does_nothing) {
    bj_list_init_default(&list, 0);

    bj_list_clear(&list);
    REQUIRE_EMPTY(bj_list, &list);
}

TEST_CASE(clear_empty_does_nothing) {
    bj_list_init_default(&list, bytes_payload);
    bj_list_clear(&list);

    REQUIRE_EQ(list.bytes_payload, bytes_payload);
    REQUIRE(list.bytes_entry > bytes_payload);
    REQUIRE_EQ(list.weak_owning, false);
    REQUIRE_EQ(list.p_head, 0);
}

TEST_CASE(len_nil_returns_0) {
    bj_list_init_default(&list, 0);
    REQUIRE_EQ(bj_list_len(&list), 0);
}

TEST_CASE(len_empty_returns_0) {
    bj_list_init_default(&list, bytes_payload);
    REQUIRE_EQ(bj_list_len(&list), 0);
}

TEST_CASE(len_returns_number_of_elements) {
    payload p;
    bj_list_init_default(&list, bytes_payload);

    for(usize i = 1 ; i < 10 ; ++i) {
        bj_list_prepend(&list, &p);
        REQUIRE_EQ(bj_list_len(&list), i);
    }
    bj_list_reset(&list);
}

TEST_CASE_ARGS(insert_to_n_makes_item_available_at_index_n, {usize n;}) {
    payload filler = {.elem0 = -1, .elem1 = -1};
    payload data = {.elem0 = 42, .elem1 = 513};

    bj_list_init_default(&list, bytes_payload);

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

TEST_CASE(prepend_to_makes_item_available_at_index_0) {
    bj_list_init_default(&list, bytes_payload);

    for(usize i = 1 ; i < 10 ; ++i) {
        payload p = {.elem0 = 42, .elem1 = -(i*3)};
        bj_list_prepend(&list, &p);
        REQUIRE_EQ(bj_list_len(&list), i);

        const payload* got = bj_list_at(&list, 0);

        REQUIRE_EQ(got->elem0, p.elem0);
        REQUIRE_EQ(got->elem1, p.elem1);
        int diff = memcmp(got, &p, bytes_payload);
        REQUIRE_EQ(diff, 0);
    }
    bj_list_reset(&list);
}

TEST_CASE(at_nil_returns_0) {
    bj_list_init_default(&list, 0);
    REQUIRE_EQ(bj_list_at(&list, 0), 0);
    REQUIRE_EQ(bj_list_at(&list, 1), 0);
    REQUIRE_EQ(bj_list_at(&list, 2), 0);
    REQUIRE_EQ(bj_list_at(&list, 3), 0);
    REQUIRE_EQ(bj_list_at(&list, 4), 0);
}

TEST_CASE(at_empty_returns_0) {
    bj_list_init_default(&list, bytes_payload);
    REQUIRE_EQ(bj_list_at(&list, 0), 0);
    REQUIRE_EQ(bj_list_at(&list, 1), 0);
    REQUIRE_EQ(bj_list_at(&list, 2), 0);
    REQUIRE_EQ(bj_list_at(&list, 3), 0);
    REQUIRE_EQ(bj_list_at(&list, 4), 0);
}

TEST_CASE(head_is_at_0) {
    bj_list_init_default(&list, bytes_payload);

    for(usize i = 1 ; i < 10 ; ++i) {
        payload p = {.elem0 = 42, .elem1 = -(i*3)};
        bj_list_prepend(&list, &p);
        REQUIRE_EQ(bj_list_len(&list), i);

        const payload* got = bj_list_head(&list);

        REQUIRE_EQ(got->elem0, p.elem0);
        REQUIRE_EQ(got->elem1, p.elem1);
        int diff = memcmp(got, &p, bytes_payload);
        REQUIRE_EQ(diff, 0);
    }
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
    RUN_TEST_ARGS(insert_to_n_makes_item_available_at_index_n, .n=5);
    RUN_TEST(prepend_to_makes_item_available_at_index_0);
    RUN_TEST(at_nil_returns_0);
    RUN_TEST(at_empty_returns_0);
    RUN_TEST(head_is_at_0);
    RUN_TEST(initialize_with_no_payload_gives_nil);

    END_TESTS();
}

