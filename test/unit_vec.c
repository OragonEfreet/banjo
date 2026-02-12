#include "test.h"
#include <banjo/vec.h>
#include <banjo/math.h>

// Tolerance for floating point comparisons
#define NEAR(a, b) (bj_abs((a) - (b)) < BJ_F(0.0001))
#define VEC2_NEAR(v, ex, ey) (NEAR((v).x, ex) && NEAR((v).y, ey))
#define VEC3_NEAR(v, ex, ey, ez) (NEAR((v).x, ex) && NEAR((v).y, ey) && NEAR((v).z, ez))
#define VEC4_NEAR(v, ex, ey, ez, ew) (NEAR((v).x, ex) && NEAR((v).y, ey) && NEAR((v).z, ez) && NEAR((v).w, ew))

////////////////////////////////////////////////////////////////////////////////
// Vec2 Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(vec2_zero_constant) {
    struct bj_vec2 z = BJ_VEC2_ZERO;
    REQUIRE(VEC2_NEAR(z, BJ_FZERO, BJ_FZERO));
}

TEST_CASE(vec2_add_basic) {
    struct bj_vec2 a = {BJ_F(1.0), BJ_F(2.0)};
    struct bj_vec2 b = {BJ_F(3.0), BJ_F(4.0)};
    struct bj_vec2 r = bj_vec2_add(a, b);
    REQUIRE(VEC2_NEAR(r, BJ_F(4.0), BJ_F(6.0)));
}

TEST_CASE(vec2_add_sub_inverse_returns_original) {
    struct bj_vec2 a = {BJ_F(5.0), BJ_F(-3.0)};
    struct bj_vec2 b = {BJ_F(2.0), BJ_F(7.0)};
    struct bj_vec2 sum = bj_vec2_add(a, b);
    struct bj_vec2 back = bj_vec2_sub(sum, b);
    REQUIRE(VEC2_NEAR(back, a.x, a.y));
}

TEST_CASE(vec2_scale_by_zero_returns_zero) {
    struct bj_vec2 v = {BJ_F(10.0), BJ_F(-20.0)};
    struct bj_vec2 r = bj_vec2_scale(v, BJ_FZERO);
    REQUIRE(VEC2_NEAR(r, BJ_FZERO, BJ_FZERO));
}

TEST_CASE(vec2_scale_by_one_unchanged) {
    struct bj_vec2 v = {BJ_F(3.0), BJ_F(4.0)};
    struct bj_vec2 r = bj_vec2_scale(v, BJ_F(1.0));
    REQUIRE(VEC2_NEAR(r, v.x, v.y));
}

TEST_CASE(vec2_dot_perpendicular_is_zero) {
    struct bj_vec2 a = {BJ_F(1.0), BJ_F(0.0)};
    struct bj_vec2 b = {BJ_F(0.0), BJ_F(1.0)};
    bj_real d = bj_vec2_dot(a, b);
    REQUIRE(NEAR(d, BJ_FZERO));
}

TEST_CASE(vec2_dot_parallel_is_product) {
    struct bj_vec2 a = {BJ_F(3.0), BJ_F(0.0)};
    struct bj_vec2 b = {BJ_F(4.0), BJ_F(0.0)};
    bj_real d = bj_vec2_dot(a, b);
    REQUIRE(NEAR(d, BJ_F(12.0)));
}

TEST_CASE(vec2_dot_is_commutative) {
    struct bj_vec2 a = {BJ_F(1.0), BJ_F(2.0)};
    struct bj_vec2 b = {BJ_F(3.0), BJ_F(4.0)};
    REQUIRE(NEAR(bj_vec2_dot(a, b), bj_vec2_dot(b, a)));
}

TEST_CASE(vec2_len_of_unit_x_is_one) {
    struct bj_vec2 v = {BJ_F(1.0), BJ_F(0.0)};
    REQUIRE(NEAR(bj_vec2_len(v), BJ_F(1.0)));
}

