////////////////////////////////////////////////////////////////////////////////
/// \file vec.h
/// vector manipulation API
////////////////////////////////////////////////////////////////////////////////
/// Vector utilities for 2D, 3D, and 4D using \c bj_real.
///
/// \file vec.h
///
/// Provides simple, inline operations (set, add, sub, scale, dot, length, normalize,
/// min/max, copy, cross, reflect) on fixed-size vectors.
/// All functions are header-only and suitable for high-performance code.
///
/// \addtogroup math
/// \{
////////////////////////////////////////////////////////////////////////////////
#ifndef BJ_VEC_H
#define BJ_VEC_H

#include <banjo/api.h>
#include <banjo/math.h>

////////////////////////////////////////////////////////////////////////////////
/// bj_vec2: 2D vector of bj_real values.
/// Intended for lightweight math operations and POD interop.
////////////////////////////////////////////////////////////////////////////////
struct bj_vec2_t {
    bj_real x;
    bj_real y;
};
typedef struct bj_vec2_t bj_vec2;

#define BJ_VEC2_ZERO ((bj_vec2){BJ_FZERO, BJ_FZERO})

////////////////////////////////////////////////////////////////////////////////
/// bj_vec3: 3D vector of bj_real values.
/// Intended for lightweight math operations and POD interop.
////////////////////////////////////////////////////////////////////////////////
struct bj_vec3_t {
    bj_real x;
    bj_real y;
    bj_real z;
};
typedef struct bj_vec3_t bj_vec3;

#define BJ_VEC3_ZERO ((bj_vec3){BJ_FZERO, BJ_FZERO, BJ_FZERO})

////////////////////////////////////////////////////////////////////////////////
/// bj_vec4: 4D vector of bj_real values.
/// Intended for lightweight math operations and POD interop.
////////////////////////////////////////////////////////////////////////////////
struct bj_vec4_t {
    bj_real x;
    bj_real y;
    bj_real z;
    bj_real w;
};
typedef struct bj_vec4_t bj_vec4;

#define BJ_VEC4_ZERO ((bj_vec4){BJ_FZERO, BJ_FZERO, BJ_FZERO, BJ_FZERO})

static BJ_INLINE bj_vec2 bj_vec2_map(
    bj_vec2 a,
    bj_real(*f)(bj_real)
) {
    return (bj_vec2){ .x = f(a.x), .y = f(a.y), };
}

////////////////////////////////////////////////////////////////////////////////
/// Component-wise addition of two 2D vectors: res = lhs + rhs.
///
/// \param lhs Left-hand input vector.
/// \param rhs Right-hand input vector.
/// \return Sum of `lhs` and `rhs`.  
///
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec2 bj_vec2_add(bj_vec2 lhs, bj_vec2 rhs) {
    return (bj_vec2){ .x = lhs.x + rhs.x, .y = lhs.y + rhs.y, };
}

////////////////////////////////////////////////////////////////////////////////
/// Add a scaled vector: res = a + s * b.
/// \param lhs Input component or vector a.
/// \param rhs Input component or vector b.
/// \param s   Scalar factor.
/// \return Sum of `lhs` and `rhs * s`.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec2 bj_vec2_add_scaled(
    bj_vec2 lhs,
    bj_vec2 rhs,
    bj_real s
) {
    return (bj_vec2){ .x = lhs.x + rhs.x * s, .y = lhs.y + rhs.y * s, };
}

////////////////////////////////////////////////////////////////////////////////
/// Component-wise subtraction of two 2D vectors: res = lhs - rhs.
/// \param lhs Left-hand input vector.
/// \param rhs Right-hand input vector.
/// \return `lhs` - `rhs`
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec2 bj_vec2_sub(
    const bj_vec2 lhs,
    const bj_vec2 rhs
) {
    return (bj_vec2){ .x = lhs.x - rhs.x, .y = lhs.y - rhs.y, };
}

////////////////////////////////////////////////////////////////////////////////
/// Uniform scaling by scalar: res = v * s.
/// \param v Input vector.
/// \param s Scalar factor.
/// \return The result of `v * s`
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec2 bj_vec2_scale(bj_vec2 v, bj_real s) {
    return (bj_vec2){ .x = v.x * s, .y = v.y * s, };
}

