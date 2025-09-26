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
/// Components are indexed as 0..1.
////////////////////////////////////////////////////////////////////////////////
typedef bj_real bj_vec2[2];

////////////////////////////////////////////////////////////////////////////////
/// bj_vec3: 3D vector of bj_real values.
/// Intended for lightweight math operations and POD interop.
/// Components are indexed as 0..2.
////////////////////////////////////////////////////////////////////////////////
typedef bj_real bj_vec3[3];

////////////////////////////////////////////////////////////////////////////////
/// bj_vec4: 4D vector of bj_real values.
/// Intended for lightweight math operations and POD interop.
/// Components are indexed as 0..3.
////////////////////////////////////////////////////////////////////////////////
typedef bj_real bj_vec4[4];

////////////////////////////////////////////////////////////////////////////////
/// Set a 2D vector from components (x, y).
/// \param res Output 2D vector.
/// \param a Input component or vector a.
/// \param b Input component or vector b.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_vec2_set(bj_vec2 res, bj_real a, bj_real b) {
    res[0] = a; res[1] = b;
}

static BJ_INLINE void bj_vec2_zero(bj_vec2 res) {
    res[0] = res[1] = BJ_FZERO;
}

static BJ_INLINE void bj_vec2_apply(bj_vec2 res, const bj_vec2 a, bj_real(*f)(bj_real)) {
    res[0] = f(a[0]);
    res[1] = f(a[1]);
}

////////////////////////////////////////////////////////////////////////////////
/// Component-wise addition of two 2D vectors: res = lhs + rhs.
/// \param res Output 2D vector.
/// \param lhs Left-hand input vector.
/// \param rhs Right-hand input vector.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_vec2_add(bj_vec2 res, const bj_vec2 lhs, const bj_vec2 rhs) {
    res[0] = lhs[0] + rhs[0];
    res[1] = lhs[1] + rhs[1];
}

////////////////////////////////////////////////////////////////////////////////
/// Add a scaled vector: res = a + s * b.
/// \param res Output 2D vector.
/// \param a Input component or vector a.
/// \param b Input component or vector b.
/// \param s Scalar factor.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_vec2_add_scaled(
    bj_vec2 res,
    const bj_vec2 a,
    const bj_vec2 b,
    bj_real s
) {
    res[0] = a[0] + b[0] * s;
    res[1] = a[1] + b[1] * s;
}

////////////////////////////////////////////////////////////////////////////////
/// Component-wise subtraction of two 2D vectors: res = lhs - rhs.
/// \param res Output 2D vector.
/// \param lhs Left-hand input vector.
/// \param rhs Right-hand input vector.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_vec2_sub(bj_vec2 res, const bj_vec2 lhs, const bj_vec2 rhs) {
    res[0] = lhs[0] - rhs[0];
    res[1] = lhs[1] - rhs[1];
}

////////////////////////////////////////////////////////////////////////////////
/// Uniform scaling by scalar: res = v * s.
/// \param res Output 2D vector.
/// \param v Input vector.
/// \param s Scalar factor.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_vec2_scale(bj_vec2 res, const bj_vec2 v, bj_real s) {
    res[0] = v[0] * s;
    res[1] = v[1] * s;
}

////////////////////////////////////////////////////////////////////////////////
/// Per-component scaling: res[i] = v[i] * s[i].
/// \param res Output 2D vector.
/// \param v Input vector.
/// \param s Scalar factor.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_vec2_scale_each(bj_vec2 res, const bj_vec2 v, const bj_vec2 s) {
    res[0] = v[0] * s[0];
    res[1] = v[1] * s[1];
}

////////////////////////////////////////////////////////////////////////////////
/// Dot product of two 2D vectors.
/// \param a Input component or vector a.
/// \param b Input component or vector b.
/// \returns The scalar dot product.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_real bj_vec2_dot(const bj_vec2 a, const bj_vec2 b) {
    return a[0] * b[0] + a[1] * b[1];
}

////////////////////////////////////////////////////////////////////////////////
/// 2D "cross product" (perp dot): returns scalar a.x*b.y - a.y*b.x.
/// Useful for orientation tests, signed area, segment intersection.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_real bj_vec2_cross(const bj_vec2 a, const bj_vec2 b) {
    return a[0]*b[1] - a[1]*b[0];
}