TEST_CASE(vec2_len_of_3_4_is_5) {
    struct bj_vec2 v = {BJ_F(3.0), BJ_F(4.0)};
    REQUIRE(NEAR(bj_vec2_len(v), BJ_F(5.0)));
}

TEST_CASE(vec2_normalize_unit_vector_unchanged) {
    struct bj_vec2 v = {BJ_F(1.0), BJ_F(0.0)};
    struct bj_vec2 n = bj_vec2_normalize(v);
    REQUIRE(VEC2_NEAR(n, BJ_F(1.0), BJ_F(0.0)));
}

TEST_CASE(vec2_normalize_result_has_unit_length) {
    struct bj_vec2 v = {BJ_F(3.0), BJ_F(4.0)};
    struct bj_vec2 n = bj_vec2_normalize(v);
    REQUIRE(NEAR(bj_vec2_len(n), BJ_F(1.0)));
}

TEST_CASE(vec2_normalize_zero_vector_returns_zero) {
    struct bj_vec2 v = BJ_VEC2_ZERO;
    struct bj_vec2 n = bj_vec2_normalize(v);
    REQUIRE(VEC2_NEAR(n, BJ_FZERO, BJ_FZERO));
}

TEST_CASE(vec2_distance_is_commutative) {
    struct bj_vec2 a = {BJ_F(1.0), BJ_F(2.0)};
    struct bj_vec2 b = {BJ_F(4.0), BJ_F(6.0)};
    REQUIRE(NEAR(bj_vec2_distance(a, b), bj_vec2_distance(b, a)));
}

TEST_CASE(vec2_distance_to_self_is_zero) {
    struct bj_vec2 v = {BJ_F(5.0), BJ_F(5.0)};
    REQUIRE(NEAR(bj_vec2_distance(v, v), BJ_FZERO));
}

TEST_CASE(vec2_perp_dot_sign_indicates_orientation) {
    struct bj_vec2 a = {BJ_F(1.0), BJ_F(0.0)};
    struct bj_vec2 b_ccw = {BJ_F(0.0), BJ_F(1.0)};  // Counter-clockwise from a
    struct bj_vec2 b_cw = {BJ_F(0.0), BJ_F(-1.0)}; // Clockwise from a

    REQUIRE(bj_vec2_perp_dot(a, b_ccw) > BJ_FZERO);
    REQUIRE(bj_vec2_perp_dot(a, b_cw) < BJ_FZERO);
}

TEST_CASE(vec2_min_max_component_wise) {
    struct bj_vec2 a = {BJ_F(1.0), BJ_F(5.0)};
    struct bj_vec2 b = {BJ_F(3.0), BJ_F(2.0)};

    struct bj_vec2 mn = bj_vec2_min(a, b);
    struct bj_vec2 mx = bj_vec2_max(a, b);

    REQUIRE(VEC2_NEAR(mn, BJ_F(1.0), BJ_F(2.0)));
    REQUIRE(VEC2_NEAR(mx, BJ_F(3.0), BJ_F(5.0)));
}

TEST_CASE(vec2_add_scaled) {
    struct bj_vec2 a = {BJ_F(1.0), BJ_F(2.0)};
    struct bj_vec2 b = {BJ_F(2.0), BJ_F(3.0)};
    struct bj_vec2 r = bj_vec2_add_scaled(a, b, BJ_F(2.0));
    REQUIRE(VEC2_NEAR(r, BJ_F(5.0), BJ_F(8.0))); // 1+2*2, 2+3*2
}

TEST_CASE(vec2_mul_comp) {
    struct bj_vec2 a = {BJ_F(2.0), BJ_F(3.0)};
    struct bj_vec2 b = {BJ_F(4.0), BJ_F(5.0)};
    struct bj_vec2 r = bj_vec2_mul_comp(a, b);
    REQUIRE(VEC2_NEAR(r, BJ_F(8.0), BJ_F(15.0)));
}

////////////////////////////////////////////////////////////////////////////////
// Vec3 Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(vec3_zero_constant) {
    struct bj_vec3 z = BJ_VEC3_ZERO;
    REQUIRE(VEC3_NEAR(z, BJ_FZERO, BJ_FZERO, BJ_FZERO));
}