////////////////////////////////////////////////////////////////////////////////
/// Per-component scaling: res[i] = v[i] * s[i].
/// \param v Input vector.
/// \param s Scalar factor.
/// \return The result of `v * s`
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec2 bj_vec2_mul_comp(
    const bj_vec2 v,
    const bj_vec2 s
) {
    return (bj_vec2){ .x = v.x * s.x, .y = v.y * s.y, };
}

////////////////////////////////////////////////////////////////////////////////
/// Dot product of two 2D vectors.
/// \param a Input component or vector a.
/// \param b Input component or vector b.
/// \returns The scalar dot product.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_real bj_vec2_dot(bj_vec2 a, bj_vec2 b) {
    return a.x * b.x + a.y * b.y;
}

////////////////////////////////////////////////////////////////////////////////
/// 2D "cross product" (perp dot): returns scalar a.x*b.y - a.y*b.x.
/// Useful for orientation tests, signed area, segment intersection.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_real bj_vec2_perp_dot(bj_vec2 a, bj_vec2 b) {
    return a.x*b.y - a.y*b.x;
}

////////////////////////////////////////////////////////////////////////////////
/// Euclidean length (L2 norm) of a 2D vector.
/// \param v Input vector.
/// \returns The Euclidean norm.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_real bj_vec2_len(bj_vec2 v) {
    return bj_sqrt(v.x * v.x + v.y * v.y);
}

////////////////////////////////////////////////////////////////////////////////
/// Scale a 2D vector to a given length.
/// \param res Output 2D vector.
/// \param v Input vector.
/// \param target_len Desired length of the result.
/// \warning Undefined if the input vector has zero length.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec2 bj_vec2_scale_to_len(bj_vec2 v, bj_real L){
    const bj_real l = bj_vec2_len(v); 
    if (bj_real_is_zero(l)){
        return (bj_vec2){BJ_FZERO,BJ_FZERO};
    }
    return bj_vec2_scale(v, L / l);
}

////////////////////////////////////////////////////////////////////////////////
/// Squared Euclidean distance between two 2D vectors.
/// \param a Input vector a.
/// \param b Input vector b.
/// \returns The squared distance ||a - b||^2.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_real bj_vec2_distance_sq(bj_vec2 a, bj_vec2 b) {
    const bj_real dx = a.x - b.x;
    const bj_real dy = a.y - b.y;
    return dx * dx + dy * dy;
}

////////////////////////////////////////////////////////////////////////////////
/// Euclidean distance between two 2D vectors.
/// \param a Input vector a.
/// \param b Input vector b.
/// \returns The Euclidean distance ||a - b||.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_real bj_vec2_distance(const bj_vec2 a, const bj_vec2 b) {
    return bj_sqrt(bj_vec2_distance_sq(a, b));
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Normalize a 2D vector to unit length (safe).
/// 
/// \details Computes v / ||v||. If the squared length is zero according to
/// bj_real_is_zero, returns the zero vector to avoid division by zero.
/// Uses a single square root.
/// 
/// \param v Input vector.
/// \return Unit-length copy of \p v, or {0,0} if ||v|| == 0 by bj_real_is_zero.
/// 
/// \warning Zero-detection follows bj_real_is_zero semantics and tolerance.
/// \see bj_vec2_normalize_unsafe, bj_real_is_zero
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec2 bj_vec2_normalize(bj_vec2 v){
    const bj_real l2 = v.x * v.x + v.y * v.y;
    if (bj_real_is_zero(l2)) {
        return (bj_vec2){ BJ_FZERO, BJ_FZERO };
    }
    const bj_real inv = BJ_F(1.0) / bj_sqrt(l2);
    return (bj_vec2){ v.x * inv, v.y * inv };
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Normalize a 2D vector to unit length (unsafe).
/// 
/// \details Computes v / ||v|| without any zero-length check.
/// Faster on hot paths where nonzero length is guaranteed.
/// 
/// \param v Input vector (must be nonzero).
/// \return Unit-length copy of \p v.
/// 
/// \pre ||v|| > 0 (caller guarantees).
/// \warning Undefined results if ||v|| == 0. Consider bj_vec2_normalize instead.
/// \see bj_vec2_normalize
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec2 bj_vec2_normalize_unsafe(bj_vec2 v) {
    const bj_real inv = BJ_F(1.0) / bj_sqrt(v.x*v.x + v.y*v.y);
    return (bj_vec2){ v.x * inv, v.y * inv };
}


////////////////////////////////////////////////////////////////////////////////
/// Component-wise minimum of two 2D vectors.
/// \param res Output 2D vector.
/// \param a Input component or vector a.
/// \param b Input component or vector b.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec2 bj_vec2_min(bj_vec2 a, bj_vec2 b) {
    return (bj_vec2){a.x < b.x ? a.x : b.x, a.y < b.y ? a.y : b.y, };
}

