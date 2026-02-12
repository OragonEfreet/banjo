// This file demonstrates the new parametric test macros
// and serves as a verification that they work correctly

#include "test.h"
#include <banjo/math.h>

////////////////////////////////////////////////////////////////////////////////
// Example 1: Simple arithmetic parametric tests
////////////////////////////////////////////////////////////////////////////////

typedef struct {
    int a;
    int b;
    int expected_sum;
} AddTestData;

TEST_PARAMS(addition, AddTestData,
    {1, 2, 3},
    {0, 0, 0},
    {-1, 1, 0},
    {100, -50, 50}
);

TEST_CASE_PARAM(addition, AddTestData) {
    int result = param->a + param->b;
    REQUIRE_EQ(result, param->expected_sum);
}

////////////////////////////////////////////////////////////////////////////////
// Example 2: Floating point math tests
////////////////////////////////////////////////////////////////////////////////

typedef struct {
    bj_real input;
    bj_real expected;
} SquareRootData;

TEST_PARAMS(sqrt_test, SquareRootData,
    {BJ_F(4.0), BJ_F(2.0)},
    {BJ_F(9.0), BJ_F(3.0)},
    {BJ_F(16.0), BJ_F(4.0)},
    {BJ_F(1.0), BJ_F(1.0)},
    {BJ_F(0.0), BJ_F(0.0)}
);

TEST_CASE_PARAM(sqrt_test, SquareRootData) {
    bj_real result = bj_sqrt(param->input);
    bj_real diff = bj_abs(result - param->expected);
    REQUIRE(diff < BJ_F(0.0001));
}

////////////////////////////////////////////////////////////////////////////////
// Example 3: Clamp boundary tests
////////////////////////////////////////////////////////////////////////////////

typedef struct {
    bj_real value;
    bj_real lo;
    bj_real hi;
    bj_real expected;
} ClampTestData;

TEST_PARAMS(clamp_test, ClampTestData,
    // Value within range
    {BJ_F(5.0), BJ_F(0.0), BJ_F(10.0), BJ_F(5.0)},
    // Value below range
    {BJ_F(-5.0), BJ_F(0.0), BJ_F(10.0), BJ_F(0.0)},
    // Value above range
    {BJ_F(15.0), BJ_F(0.0), BJ_F(10.0), BJ_F(10.0)},
    // Value at low boundary
    {BJ_F(0.0), BJ_F(0.0), BJ_F(10.0), BJ_F(0.0)},
    // Value at high boundary
    {BJ_F(10.0), BJ_F(0.0), BJ_F(10.0), BJ_F(10.0)}
);

TEST_CASE_PARAM(clamp_test, ClampTestData) {
    bj_real result = bj_clamp(param->value, param->lo, param->hi);
    REQUIRE(bj_real_eq(result, param->expected));
}

////////////////////////////////////////////////////////////////////////////////
// Example 4: String/character tests (demonstrating non-numeric params)
////////////////////////////////////////////////////////////////////////////////

typedef struct {
    char input;
    int is_digit;
} CharTestData;

TEST_PARAMS(is_digit_test, CharTestData,
    {'0', 1},
    {'5', 1},
    {'9', 1},
    {'a', 0},
    {'Z', 0},
    {' ', 0},
    {'\n', 0}
);

TEST_CASE_PARAM(is_digit_test, CharTestData) {
    int result = (param->input >= '0' && param->input <= '9') ? 1 : 0;
    REQUIRE_EQ(result, param->is_digit);
}

////////////////////////////////////////////////////////////////////////////////
// Example 5: Trigonometric identity tests
////////////////////////////////////////////////////////////////////////////////

typedef struct {
    bj_real angle;
} AngleTestData;

TEST_PARAMS(trig_identity, AngleTestData,
    {BJ_F(0.0)},
    {BJ_F(0.5)},
    {BJ_F(1.0)},
    {BJ_F(1.5)},
    {BJ_F(2.0)},
    {BJ_PI / BJ_F(4.0)},
    {BJ_PI / BJ_F(2.0)},
    {BJ_PI}
);

TEST_CASE_PARAM(trig_identity, AngleTestData) {
    // sin^2(x) + cos^2(x) = 1
    bj_real s = bj_sin(param->angle);
    bj_real c = bj_cos(param->angle);
    bj_real sum = s * s + c * c;
    bj_real diff = bj_abs(sum - BJ_F(1.0));
    REQUIRE(diff < BJ_F(0.0001));
}

////////////////////////////////////////////////////////////////////////////////
// Regular (non-parametric) test to verify mixing works
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(regular_test_still_works) {
    int x = 1 + 1;
    REQUIRE_EQ(x, 2);
}

int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    // Parametric tests - each runs multiple iterations
    RUN_TEST_PARAM(addition);       // 4 iterations
    RUN_TEST_PARAM(sqrt_test);      // 5 iterations
    RUN_TEST_PARAM(clamp_test);     // 5 iterations
    RUN_TEST_PARAM(is_digit_test);  // 7 iterations
    RUN_TEST_PARAM(trig_identity);  // 8 iterations

    // Regular test
    RUN_TEST(regular_test_still_works);

    END_TESTS();
}
