#ifndef BJ_PHYSICS_H
#define BJ_PHYSICS_H

#include <banjo/math.h>
#include <banjo/vec.h>

#define BJ_GRAVITATIONAL_CONSTANT_SI BJ_F(6.67430e-11)

////////////////////////////////////////////////////////////////////////////////
/// \file physics.h
/// Physics helpers (SI units, but dimensionally consistent with any unit system).
////////////////////////////////////////////////////////////////////////////////
/// \defgroup physics Physics
/// \ingroup core
///
/// \brief Basic physics utilities (constant-acceleration kinematics, etc.)
///
/// This header provides small helpers for common physics operations. By default,
/// quantities are interpreted in **SI units** (meters, seconds), but formulas are
/// **dimensionally homogeneous**: results are correct for **any consistent unit
/// system** (e.g., km and s; cm and s), provided all inputs use the same system.
///
/// Dimensionality uses the base dimensions
/// **L** (length) and **T** (time). For example:
/// - position: [L]
/// - velocity: [L T^-1]
/// - acceleration: [L T^-2]
/// - time: [T]
///
/// \{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// \brief Computes position at time \a t under constant acceleration.
///
/// Uses the standard constant-acceleration kinematics formula:
/// `x = 0.5 * a * t^2 + v0 * t + x0`
///
/// \param position     Initial position [L]
/// \param velocity     Initial velocity [L T^-1]
/// \param acceleration Constant acceleration [L T^-2]
/// \param time         Elapsed time [T]
///
/// \return Position at time \a t, in [L] in the chosen system.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_real bj_kinematics(
    bj_real position,
    bj_real velocity,
    bj_real acceleration,
    bj_real time
) {
    return BJ_F(0.5) * acceleration * time * time + velocity * time + position;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Computes velocity at time \a t under constant acceleration.
///
/// Uses: `v(t) = a * t + v0`
///
/// \param velocity     Initial velocity [L T^-1]
/// \param acceleration Constant acceleration [L T^-2]
/// \param time         Elapsed time [T]
///
/// \return Velocity at time \a t, in [L T^-1].
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_real bj_kinematics_velocity(
    bj_real velocity,
    bj_real acceleration,
    bj_real time
) {
    return acceleration * time + velocity;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Computes the Newtonian gravitational force magnitude between two masses.
///
/// Uses the standard law of universal gravitation:
/// \f[
///   F = G \cdot \frac{m_1 \cdot m_2}{r^2}
/// \f]
/// where:
/// - \a G is the gravitational constant
/// - \a m1 and \a m2 are the interacting masses
/// - \a r is the separation distance
///
/// \param m1 Mass of first body [M]
/// \param m2 Mass of second body [M]
/// \param r  Separation distance [L]
/// \param g  Gravitational constant \a G [L^3 M^-1 T^-2]
///
/// \return Gravitational force magnitude [M L T^-2].
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_real bj_gravity(
    bj_real m1,
    bj_real m2,
    bj_real r,
    bj_real g
) {
    return g * (m1 * m2) / (r * r);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Softened Newtonian gravitational force magnitude between two masses.
///
/// Plummer softening:
/// \f[
///   \|\mathbf{F}\| = G\,m_1 m_2 \frac{r}{\left(r^2 + \varepsilon^2\right)^{3/2}}
/// \f]
/// so that \f$\mathbf{F} = \|\mathbf{F}\|\,\hat{\mathbf{r}}\f$ equals
/// \f$G\,m_1 m_2\,\mathbf{r}/\left(r^2+\varepsilon^2\right)^{3/2}\f$.
///
/// \param m1  Mass of first body [M]
/// \param m2  Mass of second body [M]
/// \param r   Separation distance [L]
/// \param g   Gravitational constant \a G [L^3 M^-1 T^-2]
/// \param eps Softening length \a ε [L]
///
/// \return Softened force magnitude [M L T^-2].
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_real bj_gravity_softened(
    bj_real m1,
    bj_real m2,
    bj_real r,
    bj_real g,
    bj_real eps
) {
    const bj_real r2    = r * r;
    const bj_real e2    = eps * eps;
    const bj_real denom = bj_pow(r2 + e2, BJ_F(1.5));
    return (denom > BJ_FZERO) ? (g * m1 * m2 * r) / denom : BJ_FZERO;
}

/// \}

#endif
