#include "test.h"
#include <banjo/quat.h>
#include <banjo/math.h>

// Tolerance for floating point comparisons
#define NEAR(a, b) (bj_abs((a) - (b)) < BJ_F(0.0001))
#define QUAT_NEAR(q, ex, ey, ez, ew) (NEAR((q).x, ex) && NEAR((q).y, ey) && NEAR((q).z, ez) && NEAR((q).w, ew))
#define VEC3_NEAR(v, ex, ey, ez) (NEAR((v).x, ex) && NEAR((v).y, ey) && NEAR((v).z, ez))

////////////////////////////////////////////////////////////////////////////////
// Identity Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(quat_identity_is_0_0_0_1) {
    struct bj_vec4 id = bj_quat_identity();
    REQUIRE(QUAT_NEAR(id, BJ_FZERO, BJ_FZERO, BJ_FZERO, BJ_F(1.0)));
}

TEST_CASE(quat_identity_mul_q_equals_q) {
    struct bj_vec4 id = bj_quat_identity();
    struct bj_vec4 q = {BJ_F(0.1), BJ_F(0.2), BJ_F(0.3), BJ_F(0.9)};
    q = bj_quat_normalize(q);

    struct bj_vec4 r = bj_quat_mul(id, q);
    REQUIRE(QUAT_NEAR(r, q.x, q.y, q.z, q.w));
}

TEST_CASE(quat_q_mul_identity_equals_q) {
    struct bj_vec4 id = bj_quat_identity();
    struct bj_vec4 q = {BJ_F(0.1), BJ_F(0.2), BJ_F(0.3), BJ_F(0.9)};
    q = bj_quat_normalize(q);

    struct bj_vec4 r = bj_quat_mul(q, id);
    REQUIRE(QUAT_NEAR(r, q.x, q.y, q.z, q.w));
}

TEST_CASE(quat_identity_rotates_nothing) {
    struct bj_vec4 id = bj_quat_identity();
    struct bj_vec3 v = {BJ_F(1.0), BJ_F(2.0), BJ_F(3.0)};

    struct bj_vec3 r = bj_quat_rotate_vec3(id, v);
    REQUIRE(VEC3_NEAR(r, v.x, v.y, v.z));
}

////////////////////////////////////////////////////////////////////////////////
// Norm and Normalize Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(quat_norm_of_identity_is_one) {
    struct bj_vec4 id = bj_quat_identity();
    REQUIRE(NEAR(bj_quat_norm(id), BJ_F(1.0)));
}

TEST_CASE(quat_normalize_produces_unit_length) {
    struct bj_vec4 q = {BJ_F(1.0), BJ_F(2.0), BJ_F(3.0), BJ_F(4.0)};
    struct bj_vec4 n = bj_quat_normalize(q);
    REQUIRE(NEAR(bj_quat_norm(n), BJ_F(1.0)));
}

TEST_CASE(quat_normalize_zero_returns_identity) {
    struct bj_vec4 zero = {BJ_FZERO, BJ_FZERO, BJ_FZERO, BJ_FZERO};
    struct bj_vec4 n = bj_quat_normalize(zero);
    // Should return identity when input is zero
    REQUIRE(QUAT_NEAR(n, BJ_FZERO, BJ_FZERO, BJ_FZERO, BJ_F(1.0)));
}

TEST_CASE(quat_norm2_is_squared_norm) {
    struct bj_vec4 q = {BJ_F(1.0), BJ_F(2.0), BJ_F(3.0), BJ_F(4.0)};
    bj_real n = bj_quat_norm(q);
    bj_real n2 = bj_quat_norm2(q);
    REQUIRE(NEAR(n2, n * n));
}

////////////////////////////////////////////////////////////////////////////////
// Conjugate and Inverse Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(quat_conjugate_negates_vector_part) {
    struct bj_vec4 q = {BJ_F(1.0), BJ_F(2.0), BJ_F(3.0), BJ_F(4.0)};
    struct bj_vec4 c = bj_quat_conjugate(q);
    REQUIRE(QUAT_NEAR(c, -q.x, -q.y, -q.z, q.w));
}

TEST_CASE(quat_conjugate_of_unit_is_inverse) {
    struct bj_vec4 q = bj_quat_normalize((struct bj_vec4){BJ_F(1.0), BJ_F(2.0), BJ_F(3.0), BJ_F(4.0)});
    struct bj_vec4 c = bj_quat_conjugate(q);
    struct bj_vec4 inv = bj_quat_inverse(q);
    REQUIRE(QUAT_NEAR(c, inv.x, inv.y, inv.z, inv.w));
}