TEST_CASE(vec3_add_basic) {
    struct bj_vec3 a = {BJ_F(1.0), BJ_F(2.0), BJ_F(3.0)};
    struct bj_vec3 b = {BJ_F(4.0), BJ_F(5.0), BJ_F(6.0)};
    struct bj_vec3 r = bj_vec3_add(a, b);
    REQUIRE(VEC3_NEAR(r, BJ_F(5.0), BJ_F(7.0), BJ_F(9.0)));
}

TEST_CASE(vec3_cross_product_is_perpendicular) {
    struct bj_vec3 a = {BJ_F(1.0), BJ_F(0.0), BJ_F(0.0)};
    struct bj_vec3 b = {BJ_F(0.0), BJ_F(1.0), BJ_F(0.0)};
    struct bj_vec3 c = bj_vec3_cross(a, b);

    // Cross product should be perpendicular to both inputs
    REQUIRE(NEAR(bj_vec3_dot(c, a), BJ_FZERO));
    REQUIRE(NEAR(bj_vec3_dot(c, b), BJ_FZERO));
}

TEST_CASE(vec3_cross_x_cross_y_equals_z) {
    struct bj_vec3 x = {BJ_F(1.0), BJ_F(0.0), BJ_F(0.0)};
    struct bj_vec3 y = {BJ_F(0.0), BJ_F(1.0), BJ_F(0.0)};
    struct bj_vec3 z = bj_vec3_cross(x, y);
    REQUIRE(VEC3_NEAR(z, BJ_F(0.0), BJ_F(0.0), BJ_F(1.0)));
}

TEST_CASE(vec3_cross_is_anticommutative) {
    struct bj_vec3 a = {BJ_F(1.0), BJ_F(2.0), BJ_F(3.0)};
    struct bj_vec3 b = {BJ_F(4.0), BJ_F(5.0), BJ_F(6.0)};
    struct bj_vec3 ab = bj_vec3_cross(a, b);
    struct bj_vec3 ba = bj_vec3_cross(b, a);
    REQUIRE(VEC3_NEAR(ab, -ba.x, -ba.y, -ba.z));
}

TEST_CASE(vec3_cross_of_parallel_is_zero) {
    struct bj_vec3 a = {BJ_F(1.0), BJ_F(2.0), BJ_F(3.0)};
    struct bj_vec3 b = {BJ_F(2.0), BJ_F(4.0), BJ_F(6.0)}; // 2*a
    struct bj_vec3 c = bj_vec3_cross(a, b);
    REQUIRE(VEC3_NEAR(c, BJ_FZERO, BJ_FZERO, BJ_FZERO));
}

TEST_CASE(vec3_normalize_zero_returns_zero) {
    struct bj_vec3 v = BJ_VEC3_ZERO;
    struct bj_vec3 n = bj_vec3_normalize(v);
    REQUIRE(VEC3_NEAR(n, BJ_FZERO, BJ_FZERO, BJ_FZERO));
}

TEST_CASE(vec3_normalize_result_has_unit_length) {
    struct bj_vec3 v = {BJ_F(1.0), BJ_F(2.0), BJ_F(2.0)};
    struct bj_vec3 n = bj_vec3_normalize(v);
    REQUIRE(NEAR(bj_vec3_len(n), BJ_F(1.0)));
}

TEST_CASE(vec3_reflect_basic) {
    // Reflect (1,-1,0) about normal (0,1,0) -> should give (1,1,0)
    struct bj_vec3 v = {BJ_F(1.0), BJ_F(-1.0), BJ_F(0.0)};
    struct bj_vec3 n = {BJ_F(0.0), BJ_F(1.0), BJ_F(0.0)};
    struct bj_vec3 r = bj_vec3_reflect(v, n);
    REQUIRE(VEC3_NEAR(r, BJ_F(1.0), BJ_F(1.0), BJ_F(0.0)));
}