////////////////////////////////////////////////////////////////////////////////
/// Euclidean length (L2 norm) of a 2D vector.
/// \param v Input vector.
/// \returns The Euclidean norm.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_real bj_vec2_len(const bj_vec2 v) {
    return bj_sqrt(v[0] * v[0] + v[1] * v[1]);
}

////////////////////////////////////////////////////////////////////////////////
/// Scale a 2D vector to a given length.
/// \param res Output 2D vector.
/// \param v Input vector.
/// \param target_len Desired length of the result.
/// \warning Undefined if the input vector has zero length.
/// \ingroup math
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_vec2_set_len(bj_vec2 res, const bj_vec2 v, bj_real target_len) {
    bj_real len = bj_vec2_len(v);
    bj_real scale = target_len / len;
    bj_vec2_scale(res, v, scale);
}

////////////////////////////////////////////////////////////////////////////////
/// Squared Euclidean distance between two 2D vectors.
/// \param a Input vector a.
/// \param b Input vector b.
/// \returns The squared distance ||a - b||^2.
/// \ingroup math
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_real bj_vec2_dist_squared(const bj_vec2 a, const bj_vec2 b) {
    const bj_real dx = a[0] - b[0];
    const bj_real dy = a[1] - b[1];
    return dx * dx + dy * dy;
}

////////////////////////////////////////////////////////////////////////////////
/// Euclidean distance between two 2D vectors.
/// \param a Input vector a.
/// \param b Input vector b.
/// \returns The Euclidean distance ||a - b||.
/// \ingroup math
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_real bj_vec2_dist(const bj_vec2 a, const bj_vec2 b) {
    return bj_sqrt(bj_vec2_dist_squared(a, b));
}

////////////////////////////////////////////////////////////////////////////////
/// Normalize a 2D vector to unit length.
/// \param res Output 2D vector.
/// \param v Input vector.
/// \warning Undefined if the input vector has zero length.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_vec2_normalize(bj_vec2 res, const bj_vec2 v) {
    bj_real inv_len = BJ_F(1.0) / bj_sqrt(v[0] * v[0] + v[1] * v[1]);
    res[0] = v[0] * inv_len;
    res[1] = v[1] * inv_len;
}

////////////////////////////////////////////////////////////////////////////////
/// Component-wise minimum of two 2D vectors.
/// \param res Output 2D vector.
/// \param a Input component or vector a.
/// \param b Input component or vector b.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_vec2_min(bj_vec2 res, const bj_vec2 a, const bj_vec2 b) {
    res[0] = a[0] < b[0] ? a[0] : b[0];
    res[1] = a[1] < b[1] ? a[1] : b[1];
}

////////////////////////////////////////////////////////////////////////////////
/// Component-wise maximum of two 2D vectors.
/// \param res Output 2D vector.
/// \param a Input component or vector a.
/// \param b Input component or vector b.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_vec2_max(bj_vec2 res, const bj_vec2 a, const bj_vec2 b) {
    res[0] = a[0] > b[0] ? a[0] : b[0];
    res[1] = a[1] > b[1] ? a[1] : b[1];
}

////////////////////////////////////////////////////////////////////////////////
/// Copy a 2D vector.
/// \param res Output 2D vector.
/// \param src Source vector to copy from.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_vec2_copy(bj_vec2 res, const bj_vec2 src) {
    res[0] = src[0];
    res[1] = src[1];
}

////////////////////////////////////////////////////////////////////////////////
/// Set a 3D vector from components (x, y, z).
/// \param res Output 3D vector.
/// \param a Input component or vector a.
/// \param b Input component or vector b.
/// \param c Input component or vector c.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_vec3_set(bj_vec3 res, bj_real a, bj_real b, bj_real c) {
    res[0] = a; res[1] = b; res[2] = c;
}

static BJ_INLINE void bj_vec3_zero(bj_vec3 res) {
    res[0] = res[1] = res[2] = BJ_FZERO;
}

static BJ_INLINE void bj_vec3_apply(bj_vec3 res, const bj_vec3 a, bj_real(*f)(bj_real)) {
    res[0] = f(a[0]);
    res[1] = f(a[1]);
    res[2] = f(a[2]);
}


////////////////////////////////////////////////////////////////////////////////
/// Component-wise addition of two 3D vectors: res = lhs + rhs.
/// \param res Output 3D vector.
/// \param lhs Left-hand input vector.
/// \param rhs Right-hand input vector.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_vec3_add(bj_vec3 res, const bj_vec3 lhs, const bj_vec3 rhs) {
    res[0] = lhs[0] + rhs[0];
    res[1] = lhs[1] + rhs[1];
    res[2] = lhs[2] + rhs[2];
}

