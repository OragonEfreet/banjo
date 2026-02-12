#include "test.h"
#include <banjo/math.h>
#include <banjo/random.h>
#include <banjo/vec.h>

#define NEAR(a, b) (bj_abs((a) - (b)) < BJ_F(0.001))
#define VEC2_NEAR(v, ex, ey) (NEAR((v).x, ex) && NEAR((v).y, ey))
#define VEC3_NEAR(v, ex, ey, ez)                                               \
  (NEAR((v).x, ex) && NEAR((v).y, ey) && NEAR((v).z, ez))

////////////////////////////////////////////////////////////////////////////////
// Property-Based Tests Using Parametric Framework
////////////////////////////////////////////////////////////////////////////////

// Test data for vec2 commutativity
typedef struct {
  bj_real ax, ay, bx, by;
} Vec2PairData;

TEST_PARAMS(vec2_add_commutative, Vec2PairData,
            {BJ_F(1.0), BJ_F(2.0), BJ_F(3.0), BJ_F(4.0)},
            {BJ_F(-1.0), BJ_F(5.0), BJ_F(2.0), BJ_F(-3.0)},
            {BJ_F(0.0), BJ_F(0.0), BJ_F(1.0), BJ_F(1.0)},
            {BJ_F(10.5), BJ_F(-7.3), BJ_F(4.2), BJ_F(8.9)},
            {BJ_F(100.0), BJ_F(200.0), BJ_F(-50.0), BJ_F(-100.0)});

TEST_CASE_PARAM(vec2_add_commutative, Vec2PairData) {
  struct bj_vec2 a = {param->ax, param->ay};
  struct bj_vec2 b = {param->bx, param->by};

  struct bj_vec2 ab = bj_vec2_add(a, b);
  struct bj_vec2 ba = bj_vec2_add(b, a);

  REQUIRE(VEC2_NEAR(ab, ba.x, ba.y));
}

// Test vec2 associativity
typedef struct {
  bj_real ax, ay, bx, by, cx, cy;
} Vec2TripleData;

TEST_PARAMS(vec2_add_associative, Vec2TripleData,
            {BJ_F(1.0), BJ_F(2.0), BJ_F(3.0), BJ_F(4.0), BJ_F(5.0), BJ_F(6.0)},
            {BJ_F(-1.0), BJ_F(1.0), BJ_F(-2.0), BJ_F(2.0), BJ_F(-3.0),
             BJ_F(3.0)},
            {BJ_F(0.5), BJ_F(0.5), BJ_F(1.5), BJ_F(1.5), BJ_F(2.5), BJ_F(2.5)});

TEST_CASE_PARAM(vec2_add_associative, Vec2TripleData) {
  struct bj_vec2 a = {param->ax, param->ay};
  struct bj_vec2 b = {param->bx, param->by};
  struct bj_vec2 c = {param->cx, param->cy};

  struct bj_vec2 ab = bj_vec2_add(a, b);
  struct bj_vec2 abc1 = bj_vec2_add(ab, c);

  struct bj_vec2 bc = bj_vec2_add(b, c);
  struct bj_vec2 abc2 = bj_vec2_add(a, bc);

  REQUIRE(VEC2_NEAR(abc1, abc2.x, abc2.y));
}

// Test scalar distributivity
typedef struct {
  bj_real ax, ay, bx, by, k;
} Vec2ScalarData;

TEST_PARAMS(vec2_scalar_distributive, Vec2ScalarData,
            {BJ_F(1.0), BJ_F(2.0), BJ_F(3.0), BJ_F(4.0), BJ_F(2.0)},
            {BJ_F(5.0), BJ_F(-3.0), BJ_F(2.0), BJ_F(7.0), BJ_F(0.5)},
            {BJ_F(-1.0), BJ_F(1.0), BJ_F(1.0), BJ_F(-1.0), BJ_F(3.0)});

