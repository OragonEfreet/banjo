#include "test.h"

#include <banjo/graphics_pipeline.h>

BjGraphicsPipeline graphics_pipeline;

TEST_CASE(initialize_with_null_info_returns_nil_object) {
    bj_graphics_pipeline_init(&graphics_pipeline, 0);
    REQUIRE_EMPTY(BjGraphicsPipeline, &graphics_pipeline);
}

TEST_CASE(alloc_function_returns_valid_pointer) {
    void* block = bj_graphics_pipeline_alloc(0);
    REQUIRE_VALUE(block);
    bj_free(block, 0);
}

TEST_CASE(delete_null_pointer_is_valid_action) {
    bj_graphics_pipeline_del(0);
}


int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    RUN_TEST(initialize_with_null_info_returns_nil_object);
    RUN_TEST(alloc_function_returns_valid_pointer);
    RUN_TEST(delete_null_pointer_is_valid_action);

    END_TESTS();
}