////////////////////////////////////////////////////////////////////////////////
/// Add a scaled vector: res = a + s * b.
/// \param res Output 3D vector.
/// \param a Input component or vector a.
/// \param b Input component or vector b.
/// \param s Scalar factor.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_vec3_add_scaled(bj_vec3 res, const bj_vec3 a, const bj_vec3 b, bj_real s) {
    res[0] = a[0] + b[0] * s;
    res[1] = a[1] + b[1] * s;
    res[2] = a[2] + b[2] * s;
}


////////////////////////////////////////////////////////////////////////////////
/// Component-wise subtraction of two 3D vectors: res = lhs - rhs.
/// \param res Output 3D vector.
/// \param lhs Left-hand input vector.
/// \param rhs Right-hand input vector.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_vec3_sub(bj_vec3 res, const bj_vec3 lhs, const bj_vec3 rhs) {
    res[0] = lhs[0] - rhs[0];
    res[1] = lhs[1] - rhs[1];
    res[2] = lhs[2] - rhs[2];
}

////////////////////////////////////////////////////////////////////////////////
/// Uniform scaling by scalar: res = v * s.
/// \param res Output 3D vector.
/// \param v Input vector.
/// \param s Scalar factor.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_vec3_scale(bj_vec3 res, const bj_vec3 v, bj_real s) {
    res[0] = v[0] * s;
    res[1] = v[1] * s;
    res[2] = v[2] * s;
}

////////////////////////////////////////////////////////////////////////////////
/// Dot product of two 3D vectors.
/// \param a Input component or vector a.
/// \param b Input component or vector b.
/// \returns The scalar dot product.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_real bj_vec3_dot(const bj_vec3 a, const bj_vec3 b) {
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

////////////////////////////////////////////////////////////////////////////////
/// Euclidean length (L2 norm) of a 3D vector.
/// \param v Input vector.
/// \returns The Euclidean norm.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_real bj_vec3_len(const bj_vec3 v) {
    return bj_sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

////////////////////////////////////////////////////////////////////////////////
/// Scale a 3D vector to a given length.
/// \param res Output 3D vector.
/// \param v Input vector.
/// \param target_len Desired length of the result.
/// \warning Undefined if the input vector has zero length.
/// \ingroup math
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_vec3_set_len(bj_vec3 res, const bj_vec3 v, bj_real target_len) {
    bj_real len = bj_vec3_len(v);
    bj_real scale = target_len / len;
    bj_vec3_scale(res, v, scale);
}

////////////////////////////////////////////////////////////////////////////////
/// Squared Euclidean distance between two 3D vectors.
/// \param a Input vector a.
/// \param b Input vector b.
/// \returns The squared distance ||a - b||^2.
/// \ingroup math
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_real bj_vec3_dist_squared(const bj_vec3 a, const bj_vec3 b) {
    const bj_real dx = a[0] - b[0];
    const bj_real dy = a[1] - b[1];
    const bj_real dz = a[2] - b[2];
    return dx * dx + dy * dy + dz * dz;
}

////////////////////////////////////////////////////////////////////////////////
/// Euclidean distance between two 3D vectors.
/// \param a Input vector a.
/// \param b Input vector b.
/// \returns The Euclidean distance ||a - b||.
/// \ingroup math
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_real bj_vec3_dist(const bj_vec3 a, const bj_vec3 b) {
    return bj_sqrt(bj_vec3_dist_squared(a, b));
}


////////////////////////////////////////////////////////////////////////////////
/// Normalize a 3D vector to unit length.
/// \param res Output 3D vector.
/// \param v Input vector.
/// \warning Undefined if the input vector has zero length.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_vec3_normalize(bj_vec3 res, const bj_vec3 v) {
    bj_real inv_len = BJ_F(1.0) / bj_sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    res[0] = v[0] * inv_len;
    res[1] = v[1] * inv_len;
    res[2] = v[2] * inv_len;
}

////////////////////////////////////////////////////////////////////////////////
/// Component-wise minimum of two 3D vectors.
/// \param res Output 3D vector.
/// \param a Input component or vector a.
/// \param b Input component or vector b.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_vec3_min(bj_vec3 res, const bj_vec3 a, const bj_vec3 b) {
    res[0] = a[0] < b[0] ? a[0] : b[0];
    res[1] = a[1] < b[1] ? a[1] : b[1];
    res[2] = a[2] < b[2] ? a[2] : b[2];
}

