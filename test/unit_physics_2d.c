#include "test.h"
#include <banjo/math.h>
#include <banjo/physics_2d.h>
#include <banjo/vec.h>

#define NEAR(a, b) (bj_abs((a) - (b)) < BJ_F(0.001))
#define VEC2_NEAR(v, ex, ey) (NEAR((v).x, ex) && NEAR((v).y, ey))

////////////////////////////////////////////////////////////////////////////////
// Particle 2D Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(particle_zero_init_is_valid) {
  struct bj_particle_2d p = {0};
  REQUIRE(VEC2_NEAR(p.position, BJ_FZERO, BJ_FZERO));
  REQUIRE(VEC2_NEAR(p.velocity, BJ_FZERO, BJ_FZERO));
}

TEST_CASE(particle_step_updates_position) {
  struct bj_particle_2d p = {0};
  p.velocity.x = BJ_F(1.0);
  p.velocity.y = BJ_F(2.0);
  p.inverse_mass = BJ_F(1.0);
  p.damping = BJ_F(1.0);

  bj_step_particle_2d(&p, BJ_F(1.0));

  REQUIRE(NEAR(p.position.x, BJ_F(1.0)));
  REQUIRE(NEAR(p.position.y, BJ_F(2.0)));
}

TEST_CASE(particle_step_with_acceleration) {
  struct bj_particle_2d p = {0};
  p.acceleration.y = BJ_F(-10.0); // Gravity
  p.inverse_mass = BJ_F(1.0);
  p.damping = BJ_F(1.0);

  bj_step_particle_2d(&p, BJ_F(1.0));

  REQUIRE(NEAR(p.velocity.y, BJ_F(-10.0)));
}

TEST_CASE(particle_step_clears_forces) {
  struct bj_particle_2d p = {0};
  p.forces.x = BJ_F(10.0);
  p.forces.y = BJ_F(20.0);
  p.inverse_mass = BJ_F(1.0);

  bj_step_particle_2d(&p, BJ_F(1.0));

  REQUIRE(VEC2_NEAR(p.forces, BJ_FZERO, BJ_FZERO));
}

TEST_CASE(particle_zero_inverse_mass_immovable) {
  struct bj_particle_2d p = {0};
  p.forces.x = BJ_F(100.0);
  p.inverse_mass = BJ_FZERO; // Infinite mass

  bj_step_particle_2d(&p, BJ_F(1.0));

  REQUIRE(VEC2_NEAR(p.velocity, BJ_FZERO, BJ_FZERO));
}

TEST_CASE(apply_force_accumulates) {
  struct bj_particle_2d p = {0};
  struct bj_vec2 f1 = {BJ_F(1.0), BJ_F(2.0)};
  struct bj_vec2 f2 = {BJ_F(3.0), BJ_F(4.0)};

  bj_apply_particle_force_2d(&p, f1);
  bj_apply_particle_force_2d(&p, f2);

  REQUIRE(NEAR(p.forces.x, BJ_F(4.0)));
  REQUIRE(NEAR(p.forces.y, BJ_F(6.0)));
}

TEST_CASE(point_gravity_attracts_particles) {
  struct bj_particle_2d p1 = {0};
  struct bj_particle_2d p2 = {0};

  p1.position.x = BJ_F(0.0);
  p2.position.x = BJ_F(10.0);
  p1.inverse_mass = BJ_F(1.0);
  p2.inverse_mass = BJ_F(1.0);

  bj_apply_point_gravity_2d(&p1, &p2, BJ_F(1.0));

  // Force should be positive (towards p2)
  REQUIRE(p1.forces.x > BJ_FZERO);
}

TEST_CASE(drag_opposes_motion) {
  struct bj_particle_2d p = {0};
  p.velocity.x = BJ_F(10.0);
  p.inverse_mass = BJ_F(1.0);

  bj_apply_drag_2d(&p, BJ_F(0.1), BJ_FZERO);

  // Drag force should be negative (opposing velocity)
  REQUIRE(p.forces.x < BJ_FZERO);
}

////////////////////////////////////////////////////////////////////////////////
// Angular 2D Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(angular_zero_init_is_valid) {
  struct bj_angular_2d a = {0};
  REQUIRE(NEAR(a.value, BJ_FZERO));
  REQUIRE(NEAR(a.velocity, BJ_FZERO));
}

TEST_CASE(angular_step_updates_angle) {
  struct bj_angular_2d a = {0};
  a.velocity = BJ_F(1.0); // 1 rad/s
  a.inverse_inertia = BJ_F(1.0);
  a.damping = BJ_F(1.0);

  bj_step_angular_2d(&a, BJ_F(1.0));

  REQUIRE(NEAR(a.value, BJ_F(1.0)));
}

TEST_CASE(angular_step_clears_torque) {
  struct bj_angular_2d a = {0};
  a.torque = BJ_F(5.0);
  a.inverse_inertia = BJ_F(1.0);

  bj_step_angular_2d(&a, BJ_F(1.0));

  REQUIRE(NEAR(a.torque, BJ_FZERO));
}

