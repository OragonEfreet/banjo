#include "test.h"

#include <banjo/physics.h>

#include <math.h>
#include <string.h>

/* Helpers */
#define X_EXPECT(p,v,a,t)   (BJ_F(0.5)*(a)*(t)*(t) + (v)*(t) + (p))
#define V_EXPECT(v,a,t)     ((a)*(t) + (v))

/* --- Position: scalar --------------------------------------------------- */

TEST_CASE(kinematics_scalar_zero_time)
{
    bj_real p = BJ_F(5), v = BJ_F(3), a = BJ_F(2), t = BJ_F(0);
    bj_real x = bj_kinematics(p, v, a, t);
    REQUIRE_EQ(x, p);
}

TEST_CASE(kinematics_scalar_zero_accel)
{
    bj_real p = BJ_F(7), v = BJ_F(4), a = BJ_F(0), t = BJ_F(6);
    bj_real x = bj_kinematics(p, v, a, t);
    REQUIRE_EQ(x, p + v * t);
}

TEST_CASE(kinematics_scalar_positive_values)
{
    bj_real p = BJ_F(5), v = BJ_F(3), a = BJ_F(2), t = BJ_F(4);
    bj_real x = bj_kinematics(p, v, a, t);            // 0.5*2*16 + 3*4 + 5 = 33
    REQUIRE_EQ(x, BJ_F(33));
}

TEST_CASE(kinematics_scalar_negative_time)
{
    bj_real p = BJ_F(10), v = BJ_F(-2), a = BJ_F(4), t = BJ_F(-5);
    bj_real x = bj_kinematics(p, v, a, t);            // 0.5*4*25 + (-2)*(-5) + 10 = 50 + 10 + 10 = 70
    REQUIRE_EQ(x, BJ_F(70));
}

/* --- Position: array ---------------------------------------------------- */

TEST_CASE(kinematics_array_small)
{
    const size_t n = 5;
    bj_real p[5] = { BJ_F(1), BJ_F(2), BJ_F(3), BJ_F(4), BJ_F(5) };
    bj_real v[5] = { BJ_F(2), BJ_F(2), BJ_F(2), BJ_F(2), BJ_F(2) };
    bj_real a[5] = { BJ_F(2), BJ_F(4), BJ_F(6), BJ_F(8), BJ_F(10) }; // even a so 0.5*a*t^2 stays integral
    bj_real t = BJ_F(3);
    bj_real out[5] = {0};

    bj_kinematics_array(n, out, p, v, a, t);

    for (size_t i = 0; i < n; ++i) {
        bj_real expect = X_EXPECT(p[i], v[i], a[i], t); // 0.5*a*9 + 2*3 + p[i]
        REQUIRE_EQ(out[i], expect);
    }
}

TEST_CASE(kinematics_array_n_zero_no_write)
{
    const size_t n = 0;
    bj_real p[3] = { BJ_F(1), BJ_F(2), BJ_F(3) };
    bj_real v[3] = { BJ_F(4), BJ_F(5), BJ_F(6) };
    bj_real a[3] = { BJ_F(2), BJ_F(2), BJ_F(2) };
    bj_real out[3] = { BJ_F(7), BJ_F(8), BJ_F(9) };

    bj_kinematics_array(n, out, p, v, a, BJ_F(10));

    REQUIRE_EQ(out[0], BJ_F(7));
    REQUIRE_EQ(out[1], BJ_F(8));
    REQUIRE_EQ(out[2], BJ_F(9));
}

/* --- Position: 2D & 3D wrappers ---------------------------------------- */

TEST_CASE(kinematics_2d_basic_and_sentinel)
{
    bj_real p[3] = { BJ_F(5), BJ_F(6), BJ_F(777) };  // [0..1] used, [2] sentinel
    bj_real v[3] = { BJ_F(3), BJ_F(-1), BJ_F(888) };
    bj_real a[3] = { BJ_F(2), BJ_F(4), BJ_F(999) };
    bj_real out[3] = { BJ_F(0), BJ_F(0), BJ_F(12345) };
    bj_real t = BJ_F(4);

    bj_kinematics_2d(out, p, v, a, t);

    REQUIRE_EQ(out[0], X_EXPECT(p[0], v[0], a[0], t));
    REQUIRE_EQ(out[1], X_EXPECT(p[1], v[1], a[1], t));
    REQUIRE_EQ(out[2], BJ_F(12345)); // untouched sentinel
}

