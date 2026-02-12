#include "test.h"
#include <banjo/memory.h>
#include <stdlib.h>
#include <time.h>

#define COUNT (2000)

TEST_CASE(memory_massive_fragmented_allocation) {
  void *pointers[COUNT];
  size_t sizes[COUNT];

  // Seed random for reproducible tests if needed, but here we just want lots of
  // allocs
  srand(42);

  // Allocate lots of small objects
  for (int i = 0; i < COUNT; ++i) {
    sizes[i] = (rand() % 128) + 1;
    pointers[i] = bj_malloc(sizes[i]);
    REQUIRE(pointers[i] != NULL);
  }

  // Shuffle deallocation order
  for (int i = 0; i < COUNT; ++i) {
    int target = rand() % COUNT;
    void *tmp = pointers[i];
    pointers[i] = pointers[target];
    pointers[target] = tmp;
  }

  // Deallocate
  for (int i = 0; i < COUNT; ++i) {
    bj_free(pointers[i]);
  }

  // mock_memory will check for leaks at exit
}

TEST_CASE(memory_realloc_churn) {
  void *ptr = bj_malloc(10);
  REQUIRE(ptr != NULL);

  for (int i = 0; i < 100; ++i) {
    size_t new_size = (i % 2 == 0) ? 1000 : 10;
    ptr = bj_realloc(ptr, new_size);
    REQUIRE(ptr != NULL);
  }

  bj_free(ptr);
}

TEST_CASE(memory_large_allocation) {
  // Attempt 10MB allocation
  size_t size = 10 * 1024 * 1024;
  void *ptr = bj_malloc(size);

  // Might fail depending on environment, but we test the handling
  if (ptr) {
    bj_memset(ptr, 0x55, size);
    REQUIRE_EQ(((uint8_t *)ptr)[size - 1], 0x55);
    bj_free(ptr);
  }
}

int main(int argc, char *argv[]) {
  // Note: We don't use BEGIN_TESTS/END_TESTS here if we want to bypass
  // the mock memory leak check for the ENTIRE program, OR we use them
  // to confirm our bj_malloc calls are tracked.
  // We want them tracked.

  BEGIN_TESTS(argc, argv);

  RUN_TEST(memory_massive_fragmented_allocation);
  RUN_TEST(memory_realloc_churn);
  RUN_TEST(memory_large_allocation);

  END_TESTS();
}
