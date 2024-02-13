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

    int data = 42;
    for(usize i = 0 ; i < n_operations ; ++i) {
        bjForwardListPrepend(list, &data);
    }

    REQUIRE_EQ(bjForwardListCount(list), n_operations);

    bjDestroyForwardList(list);
}


TEST_CASE_ARGS(test_prepends, { element_type* value_type;  bool weak_owning;}) {
    BjForwardListCreateInfo create_info = {
        .elem_size = test_data->value_type->mem_size,
    };

    BjForwardList list;
    BjResult result = bjCreateForwardList(&create_info, &list);
    CHECK_EQ(result, BJ_SUCCESS);

    for(usize n = 0 ; n < test_data->value_type->n_values ; ++n) {
        void* data = test_data->value_type->values + test_data->value_type->mem_size * n;

        bjForwardListPrepend(list, data);
        REQUIRE_EQ(bjForwardListCount(list), n+1);

        // Test if the first entry is the newly assigned one
        void* res = bjForwardListHead(list);
        int cmp = memcmp(res, data, test_data->value_type->mem_size);
        REQUIRE_EQ(cmp, 0);
    }

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

    element_type element_types[] = {
        {
            .mem_size = sizeof(int),
            .n_values = 5,
            .values = (int[]){45, 104, 0, -30, 128},
        },{
            .mem_size = sizeof(big_struct),
            .n_values = 1,
            .values = (big_struct[]){{
                .value00 = 0.0, .value01 = 0.1,
                .value11 = 1.1, .value12 = 1.2,
                .value22 = 2.2, .value23 = 2.3,
                .value33 = 3.3, .value34 = 3.4,
                .value44 = 4.4, .value45 = 4.5,
            }},
        }
    };
    usize n_elements = sizeof(element_types) / sizeof(element_types[0]);

    for(usize e = 0 ; e < n_elements ; ++e) {
        RUN_TEST(default_initialization_is_full_empty,     .value_type = &element_types[e]);
        RUN_TEST(default_initialization_has_empty_count,   .value_type = &element_types[e]);
        RUN_TEST(a_first_prepend_initializes_first_entry,  .value_type = &element_types[e]);
        RUN_TEST(n_prepends_means_count_is_n,              .value_type = &element_types[e]);
        RUN_TEST(test_prepends,                            .value_type = &element_types[e]);
        RUN_TEST(test_prepends,                            .value_type = &element_types[e], .weak_owning = true);
    }

    END_TESTS();
}
