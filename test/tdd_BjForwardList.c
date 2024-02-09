#include "test.h"

#include <forward_list.h>

typedef float elem_type;

elem_type values[] = {
    42.5f,
};

static BjForwardListCreateInfo s_create_info = {
    .elem_size = sizeof(int)
};

TEST_CASE(default_initialization_is_full_empty,  {
    int value;
}) {
    BjForwardList list;

    BjResult result = bjCreateForwardList(&s_create_info, &list);
    REQUIRE_EQ(result, BJ_SUCCESS);

    REQUIRE_EQ(list->elem_size, sizeof(elem_type));
    REQUIRE_NULL(list->pAllocator);
    REQUIRE_NULL(list->pFirstEntry);

    bjDestroyForwardList(list);
}

TEST_CASE(default_initialization_has_empty_count, {}) {
    BjForwardList list;

    BjResult result = bjCreateForwardList(&s_create_info, &list);
    REQUIRE_EQ(result, BJ_SUCCESS);

    REQUIRE_EQ(bjForwardListCount(list), 0);

    bjDestroyForwardList(list);
}

TEST_CASE(a_first_append_initializes_first_entry, {}) {
    BjForwardList list;

    BjResult result = bjCreateForwardList(&s_create_info, &list);
    REQUIRE_EQ(result, BJ_SUCCESS);

    REQUIRE_NULL(list->pFirstEntry);
    bjForwardListAppend(list, &values[0]);
    REQUIRE_VALUE(list->pFirstEntry);

    bjDestroyForwardList(list);
}

int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    RUN_TEST(default_initialization_is_full_empty, {.value=32});
    RUN_TEST(default_initialization_has_empty_count, {});
    RUN_TEST(a_first_append_initializes_first_entry, {});

    END_TESTS();
}
