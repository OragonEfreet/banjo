////////////////////////////////////////////////////////////////////////////////
/// \file math.h
/// C99 math shim with bj_real precision type and scalar utilities.
////////////////////////////////////////////////////////////////////////////////
/// \defgroup math Math
///
/// \brief Math utilities (precision abstraction, constants, scalar functions).
///
/// This header provides:
/// - Compile-time selection of real precision (\ref bj_real).
/// - Constants such as PI and TAU in multiple precisions.
/// - Typed wrappers for C math functions.
/// - Generic bj_* math macros mapped to the selected \ref bj_real type.
/// - Scalar helpers: clamp, step, smoothstep, fract, modulus.
/// - Floating-point comparison utilities (absolute and relative epsilon).
/// - Zero tests and safe normalization.
///
/// All functions are thin wrappers around <math.h> with consistent naming and
/// precision handling. They are dimensionless utilities: values are treated
/// as pure scalars without physical units.
///
/// \{
////////////////////////////////////////////////////////////////////////////////
#ifndef BJ_MATH_H
#define BJ_MATH_H

#include <math.h>
#include <float.h>

////////////////////////////////////////////////////////////////////////////////
/// \name Real type selection and helpers
/// @{
////////////////////////////////////////////////////////////////////////////////

#if defined(BJ_API_LONG_DOUBLE)
    /// \brief Selected real type for long double configuration.
    typedef long double bj_real;
    /// \brief Literal suffix helper for bj_real when long double is selected.
    #define BJ_F(x) x##L
    /// \brief Machine epsilon for bj_real when long double is selected.
    #define BJ_EPSILON (LDBL_EPSILON)
#elif defined(BJ_API_FLOAT64)
    /// \brief Selected real type for double configuration.
    typedef double bj_real;
    /// \brief Literal suffix helper for bj_real when double is selected.
    #define BJ_F(x) x
    /// \brief Machine epsilon for bj_real when double is selected.
    #define BJ_EPSILON (DBL_EPSILON)
#else
    /// \brief Selected real type for float configuration.
    typedef float bj_real;
    /// \brief Literal suffix helper for bj_real when float is selected.
    #define BJ_F(x) x##f
    /// \brief Machine epsilon for bj_real when float is selected.
    #define BJ_EPSILON (FLT_EPSILON)
#endif

/// \brief Convenience reciprocal literal generator.
/// \param x Positive scalar literal
/// \return 1.0 / x in bj_real
#define BJ_FI(x) BJ_F(1.0) / BJ_F(x)

/// \brief Zero constant in bj_real.
#define BJ_FZERO (BJ_F(0.0))

/// @}

////////////////////////////////////////////////////////////////////////////////
/// \name Circle constants
/// @{
////////////////////////////////////////////////////////////////////////////////

/// \brief Single-precision PI.
#define BJ_PI_F   (3.14159265358979323846f)
/// \brief Single-precision TAU (2 * PI).
#define BJ_TAU_F  (6.28318530717958647692f)
/// \brief Double-precision PI.
#define BJ_PI_D   (3.14159265358979323846264338327950288)
/// \brief Double-precision TAU (2 * PI).
#define BJ_TAU_D  (6.28318530717958647692528676655900576)
/// \brief Long-double PI.
#define BJ_PI_L   (3.141592653589793238462643383279502884L)
/// \brief Long-double TAU (2 * PI).
#define BJ_TAU_L  (6.283185307179586476925286766559005768L)
/// \brief PI in the selected bj_real precision.
#define BJ_PI     (BJ_F(3.141592653589793238462643383279502884))
/// \brief TAU in the selected bj_real precision.
#define BJ_TAU    (BJ_F(6.283185307179586476925286766559005768))

/// @}

////////////////////////////////////////////////////////////////////////////////
/// \name Typed wrappers (float)
/// \brief Thin aliases to <math.h> float functions.
/// @{
////////////////////////////////////////////////////////////////////////////////