////////////////////////////////////////////////////////////////////////////////
/// Component-wise maximum of two 2D vectors.
/// \param res Output 2D vector.
/// \param a Input component or vector a.
/// \param b Input component or vector b.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec2 bj_vec2_max(bj_vec2 a, bj_vec2 b) {
    return (bj_vec2){a.x > b.x ? a.x : b.x, a.y > b.y ? a.y : b.y, };
}

static BJ_INLINE bj_vec3 bj_vec3_map(
    bj_vec3 a,
    bj_real(*f)(bj_real)
) {
    return (bj_vec3){ .x = f(a.x), .y = f(a.y), .z = f(a.z), };
}

////////////////////////////////////////////////////////////////////////////////
/// Component-wise addition of two 3D vectors: res = lhs + rhs.
/// \param res Output 3D vector.
/// \param lhs Left-hand input vector.
/// \param rhs Right-hand input vector.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec3 bj_vec3_add(bj_vec3 lhs, bj_vec3 rhs) {
    return (bj_vec3){ 
        .x = lhs.x + rhs.x,
        .y = lhs.y + rhs.y,
        .z = lhs.z + rhs.z,
    };
}

////////////////////////////////////////////////////////////////////////////////
/// Add a scaled vector: res = a + s * b.
/// \param lhs Input component or vector a.
/// \param rhs Input component or vector b.
/// \param s   Scalar factor.
/// \return Sum of `lhs` and `rhs * s`.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec3 bj_vec3_add_scaled(
    bj_vec3 lhs,
    bj_vec3 rhs,
    bj_real s
) {
    return (bj_vec3){ 
        .x = lhs.x + rhs.x * s,
        .y = lhs.y + rhs.y * s,
        .z = lhs.z + rhs.z * s,  
    };
}


////////////////////////////////////////////////////////////////////////////////
/// Component-wise subtraction of two 3D vectors: res = lhs - rhs.
/// \param lhs Left-hand input vector.
/// \param rhs Right-hand input vector.
/// \return `lhs` - `rhs`
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec3 bj_vec3_sub(
    bj_vec3 lhs,
    bj_vec3 rhs
) {
    return (bj_vec3){ 
        .x = lhs.x - rhs.x,
        .y = lhs.y - rhs.y,
        .z = lhs.z - rhs.z, 
    };
}

////////////////////////////////////////////////////////////////////////////////
/// Uniform scaling by scalar: res = v * s.
/// \param v Input vector.
/// \param s Scalar factor.
/// \return The result of `v * s`
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec3 bj_vec3_scale(bj_vec3 v, bj_real s) {
    return (bj_vec3){ 
        .x = v.x * s,
        .y = v.y * s,
        .z = v.z * s,
    };
}

////////////////////////////////////////////////////////////////////////////////
/// Dot product of two 3D vectors.
/// \param a Input component or vector a.
/// \param b Input component or vector b.
/// \returns The scalar dot product.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_real bj_vec3_dot(bj_vec3 a, bj_vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

////////////////////////////////////////////////////////////////////////////////
/// Euclidean length (L2 norm) of a 3D vector.
/// \param v Input vector.
/// \returns The Euclidean norm.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_real bj_vec3_len(bj_vec3 v) {
    return bj_sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