TEST_CASE(kinematics_3d_basic_and_sentinel)
{
    bj_real p[4] = { BJ_F(10), BJ_F(20), BJ_F(30), BJ_F(777) };
    bj_real v[4] = { BJ_F(-2), BJ_F(0),  BJ_F(5),  BJ_F(888) };
    bj_real a[4] = { BJ_F(4),  BJ_F(6),  BJ_F(2),  BJ_F(999) };
    bj_real out[4] = { BJ_F(0), BJ_F(0), BJ_F(0), BJ_F(4242) };
    bj_real t = BJ_F(5);

    bj_kinematics_3d(out, p, v, a, t);

    REQUIRE_EQ(out[0], X_EXPECT(p[0], v[0], a[0], t)); // 0.5*4*25 + (-2)*5 + 10 = 50 - 10 + 10 = 50
    REQUIRE_EQ(out[1], X_EXPECT(p[1], v[1], a[1], t)); // 0.5*6*25 + 0 + 20 = 75 + 20 = 95
    REQUIRE_EQ(out[2], X_EXPECT(p[2], v[2], a[2], t)); // 0.5*2*25 + 5*5 + 30 = 25 + 25 + 30 = 80
    REQUIRE_EQ(out[3], BJ_F(4242)); // untouched sentinel
}

/* Cross-consistency: x(t+Δ)-x(t) identity */
TEST_CASE(kinematics_array_delta_identity_dt1)
{
    /* (x(t+1) - x(t)) = v(t) * 1 + 0.5 * a * 1^2 */
    const size_t n = 4;
    bj_real p[4] = { BJ_F(0), BJ_F(1), BJ_F(2), BJ_F(3) };
    bj_real v[4] = { BJ_F(3), BJ_F(2), BJ_F(1), BJ_F(0) };
    bj_real a[4] = { BJ_F(2), BJ_F(4), BJ_F(6), BJ_F(8) }; // even
    bj_real t = BJ_F(7);
    bj_real x_t[4]   = {0};
    bj_real x_t1[4]  = {0};

    bj_kinematics_array(n, x_t,  p, v, a, t);
    bj_kinematics_array(n, x_t1, p, v, a, BJ_F(8));

    for (size_t i = 0; i < n; ++i) {
        bj_real diff = x_t1[i] - x_t[i];
        bj_real rhs  = V_EXPECT(v[i], a[i], t) + BJ_F(0.5) * a[i];
        REQUIRE_EQ(diff, rhs);
    }
}

/* --- Velocity: scalar --------------------------------------------------- */

TEST_CASE(kinematics_velocity_scalar_zero_time)
{
    bj_real v0 = BJ_F(7), a = BJ_F(3), t = BJ_F(0);
    bj_real v = bj_kinematics_velocity(v0, a, t);
    REQUIRE_EQ(v, v0);
}

TEST_CASE(kinematics_velocity_scalar_positive)
{
    bj_real v0 = BJ_F(-2), a = BJ_F(4), t = BJ_F(5);   // 4*5 - 2 = 18
    bj_real v = bj_kinematics_velocity(v0, a, t);
    REQUIRE_EQ(v, BJ_F(18));
}

TEST_CASE(kinematics_velocity_scalar_negative_time)
{
    bj_real v0 = BJ_F(3), a = BJ_F(2), t = BJ_F(-4);   // 2*(-4) + 3 = -5
    bj_real v = bj_kinematics_velocity(v0, a, t);
    REQUIRE_EQ(v, BJ_F(-5));
}

/* --- Velocity: array ---------------------------------------------------- */

TEST_CASE(kinematics_velocity_array_small)
{
    const size_t n = 6;
    bj_real v0[6] = { BJ_F(0), BJ_F(1), BJ_F(2), BJ_F(3), BJ_F(4), BJ_F(5) };
    bj_real a [6] = { BJ_F(2), BJ_F(2), BJ_F(2), BJ_F(2), BJ_F(2), BJ_F(2) };
    bj_real t = BJ_F(4);
    bj_real out[6] = {0};

    bj_kinematics_velocity_array(n, out, v0, a, t);

    for (size_t i = 0; i < n; ++i) {
        REQUIRE_EQ(out[i], V_EXPECT(v0[i], a[i], t)); // 2*4 + v0[i]
    }
}

TEST_CASE(kinematics_velocity_array_n_zero_no_write)
{
    const size_t n = 0;
    bj_real v0[2] = { BJ_F(1), BJ_F(2) };
    bj_real a [2] = { BJ_F(3), BJ_F(4) };
    bj_real out[2] = { BJ_F(11), BJ_F(22) };

    bj_kinematics_velocity_array(n, out, v0, a, BJ_F(10));

    REQUIRE_EQ(out[0], BJ_F(11));
    REQUIRE_EQ(out[1], BJ_F(22));
}

