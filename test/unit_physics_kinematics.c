#include "test.h"

#include <banjo/physics.h>
#include <banjo/physics_2d.h>

#include <math.h>
#include <string.h>

/* Helpers */
#define X_EXPECT(p,v,a,t)   (BJ_F(0.5)*(a)*(t)*(t) + (v)*(t) + (p))
#define V_EXPECT(v,a,t)     ((a)*(t) + (v))

/* --- Position: scalar --------------------------------------------------- */

TEST_CASE(kinematics_scalar_zero_time)
{
    bj_real p = BJ_F(5.0), v = BJ_F(3.0), a = BJ_F(2.0), t = BJ_FZERO;
    bj_real x = bj_galileo_position(p, v, a, t);
    REQUIRE_EQ(x, p);
}

TEST_CASE(kinematics_scalar_zero_accel)
{
    bj_real p = BJ_F(7.0), v = BJ_F(4.0), a = BJ_FZERO, t = BJ_F(6.0);
    bj_real x = bj_galileo_position(p, v, a, t);
    REQUIRE_EQ(x, p + v * t);
}

TEST_CASE(kinematics_scalar_positive_values)
{
    bj_real p = BJ_F(5.0), v = BJ_F(3.0), a = BJ_F(2.0), t = BJ_F(4.0);
    bj_real x = bj_galileo_position(p, v, a, t);            // 0.5*2*16 + 3*4 + 5 = 33
    REQUIRE_EQ(x, BJ_F(33.0));
}

TEST_CASE(kinematics_scalar_negative_time)
{
    bj_real p = BJ_F(10.0), v = BJ_F(-2.0), a = BJ_F(4.0), t = BJ_F(-5.0);
    bj_real x = bj_galileo_position(p, v, a, t);            // 0.5*4*25 + (-2)*(-5) + 10 = 50 + 10 + 10 = 70
    REQUIRE_EQ(x, BJ_F(70.0));
}

/* --- Velocity: scalar --------------------------------------------------- */

TEST_CASE(kinematics_velocity_scalar_zero_time)
{
    bj_real v0 = BJ_F(7.0), a = BJ_F(3.0), t = BJ_FZERO;
    bj_real v = bj_galileo_velocity(v0, a, t);
    REQUIRE_EQ(v, v0);
}

TEST_CASE(kinematics_velocity_scalar_positive)
{
    bj_real v0 = BJ_F(-2.0), a = BJ_F(4.0), t = BJ_F(5.0);   // 4*5 - 2 = 18
    bj_real v = bj_galileo_velocity(v0, a, t);
    REQUIRE_EQ(v, BJ_F(18.0));
}

TEST_CASE(kinematics_velocity_scalar_negative_time)
{
    bj_real v0 = BJ_F(3.0), a = BJ_F(2.0), t = BJ_F(-4.0);   // 2*(-4) + 3 = -5
    bj_real v = bj_galileo_velocity(v0, a, t);
    REQUIRE_EQ(v, BJ_F(-5.0));
}


/* --- Main: run them all ------------------------------------------------- */

int main(int argc, char** argv)
{
    BEGIN_TESTS(argc, argv);

    RUN_TEST(kinematics_scalar_zero_time);
    RUN_TEST(kinematics_scalar_zero_accel);
    RUN_TEST(kinematics_scalar_positive_values);
    RUN_TEST(kinematics_scalar_negative_time);



    RUN_TEST(kinematics_velocity_scalar_zero_time);
    RUN_TEST(kinematics_velocity_scalar_positive);
    RUN_TEST(kinematics_velocity_scalar_negative_time);



    END_TESTS();
}