////////////////////////////////////////////////////////////////////////////////
/// Scale a 3D vector to a given length.
/// \param res Output 3D vector.
/// \param v Input vector.
/// \param target_len Desired length of the result.
/// \warning Undefined if the input vector has zero length.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec3 bj_vec3_scale_to_len(bj_vec3 v, bj_real L){
    const bj_real l = bj_vec3_len(v);
    if (bj_real_is_zero(l)) {
        return (bj_vec3){BJ_FZERO,BJ_FZERO,BJ_FZERO};
    }
    return bj_vec3_scale(v, L / l);
}

////////////////////////////////////////////////////////////////////////////////
/// Squared Euclidean distance between two 3D vectors.
/// \param a Input vector a.
/// \param b Input vector b.
/// \returns The squared distance ||a - b||^2.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_real bj_vec3_distance_sq(bj_vec3 a, bj_vec3 b) {
    const bj_real dx = a.x - b.x;
    const bj_real dy = a.y - b.y;
    const bj_real dz = a.z - b.z;
    return dx * dx + dy * dy + dz * dz;
}

////////////////////////////////////////////////////////////////////////////////
/// Euclidean distance between two 3D vectors.
/// \param a Input vector a.
/// \param b Input vector b.
/// \returns The Euclidean distance ||a - b||.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_real bj_vec3_distance(bj_vec3 a, bj_vec3 b) {
    return bj_sqrt(bj_vec3_distance_sq(a, b));
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Normalize a 3D vector to unit length (safe).
/// 
/// \details Computes v / ||v||. Returns {0,0,0} if ||v|| is zero by
/// bj_real_is_zero. Uses one square root.
/// 
/// \param v Input vector.
/// \return Unit-length copy of \p v, or {0,0,0} if ||v|| == 0 by bj_real_is_zero.
/// 
/// \warning Zero-detection follows bj_real_is_zero semantics and tolerance.
/// \see bj_vec3_normalize_unsafe, bj_real_is_zero
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec3 bj_vec3_normalize(bj_vec3 v){
    const bj_real l2 = v.x * v.x + v.y * v.y + v.z * v.z;
    if (bj_real_is_zero(l2)) {
        return (bj_vec3){ BJ_FZERO, BJ_FZERO, BJ_FZERO };
    }
    const bj_real inv = BJ_F(1.0) / bj_sqrt(l2);
    return (bj_vec3){ v.x * inv, v.y * inv, v.z * inv };
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Normalize a 3D vector to unit length (unsafe).
/// 
/// \details Computes v / ||v|| without any zero-length check.
/// 
/// \param v Input vector (must be nonzero).
/// \return Unit-length copy of \p v.
/// 
/// \pre ||v|| > 0 (caller guarantees).
/// \warning Undefined results if ||v|| == 0. Consider bj_vec3_normalize instead.
/// \see bj_vec3_normalize
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec3 bj_vec3_normalize_unsafe(bj_vec3 v) {
    const bj_real inv = BJ_F(1.0) / bj_sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
    return (bj_vec3){ v.x * inv, v.y * inv, v.z * inv };
}

////////////////////////////////////////////////////////////////////////////////
/// Component-wise minimum of two 3D vectors.
/// \param res Output 3D vector.
/// \param a Input component or vector a.
/// \param b Input component or vector b.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec3 bj_vec3_min(bj_vec3 a, bj_vec3 b) {
    return (bj_vec3){
        a.x < b.x ? a.x : b.x,
        a.y < b.y ? a.y : b.y,
        a.z < b.z ? a.z : b.z,
    };
}

////////////////////////////////////////////////////////////////////////////////
/// Component-wise maximum of two 3D vectors.
/// \param res Output 3D vector.
/// \param a Input component or vector a.
/// \param b Input component or vector b.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec3 bj_vec3_max(bj_vec3 a, bj_vec3 b) {
    return (bj_vec3){a.x > b.x ? a.x : b.x, a.y > b.y ? a.y : b.y, a.z > b.z ? a.z : b.z,};
}

