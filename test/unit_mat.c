#include "test.h"
#include <banjo/mat.h>
#include <banjo/math.h>
#include <banjo/vec.h>

#define NEAR(a, b) (bj_abs((a) - (b)) < BJ_F(0.0001))
#define MAT3_NEAR(m, expected) (mat3_all_near(&(m), &(expected)))

static bj_bool mat3_all_near(const struct bj_mat3x3 *a,
                             const struct bj_mat3x3 *b) {
  for (int i = 0; i < 9; ++i) {
    if (!NEAR(a->m[i], b->m[i]))
      return BJ_FALSE;
  }
  return BJ_TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// Mat3x3 Basic Operations
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(mat3_identity_is_correct) {
  struct bj_mat3x3 I;
  bj_mat3_set_identity(&I);

  REQUIRE(NEAR(I.m[BJ_M3(0, 0)], BJ_F(1.0)));
  REQUIRE(NEAR(I.m[BJ_M3(1, 1)], BJ_F(1.0)));
  REQUIRE(NEAR(I.m[BJ_M3(2, 2)], BJ_F(1.0)));
  REQUIRE(NEAR(I.m[BJ_M3(0, 1)], BJ_FZERO));
  REQUIRE(NEAR(I.m[BJ_M3(1, 0)], BJ_FZERO));
}

TEST_CASE(mat3_copy_preserves_values) {
  struct bj_mat3x3 src = {.m = {1, 2, 3, 4, 5, 6, 7, 8, 9}};
  struct bj_mat3x3 dst;
  bj_mat3_copy(&dst, &src);

  for (int i = 0; i < 9; ++i) {
    REQUIRE(NEAR(dst.m[i], src.m[i]));
  }
}

TEST_CASE(mat3_transpose_swaps_rows_and_cols) {
  struct bj_mat3x3 A = {.m = {1, 2, 3, 4, 5, 6, 7, 8, 9}};
  struct bj_mat3x3 AT;
  bj_mat3_transpose(&AT, &A);

  REQUIRE(NEAR(AT.m[BJ_M3(0, 1)], A.m[BJ_M3(1, 0)]));
  REQUIRE(NEAR(AT.m[BJ_M3(1, 0)], A.m[BJ_M3(0, 1)]));
  REQUIRE(NEAR(AT.m[BJ_M3(0, 2)], A.m[BJ_M3(2, 0)]));
}

TEST_CASE(mat3_transpose_twice_returns_original) {
  struct bj_mat3x3 A = {.m = {1, 2, 3, 4, 5, 6, 7, 8, 9}};
  struct bj_mat3x3 AT, ATT;
  bj_mat3_transpose(&AT, &A);
  bj_mat3_transpose(&ATT, &AT);

  REQUIRE(MAT3_NEAR(ATT, A));
}

TEST_CASE(mat3_add_basic) {
  struct bj_mat3x3 A = {.m = {1, 2, 3, 4, 5, 6, 7, 8, 9}};
  struct bj_mat3x3 B = {.m = {9, 8, 7, 6, 5, 4, 3, 2, 1}};
  struct bj_mat3x3 C;
  bj_mat3_add(&C, &A, &B);

  for (int i = 0; i < 9; ++i) {
    REQUIRE(NEAR(C.m[i], A.m[i] + B.m[i]));
  }
}

TEST_CASE(mat3_add_is_commutative) {
  struct bj_mat3x3 A = {.m = {1, 2, 3, 4, 5, 6, 7, 8, 9}};
  struct bj_mat3x3 B = {.m = {9, 8, 7, 6, 5, 4, 3, 2, 1}};
  struct bj_mat3x3 AB, BA;
  bj_mat3_add(&AB, &A, &B);
  bj_mat3_add(&BA, &B, &A);

  REQUIRE(MAT3_NEAR(AB, BA));
}

TEST_CASE(mat3_sub_basic) {
  struct bj_mat3x3 A = {.m = {10, 20, 30, 40, 50, 60, 70, 80, 90}};
  struct bj_mat3x3 B = {.m = {1, 2, 3, 4, 5, 6, 7, 8, 9}};
  struct bj_mat3x3 C;
  bj_mat3_sub(&C, &A, &B);

  for (int i = 0; i < 9; ++i) {
    REQUIRE(NEAR(C.m[i], A.m[i] - B.m[i]));
  }
}

TEST_CASE(mat3_mul_scalar_basic) {
  struct bj_mat3x3 A = {.m = {1, 2, 3, 4, 5, 6, 7, 8, 9}};
  struct bj_mat3x3 B;
  bj_mat3_mul_scalar(&B, &A, BJ_F(2.0));

  for (int i = 0; i < 9; ++i) {
    REQUIRE(NEAR(B.m[i], A.m[i] * BJ_F(2.0)));
  }
}

TEST_CASE(mat3_mul_scalar_by_zero_gives_zero) {
  struct bj_mat3x3 A = {.m = {1, 2, 3, 4, 5, 6, 7, 8, 9}};
  struct bj_mat3x3 B;
  bj_mat3_mul_scalar(&B, &A, BJ_FZERO);

  for (int i = 0; i < 9; ++i) {
    REQUIRE(NEAR(B.m[i], BJ_FZERO));
  }
}

TEST_CASE(mat3_mul_identity_preserves_matrix) {
  struct bj_mat3x3 A = {.m = {1, 2, 3, 4, 5, 6, 7, 8, 9}};
  struct bj_mat3x3 I, result;
  bj_mat3_set_identity(&I);
  bj_mat3_mul(&result, &A, &I);

  REQUIRE(MAT3_NEAR(result, A));
}

TEST_CASE(mat3_mul_basic) {
  struct bj_mat3x3 A = {.m = {1, 0, 0, 0, 1, 0, 0, 0, 1}};
  struct bj_mat3x3 B = {.m = {2, 0, 0, 0, 2, 0, 0, 0, 2}};
  struct bj_mat3x3 C;
  bj_mat3_mul(&C, &A, &B);

  REQUIRE(NEAR(C.m[BJ_M3(0, 0)], BJ_F(2.0)));
  REQUIRE(NEAR(C.m[BJ_M3(1, 1)], BJ_F(2.0)));
  REQUIRE(NEAR(C.m[BJ_M3(2, 2)], BJ_F(2.0)));
}

TEST_CASE(mat3_transform_vec3_identity_unchanged) {
  struct bj_mat3x3 I;
  bj_mat3_set_identity(&I);
  struct bj_vec3 v = {BJ_F(1.0), BJ_F(2.0), BJ_F(3.0)};
  struct bj_vec3 result = bj_mat3_transform_vec3(&I, v);

  REQUIRE(NEAR(result.x, v.x));
  REQUIRE(NEAR(result.y, v.y));
  REQUIRE(NEAR(result.z, v.z));
}

TEST_CASE(mat3_transform_vec3_scale) {
  struct bj_mat3x3 S = {.m = {2, 0, 0, 0, 3, 0, 0, 0, 4}};
  struct bj_vec3 v = {BJ_F(1.0), BJ_F(1.0), BJ_F(1.0)};
  struct bj_vec3 result = bj_mat3_transform_vec3(&S, v);

  REQUIRE(NEAR(result.x, BJ_F(2.0)));
  REQUIRE(NEAR(result.y, BJ_F(3.0)));
  REQUIRE(NEAR(result.z, BJ_F(4.0)));
}

TEST_CASE(mat3_row_extraction) {
  struct bj_mat3x3 M = {.m = {1, 2, 3, 4, 5, 6, 7, 8, 9}};
  struct bj_vec3 row0 = bj_mat3_row(&M, 0);
  struct bj_vec3 row1 = bj_mat3_row(&M, 1);

  REQUIRE(NEAR(row0.x, M.m[BJ_M3(0, 0)]));
  REQUIRE(NEAR(row0.y, M.m[BJ_M3(1, 0)]));
  REQUIRE(NEAR(row0.z, M.m[BJ_M3(2, 0)]));
  REQUIRE(NEAR(row1.x, M.m[BJ_M3(0, 1)]));
}

TEST_CASE(mat3_col_extraction) {
  struct bj_mat3x3 M = {.m = {1, 2, 3, 4, 5, 6, 7, 8, 9}};
  struct bj_vec3 col0 = bj_mat3_col(&M, 0);
  struct bj_vec3 col1 = bj_mat3_col(&M, 1);

  REQUIRE(NEAR(col0.x, M.m[BJ_M3(0, 0)]));
  REQUIRE(NEAR(col0.y, M.m[BJ_M3(0, 1)]));
  REQUIRE(NEAR(col0.z, M.m[BJ_M3(0, 2)]));
  REQUIRE(NEAR(col1.x, M.m[BJ_M3(1, 0)]));
}

////////////////////////////////////////////////////////////////////////////////
// Mat3x3 Property Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(mat3_add_associative) {
  struct bj_mat3x3 A = {.m = {1, 2, 3, 4, 5, 6, 7, 8, 9}};
  struct bj_mat3x3 B = {.m = {9, 8, 7, 6, 5, 4, 3, 2, 1}};
  struct bj_mat3x3 C = {.m = {1, 1, 1, 2, 2, 2, 3, 3, 3}};
  struct bj_mat3x3 AB, ABC1, BC, ABC2;

  bj_mat3_add(&AB, &A, &B);
  bj_mat3_add(&ABC1, &AB, &C);

  bj_mat3_add(&BC, &B, &C);
  bj_mat3_add(&ABC2, &A, &BC);

  REQUIRE(MAT3_NEAR(ABC1, ABC2));
}

