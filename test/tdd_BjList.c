#include "banjo/list.h"
#include "test.h"

#include <banjo/list.h>

#include <string.h>

typedef struct {
    usize mem_size;
    usize n_values;
    void* values;
} element_type;

TEST_CASE_ARGS(default_initialization_is_full_empty, {element_type* value_type;}) {

    BjListInfo create_info = {
        .value_size = test_data->value_type->mem_size,
    };

    BjList* list = bj_list_create(&create_info, 0);
    REQUIRE_VALUE(list);

    REQUIRE_EQ(list->value_size, test_data->value_type->mem_size);
    REQUIRE_NULL(list->p_allocator);
    REQUIRE_NULL(list->p_head);

    bj_list_destroy(list);
}

TEST_CASE_ARGS(default_initialization_has_empty_count, {element_type* value_type;}) {

    BjListInfo create_info = {
        .value_size = test_data->value_type->mem_size,
    };

    BjList* list = bj_list_create(&create_info, 0);
    REQUIRE_VALUE(list);

    REQUIRE_EQ(bj_list_count(list), 0);

    bj_list_destroy(list);
}

TEST_CASE_ARGS(a_first_prepend_initializes_first_entry, {element_type* value_type;}) {

    BjListInfo create_info = {
        .value_size = test_data->value_type->mem_size,
    };

    BjList* list = bj_list_create(&create_info, 0);
    REQUIRE_VALUE(list);

    REQUIRE_NULL(list->p_head);
    bj_list_prepend(list, test_data->value_type->values);
    REQUIRE_VALUE(list->p_head);

    bj_list_destroy(list);
}

TEST_CASE_ARGS(n_prepends_means_count_is_n, { element_type* value_type; }) {
    usize n_operations = 3;

    BjListInfo create_info = {
        .value_size = test_data->value_type->mem_size,
    };

    BjList* list = bj_list_create(&create_info, 0);
    REQUIRE_VALUE(list);

    int data = 42;
    for(usize i = 0 ; i < n_operations ; ++i) {
        bj_list_prepend(list, &data);
    }

    REQUIRE_EQ(bj_list_count(list), n_operations);

    bj_list_destroy(list);
}


TEST_CASE_ARGS(test_prepends, { element_type* value_type;  bool weak_owning;}) {
    BjListInfo create_info = {
        .value_size  = test_data->value_type->mem_size,
        .weak_owning = test_data->weak_owning,
    };

    BjList* list = bj_list_create(&create_info, 0);
    REQUIRE_VALUE(list);

    for(usize n = 0 ; n < test_data->value_type->n_values ; ++n) {
        void* data = (byte*)test_data->value_type->values + test_data->value_type->mem_size * n;

        bj_list_prepend(list, data);
        REQUIRE_EQ(bj_list_count(list), n+1);

        // Test if the first entry is the newly assigned one
        void* res = bj_list_head(list);
        int cmp = memcmp(res, data, test_data->value_type->mem_size);
        REQUIRE_EQ(cmp, 0);
    }

    bj_list_destroy(list);
}

TEST_CASE(iterator) {
    BjListInfo create_info = {
        .value_size = sizeof(short),
    };

    BjList* list = bj_list_create(&create_info, 0);
    REQUIRE_VALUE(list);

    short values[] = {4, -1, 102};
    usize n_elements = sizeof(values) / sizeof(short);

    for(usize n = 0 ; n < n_elements ; ++n) {
        bj_list_prepend(list, &values[n]);
    }

    BjListIterator* it = bj_list_iterator_create(list);

    usize i = n_elements - 1;
    while(bj_list_iterator_has_next(it)) {
        short expected = values[i--];
        short* got = bj_list_iterator_next(it);
        REQUIRE_EQ(*got, expected);
    }


    bj_list_iterator_destroy(it);
    bj_list_destroy(list);
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
        RUN_TEST_ARGS(default_initialization_is_full_empty,     .value_type = &element_types[e]);
        RUN_TEST_ARGS(default_initialization_has_empty_count,   .value_type = &element_types[e]);
        RUN_TEST_ARGS(a_first_prepend_initializes_first_entry,  .value_type = &element_types[e]);
        RUN_TEST_ARGS(n_prepends_means_count_is_n,              .value_type = &element_types[e]);
        RUN_TEST_ARGS(test_prepends,                            .value_type = &element_types[e]);
        RUN_TEST_ARGS(test_prepends,                            .value_type = &element_types[e], .weak_owning = true);
    }
    RUN_TEST(iterator);
    END_TESTS();
}