TEST_CASE(quat_q_mul_inverse_is_identity) {
    struct bj_vec4 q = bj_quat_normalize((struct bj_vec4){BJ_F(0.5), BJ_F(0.5), BJ_F(0.5), BJ_F(0.5)});
    struct bj_vec4 inv = bj_quat_inverse(q);
    struct bj_vec4 r = bj_quat_mul(q, inv);

    // Should be close to identity
    REQUIRE(QUAT_NEAR(r, BJ_FZERO, BJ_FZERO, BJ_FZERO, BJ_F(1.0)));
}

TEST_CASE(quat_inverse_of_identity_is_identity) {
    struct bj_vec4 id = bj_quat_identity();
    struct bj_vec4 inv = bj_quat_inverse(id);
    REQUIRE(QUAT_NEAR(inv, BJ_FZERO, BJ_FZERO, BJ_FZERO, BJ_F(1.0)));
}

TEST_CASE(quat_inverse_of_zero_returns_identity) {
    struct bj_vec4 zero = {BJ_FZERO, BJ_FZERO, BJ_FZERO, BJ_FZERO};
    struct bj_vec4 inv = bj_quat_inverse(zero);
    REQUIRE(QUAT_NEAR(inv, BJ_FZERO, BJ_FZERO, BJ_FZERO, BJ_F(1.0)));
}

////////////////////////////////////////////////////////////////////////////////
// Rotation Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(quat_90deg_around_z_rotates_x_to_y) {
    // 90 degrees around Z axis
    struct bj_vec3 axis = {BJ_FZERO, BJ_FZERO, BJ_F(1.0)};
    struct bj_vec4 q = bj_quat_from_axis_angle(axis, BJ_PI / BJ_F(2.0));

    struct bj_vec3 x = {BJ_F(1.0), BJ_FZERO, BJ_FZERO};
    struct bj_vec3 r = bj_quat_rotate_vec3(q, x);

    // X should become Y after 90 deg rotation around Z
    REQUIRE(VEC3_NEAR(r, BJ_FZERO, BJ_F(1.0), BJ_FZERO));
}

TEST_CASE(quat_180deg_rotation_twice_is_identity) {
    struct bj_vec3 axis = {BJ_F(1.0), BJ_FZERO, BJ_FZERO};
    struct bj_vec4 q180 = bj_quat_from_axis_angle(axis, BJ_PI);

    // Apply 180 deg rotation twice
    struct bj_vec4 q360 = bj_quat_mul(q180, q180);

    // Should be equivalent to identity (or negative identity, both represent same rotation)
    struct bj_vec3 v = {BJ_F(0.0), BJ_F(1.0), BJ_F(2.0)};
    struct bj_vec3 r = bj_quat_rotate_vec3(q360, v);
    REQUIRE(VEC3_NEAR(r, v.x, v.y, v.z));
}

TEST_CASE(quat_from_axis_angle_zero_axis_returns_identity) {
    struct bj_vec3 zero_axis = {BJ_FZERO, BJ_FZERO, BJ_FZERO};
    struct bj_vec4 q = bj_quat_from_axis_angle(zero_axis, BJ_PI);
    REQUIRE(QUAT_NEAR(q, BJ_FZERO, BJ_FZERO, BJ_FZERO, BJ_F(1.0)));
}

TEST_CASE(quat_rotate_then_inverse_restores) {
    struct bj_vec3 axis = {BJ_F(1.0), BJ_F(1.0), BJ_F(1.0)};
    struct bj_vec4 q = bj_quat_from_axis_angle(axis, BJ_F(1.23));
    struct bj_vec4 inv = bj_quat_inverse(q);

    struct bj_vec3 v = {BJ_F(1.0), BJ_F(2.0), BJ_F(3.0)};
    struct bj_vec3 rotated = bj_quat_rotate_vec3(q, v);
    struct bj_vec3 restored = bj_quat_rotate_vec3(inv, rotated);

    REQUIRE(VEC3_NEAR(restored, v.x, v.y, v.z));
}

