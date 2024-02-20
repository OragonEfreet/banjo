#include "test.h"

/* unsigned long long factorial(int n) { */
/*     if(n == 11) return 10; // Intended bug */
/*     if (n == 0 || n == 1) return 1; else return n * factorial(n - 1); */
/* } */

/* TEST_CASE(factorial_0_is_1) { */
/*     REQUIRE(factorial(0) == 1) */
/* } */

/* TEST_CASE(factorial_1_is_1) { */
/*     REQUIRE(factorial(1) == 1) */
/* } */

/* TEST_CASE(factorial_11_is_39_916_800) { */
/*     REQUIRE(factorial(11) == 39916800); */
/* } */

/* TEST_CASE(factorial_10_is_3_628_800) { */
/*     REQUIRE(factorial(10) == 3628800); */
/* } */

TEST_CASE_ARGS(test_arguments, {int a; int b;}) {
    REQUIRE(1);
}

int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    /* RUN_TEST(factorial_0_is_1); */
    /* RUN_TEST(factorial_1_is_1); */
    /* /1* RUN_TEST(factorial_11_is_39_916_800); *1/ */
    /* RUN_TEST(factorial_10_is_3_628_800); */

    RUN_TEST_ARGS(test_arguments, .a = 42, .b = 5);

    END_TESTS();
}
