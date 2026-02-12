#include "test.h"
#include <banjo/string.h>

TEST_CASE(strlen_empty_string) {
  const char *s = "";
  REQUIRE_EQ(bj_strlen(s), 0);
}

TEST_CASE(strlen_normal_string) {
  const char *s = "hello";
  REQUIRE_EQ(bj_strlen(s), 5);
}

TEST_CASE(strlen_long_string) {
  const char *s = "this is a longer string with spaces";
  REQUIRE_EQ(bj_strlen(s), 35);
}

TEST_CASE(strcmp_equal_strings) {
  const char *s1 = "hello";
  const char *s2 = "hello";
  REQUIRE_EQ(bj_strcmp(s1, s2), 0);
}

TEST_CASE(strcmp_different_strings) {
  const char *s1 = "hello";
  const char *s2 = "world";
  REQUIRE_NEQ(bj_strcmp(s1, s2), 0);
}

TEST_CASE(strcmp_less_than) {
  const char *s1 = "apple";
  const char *s2 = "banana";
  REQUIRE(bj_strcmp(s1, s2) < 0);
}

TEST_CASE(strcmp_greater_than) {
  const char *s1 = "zebra";
  const char *s2 = "apple";
  REQUIRE(bj_strcmp(s1, s2) > 0);
}

TEST_CASE(strcmp_empty_strings) {
  const char *s1 = "";
  const char *s2 = "";
  REQUIRE_EQ(bj_strcmp(s1, s2), 0);
}

TEST_CASE(strcmp_empty_vs_nonempty) {
  const char *s1 = "";
  const char *s2 = "a";
  REQUIRE(bj_strcmp(s1, s2) < 0);
}

TEST_CASE(strcmp_case_sensitive) {
  const char *s1 = "Hello";
  const char *s2 = "hello";
  REQUIRE_NEQ(bj_strcmp(s1, s2), 0);
}

TEST_CASE(strcmp_prefix) {
  const char *s1 = "test";
  const char *s2 = "testing";
  REQUIRE(bj_strcmp(s1, s2) < 0);
}

TEST_CASE(memset_fills_buffer) {
  char buffer[10];
  bj_memset(buffer, 'A', 10);

  for (int i = 0; i < 10; ++i) {
    REQUIRE_EQ(buffer[i], 'A');
  }
}

TEST_CASE(memset_zero_fills) {
  char buffer[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  bj_memset(buffer, 0, 10);

  for (int i = 0; i < 10; ++i) {
    REQUIRE_EQ(buffer[i], 0);
  }
}

TEST_CASE(memcpy_copies_data) {
  char src[5] = {1, 2, 3, 4, 5};
  char dst[5] = {0};

  bj_memcpy(dst, src, 5);

  for (int i = 0; i < 5; ++i) {
    REQUIRE_EQ(dst[i], src[i]);
  }
}

TEST_CASE(memmove_handles_overlap) {
  char buffer[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  bj_memmove(buffer + 2, buffer, 5);

  REQUIRE_EQ(buffer[2], 1);
  REQUIRE_EQ(buffer[3], 2);
  REQUIRE_EQ(buffer[4], 3);
}

int main(int argc, char *argv[]) {
  BEGIN_TESTS(argc, argv);

  RUN_TEST(strlen_empty_string);
  RUN_TEST(strlen_normal_string);
  RUN_TEST(strlen_long_string);
  RUN_TEST(strcmp_equal_strings);
  RUN_TEST(strcmp_different_strings);
  RUN_TEST(strcmp_less_than);
  RUN_TEST(strcmp_greater_than);
  RUN_TEST(strcmp_empty_strings);
  RUN_TEST(strcmp_empty_vs_nonempty);
  RUN_TEST(strcmp_case_sensitive);
  RUN_TEST(strcmp_prefix);
  RUN_TEST(memset_fills_buffer);
  RUN_TEST(memset_zero_fills);
  RUN_TEST(memcpy_copies_data);
  RUN_TEST(memmove_handles_overlap);

  END_TESTS();
}
