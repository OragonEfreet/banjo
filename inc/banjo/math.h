////////////////////////////////////////////////////////////////////////////////
/// \file math.h
/// C99 math shim with bj_real precision type and scalar utilities.
////////////////////////////////////////////////////////////////////////////////
/// \defgroup math Math
/// \ingroup core
///
/// \brief Math utilities (precision abstraction, constants, scalar functions).
///
/// This header provides:
/// - Compile-time selection of real precision (\ref bj_real).
/// - Constants such as PI and TAU in multiple precisions.
/// - Typed wrappers for C math functions.
/// - Generic `bj_*` math macros mapped to the selected \ref bj_real type.
/// - Scalar helpers: clamp, step, smoothstep, fract, modulus.
/// - Floating-point comparison utilities (absolute and relative epsilon).
/// - Zero tests and safe normalization.
///
/// All functions are thin wrappers around `<math.h>` with consistent naming and
/// precision handling. They are **dimensionless** utilities: values are treated
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
    typedef long double bj_real;           ///< Selected real type
    #define BJ_F(x) x##L                   ///< Literal suffix helper
    #define BJ_EPSILON (LDBL_EPSILON)      ///< Epsilon for bj_real
#elif defined(BJ_API_FLOAT64)
    typedef double bj_real;
    #define BJ_F(x) x
    #define BJ_EPSILON (DBL_EPSILON)
#else
    typedef float bj_real;
    #define BJ_F(x) x##f
    #define BJ_EPSILON (FLT_EPSILON)
#endif
#define BJ_FI(x) BJ_F(1.0) / BJ_F(x)
#define BJ_FZERO (BJ_F(0.0))               ///< Zero constant in bj_real
/// @}

////////////////////////////////////////////////////////////////////////////////
/// \name Circle constants
/// @{
////////////////////////////////////////////////////////////////////////////////
#define BJ_PI_F   (3.14159265358979323846f)
#define BJ_TAU_F  (6.28318530717958647692f)
#define BJ_PI_D   (3.14159265358979323846264338327950288)
#define BJ_TAU_D  (6.28318530717958647692528676655900576)
#define BJ_PI_L   (3.141592653589793238462643383279502884L)
#define BJ_TAU_L  (6.283185307179586476925286766559005768L)
#define BJ_PI     (BJ_F(3.141592653589793238462643383279502884))
#define BJ_TAU    (BJ_F(6.283185307179586476925286766559005768))
/// @}

////////////////////////////////////////////////////////////////////////////////
/// \name Typed wrappers (float)
/// @{
////////////////////////////////////////////////////////////////////////////////
#define bj_absf       fabsf
#define bj_maxf       fmaxf
#define bj_minf       fminf
#define bj_floorf     floorf
#define bj_roundf     roundf
#define bj_sinf       sinf
#define bj_cosf       cosf
#define bj_tanf       tanf
#define bj_acosf      acosf
#define bj_expf       expf
#define bj_sqrtf      sqrtf
#define bj_powf       powf
#define bj_fmodf      fmodf
#define bj_copysignf  copysignf
/// @}

////////////////////////////////////////////////////////////////////////////////
/// \name Typed wrappers (double)
/// @{
////////////////////////////////////////////////////////////////////////////////
#define bj_absd       fabs
#define bj_maxd       fmax
#define bj_mind       fmin
#define bj_floord     floor
#define bj_roundd     round
#define bj_sind       sin
#define bj_cosd       cos
#define bj_tand       tan
#define bj_acosd      acos
#define bj_expd       exp
#define bj_sqrtd      sqrt
#define bj_powd       pow
#define bj_fmodd      fmod
#define bj_copysignd  copysign
/// @}

////////////////////////////////////////////////////////////////////////////////
/// \name Typed wrappers (long double)
/// @{
////////////////////////////////////////////////////////////////////////////////
#define bj_absl       fabsl
#define bj_maxl       fmaxl
#define bj_minl       fminl
#define bj_floorl     floorl
#define bj_roundl     roundl
#define bj_sinl       sinl
#define bj_cosl       cosl
#define bj_tanl       tanl
#define bj_acosl      acosl
#define bj_expl       expl
#define bj_sqrtl      sqrtl
#define bj_powl       powl
#define bj_fmodl      fmodl
#define bj_copysignl  copysignl
/// @}

////////////////////////////////////////////////////////////////////////////////
/// \name Precision-dispatch to match bj_real
/// @{
////////////////////////////////////////////////////////////////////////////////
#if defined(BJ_API_LONG_DOUBLE)
    #define bj_abs       bj_absl
    #define bj_max       bj_maxl
    #define bj_min       bj_minl
    #define bj_floor     bj_floorl
    #define bj_round     bj_roundl
    #define bj_sin       bj_sinl
    #define bj_cos       bj_cosl
    #define bj_tan       bj_tanl
    #define bj_acos      bj_acosl
    #define bj_exp       bj_expl
    #define bj_sqrt      bj_sqrtl
    #define bj_pow       bj_powl
    #define bj_fmod      bj_fmodl
    #define bj_copysign  bj_copysignl
#elif defined(BJ_API_FLOAT64)
    #define bj_abs       bj_absd
    #define bj_max       bj_maxd
    #define bj_min       bj_mind
    #define bj_floor     bj_floord
    #define bj_round     bj_roundd
    #define bj_sin       bj_sind
    #define bj_cos       bj_cosd
    #define bj_tan       bj_tand
    #define bj_acos      bj_acosd
    #define bj_exp       bj_expd
    #define bj_sqrt      bj_sqrtd
    #define bj_pow       bj_powd
    #define bj_fmod      bj_fmodd
    #define bj_copysign  bj_copysignd