TEST_CASE(quat_rotate_vec4_preserves_w) {
    struct bj_vec3 axis = {BJ_FZERO, BJ_FZERO, BJ_F(1.0)};
    struct bj_vec4 q = bj_quat_from_axis_angle(axis, BJ_PI / BJ_F(2.0));

    struct bj_vec4 v = {BJ_F(1.0), BJ_FZERO, BJ_FZERO, BJ_F(42.0)};
    struct bj_vec4 r = bj_quat_rotate_vec4(q, v);

    // w should be preserved
    REQUIRE(NEAR(r.w, BJ_F(42.0)));
    // xyz should be rotated
    REQUIRE(NEAR(r.x, BJ_FZERO));
    REQUIRE(NEAR(r.y, BJ_F(1.0)));
}

////////////////////////////////////////////////////////////////////////////////
// Slerp Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(quat_slerp_t0_returns_start) {
    struct bj_vec4 a = bj_quat_identity();
    struct bj_vec3 axis = {BJ_FZERO, BJ_F(1.0), BJ_FZERO};
    struct bj_vec4 b = bj_quat_from_axis_angle(axis, BJ_PI / BJ_F(2.0));

    struct bj_vec4 r = bj_quat_slerp(a, b, BJ_FZERO);
    REQUIRE(QUAT_NEAR(r, a.x, a.y, a.z, a.w));
}

TEST_CASE(quat_slerp_t1_returns_end) {
    struct bj_vec4 a = bj_quat_identity();
    struct bj_vec3 axis = {BJ_FZERO, BJ_F(1.0), BJ_FZERO};
    struct bj_vec4 b = bj_quat_from_axis_angle(axis, BJ_PI / BJ_F(2.0));

    struct bj_vec4 r = bj_quat_slerp(a, b, BJ_F(1.0));
    REQUIRE(QUAT_NEAR(r, b.x, b.y, b.z, b.w));
}

TEST_CASE(quat_slerp_t05_is_halfway) {
    struct bj_vec4 a = bj_quat_identity();
    struct bj_vec3 axis = {BJ_FZERO, BJ_F(1.0), BJ_FZERO};
    struct bj_vec4 b = bj_quat_from_axis_angle(axis, BJ_PI / BJ_F(2.0)); // 90 deg

    struct bj_vec4 mid = bj_quat_slerp(a, b, BJ_F(0.5));

    // Halfway between 0 and 90 deg should be 45 deg
    struct bj_vec4 expected = bj_quat_from_axis_angle(axis, BJ_PI / BJ_F(4.0));
    REQUIRE(QUAT_NEAR(mid, expected.x, expected.y, expected.z, expected.w));
}

TEST_CASE(quat_slerp_identical_quaternions) {
    struct bj_vec4 q = bj_quat_normalize((struct bj_vec4){BJ_F(0.5), BJ_F(0.5), BJ_F(0.5), BJ_F(0.5)});
    struct bj_vec4 r = bj_quat_slerp(q, q, BJ_F(0.5));
    REQUIRE(QUAT_NEAR(r, q.x, q.y, q.z, q.w));
}

////////////////////////////////////////////////////////////////////////////////
// Matrix Conversion Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(quat_to_mat4_identity_is_identity_matrix) {
    struct bj_vec4 id = bj_quat_identity();
    struct bj_mat4x4 m;
    bj_quat_to_mat4(&m, id);

    // Check diagonal is 1, rest is 0
    REQUIRE(NEAR(m.m[BJ_M4(0,0)], BJ_F(1.0)));
    REQUIRE(NEAR(m.m[BJ_M4(1,1)], BJ_F(1.0)));
    REQUIRE(NEAR(m.m[BJ_M4(2,2)], BJ_F(1.0)));
    REQUIRE(NEAR(m.m[BJ_M4(3,3)], BJ_F(1.0)));
    REQUIRE(NEAR(m.m[BJ_M4(0,1)], BJ_FZERO));
    REQUIRE(NEAR(m.m[BJ_M4(1,0)], BJ_FZERO));
}

TEST_CASE(quat_to_mat4_from_mat4_roundtrip) {
    struct bj_vec3 axis = {BJ_F(1.0), BJ_F(1.0), BJ_F(1.0)};
    struct bj_vec4 original = bj_quat_from_axis_angle(axis, BJ_F(1.0));

    struct bj_mat4x4 m;
    bj_quat_to_mat4(&m, original);
    struct bj_vec4 restored = bj_quat_from_mat4(&m);

    // Quaternions may differ by sign but represent same rotation
    // Check that they rotate a vector the same way
    struct bj_vec3 v = {BJ_F(1.0), BJ_F(2.0), BJ_F(3.0)};
    struct bj_vec3 r1 = bj_quat_rotate_vec3(original, v);
    struct bj_vec3 r2 = bj_quat_rotate_vec3(restored, v);

    REQUIRE(VEC3_NEAR(r1, r2.x, r2.y, r2.z));
}