TEST_CASE(apply_torque_accumulates) {
  struct bj_angular_2d a = {0};

  bj_apply_angular_torque_2d(&a, BJ_F(5.0));
  bj_apply_angular_torque_2d(&a, BJ_F(3.0));

  REQUIRE(NEAR(a.torque, BJ_F(8.0)));
}

////////////////////////////////////////////////////////////////////////////////
// Rigid Body 2D Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(rigidbody_zero_init_is_valid) {
  struct bj_rigid_body_2d rb = {0};
  REQUIRE(VEC2_NEAR(rb.particle.position, BJ_FZERO, BJ_FZERO));
  REQUIRE(NEAR(rb.angular.value, BJ_FZERO));
}

TEST_CASE(rigidbody_step_updates_both_components) {
  struct bj_rigid_body_2d rb = {0};
  rb.particle.velocity.x = BJ_F(1.0);
  rb.particle.inverse_mass = BJ_F(1.0);
  rb.particle.damping = BJ_F(1.0);
  rb.angular.velocity = BJ_F(0.5);
  rb.angular.inverse_inertia = BJ_F(1.0);
  rb.angular.damping = BJ_F(1.0);

  bj_step_rigid_body_2d(&rb, 1.0);

  REQUIRE(NEAR(rb.particle.position.x, BJ_F(1.0)));
  REQUIRE(NEAR(rb.angular.value, BJ_F(0.5)));
}

TEST_CASE(rigidbody_apply_force) {
  struct bj_rigid_body_2d rb = {0};
  struct bj_vec2 force = {BJ_F(10.0), BJ_F(20.0)};

  bj_apply_rigidbody_force_2d(&rb, force);

  REQUIRE(NEAR(rb.particle.forces.x, BJ_F(10.0)));
  REQUIRE(NEAR(rb.particle.forces.y, BJ_F(20.0)));
}

////////////////////////////////////////////////////////////////////////////////
// Kinematics Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(kinematics_constant_velocity) {
  struct bj_vec2 pos = {BJ_F(0.0), BJ_F(0.0)};
  struct bj_vec2 vel = {BJ_F(5.0), BJ_F(10.0)};
  struct bj_vec2 acc = {BJ_FZERO, BJ_FZERO};

  struct bj_vec2 result = bj_compute_kinematics_2d(pos, vel, acc, BJ_F(2.0));

  REQUIRE(NEAR(result.x, BJ_F(10.0)));
  REQUIRE(NEAR(result.y, BJ_F(20.0)));
}

TEST_CASE(kinematics_with_acceleration) {
  struct bj_vec2 pos = {BJ_F(0.0), BJ_F(0.0)};
  struct bj_vec2 vel = {BJ_F(0.0), BJ_F(0.0)};
  struct bj_vec2 acc = {BJ_F(0.0), BJ_F(-10.0)}; // Gravity

  struct bj_vec2 result = bj_compute_kinematics_2d(pos, vel, acc, BJ_F(1.0));

  REQUIRE(NEAR(result.y, BJ_F(-5.0))); // 0.5 * -10 * 1^2
}

TEST_CASE(kinematics_velocity_with_acceleration) {
  struct bj_vec2 vel = {BJ_F(0.0), BJ_F(0.0)};
  struct bj_vec2 acc = {BJ_F(0.0), BJ_F(-10.0)};

  struct bj_vec2 result =
      bj_compute_kinematics_velocity_2d(vel, acc, BJ_F(2.0));

  REQUIRE(NEAR(result.y, BJ_F(-20.0)));
}

int main(int argc, char *argv[]) {
  BEGIN_TESTS(argc, argv);

  // Particle tests
  RUN_TEST(particle_zero_init_is_valid);
  RUN_TEST(particle_step_updates_position);
  RUN_TEST(particle_step_with_acceleration);
  RUN_TEST(particle_step_clears_forces);
  RUN_TEST(particle_zero_inverse_mass_immovable);
  RUN_TEST(apply_force_accumulates);
  RUN_TEST(point_gravity_attracts_particles);
  RUN_TEST(drag_opposes_motion);

  // Angular tests
  RUN_TEST(angular_zero_init_is_valid);
  RUN_TEST(angular_step_updates_angle);
  RUN_TEST(angular_step_clears_torque);
  RUN_TEST(apply_torque_accumulates);

  // Rigid body tests
  RUN_TEST(rigidbody_zero_init_is_valid);
  RUN_TEST(rigidbody_step_updates_both_components);
  RUN_TEST(rigidbody_apply_force);

  // Kinematics tests
  RUN_TEST(kinematics_constant_velocity);
  RUN_TEST(kinematics_with_acceleration);
  RUN_TEST(kinematics_velocity_with_acceleration);

  END_TESTS();
}
