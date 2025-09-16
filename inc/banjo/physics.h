#ifndef BJ_PHYSICS_H
#define BJ_PHYSICS_H

#include <banjo/math.h>
#include <banjo/vec.h>

////////////////////////////////////////////////////////////////////////////////
/// \file physics.h
/// Physics helpers (SI units, but dimensionally consistent with any unit system).
////////////////////////////////////////////////////////////////////////////////
/// \defgroup physics Physics
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
/// \def BJ_GRAVITATIONAL_CONSTANT_SI
/// \brief Newtonian constant of gravitation in SI units.
///
/// Defines the physical constant G = 6.67430 × 10⁻¹¹ m³·kg⁻¹·s⁻²,
/// expressed in the active \ref bj_real precision type.
///
/// \note Use only when working in SI units (meters, kilograms, seconds).
////////////////////////////////////////////////////////////////////////////////
#define BJ_GRAVITATIONAL_CONSTANT_SI BJ_F(6.67430e-11)


////////////////////////////////////////////////////////////////////////////////
/// \brief Galileo’s uniformly accelerated motion — position at time \a t.
///
/// Uses:
/// `x(t) = x0 + v0 * t + 0.5 * a * t^2`
///
/// \param position     Initial position [L]
/// \param velocity     Initial velocity [L T^-1]
/// \param acceleration Constant acceleration [L T^-2]
/// \param time         Elapsed time [T]
///
/// \return Position at time \a t, in [L].
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_real bj_galileo_position(
    bj_real position,
    bj_real velocity,
    bj_real acceleration,
    bj_real time
) {
    return BJ_F(0.5) * acceleration * time * time + velocity * time + position;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Galileo’s uniformly accelerated motion — velocity at time \a t.
///
/// Uses:
/// `v(t) = v0 + a * t`
///
/// \param velocity     Initial velocity [L T^-1]
/// \param acceleration Constant acceleration [L T^-2]
/// \param time         Elapsed time [T]
///
/// \return Velocity at time \a t, in [L T^-1].
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_real bj_galileo_velocity(
    bj_real velocity,
    bj_real acceleration,
    bj_real time
) {
    return acceleration * time + velocity;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Newton’s law of universal gravitation — force magnitude.
///
/// \f$ F = G\,\dfrac{m_1 m_2}{r^2} \f$
///
/// \param m1 Mass of first body [M]
/// \param m2 Mass of second body [M]
/// \param r  Separation distance [L]
/// \param g  Gravitational constant \a G [L^3 M^-1 T^-2]
///
/// \return Force magnitude [M L T^-2].
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_real bj_newton_gravitation(
    bj_real m1,
    bj_real m2,
    bj_real r,
    bj_real g
) {
    return g * (m1 * m2) / (r * r);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Newtonian gravitation with Plummer softening — force magnitude.
///
/// \f$ \|\mathbf{F}\| = G\,m_1 m_2 \dfrac{r}{(r^2+\varepsilon^2)^{3/2}} \f$
///
/// \param m1  Mass of first body [M]
/// \param m2  Mass of second body [M]
/// \param r   Separation distance [L]
/// \param g   Gravitational constant \a G [L^3 M^-1 T^-2]
/// \param eps Softening length \a ε [L]
///
/// \return Softened force magnitude [M L T^-2].
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_real bj_newton_plummer_gravitation(
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