TEST_CASE(mat3_mul_associative) {
  struct bj_mat3x3 A = {.m = {1, 0, 0, 0, 2, 0, 0, 0, 3}};
  struct bj_mat3x3 B = {.m = {2, 0, 0, 0, 1, 0, 0, 0, 1}};
  struct bj_mat3x3 C = {.m = {1, 0, 0, 0, 1, 0, 0, 0, 2}};
  struct bj_mat3x3 AB, ABC1, BC, ABC2;

  bj_mat3_mul(&AB, &A, &B);
  bj_mat3_mul(&ABC1, &AB, &C);

  bj_mat3_mul(&BC, &B, &C);
  bj_mat3_mul(&ABC2, &A, &BC);

  REQUIRE(MAT3_NEAR(ABC1, ABC2));
}

TEST_CASE(mat3_transpose_of_product) {
  struct bj_mat3x3 A = {.m = {1, 2, 0, 3, 4, 0, 0, 0, 1}};
  struct bj_mat3x3 B = {.m = {5, 6, 0, 7, 8, 0, 0, 0, 1}};
  struct bj_mat3x3 AB, AB_T, A_T, B_T, B_T_A_T;

  bj_mat3_mul(&AB, &A, &B);
  bj_mat3_transpose(&AB_T, &AB);

  bj_mat3_transpose(&A_T, &A);
  bj_mat3_transpose(&B_T, &B);
  bj_mat3_mul(&B_T_A_T, &B_T, &A_T);

  REQUIRE(MAT3_NEAR(AB_T, B_T_A_T));
}