////////////////////////////////////////////////////////////////////////////////
/// Component-wise maximum of two 3D vectors.
/// \param res Output 3D vector.
/// \param a Input component or vector a.
/// \param b Input component or vector b.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_vec3_max(bj_vec3 res, const bj_vec3 a, const bj_vec3 b) {
    res[0] = a[0] > b[0] ? a[0] : b[0];
    res[1] = a[1] > b[1] ? a[1] : b[1];
    res[2] = a[2] > b[2] ? a[2] : b[2];
}

////////////////////////////////////////////////////////////////////////////////
/// Copy a 3D vector.
/// \param res Output 3D vector.
/// \param src Source vector to copy from.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_vec3_copy(bj_vec3 res, const bj_vec3 src) {
    res[0] = src[0];
    res[1] = src[1];
    res[2] = src[2];
}

////////////////////////////////////////////////////////////////////////////////
/// 3D cross product: res = l × r (right-hand rule).
/// \param res Output 3D vector.
/// \param l Left-hand input vector.
/// \param r Right-hand input vector.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_vec3_cross(bj_vec3 res, const bj_vec3 l, const bj_vec3 r)
{
    res[0] = l[1] * r[2] - l[2] * r[1];
    res[1] = l[2] * r[0] - l[0] * r[2];
    res[2] = l[0] * r[1] - l[1] * r[0];
}

////////////////////////////////////////////////////////////////////////////////
/// Reflect a vector about a normal: res = v - 2*dot(v, n)*n.
/// \param res Output 3D vector.
/// \param v Input vector.
/// \param n Surface normal (expected normalized).
/// \note The normal \p n should be normalized for a true reflection.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_vec3_reflect(bj_vec3 res, const bj_vec3 v, const bj_vec3 n)
{
    const bj_real p = BJ_F(2.) * bj_vec3_dot(v, n);
    for (int i = 0; i < 3; ++i) {
        res[i] = v[i] - p * n[i];
    }
}


////////////////////////////////////////////////////////////////////////////////
/// Set a 4D vector from components (x, y, z, w).
/// \param res Output 4D vector.
/// \param a Input component or vector a.
/// \param b Input component or vector b.
/// \param c Input component or vector c.
/// \param d Input component or vector d.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_vec4_set(bj_vec4 res, bj_real a, bj_real b, bj_real c, bj_real d) {
    res[0] = a; res[1] = b; res[2] = c; res[3] = d;
}

static BJ_INLINE void bj_vec4_apply(bj_vec4 res, const bj_vec4 a, bj_real(*f)(bj_real)) {
    res[0] = f(a[0]);
    res[1] = f(a[1]);
    res[2] = f(a[2]);
    res[3] = f(a[3]);
}

////////////////////////////////////////////////////////////////////////////////
/// Component-wise addition of two 4D vectors: res = lhs + rhs.
/// \param res Output 4D vector.
/// \param lhs Left-hand input vector.
/// \param rhs Right-hand input vector.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_vec4_add(bj_vec4 res, const bj_vec4 lhs, const bj_vec4 rhs) {
    res[0] = lhs[0] + rhs[0];
    res[1] = lhs[1] + rhs[1];
    res[2] = lhs[2] + rhs[2];
    res[3] = lhs[3] + rhs[3];
}

////////////////////////////////////////////////////////////////////////////////
/// Add a scaled vector: res = a + s * b.
/// \param res Output 4D vector.
/// \param a Input component or vector a.
/// \param b Input component or vector b.
/// \param s Scalar factor.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_vec4_add_scaled(bj_vec4 res, const bj_vec4 a, const bj_vec4 b, bj_real s) {
    res[0] = a[0] + b[0] * s;
    res[1] = a[1] + b[1] * s;
    res[2] = a[2] + b[2] * s;
    res[3] = a[3] + b[3] * s;
}

////////////////////////////////////////////////////////////////////////////////
/// Component-wise subtraction of two 4D vectors: res = lhs - rhs.
/// \param res Output 4D vector.
/// \param lhs Left-hand input vector.
/// \param rhs Right-hand input vector.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_vec4_sub(bj_vec4 res, const bj_vec4 lhs, const bj_vec4 rhs) {
    res[0] = lhs[0] - rhs[0];
    res[1] = lhs[1] - rhs[1];
    res[2] = lhs[2] - rhs[2];
    res[3] = lhs[3] - rhs[3];
}