////////////////////////////////////////////////////////////////////////////////
/// 3D cross product: res = l × r (right-hand rule).
/// \param l Left-hand input vector.
/// \param r Right-hand input vector.
/// \return Resulting cross product.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec3 bj_vec3_cross(bj_vec3 l, bj_vec3 r)
{
    return (bj_vec3) {
        .x = l.y * r.z - l.z * r.y,
        .y = l.z * r.x - l.x * r.z,
        .z = l.x * r.y - l.y * r.x,
    };
}

////////////////////////////////////////////////////////////////////////////////
/// Reflect a vector about a normal: res = v - 2*dot(v, n)*n.
/// \param v Input vector.
/// \param n Surface normal (expected normalized).
/// \reutrn Resulting reflect vector.
/// \note The normal \p n should be normalized for a true reflection.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec3 bj_vec3_reflect(bj_vec3 v, bj_vec3 n)
{
    const bj_real p = BJ_F(2.) * bj_vec3_dot(v, n);
    return (bj_vec3) {
        .x = v.x - p * n.x,
        .y = v.y - p * n.y,
        .z = v.z - p * n.z,
    };
}

static BJ_INLINE bj_vec4 bj_vec4_map(
    bj_vec4 a,
    bj_real(*f)(bj_real)
) {
    return (bj_vec4){ .x = f(a.x), .y = f(a.y), .z = f(a.z), .w = f(a.w), };
}

////////////////////////////////////////////////////////////////////////////////
/// Component-wise addition of two 4D vectors: res = lhs + rhs.
/// \param lhs Left-hand input vector.
/// \param rhs Right-hand input vector.
/// \return Result of `lhs` + `rhs`
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec4 bj_vec4_add(bj_vec4 lhs, bj_vec4 rhs) {
    return (bj_vec4){ 
        .x = lhs.x + rhs.x,
        .y = lhs.y + rhs.y,
        .z = lhs.z + rhs.z,
        .w = lhs.w + rhs.w, 
    };
}

////////////////////////////////////////////////////////////////////////////////
/// Add a scaled vector: res = a + s * b.
/// \param lhs Input component or vector a.
/// \param rhs Input component or vector b.
/// \param s   Scalar factor.
/// \return Sum of `lhs` and `rhs * s`.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec4 bj_vec4_add_scaled(
    bj_vec4 lhs,
    bj_vec4 rhs,
    bj_real s
) {
    return (bj_vec4){ 
        .x = lhs.x + rhs.x * s,
        .y = lhs.y + rhs.y * s,
        .z = lhs.z + rhs.z * s,  
        .w = lhs.w + rhs.w * s,  
    };
}

////////////////////////////////////////////////////////////////////////////////
/// Component-wise subtraction of two 4D vectors: res = lhs - rhs.
/// \param lhs Left-hand input vector.
/// \param rhs Right-hand input vector.
/// \return `lhs` - `rhs`
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec4 bj_vec4_sub(
    bj_vec4 lhs,
    bj_vec4 rhs
) {
    return (bj_vec4){ 
        .x = lhs.x - rhs.x,
        .y = lhs.y - rhs.y,
        .z = lhs.z - rhs.z, 
        .w = lhs.w - rhs.w, 
    };
}

////////////////////////////////////////////////////////////////////////////////
/// Uniform scaling by scalar: res = v * s.
/// \param v Input vector.
/// \param s Scalar factor.
/// \return The result of `v * s`
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec4 bj_vec4_scale(bj_vec4 v, bj_real s) {
    return (bj_vec4){ 
        .x = v.x * s,
        .y = v.y * s,
        .z = v.z * s,
        .w = v.w * s,
    };
}

////////////////////////////////////////////////////////////////////////////////
/// Dot product of two 4D vectors.
/// \param a Input component or vector a.
/// \param b Input component or vector b.
/// \returns The scalar dot product.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_real bj_vec4_dot(bj_vec4 a, bj_vec4 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

