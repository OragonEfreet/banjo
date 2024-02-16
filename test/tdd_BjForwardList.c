#include "banjo/forward_list.h"
#include "test.h"

#include <forward_list.h>

typedef struct {
    usize mem_size;
    usize n_values;
    void* values;
} element_type;

TEST_CASE_ARGS(default_initialization_is_full_empty, {element_type* value_type;}) {

    BjForwardListInfo create_info = {
        .value_size = test_data->value_type->mem_size,
    };

    BjForwardList list = bj_forward_list_create(&create_info, 0);
    REQUIRE_VALUE(list);

    REQUIRE_EQ(list->value_size, test_data->value_type->mem_size);
    REQUIRE_NULL(list->p_allocator);
    REQUIRE_NULL(list->p_head);

    bj_forward_list_destroy(list);
}

TEST_CASE_ARGS(default_initialization_has_empty_count, {element_type* value_type;}) {

    BjForwardListInfo create_info = {
        .value_size = test_data->value_type->mem_size,
    };

    BjForwardList list = bj_forward_list_create(&create_info, 0);
    REQUIRE_VALUE(list);

    REQUIRE_EQ(bj_forward_list_count(list), 0);

    bj_forward_list_destroy(list);
}

TEST_CASE_ARGS(a_first_prepend_initializes_first_entry, {element_type* value_type;}) {

    BjForwardListInfo create_info = {
        .value_size = test_data->value_type->mem_size,
    };

    BjForwardList list = bj_forward_list_create(&create_info, 0);
    REQUIRE_VALUE(list);

    REQUIRE_NULL(list->p_head);
    bj_forward_list_prepend(list, test_data->value_type->values);
    REQUIRE_VALUE(list->p_head);

    bj_forward_list_destroy(list);
}

TEST_CASE_ARGS(n_prepends_means_count_is_n, { element_type* value_type; }) {
    usize n_operations = 3;

    BjForwardListInfo create_info = {
        .value_size = test_data->value_type->mem_size,
    };

    BjForwardList list = bj_forward_list_create(&create_info, 0);
    REQUIRE_VALUE(list);

    int data = 42;
    for(usize i = 0 ; i < n_operations ; ++i) {
        bj_forward_list_prepend(list, &data);
    }

    REQUIRE_EQ(bj_forward_list_count(list), n_operations);

    bj_forward_list_destroy(list);
}


TEST_CASE_ARGS(test_prepends, { element_type* value_type;  bool weak_owning;}) {
    BjForwardListInfo create_info = {
        .value_size = test_data->value_type->mem_size,
    };

    BjForwardList list = bj_forward_list_create(&create_info, 0);
    REQUIRE_VALUE(list);

    for(usize n = 0 ; n < test_data->value_type->n_values ; ++n) {
        void* data = test_data->value_type->values + test_data->value_type->mem_size * n;

        bj_forward_list_prepend(list, data);
        REQUIRE_EQ(bj_forward_list_count(list), n+1);

        // Test if the first entry is the newly assigned one
        void* res = bj_forward_list_head(list);
        int cmp = memcmp(res, data, test_data->value_type->mem_size);
        REQUIRE_EQ(cmp, 0);
    }

    bj_forward_list_destroy(list);
}

TEST_CASE_ARGS(find_in_empty_always_return_null, { element_type* value_type;  bool weak_owning;}) {

    BjForwardListInfo create_info = {
        .value_size = test_data->value_type->mem_size,
    };


    BjForwardList list = bj_forward_list_create(&create_info, 0);
    REQUIRE_VALUE(list);

    void* found = bj_forward_list_find(list, test_data->value_type->values, 0);
    REQUIRE_NULL(found);

    bj_forward_list_destroy(list);
}

TEST_CASE_ARGS(each_time_a_value_is_added_we_can_find_it, { element_type* value_type;  bool weak_owning;}) {
    BjForwardListInfo create_info = {
        .value_size = test_data->value_type->mem_size,
    };

    BjForwardList list = bj_forward_list_create(&create_info, 0);
    REQUIRE_VALUE(list);

    for(usize n = 0 ; n < test_data->value_type->n_values ; ++n) {
        void* data = test_data->value_type->values + test_data->value_type->mem_size * n;

        bj_forward_list_prepend(list, data);
        REQUIRE_EQ(bj_forward_list_count(list), n+1);

        void* found = bj_forward_list_find(list, data, 0);
        REQUIRE_VALUE(found);



    }

    bj_forward_list_destroy(list);
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
        RUN_TEST(find_in_empty_always_return_null,         .value_type = &element_types[e]);
        /* RUN_TEST(each_time_a_value_is_added_we_can_find_it,.value_type = &element_types[e]); */
        /* RUN_TEST(each_time_a_value_is_added_we_can_find_it,.value_type = &element_types[e], .weak_owning = true); */
    }
    END_TESTS();
}