#define bj_absf       fabsf      ///< Absolute value (float)
#define bj_acosf      acosf      ///< Arc cosine (float)
#define bj_atan2f     atan2f     ///< Arc tangent (float)
#define bj_copysignf  copysignf  ///< Copy sign (float)
#define bj_cosf       cosf       ///< Cosine (float)
#define bj_expf       expf       ///< Exponential (float)
#define bj_floorf     floorf     ///< Floor (float)
#define bj_fmodf      fmodf      ///< Floating modulus (float)
#define bj_logf       logf       ///< Natural log (float)
#define bj_maxf       fmaxf      ///< Maximum of two floats
#define bj_minf       fminf      ///< Minimum of two floats
#define bj_powf       powf       ///< Power (float)
#define bj_roundf     roundf     ///< Round to nearest (float)
#define bj_sinf       sinf       ///< Sine (float)
#define bj_sqrtf      sqrtf      ///< Square root (float)
#define bj_tanf       tanf       ///< Tangent (float)

/// @}

////////////////////////////////////////////////////////////////////////////////
/// \name Typed wrappers (double)
/// \brief Thin aliases to <math.h> double functions.
/// @{
////////////////////////////////////////////////////////////////////////////////

#define bj_absd       fabs       ///< Absolute value (double)
#define bj_acosd      acos       ///< Arc cosine (double)
#define bj_atan2d     atan2      ///< Arc tangent (float)
#define bj_copysignd  copysign   ///< Copy sign (double)
#define bj_cosd       cos        ///< Cosine (double)
#define bj_expd       exp        ///< Exponential (double)
#define bj_floord     floor      ///< Floor (double)
#define bj_fmodd      fmod       ///< Floating modulus (double)
#define bj_logd       log        ///< Natural log (double)
#define bj_maxd       fmax       ///< Maximum of two doubles
#define bj_mind       fmin       ///< Minimum of two doubles
#define bj_powd       pow        ///< Power (double)
#define bj_roundd     round      ///< Round to nearest (double)
#define bj_sind       sin        ///< Sine (double)
#define bj_sqrtd      sqrt       ///< Square root (double)
#define bj_tand       tan        ///< Tangent (double)

/// @}

////////////////////////////////////////////////////////////////////////////////
/// \name Typed wrappers (long double)
/// \brief Thin aliases to <math.h> long double functions.
/// @{
////////////////////////////////////////////////////////////////////////////////

#define bj_absl       fabsl      ///< Absolute value (long double)
#define bj_acosl      acosl      ///< Arc cosine (long double)
#define bj_atan2l     atan2l     ///< Arc tangent (float)
#define bj_copysignl  copysignl  ///< Copy sign (long double)
#define bj_cosl       cosl       ///< Cosine (long double)
#define bj_expl       expl       ///< Exponential (long double)
#define bj_floorl     floorl     ///< Floor (long double)
#define bj_fmodl      fmodl      ///< Floating modulus (long double)
#define bj_logl       logl       ///< Natural log (long double)
#define bj_maxl       fmaxl      ///< Maximum of two long doubles
#define bj_minl       fminl      ///< Minimum of two long doubles
#define bj_powl       powl       ///< Power (long double)
#define bj_roundl     roundl     ///< Round to nearest (long double)
#define bj_sinl       sinl       ///< Sine (long double)
#define bj_sqrtl      sqrtl      ///< Square root (long double)
#define bj_tanl       tanl       ///< Tangent (long double)

/// @}

////////////////////////////////////////////////////////////////////////////////
/// \name Precision-dispatch to match bj_real
/// \brief Map generic bj_* names to the active precision.
/// @{
////////////////////////////////////////////////////////////////////////////////

#if defined(BJ_API_LONG_DOUBLE)
    #define bj_abs       bj_absl
    #define bj_acos      bj_acosl
    #define bj_atan2     bj_atan2l
    #define bj_copysign  bj_copysignl
    #define bj_cos       bj_cosl
    #define bj_exp       bj_expl
    #define bj_floor     bj_floorl
    #define bj_fmod      bj_fmodl
    #define bj_log       bj_logl
    #define bj_max       bj_maxl
    #define bj_min       bj_minl
    #define bj_pow       bj_powl
    #define bj_round     bj_roundl
    #define bj_sin       bj_sinl
    #define bj_sqrt      bj_sqrtl
    #define bj_tan       bj_tanl
