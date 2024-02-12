#include "test.h"

#include <forward_list.h>

typedef struct {
    usize mem_size;
    usize n_values;
    void* values;
} element_type;

/* typedef float elem_type; */
/* elem_type values[] = { */
/*     42.5f, */
/* }; */

/* static BjForwardListCreateInfo s_create_info = { */
/*     .elem_size = sizeof(elem_type) */
/* }; */

TEST_CASE_ARGS(default_initialization_is_full_empty, {element_type* value_type;}) {
    BjForwardList list;

    BjForwardListCreateInfo create_info = {
        .elem_size = test_data->value_type->mem_size,
    };

    BjResult result = bjCreateForwardList(&create_info, &list);
    CHECK_EQ(result, BJ_SUCCESS);

    REQUIRE_EQ(list->elem_size, test_data->value_type->mem_size);
    REQUIRE_NULL(list->pAllocator);
    REQUIRE_NULL(list->pHead);

    bjDestroyForwardList(list);
}

TEST_CASE_ARGS(default_initialization_has_empty_count, {element_type* value_type;}) {
    BjForwardList list;

    BjForwardListCreateInfo create_info = {
        .elem_size = test_data->value_type->mem_size,
    };

    BjResult result = bjCreateForwardList(&create_info, &list);
    CHECK_EQ(result, BJ_SUCCESS);

    REQUIRE_EQ(bjForwardListCount(list), 0);

    bjDestroyForwardList(list);
}

TEST_CASE_ARGS(a_first_prepend_initializes_first_entry, {element_type* value_type;}) {
    BjForwardList list;

    BjForwardListCreateInfo create_info = {
        .elem_size = test_data->value_type->mem_size,
    };

    BjResult result = bjCreateForwardList(&create_info, &list);
    CHECK_EQ(result, BJ_SUCCESS);

    REQUIRE_NULL(list->pHead);
    bjForwardListPrepend(list, test_data->value_type->values);
    REQUIRE_VALUE(list->pHead);

    bjDestroyForwardList(list);
}

TEST_CASE_ARGS(n_prepends_means_count_is_n, { element_type* value_type; }) {
    usize n_operations = 3;
    BjForwardList list;

    BjForwardListCreateInfo create_info = {
        .elem_size = test_data->value_type->mem_size,
    };

    BjResult result = bjCreateForwardList(&create_info, &list);
    CHECK_EQ(result, BJ_SUCCESS);

    int data = {};
    for(usize i = 0 ; i < n_operations ; ++i) {
        bjForwardListPrepend(list, &data);
    }

    REQUIRE_EQ(bjForwardListCount(list), n_operations);

    bjDestroyForwardList(list);
}

int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    element_type float_type = {
        .mem_size = sizeof(float),
        .n_values = 1,
        .values = (float[]){4.5f},
    };

    RUN_TEST_ARGS(default_initialization_is_full_empty,    { .value_type = &float_type});
    RUN_TEST_ARGS(default_initialization_has_empty_count,  { .value_type = &float_type});
    RUN_TEST_ARGS(a_first_prepend_initializes_first_entry, { .value_type = &float_type});
    RUN_TEST_ARGS(n_prepends_means_count_is_n,             { .value_type = &float_type});

    END_TESTS();
}