////////////////////////////////////////////////////////////////////////////////
/// Uniform scaling by scalar: res = v * s.
/// \param res Output 4D vector.
/// \param v Input vector.
/// \param s Scalar factor.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_vec4_scale(bj_vec4 res, const bj_vec4 v, bj_real s) {
    res[0] = v[0] * s;
    res[1] = v[1] * s;
    res[2] = v[2] * s;
    res[3] = v[3] * s;
}

////////////////////////////////////////////////////////////////////////////////
/// Dot product of two 4D vectors.
/// \param a Input component or vector a.
/// \param b Input component or vector b.
/// \returns The scalar dot product.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_real bj_vec4_dot(const bj_vec4 a, const bj_vec4 b) {
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3];
}

////////////////////////////////////////////////////////////////////////////////
/// Euclidean length (L2 norm) of a 4D vector.
/// \param v Input vector.
/// \returns The Euclidean norm.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_real bj_vec4_len(const bj_vec4 v) {
    return bj_sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3]);
}

////////////////////////////////////////////////////////////////////////////////
/// Normalize a 4D vector to unit length.
/// \param res Output 4D vector.
/// \param v Input vector.
/// \warning Undefined if the input vector has zero length.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_vec4_normalize(bj_vec4 res, const bj_vec4 v) {
    bj_real inv_len = BJ_F(1.0) / bj_sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3]);
    res[0] = v[0] * inv_len;
    res[1] = v[1] * inv_len;
    res[2] = v[2] * inv_len;
    res[3] = v[3] * inv_len;
}

////////////////////////////////////////////////////////////////////////////////
/// Component-wise minimum of two 4D vectors.
/// \param res Output 4D vector.
/// \param a Input component or vector a.
/// \param b Input component or vector b.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_vec4_min(bj_vec4 res, const bj_vec4 a, const bj_vec4 b) {
    res[0] = a[0] < b[0] ? a[0] : b[0];
    res[1] = a[1] < b[1] ? a[1] : b[1];
    res[2] = a[2] < b[2] ? a[2] : b[2];
    res[3] = a[3] < b[3] ? a[3] : b[3];
}

////////////////////////////////////////////////////////////////////////////////
/// Component-wise maximum of two 4D vectors.
/// \param res Output 4D vector.
/// \param a Input component or vector a.
/// \param b Input component or vector b.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_vec4_max(bj_vec4 res, const bj_vec4 a, const bj_vec4 b) {
    res[0] = a[0] > b[0] ? a[0] : b[0];
    res[1] = a[1] > b[1] ? a[1] : b[1];
    res[2] = a[2] > b[2] ? a[2] : b[2];
    res[3] = a[3] > b[3] ? a[3] : b[3];
}

////////////////////////////////////////////////////////////////////////////////
/// Copy a 4D vector.
/// \param res Output 4D vector.
/// \param src Source vector to copy from.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_vec4_copy(bj_vec4 res, const bj_vec4 src) {
    res[0] = src[0];
    res[1] = src[1];
    res[2] = src[2];
    res[3] = src[3];
}

////////////////////////////////////////////////////////////////////////////////
/// Cross product using xyz components; w is set to 1.
/// \param res Output 4D vector.
/// \param l Left-hand input vector.
/// \param r Right-hand input vector.
/// \note Uses only xyz components; the result w component is set to 1.0.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_vec4_cross(bj_vec4 res, const bj_vec4 l, const bj_vec4 r)
{
    res[0] = l[1] * r[2] - l[2] * r[1];
    res[1] = l[2] * r[0] - l[0] * r[2];
    res[2] = l[0] * r[1] - l[1] * r[0];
    res[3] = BJ_F(1.0);
}

////////////////////////////////////////////////////////////////////////////////
/// Reflect a vector about a normal: res = v - 2*dot(v, n)*n.
/// \param res Output 4D vector.
/// \param v Input vector.
/// \param n Surface normal (expected normalized).
/// \note The normal \p n should be normalized for a true reflection.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_vec4_reflect(bj_vec4 res, const bj_vec4 v, const bj_vec4 n)
{
    bj_real p = BJ_F(2.0) * bj_vec4_dot(v, n);
    for (int i = 0; i < 4; ++i) {
        res[i] = v[i] - p * n[i];
    }
}


#endif

/// \}