#elif defined(BJ_API_FLOAT64)
    #define bj_abs       bj_absd
    #define bj_acos      bj_acosd
    #define bj_atan2     bj_atan2d
    #define bj_copysign  bj_copysignd
    #define bj_cos       bj_cosd
    #define bj_exp       bj_expd
    #define bj_floor     bj_floord
    #define bj_fmod      bj_fmodd
    #define bj_log       bj_logd
    #define bj_max       bj_maxd
    #define bj_min       bj_mind
    #define bj_pow       bj_powd
    #define bj_round     bj_roundd
    #define bj_sin       bj_sind
    #define bj_sqrt      bj_sqrtd
    #define bj_tan       bj_tand
#else
    #define bj_abs       bj_absf      ///< Absolute value 
    #define bj_acos      bj_acosf     ///< Arc cosine 
    #define bj_atan2     bj_atan2f    ///< Arc tangent 
    #define bj_copysign  bj_copysignf ///< Copy sign 
    #define bj_cos       bj_cosf      ///< Cosine 
    #define bj_exp       bj_expf      ///< Exponential 
    #define bj_floor     bj_floorf    ///< Floor 
    #define bj_fmod      bj_fmodf     ///< Floating modulus 
    #define bj_log       bj_logf      ///< Natural logarithm 
    #define bj_max       bj_maxf      ///< Maximum of two floats
    #define bj_min       bj_minf      ///< Minimum of two floats
    #define bj_pow       bj_powf      ///< Power 
    #define bj_round     bj_roundf    ///< Round to nearest integer 
    #define bj_sin       bj_sinf      ///< Sine 
    #define bj_sqrt      bj_sqrtf     ///< Square root 
    #define bj_tan       bj_tanf      ///< Tangent 
#endif

/// @}