/* --- Velocity: 2D & 3D wrappers ---------------------------------------- */

TEST_CASE(kinematics_velocity_2d_basic_and_sentinel)
{
    bj_real v0[3] = { BJ_F(3), BJ_F(-1), BJ_F(777) };
    bj_real a [3] = { BJ_F(2), BJ_F(4),  BJ_F(888) };
    bj_real out[3] = { BJ_F(0), BJ_F(0), BJ_F(12345) };
    bj_real t = BJ_F(4);

    bj_kinematics_velocity_2d(out, v0, a, t);

    REQUIRE_EQ(out[0], V_EXPECT(v0[0], a[0], t));
    REQUIRE_EQ(out[1], V_EXPECT(v0[1], a[1], t));
    REQUIRE_EQ(out[2], BJ_F(12345)); // untouched sentinel
}

TEST_CASE(kinematics_velocity_3d_basic_and_sentinel)
{
    bj_real v0[4] = { BJ_F(-2), BJ_F(0), BJ_F(5), BJ_F(777) };
    bj_real a [4] = { BJ_F(4),  BJ_F(6), BJ_F(2), BJ_F(888) };
    bj_real out[4] = { BJ_F(0), BJ_F(0), BJ_F(0), BJ_F(4242) };
    bj_real t = BJ_F(5);

    bj_kinematics_velocity_3d(out, v0, a, t);

    REQUIRE_EQ(out[0], V_EXPECT(v0[0], a[0], t)); // 4*5 - 2 = 18
    REQUIRE_EQ(out[1], V_EXPECT(v0[1], a[1], t)); // 6*5 + 0 = 30
    REQUIRE_EQ(out[2], V_EXPECT(v0[2], a[2], t)); // 2*5 + 5 = 15
    REQUIRE_EQ(out[3], BJ_F(4242)); // untouched sentinel
}

/* Cross-consistency: v(t) relates to x(t+1)-x(t) - 0.5*a */
TEST_CASE(velocity_matches_position_difference_dt1)
{
    /* From x(t+Δ)-x(t) = v(t)Δ + 0.5 a Δ^2; with Δ=1, v(t) = x(t+1)-x(t) - 0.5 a */
    const size_t n = 5;
    bj_real p[5] = { BJ_F(0), BJ_F(1), BJ_F(2), BJ_F(3), BJ_F(4) };
    bj_real v0[5] = { BJ_F(1), BJ_F(2), BJ_F(3), BJ_F(4), BJ_F(5) };
    bj_real a [5] = { BJ_F(2), BJ_F(4), BJ_F(6), BJ_F(8), BJ_F(10) }; // even
    bj_real t = BJ_F(6);
    bj_real x_t[5]  = {0};
    bj_real x_t1[5] = {0};

    bj_kinematics_array(n, x_t,  p, v0, a, t);
    bj_kinematics_array(n, x_t1, p, v0, a, BJ_F(7));

    for (size_t i = 0; i < n; ++i) {
        bj_real v = bj_kinematics_velocity(v0[i], a[i], t);
        bj_real rhs = x_t1[i] - x_t[i] - BJ_F(0.5) * a[i];
        REQUIRE_EQ(v, rhs);
    }
}

/* --- Main: run them all ------------------------------------------------- */

int main(int argc, char** argv)
{
    BEGIN_TESTS(argc, argv);

    RUN_TEST(kinematics_scalar_zero_time);
    RUN_TEST(kinematics_scalar_zero_accel);
    RUN_TEST(kinematics_scalar_positive_values);
    RUN_TEST(kinematics_scalar_negative_time);

    RUN_TEST(kinematics_array_small);
    RUN_TEST(kinematics_array_n_zero_no_write);

    RUN_TEST(kinematics_2d_basic_and_sentinel);
    RUN_TEST(kinematics_3d_basic_and_sentinel);
    RUN_TEST(kinematics_array_delta_identity_dt1);

    RUN_TEST(kinematics_velocity_scalar_zero_time);
    RUN_TEST(kinematics_velocity_scalar_positive);
    RUN_TEST(kinematics_velocity_scalar_negative_time);

    RUN_TEST(kinematics_velocity_array_small);
    RUN_TEST(kinematics_velocity_array_n_zero_no_write);

    RUN_TEST(kinematics_velocity_2d_basic_and_sentinel);
    RUN_TEST(kinematics_velocity_3d_basic_and_sentinel);
    RUN_TEST(velocity_matches_position_difference_dt1);

    END_TESTS();
}