TEST_CASE_PARAM(vec2_scalar_distributive, Vec2ScalarData) {
  struct bj_vec2 a = {param->ax, param->ay};
  struct bj_vec2 b = {param->bx, param->by};
  bj_real k = param->k;

  // k * (a + b) = k*a + k*b
  struct bj_vec2 sum = bj_vec2_add(a, b);
  struct bj_vec2 lhs = bj_vec2_scale(sum, k);

  struct bj_vec2 ka = bj_vec2_scale(a, k);
  struct bj_vec2 kb = bj_vec2_scale(b, k);
  struct bj_vec2 rhs = bj_vec2_add(ka, kb);

  REQUIRE(VEC2_NEAR(lhs, rhs.x, rhs.y));
}

// Test dot product commutativity
TEST_PARAMS(vec2_dot_commutative, Vec2PairData,
            {BJ_F(1.0), BJ_F(2.0), BJ_F(3.0), BJ_F(4.0)},
            {BJ_F(-2.0), BJ_F(5.0), BJ_F(1.0), BJ_F(-1.0)},
            {BJ_F(7.5), BJ_F(3.2), BJ_F(4.1), BJ_F(9.8)});

TEST_CASE_PARAM(vec2_dot_commutative, Vec2PairData) {
  struct bj_vec2 a = {param->ax, param->ay};
  struct bj_vec2 b = {param->bx, param->by};

  bj_real ab = bj_vec2_dot(a, b);
  bj_real ba = bj_vec2_dot(b, a);

  REQUIRE(NEAR(ab, ba));
}

// Test normalize produces unit length
typedef struct {
  bj_real x, y;
} Vec2Data;

TEST_PARAMS(vec2_normalize_unit_length, Vec2Data, {BJ_F(3.0), BJ_F(4.0)},
            {BJ_F(1.0), BJ_F(1.0)}, {BJ_F(5.0), BJ_F(12.0)},
            {BJ_F(-3.0), BJ_F(4.0)}, {BJ_F(7.0), BJ_F(-24.0)});

TEST_CASE_PARAM(vec2_normalize_unit_length, Vec2Data) {
  struct bj_vec2 v = {param->x, param->y};
  struct bj_vec2 n = bj_vec2_normalize(v);
  bj_real len = bj_vec2_len(n);

  // Skip if input was zero vector
  if (bj_vec2_len(v) > BJ_F(0.0001)) {
    REQUIRE(NEAR(len, BJ_F(1.0)));
  }
}

// Test triangle inequality: |a + b| <= |a| + |b|
TEST_PARAMS(vec2_triangle_inequality, Vec2PairData,
            {BJ_F(3.0), BJ_F(4.0), BJ_F(5.0), BJ_F(12.0)},
            {BJ_F(1.0), BJ_F(0.0), BJ_F(0.0), BJ_F(1.0)},
            {BJ_F(-2.0), BJ_F(3.0), BJ_F(4.0), BJ_F(-1.0)});

TEST_CASE_PARAM(vec2_triangle_inequality, Vec2PairData) {
  struct bj_vec2 a = {param->ax, param->ay};
  struct bj_vec2 b = {param->bx, param->by};

  struct bj_vec2 sum = bj_vec2_add(a, b);
  bj_real len_sum = bj_vec2_len(sum);
  bj_real len_a = bj_vec2_len(a);
  bj_real len_b = bj_vec2_len(b);

  REQUIRE(len_sum <=
          len_a + len_b + BJ_F(0.001)); // Small epsilon for floating point
}

// Test Cauchy-Schwarz inequality: |a · b| <= |a| * |b|
TEST_PARAMS(vec2_cauchy_schwarz, Vec2PairData,
            {BJ_F(3.0), BJ_F(4.0), BJ_F(5.0), BJ_F(12.0)},
            {BJ_F(1.0), BJ_F(2.0), BJ_F(3.0), BJ_F(4.0)},
            {BJ_F(-1.0), BJ_F(1.0), BJ_F(1.0), BJ_F(-1.0)});

TEST_CASE_PARAM(vec2_cauchy_schwarz, Vec2PairData) {
  struct bj_vec2 a = {param->ax, param->ay};
  struct bj_vec2 b = {param->bx, param->by};

  bj_real dot = bj_abs(bj_vec2_dot(a, b));
  bj_real len_a = bj_vec2_len(a);
  bj_real len_b = bj_vec2_len(b);

  REQUIRE(dot <= len_a * len_b + BJ_F(0.001));
}