////////////////////////////////////////////////////////////////////////////////
/// \name Scalar utilities
/// \brief Generic scalar helpers. All arguments and results are bj_real.
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// \brief Clamp x to the closed interval [lo, hi].
/// \param x  Input value
/// \param lo Lower bound
/// \param hi Upper bound
/// \return lo if x < lo, hi if x > hi, else x
////////////////////////////////////////////////////////////////////////////////
static inline bj_real bj_clamp(bj_real x, bj_real lo, bj_real hi) {
    return (x < lo) ? lo : (x > hi) ? hi : x;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Step function.
/// \param edge Threshold edge
/// \param x    Input value
/// \return 0 if x < edge, else 1
////////////////////////////////////////////////////////////////////////////////
static inline bj_real bj_step(bj_real edge, bj_real x) {
    return (x < edge) ? BJ_FZERO : BJ_F(1.0);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Smooth Hermite step between e0 and e1. Clamps t to [0,1].
/// \param e0 Start edge
/// \param e1 End edge
/// \param x  Input value
/// \return Interpolant in [0,1]
////////////////////////////////////////////////////////////////////////////////
static inline bj_real bj_smoothstep(bj_real e0, bj_real e1, bj_real x) {
    bj_real t = (x - e0) / (e1 - e0);
    t = (t < BJ_FZERO) ? BJ_FZERO : (t > BJ_F(1.0)) ? BJ_F(1.0) : t;
    return t * t * (BJ_F(3.0) - BJ_F(2.0) * t);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Fractional part of x.
/// \param x Input value
/// \return x - floor(x)
////////////////////////////////////////////////////////////////////////////////
static inline bj_real bj_fract(bj_real x) {
    return x - bj_floor(x);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Positive modulus with non-negative result magnitude.
/// \param x Dividend
/// \param y Divisor (must be non-zero)
/// \return Remainder in [0, |y|) with the sign of y
/// \pre y != 0
////////////////////////////////////////////////////////////////////////////////
static inline bj_real bj_mod(bj_real x, bj_real y) {
    bj_real m = bj_fmod(x, y);
    if (m < BJ_FZERO) m += (y < BJ_FZERO) ? -y : y;
    return m;
}

/// @}

////////////////////////////////////////////////////////////////////////////////
/// \name Absolute-epsilon comparisons
/// \brief Comparisons using BJ_EPSILON as an absolute tolerance.
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// \brief Equality within absolute epsilon.
/// \param a First value
/// \param b Second value
/// \return Non-zero if |a - b| <= BJ_EPSILON
////////////////////////////////////////////////////////////////////////////////
static inline int  bj_real_eq (bj_real a, bj_real b) { return bj_abs(a - b) <= BJ_EPSILON; }

////////////////////////////////////////////////////////////////////////////////
/// \brief Inequality within absolute epsilon.
/// \param a First value
/// \param b Second value
/// \return Non-zero if not equal within absolute epsilon
////////////////////////////////////////////////////////////////////////////////
static inline int  bj_real_neq(bj_real a, bj_real b) { return !bj_real_eq(a, b); }

////////////////////////////////////////////////////////////////////////////////
/// \brief a < b by more than absolute epsilon.
/// \param a First value
/// \param b Second value
/// \return Non-zero if (b - a) > BJ_EPSILON
////////////////////////////////////////////////////////////////////////////////
static inline int  bj_real_lt (bj_real a, bj_real b) { return (b - a) >  BJ_EPSILON; }

////////////////////////////////////////////////////////////////////////////////
/// \brief a > b by more than absolute epsilon.
/// \param a First value
/// \param b Second value
/// \return Non-zero if (a - b) > BJ_EPSILON
////////////////////////////////////////////////////////////////////////////////
static inline int  bj_real_gt (bj_real a, bj_real b) { return (a - b) >  BJ_EPSILON; }

////////////////////////////////////////////////////////////////////////////////
/// \brief a <= b within absolute epsilon.
/// \param a First value
/// \param b Second value
/// \return Non-zero if a <= b within tolerance
////////////////////////////////////////////////////////////////////////////////
static inline int  bj_real_lte(bj_real a, bj_real b) { return !bj_real_gt(a, b); }

////////////////////////////////////////////////////////////////////////////////
/// \brief a >= b within absolute epsilon.
/// \param a First value
/// \param b Second value
/// \return Non-zero if a >= b within tolerance
////////////////////////////////////////////////////////////////////////////////
static inline int  bj_real_gte(bj_real a, bj_real b) { return !bj_real_lt(a, b); }

////////////////////////////////////////////////////////////////////////////////
/// \brief Three-way compare using absolute epsilon.
/// \param a First value
/// \param b Second value
/// \return -1 if a < b, 1 if a > b, 0 otherwise
////////////////////////////////////////////////////////////////////////////////
static inline int  bj_real_cmp(bj_real a, bj_real b) { return bj_real_lt(a,b) ? -1 : (bj_real_gt(a,b) ? 1 : 0); }

/// @}

////////////////////////////////////////////////////////////////////////////////
/// \name Relative-epsilon comparisons
/// \brief Comparisons using a scale-dependent tolerance.
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// \brief Internal scale helper max(1, |a|, |b|).
/// \param a First value
/// \param b Second value
/// \return Scale factor s >= 1
////////////////////////////////////////////////////////////////////////////////
static inline bj_real bj_real_relative_scale(bj_real a, bj_real b) {
    return bj_max(BJ_F(1.0), bj_max(bj_abs(a), bj_abs(b)));
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Equality within relative epsilon.
/// \param a First value
/// \param b Second value
/// \return Non-zero if |a - b| <= BJ_EPSILON * max(1, |a|, |b|)
////////////////////////////////////////////////////////////////////////////////
static inline int  bj_real_eq_rel (bj_real a, bj_real b) { bj_real s = bj_real_relative_scale(a,b); return bj_abs(a - b) <= (BJ_EPSILON * s); }

////////////////////////////////////////////////////////////////////////////////
/// \brief Inequality within relative epsilon.
/// \param a First value
/// \param b Second value
/// \return Non-zero if not equal within relative epsilon
////////////////////////////////////////////////////////////////////////////////
static inline int  bj_real_neq_rel(bj_real a, bj_real b) { return !bj_real_eq_rel(a, b); }

////////////////////////////////////////////////////////////////////////////////
/// \brief a < b by more than relative epsilon.
/// \param a First value
/// \param b Second value
/// \return Non-zero if (b - a) > BJ_EPSILON * max(1, |a|, |b|)
////////////////////////////////////////////////////////////////////////////////
static inline int  bj_real_lt_rel (bj_real a, bj_real b) { bj_real s = bj_real_relative_scale(a,b); return (b - a) >  (BJ_EPSILON * s); }

////////////////////////////////////////////////////////////////////////////////
/// \brief a > b by more than relative epsilon.
/// \param a First value
/// \param b Second value
/// \return Non-zero if (a - b) > BJ_EPSILON * max(1, |a|, |b|)
////////////////////////////////////////////////////////////////////////////////
static inline int  bj_real_gt_rel (bj_real a, bj_real b) { bj_real s = bj_real_relative_scale(a,b); return (a - b) >  (BJ_EPSILON * s); }

////////////////////////////////////////////////////////////////////////////////
/// \brief a <= b within relative epsilon.
/// \param a First value
/// \param b Second value
/// \return Non-zero if a <= b within tolerance
////////////////////////////////////////////////////////////////////////////////
static inline int  bj_real_lte_rel(bj_real a, bj_real b) { return !bj_real_gt_rel(a, b); }

////////////////////////////////////////////////////////////////////////////////
/// \brief a >= b within relative epsilon.
/// \param a First value
/// \param b Second value
/// \return Non-zero if a >= b within tolerance
////////////////////////////////////////////////////////////////////////////////
static inline int  bj_real_gte_rel(bj_real a, bj_real b) { return !bj_real_lt_rel(a, b); }

////////////////////////////////////////////////////////////////////////////////
/// \brief Three-way compare using relative epsilon.
/// \param a First value
/// \param b Second value
/// \return -1 if a < b, 1 if a > b, 0 otherwise
////////////////////////////////////////////////////////////////////////////////
static inline int  bj_real_cmp_rel(bj_real a, bj_real b) { return bj_real_lt_rel(a,b) ? -1 : (bj_real_gt_rel(a,b) ? 1 : 0); }

/// @}

////////////////////////////////////////////////////////////////////////////////
/// \name Zero tests and helpers
/// \brief Utilities for zero checks and safe normalization.
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// \brief Absolute-zero test.
/// \param x Input value
/// \return Non-zero if |x| <= BJ_EPSILON
////////////////////////////////////////////////////////////////////////////////
static inline int   bj_real_is_zero(bj_real x) { return bj_abs(x) <= BJ_EPSILON; }

////////////////////////////////////////////////////////////////////////////////
/// \brief Scaled zero test using max(1, |scale|).
/// \param x     Input value
/// \param scale Scale reference
/// \return Non-zero if |x| <= BJ_EPSILON * max(1, |scale|)
////////////////////////////////////////////////////////////////////////////////
static inline int   bj_real_is_zero_scaled(bj_real x, bj_real scale) {
    bj_real s = bj_max(BJ_F(1.0), bj_abs(scale));
    return bj_abs(x) <= (BJ_EPSILON * s);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Snap to exact zero under absolute epsilon.
/// \param x Input value
/// \return 0 if |x| <= BJ_EPSILON, else x
////////////////////////////////////////////////////////////////////////////////
static inline bj_real bj_real_snap_zero(bj_real x) { return bj_real_is_zero(x) ? BJ_FZERO : x; }

////////////////////////////////////////////////////////////////////////////////
/// \brief Safe scalar normalization.
/// \param x   Numerator
/// \param len Denominator magnitude
/// \return 0 if len is zero, else x / len
////////////////////////////////////////////////////////////////////////////////
static inline bj_real bj_real_snorm_safe(bj_real x, bj_real len) { return bj_real_is_zero(len) ? BJ_FZERO : (x / len); }

/// @}

////////////////////////////////////////////////////////////////////////////////
/// \}
////////////////////////////////////////////////////////////////////////////////

#endif /* BJ_MATH_H */
