#include "test.h"

#include <banjo/list.h>

BjList list;

TEST_CASE(initialize_with_null_info_returns_nil_object) {
    bj_list_init(&list, 0);
    REQUIRE_EMPTY(BjList, &list);
}

TEST_CASE(alloc_function_returns_valid_pointer) {
    void* block = bj_list_alloc(0);
    REQUIRE_VALUE(block);
    bj_free(block, 0);
}

TEST_CASE(delete_null_pointer_is_valid_action) {
    bj_list_del(0);
}

TEST_CASE_ARGS(initializes_to_nil, {BjListInfo info;} ) {
    bj_list_init(&list, &test_data->info);
    REQUIRE_EMPTY(BjList, &list);
}

int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    RUN_TEST(initialize_with_null_info_returns_nil_object);
    RUN_TEST(alloc_function_returns_valid_pointer);
    RUN_TEST(delete_null_pointer_is_valid_action);
    RUN_TEST_ARGS(initializes_to_nil, {.bytes_payload = 0});

    END_TESTS();
}