////////////////////////////////////////////////////////////////////////////////
/// Euclidean length (L2 norm) of a 4D vector.
/// \param v Input vector.
/// \returns The Euclidean norm.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_real bj_vec4_len(bj_vec4 v) {
    return bj_sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Normalize a 4D vector to unit length (safe).
/// 
/// \details Computes v / ||v||. Returns {0,0,0,0} if ||v|| is zero by
/// bj_real_is_zero. Uses one square root.
/// 
/// \param v Input vector.
/// \return Unit-length copy of \p v, or {0,0,0,0} if ||v|| == 0 by bj_real_is_zero.
/// 
/// \warning Zero-detection follows bj_real_is_zero semantics and tolerance.
/// \see bj_vec4_normalize_unsafe, bj_real_is_zero
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec4 bj_vec4_normalize(bj_vec4 v) { 
    const bj_real len2 = v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w;
    if (bj_real_is_zero(len2)) { 
        return (bj_vec4){ BJ_FZERO, BJ_FZERO, BJ_FZERO, BJ_FZERO };
    }
    const bj_real inv = BJ_F(1.0) / bj_sqrt(len2);
    return (bj_vec4){ v.x * inv, v.y * inv, v.z * inv, v.w * inv };
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Normalize a 4D vector to unit length (unsafe).
/// 
/// \details Computes v / ||v|| without any zero-length check.
/// 
/// \param v Input vector (must be nonzero).
/// \return Unit-length copy of \p v.
/// 
/// \pre ||v|| > 0 (caller guarantees).
/// \warning Undefined results if ||v|| == 0. Consider bj_vec4_normalize instead.
/// \see bj_vec4_normalize
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec4 bj_vec4_normalize_unsafe(bj_vec4 v) {
    const bj_real inv = BJ_F(1.0) / bj_sqrt(v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w);
    return (bj_vec4){ v.x * inv, v.y * inv, v.z * inv, v.w * inv };
}

////////////////////////////////////////////////////////////////////////////////
/// Component-wise minimum of two 4D vectors.
/// \param res Output 4D vector.
/// \param a Input component or vector a.
/// \param b Input component or vector b.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec4 bj_vec4_min(bj_vec4 a, bj_vec4 b) {
    return (bj_vec4){
        a.x < b.x ? a.x : b.x,
        a.y < b.y ? a.y : b.y,
        a.z < b.z ? a.z : b.z,
        a.w < b.w ? a.w : b.w,
    };
}

////////////////////////////////////////////////////////////////////////////////
/// Component-wise maximum of two 4D vectors.
/// \param res Output 4D vector.
/// \param a Input component or vector a.
/// \param b Input component or vector b.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec4 bj_vec4_max(bj_vec4 a, bj_vec4 b) {
    return (bj_vec4){
        a.x > b.x ? a.x : b.x,
        a.y > b.y ? a.y : b.y,
        a.z > b.z ? a.z : b.z,
        a.w > b.w ? a.w : b.w,
    };
}

////////////////////////////////////////////////////////////////////////////////
/// Cross product using xyz components; w is set to 1.
/// \param res Output 4D vector.
/// \param l Left-hand input vector.
/// \param r Right-hand input vector.
/// \note Uses only xyz components; the result w component is set to 1.0.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec4 bj_vec4_cross_xyz(bj_vec4 l, bj_vec4 r){
    return (bj_vec4){ 
        l.y*r.z - l.z*r.y,
        l.z*r.x - l.x*r.z,
        l.x*r.y - l.y*r.x,
        BJ_FZERO
    };
}

////////////////////////////////////////////////////////////////////////////////
/// Reflect a vector about a normal: res = v - 2*dot(v, n)*n.
/// \param res Output 4D vector.
/// \param v Input vector.
/// \param n Surface normal (expected normalized).
/// \note The normal \p n should be normalized for a true reflection.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec4 bj_vec4_reflect(bj_vec4 v, bj_vec4 n)
{
    bj_real p = BJ_F(2.0) * bj_vec4_dot(v, n);
    return (bj_vec4) {
        .x = v.x - p * n.x,
        .y = v.y - p * n.y,
        .z = v.z - p * n.z,
        .w = v.w - p * n.w,
    };
}


#endif

/// \}