////////////////////////////////////////////////////////////////////////////////
// Mat4x4 Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(mat4_identity_is_correct) {
  struct bj_mat4x4 I;
  bj_mat4_set_identity(&I);

  REQUIRE(NEAR(I.m[BJ_M4(0, 0)], BJ_F(1.0)));
  REQUIRE(NEAR(I.m[BJ_M4(1, 1)], BJ_F(1.0)));
  REQUIRE(NEAR(I.m[BJ_M4(2, 2)], BJ_F(1.0)));
  REQUIRE(NEAR(I.m[BJ_M4(3, 3)], BJ_F(1.0)));
  REQUIRE(NEAR(I.m[BJ_M4(0, 1)], BJ_FZERO));
}

TEST_CASE(mat4_transform_vec4_identity_unchanged) {
  struct bj_mat4x4 I;
  bj_mat4_set_identity(&I);
  struct bj_vec4 v = {BJ_F(1.0), BJ_F(2.0), BJ_F(3.0), BJ_F(1.0)};
  struct bj_vec4 result = bj_mat4_transform_vec4(&I, v);

  REQUIRE(NEAR(result.x, v.x));
  REQUIRE(NEAR(result.y, v.y));
  REQUIRE(NEAR(result.z, v.z));
  REQUIRE(NEAR(result.w, v.w));
}

TEST_CASE(mat4_mul_identity_preserves_matrix) {
  struct bj_mat4x4 A, I, result;
  for (int i = 0; i < 16; ++i)
    A.m[i] = (bj_real)(i + 1);
  bj_mat4_set_identity(&I);
  bj_mat4_mul(&result, &A, &I);

  for (int i = 0; i < 16; ++i) {
    REQUIRE(NEAR(result.m[i], A.m[i]));
  }
}

int main(int argc, char *argv[]) {
  BEGIN_TESTS(argc, argv);

  // Mat3x3 basic operations
  RUN_TEST(mat3_identity_is_correct);
  RUN_TEST(mat3_copy_preserves_values);
  RUN_TEST(mat3_transpose_swaps_rows_and_cols);
  RUN_TEST(mat3_transpose_twice_returns_original);
  RUN_TEST(mat3_add_basic);
  RUN_TEST(mat3_add_is_commutative);
  RUN_TEST(mat3_sub_basic);
  RUN_TEST(mat3_mul_scalar_basic);
  RUN_TEST(mat3_mul_scalar_by_zero_gives_zero);
  RUN_TEST(mat3_mul_identity_preserves_matrix);
  RUN_TEST(mat3_mul_basic);
  RUN_TEST(mat3_transform_vec3_identity_unchanged);
  RUN_TEST(mat3_transform_vec3_scale);
  RUN_TEST(mat3_row_extraction);
  RUN_TEST(mat3_col_extraction);

  // Mat3x3 properties
  RUN_TEST(mat3_add_associative);
  RUN_TEST(mat3_mul_associative);
  RUN_TEST(mat3_transpose_of_product);

  // Mat4x4 tests
  RUN_TEST(mat4_identity_is_correct);
  RUN_TEST(mat4_transform_vec4_identity_unchanged);
  RUN_TEST(mat4_mul_identity_preserves_matrix);

  END_TESTS();
}