TEST_CASE(vec3_distance_sq) {
    struct bj_vec3 a = {BJ_F(0.0), BJ_F(0.0), BJ_F(0.0)};
    struct bj_vec3 b = {BJ_F(1.0), BJ_F(2.0), BJ_F(2.0)};
    // distance = sqrt(1+4+4) = 3, distance_sq = 9
    REQUIRE(NEAR(bj_vec3_distance_sq(a, b), BJ_F(9.0)));
    REQUIRE(NEAR(bj_vec3_distance(a, b), BJ_F(3.0)));
}

////////////////////////////////////////////////////////////////////////////////
// Vec4 Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(vec4_zero_constant) {
    struct bj_vec4 z = BJ_VEC4_ZERO;
    REQUIRE(VEC4_NEAR(z, BJ_FZERO, BJ_FZERO, BJ_FZERO, BJ_FZERO));
}

TEST_CASE(vec4_add_basic) {
    struct bj_vec4 a = {BJ_F(1.0), BJ_F(2.0), BJ_F(3.0), BJ_F(4.0)};
    struct bj_vec4 b = {BJ_F(5.0), BJ_F(6.0), BJ_F(7.0), BJ_F(8.0)};
    struct bj_vec4 r = bj_vec4_add(a, b);
    REQUIRE(VEC4_NEAR(r, BJ_F(6.0), BJ_F(8.0), BJ_F(10.0), BJ_F(12.0)));
}

TEST_CASE(vec4_dot_basic) {
    struct bj_vec4 a = {BJ_F(1.0), BJ_F(0.0), BJ_F(0.0), BJ_F(0.0)};
    struct bj_vec4 b = {BJ_F(1.0), BJ_F(0.0), BJ_F(0.0), BJ_F(0.0)};
    REQUIRE(NEAR(bj_vec4_dot(a, b), BJ_F(1.0)));
}

TEST_CASE(vec4_normalize_zero_returns_zero) {
    struct bj_vec4 v = BJ_VEC4_ZERO;
    struct bj_vec4 n = bj_vec4_normalize(v);
    REQUIRE(VEC4_NEAR(n, BJ_FZERO, BJ_FZERO, BJ_FZERO, BJ_FZERO));
}

TEST_CASE(vec4_normalize_result_has_unit_length) {
    struct bj_vec4 v = {BJ_F(1.0), BJ_F(1.0), BJ_F(1.0), BJ_F(1.0)};
    struct bj_vec4 n = bj_vec4_normalize(v);
    REQUIRE(NEAR(bj_vec4_len(n), BJ_F(1.0)));
}

TEST_CASE(vec4_cross_xyz_basic) {
    struct bj_vec4 x = {BJ_F(1.0), BJ_F(0.0), BJ_F(0.0), BJ_F(99.0)};
    struct bj_vec4 y = {BJ_F(0.0), BJ_F(1.0), BJ_F(0.0), BJ_F(99.0)};
    struct bj_vec4 z = bj_vec4_cross_xyz(x, y);
    // Should be (0,0,1,0) - w is set to 0
    REQUIRE(VEC4_NEAR(z, BJ_F(0.0), BJ_F(0.0), BJ_F(1.0), BJ_F(0.0)));
}

TEST_CASE(vec4_reflect_preserves_w) {
    struct bj_vec4 v = {BJ_F(1.0), BJ_F(-1.0), BJ_F(0.0), BJ_F(1.0)};
    struct bj_vec4 n = {BJ_F(0.0), BJ_F(1.0), BJ_F(0.0), BJ_F(0.0)};
    struct bj_vec4 r = bj_vec4_reflect(v, n);
    // w should be reflected too by the formula (it uses full 4D dot)
    REQUIRE(VEC4_NEAR(r, BJ_F(1.0), BJ_F(1.0), BJ_F(0.0), BJ_F(1.0)));
}

