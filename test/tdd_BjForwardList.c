#include "test.h"

#include <forward_list.h>

typedef struct {
    usize mem_size;
    usize n_values;
    void* values;
} element_type;

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

typedef struct {
    double value00;    double value01;
    double value11;    double value12;
    double value22;    double value23;
    double value33;    double value34;
    double value44;    double value45;
} big_struct;

int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    element_type float_type = {
        .mem_size = sizeof(float),
        .n_values = 5,
        .values = (float[]){4.5f, 10.4f, 0.0f, -3.0f, 128.0f},
    };

    RUN_TEST(default_initialization_is_full_empty,     .value_type = &float_type);
    RUN_TEST(default_initialization_has_empty_count,   .value_type = &float_type);
    RUN_TEST(a_first_prepend_initializes_first_entry,  .value_type = &float_type);
    RUN_TEST(n_prepends_means_count_is_n,              .value_type = &float_type);

    /* element_type big_type = { */
    /*     .mem_size = sizeof(big_struct), */
    /*     .n_values = 1, */
    /*     .values = (big_struct[]){{ */
    /*         .value00 = 0.0, .value01 = 0.1, */
    /*         .value11 = 1.1, .value12 = 1.2, */
    /*         .value22 = 2.2, .value23 = 2.3, */
    /*         .value33 = 3.3, .value34 = 3.4, */
    /*         .value44 = 4.4, .value45 = 4.5, */
    /*     }}, */
    /* }; */

    /* RUN_TEST(default_initialization_is_full_empty,     .value_type = &big_type); */
    /* RUN_TEST(default_initialization_has_empty_count,   .value_type = &big_type); */
    /* RUN_TEST(a_first_prepend_initializes_first_entry,  .value_type = &big_type); */
    /* RUN_TEST(n_prepends_means_count_is_n,              .value_type = &big_type); */

    END_TESTS();
}
