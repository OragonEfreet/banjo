#include "test.h"
#include "mock_memory.h"

static usize s_mem_size = sizeof(int);

TEST_CASE(fallback_allocator_works) {
    void* blocks = bjAllocate(s_mem_size, 0);
    REQUIRE_VALUE(blocks);
    blocks = bjReallocate(blocks, s_mem_size * 2, 0);
    REQUIRE_VALUE(blocks);
    bjFree(blocks, 0);
}

TEST_CASE(forcing_default_allocators_is_possible) {
    BjAllocationCallbacks allocators = mock_allocators(0);

    bjSetDefaultAllocator(&allocators);
    bjSetDefaultAllocator(0);

    void* blocks = bjAllocate(s_mem_size, 0);
    REQUIRE_VALUE(blocks);
    blocks = bjReallocate(blocks, s_mem_size * 2, 0);
    REQUIRE_VALUE(blocks);
    bjFree(blocks, 0);
}

TEST_CASE(default_allocator_cannot_have_no_malloc) {
    BjAllocationCallbacks allocators = mock_allocators(0);
    allocators.pfnAllocation = 0;
    BjResult res = bjSetDefaultAllocator(&allocators);
    REQUIRE_EQ(res, BJ_INVALID_PARAMETER);
}

TEST_CASE(default_allocator_cannot_have_no_realloc) {
    BjAllocationCallbacks allocators = mock_allocators(0);
    allocators.pfnReallocation = 0;
    BjResult res = bjSetDefaultAllocator(&allocators);
    REQUIRE_EQ(res, BJ_INVALID_PARAMETER);
}

TEST_CASE(default_allocator_cannot_have_no_free) {
    BjAllocationCallbacks allocators = mock_allocators(0);
    allocators.pfnReallocation = 0;
    BjResult res = bjSetDefaultAllocator(&allocators);
    REQUIRE_EQ(res, BJ_INVALID_PARAMETER);
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
    usize n_ops = sizeof(allocations) / sizeof(allocations[0]);

    // At each iteration, the underlying instance of allocation_data is modified
    // and a test instance of allocation_data is modified here to compare the results.
    allocation_data result = {};
    allocation_data expected = {};
    CHECK_CLEAN_ALLOC(result);
    BjAllocationCallbacks allocators = mock_allocators(&result);

    // Stack the allocated ptrs
    void* ptrs_fifo[n_ops] = {};
    usize ptrs_fifo_len = 0;

    // Stack the allocated sizes
    usize size_fifo[n_ops] = {};
    usize size_fifo_len = 0;

    for(usize i = 0 ; i < n_ops ; ++i) {
        if(allocations[i] == 0) {
            bjFree(ptrs_fifo[--ptrs_fifo_len], &allocators);
            expected.n_free += 1;
            expected.application_current_allocated -= size_fifo[(--size_fifo_len)];
        } else if(allocations[i] < 0) {
            usize size = -allocations[i];
            ptrs_fifo[ptrs_fifo_len-1] = bjReallocate(ptrs_fifo[ptrs_fifo_len-1], size, &allocators);
            expected.n_reallocations += 1;
            expected.application_current_allocated += size;
            if(expected.application_current_allocated > expected.application_max_allocated) {
                expected.application_max_allocated = expected.application_current_allocated;
            }
            expected.application_current_allocated -= size_fifo[size_fifo_len-1];
            size_fifo[size_fifo_len-1] = size;
        } else {
            usize size = allocations[i];
            ptrs_fifo[ptrs_fifo_len++] = bjAllocate(size, &allocators);
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
}

int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    // Special case of the unit tests where the mock_allocator are not used
    bjUnsetDefaultAllocator();


    RUN_TEST(fallback_allocator_works);
    RUN_TEST(forcing_default_allocators_is_possible);
    RUN_TEST(default_allocator_cannot_have_no_malloc);
    RUN_TEST(default_allocator_cannot_have_no_realloc);
    RUN_TEST(default_allocator_cannot_have_no_free);
    RUN_TEST(test_custom_default_allocators);

    END_TESTS();
}