TEST_CASE(quat_mat4_rotation_matches_quat_rotation) {
    struct bj_vec3 axis = {BJ_FZERO, BJ_F(1.0), BJ_FZERO};
    struct bj_vec4 q = bj_quat_from_axis_angle(axis, BJ_PI / BJ_F(3.0));

    struct bj_mat4x4 m;
    bj_quat_to_mat4(&m, q);

    struct bj_vec3 v = {BJ_F(1.0), BJ_FZERO, BJ_FZERO};

    // Rotate with quaternion
    struct bj_vec3 rq = bj_quat_rotate_vec3(q, v);

    // Rotate with matrix (apply m to v as column vector)
    bj_real rx = m.m[BJ_M4(0,0)] * v.x + m.m[BJ_M4(1,0)] * v.y + m.m[BJ_M4(2,0)] * v.z;
    bj_real ry = m.m[BJ_M4(0,1)] * v.x + m.m[BJ_M4(1,1)] * v.y + m.m[BJ_M4(2,1)] * v.z;
    bj_real rz = m.m[BJ_M4(0,2)] * v.x + m.m[BJ_M4(1,2)] * v.y + m.m[BJ_M4(2,2)] * v.z;

    REQUIRE(NEAR(rq.x, rx));
    REQUIRE(NEAR(rq.y, ry));
    REQUIRE(NEAR(rq.z, rz));
}

////////////////////////////////////////////////////////////////////////////////
// Dot Product Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(quat_dot_self_equals_norm2) {
    struct bj_vec4 q = {BJ_F(1.0), BJ_F(2.0), BJ_F(3.0), BJ_F(4.0)};
    REQUIRE(NEAR(bj_quat_dot(q, q), bj_quat_norm2(q)));
}

TEST_CASE(quat_dot_is_commutative) {
    struct bj_vec4 a = {BJ_F(1.0), BJ_F(2.0), BJ_F(3.0), BJ_F(4.0)};
    struct bj_vec4 b = {BJ_F(5.0), BJ_F(6.0), BJ_F(7.0), BJ_F(8.0)};
    REQUIRE(NEAR(bj_quat_dot(a, b), bj_quat_dot(b, a)));
}

int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    // Identity
    RUN_TEST(quat_identity_is_0_0_0_1);
    RUN_TEST(quat_identity_mul_q_equals_q);
    RUN_TEST(quat_q_mul_identity_equals_q);
    RUN_TEST(quat_identity_rotates_nothing);

    // Norm and normalize
    RUN_TEST(quat_norm_of_identity_is_one);
    RUN_TEST(quat_normalize_produces_unit_length);
    RUN_TEST(quat_normalize_zero_returns_identity);
    RUN_TEST(quat_norm2_is_squared_norm);

    // Conjugate and inverse
    RUN_TEST(quat_conjugate_negates_vector_part);
    RUN_TEST(quat_conjugate_of_unit_is_inverse);
    RUN_TEST(quat_q_mul_inverse_is_identity);
    RUN_TEST(quat_inverse_of_identity_is_identity);
    RUN_TEST(quat_inverse_of_zero_returns_identity);

    // Rotation
    RUN_TEST(quat_90deg_around_z_rotates_x_to_y);
    RUN_TEST(quat_180deg_rotation_twice_is_identity);
    RUN_TEST(quat_from_axis_angle_zero_axis_returns_identity);
    RUN_TEST(quat_rotate_then_inverse_restores);
    RUN_TEST(quat_rotate_vec4_preserves_w);

    // Slerp
    RUN_TEST(quat_slerp_t0_returns_start);
    RUN_TEST(quat_slerp_t1_returns_end);
    RUN_TEST(quat_slerp_t05_is_halfway);
    RUN_TEST(quat_slerp_identical_quaternions);

    // Matrix conversion
    RUN_TEST(quat_to_mat4_identity_is_identity_matrix);
    RUN_TEST(quat_to_mat4_from_mat4_roundtrip);
    RUN_TEST(quat_mat4_rotation_matches_quat_rotation);

    // Dot product
    RUN_TEST(quat_dot_self_equals_norm2);
    RUN_TEST(quat_dot_is_commutative);

    END_TESTS();
}