TEST_CASE(vec4_min_max_component_wise) {
    struct bj_vec4 a = {BJ_F(1.0), BJ_F(5.0), BJ_F(2.0), BJ_F(8.0)};
    struct bj_vec4 b = {BJ_F(3.0), BJ_F(2.0), BJ_F(4.0), BJ_F(1.0)};

    struct bj_vec4 mn = bj_vec4_min(a, b);
    struct bj_vec4 mx = bj_vec4_max(a, b);

    REQUIRE(VEC4_NEAR(mn, BJ_F(1.0), BJ_F(2.0), BJ_F(2.0), BJ_F(1.0)));
    REQUIRE(VEC4_NEAR(mx, BJ_F(3.0), BJ_F(5.0), BJ_F(4.0), BJ_F(8.0)));
}

////////////////////////////////////////////////////////////////////////////////
// Property Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(vec2_add_is_commutative) {
    struct bj_vec2 a = {BJ_F(1.5), BJ_F(-2.5)};
    struct bj_vec2 b = {BJ_F(3.0), BJ_F(4.0)};
    struct bj_vec2 ab = bj_vec2_add(a, b);
    struct bj_vec2 ba = bj_vec2_add(b, a);
    REQUIRE(VEC2_NEAR(ab, ba.x, ba.y));
}

TEST_CASE(vec3_add_is_commutative) {
    struct bj_vec3 a = {BJ_F(1.0), BJ_F(2.0), BJ_F(3.0)};
    struct bj_vec3 b = {BJ_F(4.0), BJ_F(5.0), BJ_F(6.0)};
    struct bj_vec3 ab = bj_vec3_add(a, b);
    struct bj_vec3 ba = bj_vec3_add(b, a);
    REQUIRE(VEC3_NEAR(ab, ba.x, ba.y, ba.z));
}

int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    // Vec2
    RUN_TEST(vec2_zero_constant);
    RUN_TEST(vec2_add_basic);
    RUN_TEST(vec2_add_sub_inverse_returns_original);
    RUN_TEST(vec2_scale_by_zero_returns_zero);
    RUN_TEST(vec2_scale_by_one_unchanged);
    RUN_TEST(vec2_dot_perpendicular_is_zero);
    RUN_TEST(vec2_dot_parallel_is_product);
    RUN_TEST(vec2_dot_is_commutative);
    RUN_TEST(vec2_len_of_unit_x_is_one);
    RUN_TEST(vec2_len_of_3_4_is_5);
    RUN_TEST(vec2_normalize_unit_vector_unchanged);
    RUN_TEST(vec2_normalize_result_has_unit_length);
    RUN_TEST(vec2_normalize_zero_vector_returns_zero);
    RUN_TEST(vec2_distance_is_commutative);
    RUN_TEST(vec2_distance_to_self_is_zero);
    RUN_TEST(vec2_perp_dot_sign_indicates_orientation);
    RUN_TEST(vec2_min_max_component_wise);
    RUN_TEST(vec2_add_scaled);
    RUN_TEST(vec2_mul_comp);

    // Vec3
    RUN_TEST(vec3_zero_constant);
    RUN_TEST(vec3_add_basic);
    RUN_TEST(vec3_cross_product_is_perpendicular);
    RUN_TEST(vec3_cross_x_cross_y_equals_z);
    RUN_TEST(vec3_cross_is_anticommutative);
    RUN_TEST(vec3_cross_of_parallel_is_zero);
    RUN_TEST(vec3_normalize_zero_returns_zero);
    RUN_TEST(vec3_normalize_result_has_unit_length);
    RUN_TEST(vec3_reflect_basic);
    RUN_TEST(vec3_distance_sq);

    // Vec4
    RUN_TEST(vec4_zero_constant);
    RUN_TEST(vec4_add_basic);
    RUN_TEST(vec4_dot_basic);
    RUN_TEST(vec4_normalize_zero_returns_zero);
    RUN_TEST(vec4_normalize_result_has_unit_length);
    RUN_TEST(vec4_cross_xyz_basic);
    RUN_TEST(vec4_reflect_preserves_w);
    RUN_TEST(vec4_min_max_component_wise);

    // Properties
    RUN_TEST(vec2_add_is_commutative);
    RUN_TEST(vec3_add_is_commutative);

    END_TESTS();
}
