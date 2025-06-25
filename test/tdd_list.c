#include "test.h"

#include "list_t.h"
#include <string.h>

typedef struct {
    short elem0;
    long  elem1;
} payload;

TEST_CASE(initialize_with_no_payload_gives_0) {
    REQUIRE_NULL(bj_list_new(0));
}

TEST_CASE(initialize_with_payload_gives_empty_list) {
    bj_list* p_list = bj_list_new_t(payload);
    REQUIRE_EQ(p_list->bytes_payload, sizeof(payload));
    REQUIRE(p_list->bytes_entry > sizeof(payload));
    REQUIRE_EQ(p_list->weak_owning, BJ_FALSE);
    REQUIRE_EQ(p_list->p_head, 0);
    bj_list_del(p_list);
}

TEST_CASE(clear_empty_does_nothing) {
    bj_list* p_list = bj_list_new_t(payload);
    bj_list_clear(p_list); // Ensure cleared
    REQUIRE_EQ(p_list->bytes_payload, sizeof(payload));
    REQUIRE(p_list->bytes_entry > sizeof(payload));
    REQUIRE_EQ(p_list->weak_owning, BJ_FALSE);
    REQUIRE_EQ(p_list->p_head, 0);
    bj_list_del(p_list);
}

TEST_CASE(len_0_returns_0) {
    REQUIRE_EQ(bj_list_len(0), 0);
}

TEST_CASE(len_empty_returns_0) {
    bj_list* p_list = bj_list_new_t(payload);
    REQUIRE_EQ(bj_list_len(p_list), 0);
    bj_list_del(p_list);
}

TEST_CASE(len_returns_number_of_elements) {
    payload p;
    bj_list* p_list = bj_list_new_t(payload);

    for(size_t i = 1 ; i < 10 ; ++i) {
        bj_list_prepend(p_list, &p);
        REQUIRE_EQ(bj_list_len(p_list), i);
    }
    bj_list_del(p_list);
}

TEST_CASE_ARGS(insert_to_n_makes_item_available_at_index_n, {size_t n;}) {
    payload filler = {.elem0 = -1, .elem1 = -1};
    payload data = {.elem0 = 42, .elem1 = 513};

    bj_list* p_list = bj_list_new_t(payload);

    size_t initial_total = 10 + (test_data->n * 2);
    for(size_t i = 0 ; i < initial_total ; ++i) {
        bj_list_prepend(p_list, &filler);
    }
    CHECK_EQ(bj_list_len(p_list), initial_total);

    bj_list_insert(p_list, test_data->n, &data);
    REQUIRE_EQ(bj_list_len(p_list), initial_total + 1);

    const payload* got = bj_list_at(p_list, test_data->n);

    REQUIRE_EQ(got->elem0, data.elem0);
    REQUIRE_EQ(got->elem1, data.elem1);
    int diff = memcmp(got, &data, sizeof(payload));
    REQUIRE_EQ(diff, 0);

    bj_list_del(p_list);
}

TEST_CASE(prepend_to_makes_item_available_at_index_0) {
    bj_list* p_list = bj_list_new_t(payload);

    for(size_t i = 1 ; i < 10 ; ++i) {
        payload p = {.elem0 = 42, .elem1 = (i*3)};
        bj_list_prepend(p_list, &p);
        REQUIRE_EQ(bj_list_len(p_list), i);

        const payload* got = bj_list_at(p_list, 0);

        REQUIRE_EQ(got->elem0, p.elem0);
        REQUIRE_EQ(got->elem1, p.elem1);
        int diff = memcmp(got, &p, sizeof(payload));
        REQUIRE_EQ(diff, 0);
    }
    bj_list_del(p_list);
}

TEST_CASE(at_from_0_returns_0) {
    REQUIRE_EQ(bj_list_at(0, 0), 0);
    REQUIRE_EQ(bj_list_at(0, 1), 0);
    REQUIRE_EQ(bj_list_at(0, 2), 0);
    REQUIRE_EQ(bj_list_at(0, 3), 0);
    REQUIRE_EQ(bj_list_at(0, 4), 0);
}

TEST_CASE(at_empty_returns_0) {
    bj_list* p_list = bj_list_new_t(payload);
    REQUIRE_EQ(bj_list_at(p_list, 0), 0);
    REQUIRE_EQ(bj_list_at(p_list, 1), 0);
    REQUIRE_EQ(bj_list_at(p_list, 2), 0);
    REQUIRE_EQ(bj_list_at(p_list, 3), 0);
    REQUIRE_EQ(bj_list_at(p_list, 4), 0);
    bj_list_del(p_list);
}

TEST_CASE(head_is_at_0) {
    bj_list* p_list = bj_list_new_t(payload);

    for(size_t i = 1 ; i < 10 ; ++i) {
        payload p = {.elem0 = 42, .elem1 = (i*3)};
        bj_list_prepend(p_list, &p);
        REQUIRE_EQ(bj_list_len(p_list), i);

        const payload* got = bj_list_head(p_list);

        REQUIRE_EQ(got->elem0, p.elem0);
        REQUIRE_EQ(got->elem1, p.elem1);
        int diff = memcmp(got, &p, sizeof(payload));
        REQUIRE_EQ(diff, 0);
    }
    bj_list_del(p_list);
}

int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    RUN_TEST(initialize_with_payload_gives_empty_list);
    RUN_TEST(clear_empty_does_nothing);
    RUN_TEST(len_0_returns_0);
    RUN_TEST(len_empty_returns_0);
    RUN_TEST(len_returns_number_of_elements);
    RUN_TEST_ARGS(insert_to_n_makes_item_available_at_index_n, .n=0);
    RUN_TEST_ARGS(insert_to_n_makes_item_available_at_index_n, .n=10);
    RUN_TEST_ARGS(insert_to_n_makes_item_available_at_index_n, .n=5);
    RUN_TEST(prepend_to_makes_item_available_at_index_0);
    RUN_TEST(at_from_0_returns_0);
    RUN_TEST(at_empty_returns_0);
    RUN_TEST(head_is_at_0);
    RUN_TEST(initialize_with_no_payload_gives_0);

    END_TESTS();
}