////////////////////////////////////////////////////////////////////////////////
// Vec3 Property Tests
////////////////////////////////////////////////////////////////////////////////

typedef struct {
  bj_real ax, ay, az, bx, by, bz;
} Vec3PairData;

TEST_PARAMS(vec3_add_commutative, Vec3PairData,
            {BJ_F(1.0), BJ_F(2.0), BJ_F(3.0), BJ_F(4.0), BJ_F(5.0), BJ_F(6.0)},
            {BJ_F(-1.0), BJ_F(2.0), BJ_F(-3.0), BJ_F(4.0), BJ_F(-5.0),
             BJ_F(6.0)});

TEST_CASE_PARAM(vec3_add_commutative, Vec3PairData) {
  struct bj_vec3 a = {param->ax, param->ay, param->az};
  struct bj_vec3 b = {param->bx, param->by, param->bz};

  struct bj_vec3 ab = bj_vec3_add(a, b);
  struct bj_vec3 ba = bj_vec3_add(b, a);

  REQUIRE(VEC3_NEAR(ab, ba.x, ba.y, ba.z));
}

// Test cross product anticommutativity: a × b = -(b × a)
TEST_PARAMS(vec3_cross_anticommutative, Vec3PairData,
            {BJ_F(1.0), BJ_F(0.0), BJ_F(0.0), BJ_F(0.0), BJ_F(1.0), BJ_F(0.0)},
            {BJ_F(1.0), BJ_F(2.0), BJ_F(3.0), BJ_F(4.0), BJ_F(5.0), BJ_F(6.0)},
            {BJ_F(2.0), BJ_F(-1.0), BJ_F(3.0), BJ_F(1.0), BJ_F(4.0),
             BJ_F(-2.0)});

TEST_CASE_PARAM(vec3_cross_anticommutative, Vec3PairData) {
  struct bj_vec3 a = {param->ax, param->ay, param->az};
  struct bj_vec3 b = {param->bx, param->by, param->bz};

  struct bj_vec3 ab = bj_vec3_cross(a, b);
  struct bj_vec3 ba = bj_vec3_cross(b, a);

  REQUIRE(VEC3_NEAR(ab, -ba.x, -ba.y, -ba.z));
}

// Test cross product perpendicularity: (a × b) · a = 0
TEST_PARAMS(vec3_cross_perpendicular, Vec3PairData,
            {BJ_F(1.0), BJ_F(2.0), BJ_F(3.0), BJ_F(4.0), BJ_F(5.0), BJ_F(6.0)},
            {BJ_F(2.0), BJ_F(-1.0), BJ_F(1.0), BJ_F(3.0), BJ_F(2.0),
             BJ_F(-1.0)});

TEST_CASE_PARAM(vec3_cross_perpendicular, Vec3PairData) {
  struct bj_vec3 a = {param->ax, param->ay, param->az};
  struct bj_vec3 b = {param->bx, param->by, param->bz};

  struct bj_vec3 cross = bj_vec3_cross(a, b);
  bj_real dot_a = bj_vec3_dot(cross, a);
  bj_real dot_b = bj_vec3_dot(cross, b);

  REQUIRE(NEAR(dot_a, BJ_FZERO));
  REQUIRE(NEAR(dot_b, BJ_FZERO));
}

int main(int argc, char *argv[]) {
  BEGIN_TESTS(argc, argv);

  // Vec2 property tests
  RUN_TEST_PARAM(vec2_add_commutative);
  RUN_TEST_PARAM(vec2_add_associative);
  RUN_TEST_PARAM(vec2_scalar_distributive);
  RUN_TEST_PARAM(vec2_dot_commutative);
  RUN_TEST_PARAM(vec2_normalize_unit_length);
  RUN_TEST_PARAM(vec2_triangle_inequality);
  RUN_TEST_PARAM(vec2_cauchy_schwarz);

  // Vec3 property tests
  RUN_TEST_PARAM(vec3_add_commutative);
  RUN_TEST_PARAM(vec3_cross_anticommutative);
  RUN_TEST_PARAM(vec3_cross_perpendicular);

  END_TESTS();
}