#else
    #define bj_abs       bj_absf
    #define bj_max       bj_maxf
    #define bj_min       bj_minf
    #define bj_floor     bj_floorf
    #define bj_round     bj_roundf
    #define bj_sin       bj_sinf
    #define bj_cos       bj_cosf
    #define bj_tan       bj_tanf
    #define bj_acos      bj_acosf
    #define bj_exp       bj_expf
    #define bj_sqrt      bj_sqrtf
    #define bj_pow       bj_powf
    #define bj_fmod      bj_fmodf
    #define bj_copysign  bj_copysignf
#endif
/// @}

////////////////////////////////////////////////////////////////////////////////
/// \name Scalar utilities
/// @{
////////////////////////////////////////////////////////////////////////////////
static inline bj_real bj_clamp(bj_real x, bj_real lo, bj_real hi) {
    return (x < lo) ? lo : (x > hi) ? hi : x;
}

static inline bj_real bj_step(bj_real edge, bj_real x) {
    return (x < edge) ? BJ_FZERO : BJ_F(1.0);
}

static inline bj_real bj_smoothstep(bj_real e0, bj_real e1, bj_real x) {
    bj_real t = (x - e0) / (e1 - e0);
    t = (t < BJ_FZERO) ? BJ_FZERO : (t > BJ_F(1.0)) ? BJ_F(1.0) : t;
    return t * t * (BJ_F(3.0) - BJ_F(2.0) * t);
}

static inline bj_real bj_fract(bj_real x) {
    return x - bj_floor(x);
}

static inline bj_real bj_mod(bj_real x, bj_real y) {
    bj_real m = bj_fmod(x, y);
    if (m < BJ_FZERO) m += (y < BJ_FZERO) ? -y : y;
    return m;
}
/// @}

////////////////////////////////////////////////////////////////////////////////
/// \name Absolute-epsilon comparisons
/// @{
////////////////////////////////////////////////////////////////////////////////
static inline int  bj_real_eq (bj_real a, bj_real b) { return bj_abs(a - b) <= BJ_EPSILON; }
static inline int  bj_real_neq(bj_real a, bj_real b) { return !bj_real_eq(a, b); }
static inline int  bj_real_lt (bj_real a, bj_real b) { return (b - a) >  BJ_EPSILON; }
static inline int  bj_real_gt (bj_real a, bj_real b) { return (a - b) >  BJ_EPSILON; }
static inline int  bj_real_lte(bj_real a, bj_real b) { return !bj_real_gt(a, b); }
static inline int  bj_real_gte(bj_real a, bj_real b) { return !bj_real_lt(a, b); }
static inline int  bj_real_cmp(bj_real a, bj_real b) { return bj_real_lt(a,b) ? -1 : (bj_real_gt(a,b) ? 1 : 0); }
/// @}

////////////////////////////////////////////////////////////////////////////////
/// \name Relative-epsilon comparisons
/// @{
////////////////////////////////////////////////////////////////////////////////
static inline bj_real bj__rel_scale(bj_real a, bj_real b) {
    return bj_max(BJ_F(1.0), bj_max(bj_abs(a), bj_abs(b)));
}
static inline int  bj_real_eq_rel (bj_real a, bj_real b) { bj_real s = bj__rel_scale(a,b); return bj_abs(a - b) <= (BJ_EPSILON * s); }
static inline int  bj_real_neq_rel(bj_real a, bj_real b) { return !bj_real_eq_rel(a, b); }
static inline int  bj_real_lt_rel (bj_real a, bj_real b) { bj_real s = bj__rel_scale(a,b); return (b - a) >  (BJ_EPSILON * s); }
static inline int  bj_real_gt_rel (bj_real a, bj_real b) { bj_real s = bj__rel_scale(a,b); return (a - b) >  (BJ_EPSILON * s); }
static inline int  bj_real_lte_rel(bj_real a, bj_real b) { return !bj_real_gt_rel(a, b); }
static inline int  bj_real_gte_rel(bj_real a, bj_real b) { return !bj_real_lt_rel(a, b); }
static inline int  bj_real_cmp_rel(bj_real a, bj_real b) { return bj_real_lt_rel(a,b) ? -1 : (bj_real_gt_rel(a,b) ? 1 : 0); }
/// @}

////////////////////////////////////////////////////////////////////////////////
/// \name Zero tests and helpers
/// @{
////////////////////////////////////////////////////////////////////////////////
static inline int   bj_real_is_zero(bj_real x) { return bj_abs(x) <= BJ_EPSILON; }

static inline int   bj_real_is_zero_scaled(bj_real x, bj_real scale) {
    bj_real s = bj_max(BJ_F(1.0), bj_abs(scale));
    return bj_abs(x) <= (BJ_EPSILON * s);
}

static inline bj_real bj_real_snap_zero(bj_real x) { return bj_real_is_zero(x) ? BJ_FZERO : x; }

static inline bj_real bj_real_snorm_safe(bj_real x, bj_real len) { return bj_real_is_zero(len) ? BJ_FZERO : (x / len); }
/// @}

////////////////////////////////////////////////////////////////////////////////
/// \}
////////////////////////////////////////////////////////////////////////////////

#endif /* BJ_MATH_H */
