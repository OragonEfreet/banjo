#include "banjo/api.h"
#include "test.h"
#include "mock_memory.h"

static size_t s_mem_size = sizeof(int);

TEST_CASE(fallback_allocator_works) {
    void* blocks = bj_malloc(s_mem_size);
    REQUIRE_VALUE(blocks);
    blocks = bj_realloc(blocks, s_mem_size * 2);
    REQUIRE_VALUE(blocks);
    bj_free(blocks);
}

TEST_CASE(forcing_default_allocators_is_possible) {
    bj_memory_callbacks allocators = mock_allocators(0);

    bj_set_memory_defaults(&allocators);
    bj_set_memory_defaults(0);

    void* blocks = bj_malloc(s_mem_size);
    REQUIRE_VALUE(blocks);
    blocks = bj_realloc(blocks, s_mem_size * 2);
    REQUIRE_VALUE(blocks);
    bj_free(blocks);
}

TEST_CASE(test_custom_default_allocators) {
    // An intentionally complex test that plays with allocations to check
    // for the status each time a memory operation is done.

    // Test data: each value represents an operation to to in order:
    int allocations[] = {
        // VALUE > 0: Allocate a new block with size = VALUE
        //         0: deallocate the previously allocated/reallocated blocks
        // VALUE < 0: reallocate the previously allocated/reallocated block with new size = -VALUE
        40, 60, 0, 10, -128, 0, 0,
        40, 60, 0, 10, -128, 0, 0,
    };
    #define n_ops 14

    // At each iteration, the underlying instance of sAllocationData is modified
    // and a test instance of sAllocationData is modified here to compare the results.
    sAllocationData result = {.actual_current_allocated=0};
    sAllocationData expected = {.actual_current_allocated=0};
    CHECK_CLEAN_ALLOC(result);
    bj_memory_callbacks allocators = mock_allocators(&result);

    bj_set_memory_defaults(&allocators);

    // Stack the allocated ptrs
    void* ptrs_fifo[n_ops];
    size_t ptrs_fifo_len = 0;

    // Stack the allocated sizes
    size_t size_fifo[n_ops];
    size_t size_fifo_len = 0;

    for(size_t i = 0 ; i < n_ops ; ++i) {
        if(allocations[i] == 0) {
            bj_free(ptrs_fifo[--ptrs_fifo_len]);
            expected.n_free += 1;
            expected.application_current_allocated -= size_fifo[(--size_fifo_len)];
        } else if(allocations[i] < 0) {
            size_t size = -allocations[i];
            ptrs_fifo[ptrs_fifo_len-1] = bj_realloc(ptrs_fifo[ptrs_fifo_len-1], size);
            expected.n_reallocations += 1;
            expected.application_current_allocated += size;
            if(expected.application_current_allocated > expected.application_max_allocated) {
                expected.application_max_allocated = expected.application_current_allocated;
            }
            expected.application_current_allocated -= size_fifo[size_fifo_len-1];
            size_fifo[size_fifo_len-1] = size;
        } else {
            size_t size = allocations[i];
            ptrs_fifo[ptrs_fifo_len++] = bj_malloc(size);
            expected.n_allocations += 1;
            expected.application_current_allocated += size;
            if(expected.application_current_allocated > expected.application_max_allocated) {
                expected.application_max_allocated = expected.application_current_allocated;
            }
            size_fifo[size_fifo_len++] = size;
        }

        REQUIRE_EQ(result.application_max_allocated, expected.application_max_allocated);
        REQUIRE_EQ(result.application_current_allocated, expected.application_current_allocated);
        REQUIRE_EQ(result.n_allocations, expected.n_allocations);
        REQUIRE_EQ(result.n_reallocations, expected.n_reallocations);
        REQUIRE_EQ(result.n_free, expected.n_free);

        /* logAllocations(&result); */
    }

    CHECK_EQ(ptrs_fifo_len, 0);
    CHECK_EQ(size_fifo_len, 0);

    REQUIRE_CLEAN_ALLOC(result);
    bj_unset_memory_defaults();
}

int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    // Special case of the unit tests where the mock_allocator are not used
    bj_unset_memory_defaults();


    RUN_TEST(fallback_allocator_works);
    RUN_TEST(forcing_default_allocators_is_possible);
    RUN_TEST(test_custom_default_allocators);

    END_TESTS();
}
