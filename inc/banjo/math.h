////////////////////////////////////////////////////////////////////////////////
/// \file bj_math.h
/// \defgroup math Math
/// \ingroup core
///
/// \brief C99 math shim with macro forwarding and bj_real-only utilities.
///
/// Define BJ_USE_DOUBLE to make bj_real = double (else float).
/// Naming:
///   - bj_*   → operates on bj_real (auto-switch with BJ_USE_DOUBLE)
///   - bj_*f  → explicit float forwards (1:1 to <math.h>)
///   - bj_*d  → explicit double forwards (1:1 to <math.h>)
///
/// The Math group also provides usual facilities for linear math, including
/// - vector types: \ref bj_vec2, \ref bj_vec3 and \ref bj_vec4
/// - 3x3 matrix with \ref bj_mat3
/// - 3x2 matrix with \ref bj_mat3x2
/// - 4x4 matrix with \ref bj_mat4
/// - 4x3 matrix with \ref bj_mat4x3
/// - Quaternion with \ref bj_quat
///
/// This library is initially a direct adaptation of 
/// [linmath.h](https://github.com/datenwolf/linmath.h) 
/// by Wolfgang Draxinger, but since I added some new types.
///
///
/// Utilities (clamp/step/smoothstep/fract/mod) exist **only** as bj_real.
////////////////////////////////////////////////////////////////////////////////

#ifndef BJ_MATH_H
#define BJ_MATH_H

#include <banjo/api.h>

#define _USE_MATH_DEFINES 
#include <math.h>

/*-------------------------- Precision & literals ---------------------------*/

#define BJ_USE_DOUBLE
#ifdef BJ_USE_DOUBLE
    typedef double bj_real;
    #define BJ_F(x) x
    #define BJ_EPSILON  BJ_F(1e-12)
#else
    typedef float  bj_real;
    #define BJ_F(x) x##f
    #define BJ_EPSILON  BJ_F(1e-6)
#endif

/*-------------------------------- Constants --------------------------------*/
#define BJ_PI_F  3.14159265358979323846f
#define BJ_TAU_F 6.28318530717958647692f
#define BJ_PI_D  3.14159265358979323846264338327950288
#define BJ_TAU_D 6.28318530717958647692528676655900576
#define BJ_PI    BJ_F(3.14159265358979323846264338327950288)
#define BJ_TAU   BJ_F(6.28318530717958647692528676655900576)

/*======================= Direct forwards (macros) ==========================*/
/* Explicit float */
#define bj_absf       fabsf
#define bj_acosf      acosf
#define bj_copysignf  copysignf
#define bj_cosf       cosf
#define bj_expf       expf
#define bj_floorf     floorf
#define bj_fmodf      fmodf
#define bj_logf       logf
#define bj_maxf       fmaxf
#define bj_minf       fminf
#define bj_powf       powf
#define bj_roundf     roundf
#define bj_sinf       sinf
#define bj_sqrtf      sqrtf
#define bj_tanf       tanf

/* Explicit double */
#define bj_absd       fabs
#define bj_acosd      acos
#define bj_copysignd  copysign
#define bj_cosd       cos
#define bj_expd       exp
#define bj_floord     floor
#define bj_fmodd      fmod
#define bj_logd       log
#define bj_maxd       fmax
#define bj_mind       fmin
#define bj_powd       pow
#define bj_roundd     round
#define bj_sind       sin
#define bj_sqrtd      sqrt
#define bj_tand       tan

#ifdef BJ_USE_DOUBLE
    #define bj_abs       bj_absd
    #define bj_acos      bj_acosd
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
    #define bj_abs       bj_absf
    #define bj_acos      bj_acosf
    #define bj_copysign  bj_copysignf
    #define bj_cos       bj_cosf
    #define bj_exp       bj_expf
    #define bj_floor     bj_floorf
    #define bj_fmod      bj_fmodf
    #define bj_log       bj_logf
    #define bj_max       bj_maxf
    #define bj_min       bj_minf
    #define bj_pow       bj_powf
    #define bj_round     bj_roundf
    #define bj_sin       bj_sinf
    #define bj_sqrt      bj_sqrtf
    #define bj_tan       bj_tanf
#endif

static inline bj_real bj_clamp(bj_real x, bj_real lo, bj_real hi) {
    return (x < lo) ? lo : (x > hi) ? hi : x;
}

static inline bj_real bj_step(bj_real edge, bj_real x) {
    return (x < edge) ? BJ_F(0.0) : BJ_F(1.0);
}

static inline bj_real bj_smoothstep(bj_real e0, bj_real e1, bj_real x) {
    bj_real t = (x - e0) / (e1 - e0);
    t = (t < BJ_F(0.0)) ? BJ_F(0.0) : (t > BJ_F(1.0)) ? BJ_F(1.0) : t;
    return t * t * (BJ_F(3.0) - BJ_F(2.0) * t);
}

static inline bj_real bj_fract(bj_real x) {
    return x - bj_floor(x);
}

static inline bj_real bj_mod(bj_real x, bj_real y) {
    return x - y * bj_floor(x / y);
}

#endif
