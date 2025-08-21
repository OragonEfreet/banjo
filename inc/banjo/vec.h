////////////////////////////////////////////////////////////////////////////////
/// \file vec.h
/// Vector math library.
////////////////////////////////////////////////////////////////////////////////
/// \addtogroup math
///
/// \brief Linear math function
///
/// The Math group provides usual facilities for linear math, including
/// - vector types: \ref bj_vec2, \ref bj_vec3 and \ref bj_vec4
///
/// This library is initially a direct adaptation of 
/// [linmath.h](https://github.com/datenwolf/linmath.h) 
/// by Wolfgang Draxinger, but since I added some new types.
///
/// \{
////////////////////////////////////////////////////////////////////////////////
#ifndef BJ_VEC_H
#define BJ_VEC_H

#include <banjo/api.h>
#include <banjo/math.h>

////////////////////////////////////////////////////////////////////////////////
/// \brief Defines a 2D vector type.
///
/// This type represents a 2D vector with two components, each of type `bj_real`.
/// It is commonly used for storing 2D coordinates, directions, or other 2D data.
///
/// \note The vector is represented as an array of two `bj_real` values.
/// \see bj_real
////////////////////////////////////////////////////////////////////////////////
typedef bj_real bj_vec2[2];

////////////////////////////////////////////////////////////////////////////////
/// \brief Defines a 3D vector type.
///
/// This type represents a 3D vector with three components, each of type `bj_real`.
/// It is commonly used for storing 3D coordinates, directions, or other 3D data.
///
/// \note The vector is represented as an array of three `bj_real` values.
/// \see bj_real
////////////////////////////////////////////////////////////////////////////////
typedef bj_real bj_vec3[3];

////////////////////////////////////////////////////////////////////////////////
/// \brief Defines a 4D vector type.
///
/// This type represents a 4D vector with four components, each of type `bj_real`.
/// It is commonly used for storing 4D homogeneous coordinates or other 4D data.
///
/// \note The vector is represented as an array of four `bj_real` values.
/// \see bj_real
////////////////////////////////////////////////////////////////////////////////
typedef bj_real bj_vec4[4];

////////////////////////////////////////////////////////////////////////////////
/// Directly set the scalar values of a \ref bj_vec2 object.
///
/// \param res A location to the target vec4
/// \param a   The first scalar value
/// \param b   The second scalar value
///
////////////////////////////////////////////////////////////////////////////////
static inline void bj_vec2_set(bj_vec2 res, bj_real a, bj_real b) {
    res[0] = a; res[1] = b;
}

////////////////////////////////////////////////////////////////////////////////
/// Invoke the given function to each scalar of the \ref bj_vec2.
///
/// \param res A location to the target vec2
/// \param a   A location to the source vec2
/// \param f   The map function
///
////////////////////////////////////////////////////////////////////////////////
static inline void bj_vec2_apply(bj_vec2 res, const bj_vec2 a, bj_real(*f)(bj_real)) {
    res[0] = f(a[0]);
    res[1] = f(a[1]);
}

////////////////////////////////////////////////////////////////////////////////
/// Set `res` to the result of `lhs`+`rhs`.
///
/// \param res The destination vec2
/// \param lhs The first source vec2
/// \param rhs The second source vec2
////////////////////////////////////////////////////////////////////////////////
static inline void bj_vec2_add(bj_vec2 res, const bj_vec2 lhs, const bj_vec2 rhs) {
    res[0] = lhs[0] + rhs[0];
    res[1] = lhs[1] + rhs[1];
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Computes `res = a + b * s` for 2D vectors.
///
/// This is the fused “add then scale” operation.
///
/// \param res The resulting vec2.
/// \param a   The base vec2.
/// \param b   The vec2 to be scaled.
/// \param s   The scalar factor.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_vec2_add_scaled(bj_vec2 res, const bj_vec2 a, const bj_vec2 b, bj_real s)
{
    res[0] = a[0] + b[0] * s;
    res[1] = a[1] + b[1] * s;
}

////////////////////////////////////////////////////////////////////////////////
/// Set `res` to the result of `lhs`-`rhs`.
///
/// \param res The destination vec2
/// \param lhs The first source vec2
/// \param rhs The second source vec2
////////////////////////////////////////////////////////////////////////////////
static inline void bj_vec2_sub(bj_vec2 res, const bj_vec2 lhs, const bj_vec2 rhs) {
    res[0] = lhs[0] - rhs[0];
    res[1] = lhs[1] - rhs[1];
}

////////////////////////////////////////////////////////////////////////////////
/// Set `res` to the result of multiplying `v` by `s`.
///
/// \param res The destination vec2
/// \param v   The source vec2
/// \param s   The factor value
////////////////////////////////////////////////////////////////////////////////
static inline void bj_vec2_scale(bj_vec2 res, const bj_vec2 v, bj_real s) {
    res[0] = v[0] * s;
    res[1] = v[1] * s;
}

////////////////////////////////////////////////////////////////////////////////
/// Multiply each scalar of `v` by the respecting scalar in `s`.
///
/// \param res The destination vec2
/// \param v   The source vec2
/// \param s   The scalar vec2
////////////////////////////////////////////////////////////////////////////////
static inline void bj_vec2_scale_each(bj_vec2 res, const bj_vec2 v, const bj_vec2 s) {
    res[0] = v[0] * s[0];
    res[1] = v[1] * s[1];
}

////////////////////////////////////////////////////////////////////////////////
/// Computes the dot product of `a` abnd  `b`
///
/// \param a   The first vec2
/// \param b   The second vec2
/// \return    The resulting dot product
////////////////////////////////////////////////////////////////////////////////
static inline bj_real bj_vec2_dot(const bj_vec2 a, const bj_vec2 b) {
    return a[0] * b[0] + a[1] * b[1];
}

////////////////////////////////////////////////////////////////////////////////
/// Computes the length of the vec2
///
/// \param v   The vec2
/// \return    The length
////////////////////////////////////////////////////////////////////////////////
static inline bj_real bj_vec2_len(const bj_vec2 v) {
    return bj_sqrt(v[0] * v[0] + v[1] * v[1]);
}

////////////////////////////////////////////////////////////////////////////////
/// Normalizes the provided vec2.
///
/// \param res   The resulting vec2
/// \param v     The vec2
////////////////////////////////////////////////////////////////////////////////
static inline void bj_vec2_normalize(bj_vec2 res, const bj_vec2 v) {
    bj_real inv_len = BJ_F(1.0) / bj_sqrt(v[0] * v[0] + v[1] * v[1]);
    res[0] = v[0] * inv_len;
    res[1] = v[1] * inv_len;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Computes the component-wise minimum of two 2D vectors.
///
/// \param res The result vector, where each component is the minimum of \a a and \a b.
/// \param a The first input vector.
/// \param b The second input vector.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_vec2_min(bj_vec2 res, const bj_vec2 a, const bj_vec2 b) {
    res[0] = a[0] < b[0] ? a[0] : b[0];
    res[1] = a[1] < b[1] ? a[1] : b[1];
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Computes the component-wise maximum of two 2D vectors.
///
/// \param res The result vector, where each component is the maximum of \a a and \a b.
/// \param a The first input vector.
/// \param b The second input vector.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_vec2_max(bj_vec2 res, const bj_vec2 a, const bj_vec2 b) {
    res[0] = a[0] > b[0] ? a[0] : b[0];
    res[1] = a[1] > b[1] ? a[1] : b[1];
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Copies the contents of one 2D vector to another.
///
/// \param res The destination vector.
/// \param src The source vector to copy.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_vec2_copy(bj_vec2 res, const bj_vec2 src) {
    res[0] = src[0];
    res[1] = src[1];
}

////////////////////////////////////////////////////////////////////////////////
/// Directly set the scalar values of a \ref bj_vec3 object.
///
/// \param res A location to the target vec4
/// \param a   The first scalar value
/// \param b   The second scalar value
/// \param c   The third scalar value
///
////////////////////////////////////////////////////////////////////////////////
static inline void bj_vec3_set(bj_vec3 res, bj_real a, bj_real b, bj_real c) {
    res[0] = a; res[1] = b; res[2] = c;
}

////////////////////////////////////////////////////////////////////////////////
/// Invoke the given function to each scalar of the \ref bj_vec3.
///
/// \param res A location to the target vec3
/// \param a   A location to the source vec3
/// \param f   The map function
///
////////////////////////////////////////////////////////////////////////////////
static inline void bj_vec3_apply(bj_vec3 res, const bj_vec3 a, bj_real(*f)(bj_real)) {
    res[0] = f(a[0]);
    res[1] = f(a[1]);
    res[2] = f(a[2]);
}


////////////////////////////////////////////////////////////////////////////////
/// Set `res` to the result of `lhs`+`rhs`.
///
/// \param res The destination vec3
/// \param lhs The first source vec3
/// \param rhs The second source vec3
////////////////////////////////////////////////////////////////////////////////
static inline void bj_vec3_add(bj_vec3 res, const bj_vec3 lhs, const bj_vec3 rhs) {
    res[0] = lhs[0] + rhs[0];
    res[1] = lhs[1] + rhs[1];
    res[2] = lhs[2] + rhs[2];
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Computes `res = a + b * s` for 3D vectors.
///
/// This is the fused “add then scale” operation.
///
/// \param res The resulting vec3.
/// \param a   The base vec3.
/// \param b   The vec3 to be scaled.
/// \param s   The scalar factor.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_vec3_add_scaled(bj_vec3 res, const bj_vec3 a, const bj_vec3 b, bj_real s) {
    res[0] = a[0] + b[0] * s;
    res[1] = a[1] + b[1] * s;
    res[2] = a[2] + b[2] * s;
}


////////////////////////////////////////////////////////////////////////////////
/// Set `res` to the result of `lhs`-`rhs`.
///
/// \param res The destination vec3
/// \param lhs The first source vec3
/// \param rhs The second source vec3
////////////////////////////////////////////////////////////////////////////////
static inline void bj_vec3_sub(bj_vec3 res, const bj_vec3 lhs, const bj_vec3 rhs) {
    res[0] = lhs[0] - rhs[0];
    res[1] = lhs[1] - rhs[1];
    res[2] = lhs[2] - rhs[2];
}

////////////////////////////////////////////////////////////////////////////////
/// Set `res` to the result of multiplying `v` by `s`.
///
/// \param res The destination vec3
/// \param v   The source vec3
/// \param s   The factor value
////////////////////////////////////////////////////////////////////////////////
static inline void bj_vec3_scale(bj_vec3 res, const bj_vec3 v, bj_real s) {
    res[0] = v[0] * s;
    res[1] = v[1] * s;
    res[2] = v[2] * s;
}

////////////////////////////////////////////////////////////////////////////////
/// Computes the dot product of `a` abnd  `b`
///
/// \param a   The first vec3
/// \param b   The second vec3
/// \return    The resulting dot product
////////////////////////////////////////////////////////////////////////////////
static inline bj_real bj_vec3_dot(const bj_vec3 a, const bj_vec3 b) {
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

////////////////////////////////////////////////////////////////////////////////
/// Computes the length of the vec3
///
/// \param v   The vec3
/// \return    The length
////////////////////////////////////////////////////////////////////////////////
static inline bj_real bj_vec3_len(const bj_vec3 v) {
    return bj_sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

////////////////////////////////////////////////////////////////////////////////
/// Normalizes the provided vec3.
///
/// \param res   The resulting vec3
/// \param v     The vec3
////////////////////////////////////////////////////////////////////////////////
static inline void bj_vec3_normalize(bj_vec3 res, const bj_vec3 v) {
    bj_real inv_len = BJ_F(1.0) / bj_sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    res[0] = v[0] * inv_len;
    res[1] = v[1] * inv_len;
    res[2] = v[2] * inv_len;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Computes the component-wise minimum of two 3D vectors.
///
/// \param res The result vector, where each component is the minimum of \a a and \a b.
/// \param a The first input vector.
/// \param b The second input vector.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_vec3_min(bj_vec3 res, const bj_vec3 a, const bj_vec3 b) {
    res[0] = a[0] < b[0] ? a[0] : b[0];
    res[1] = a[1] < b[1] ? a[1] : b[1];
    res[2] = a[2] < b[2] ? a[2] : b[2];
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Computes the component-wise maximum of two 3D vectors.
///
/// \param res The result vector, where each component is the maximum of \a a and \a b.
/// \param a The first input vector.
/// \param b The second input vector.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_vec3_max(bj_vec3 res, const bj_vec3 a, const bj_vec3 b) {
    res[0] = a[0] > b[0] ? a[0] : b[0];
    res[1] = a[1] > b[1] ? a[1] : b[1];
    res[2] = a[2] > b[2] ? a[2] : b[2];
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Copies the contents of one 3D vector to another.
///
/// \param res The destination vector.
/// \param src The source vector to copy.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_vec3_copy(bj_vec3 res, const bj_vec3 src) {
    res[0] = src[0];
    res[1] = src[1];
    res[2] = src[2];
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Computes the 3D cross product of two 3D vectors.
///
/// \param res The resulting vector (cross product of \a l and \a r).
/// \param l The left-hand input vector.
/// \param r The right-hand input vector.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_vec3_cross(bj_vec3 res, const bj_vec3 l, const bj_vec3 r)
{
    res[0] = l[1] * r[2] - l[2] * r[1];
    res[1] = l[2] * r[0] - l[0] * r[2];
    res[2] = l[0] * r[1] - l[1] * r[0];
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Reflects a 3D vector around a given normal.
///
/// Computes the reflection of vector \a v across the normal vector \a n.
/// Assumes both vectors are 3D, and uses all three components.
///
/// \param res The reflected vector.
/// \param v The incident vector.
/// \param n The normal vector (should be normalized).
////////////////////////////////////////////////////////////////////////////////
static inline void bj_vec3_reflect(bj_vec3 res, const bj_vec3 v, const bj_vec3 n)
{
    const bj_real p = BJ_F(2.) * bj_vec3_dot(v, n);
    for (int i = 0; i < 3; ++i) {
        res[i] = v[i] - p * n[i];
    }
}


////////////////////////////////////////////////////////////////////////////////
/// Directly set the scalar values of a \ref bj_vec4 object.
///
/// \param res A location to the target vec4
/// \param a   The first scalar value
/// \param b   The second scalar value
/// \param c   The third scalar value
/// \param d   The fourth scalar value
///
////////////////////////////////////////////////////////////////////////////////
static inline void bj_vec4_set(bj_vec4 res, bj_real a, bj_real b, bj_real c, bj_real d) {
    res[0] = a; res[1] = b; res[2] = c; res[3] = d;
}

////////////////////////////////////////////////////////////////////////////////
/// Invoke the given function to each scalar of the \ref bj_vec4.
///
/// \param res A location to the target vec4
/// \param a   A location to the source vec4
/// \param f   The map function
///
////////////////////////////////////////////////////////////////////////////////
static inline void bj_vec4_apply(bj_vec4 res, const bj_vec4 a, bj_real(*f)(bj_real)) {
    res[0] = f(a[0]);
    res[1] = f(a[1]);
    res[2] = f(a[2]);
    res[3] = f(a[3]);
}

////////////////////////////////////////////////////////////////////////////////
/// Set `res` to the result of `lhs`+`rhs`.
///
/// \param res The destination vec4
/// \param lhs The first source vec4
/// \param rhs The second source vec4
////////////////////////////////////////////////////////////////////////////////
static inline void bj_vec4_add(bj_vec4 res, const bj_vec4 lhs, const bj_vec4 rhs) {
    res[0] = lhs[0] + rhs[0];
    res[1] = lhs[1] + rhs[1];
    res[2] = lhs[2] + rhs[2];
    res[3] = lhs[3] + rhs[3];
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Computes `res = a + b * s` for 4D vectors.
///
/// This is the fused “add then scale” operation.
///
/// \param res The resulting vec4.
/// \param a   The base vec4.
/// \param b   The vec4 to be scaled.
/// \param s   The scalar factor.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_vec4_add_scaled(bj_vec4 res, const bj_vec4 a, const bj_vec4 b, bj_real s) {
    res[0] = a[0] + b[0] * s;
    res[1] = a[1] + b[1] * s;
    res[2] = a[2] + b[2] * s;
    res[3] = a[3] + b[3] * s;
}

////////////////////////////////////////////////////////////////////////////////
/// Set `res` to the result of `lhs`-`rhs`.
///
/// \param res The destination vec4
/// \param lhs The first source vec4
/// \param rhs The second source vec4
////////////////////////////////////////////////////////////////////////////////
static inline void bj_vec4_sub(bj_vec4 res, const bj_vec4 lhs, const bj_vec4 rhs) {
    res[0] = lhs[0] - rhs[0];
    res[1] = lhs[1] - rhs[1];
    res[2] = lhs[2] - rhs[2];
    res[3] = lhs[3] - rhs[3];
}

////////////////////////////////////////////////////////////////////////////////
/// Set `res` to the result of multiplying `v` by `s`.
///
/// \param res The destination vec2
/// \param v   The source vec2
/// \param s   The factor value
////////////////////////////////////////////////////////////////////////////////
static inline void bj_vec4_scale(bj_vec4 res, const bj_vec4 v, bj_real s) {
    res[0] = v[0] * s;
    res[1] = v[1] * s;
    res[2] = v[2] * s;
    res[3] = v[3] * s;
}

////////////////////////////////////////////////////////////////////////////////
/// Computes the dot product of `a` abnd  `b`
///
/// \param a   The first vec4
/// \param b   The second vec4
/// \return    The resulting dot product
////////////////////////////////////////////////////////////////////////////////
static inline bj_real bj_vec4_dot(const bj_vec4 a, const bj_vec4 b) {
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3];
}

////////////////////////////////////////////////////////////////////////////////
/// Computes the length of the vec4
///
/// \param v   The vec4
/// \return    The length
////////////////////////////////////////////////////////////////////////////////
static inline bj_real bj_vec4_len(const bj_vec4 v) {
    return bj_sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3]);
}

////////////////////////////////////////////////////////////////////////////////
/// Normalizes the provided vec4.
///
/// \param res   The resulting vec4
/// \param v     The vec4
////////////////////////////////////////////////////////////////////////////////
static inline void bj_vec4_normalize(bj_vec4 res, const bj_vec4 v) {
    bj_real inv_len = BJ_F(1.0) / bj_sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3]);
    res[0] = v[0] * inv_len;
    res[1] = v[1] * inv_len;
    res[2] = v[2] * inv_len;
    res[3] = v[3] * inv_len;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Computes the component-wise minimum of two 4D vectors.
///
/// \param res The result vector, where each component is the minimum of \a a and \a b.
/// \param a The first input vector.
/// \param b The second input vector.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_vec4_min(bj_vec4 res, const bj_vec4 a, const bj_vec4 b) {
    res[0] = a[0] < b[0] ? a[0] : b[0];
    res[1] = a[1] < b[1] ? a[1] : b[1];
    res[2] = a[2] < b[2] ? a[2] : b[2];
    res[3] = a[3] < b[3] ? a[3] : b[3];
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Computes the component-wise maximum of two 4D vectors.
///
/// \param res The result vector, where each component is the maximum of \a a and \a b.
/// \param a The first input vector.
/// \param b The second input vector.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_vec4_max(bj_vec4 res, const bj_vec4 a, const bj_vec4 b) {
    res[0] = a[0] > b[0] ? a[0] : b[0];
    res[1] = a[1] > b[1] ? a[1] : b[1];
    res[2] = a[2] > b[2] ? a[2] : b[2];
    res[3] = a[3] > b[3] ? a[3] : b[3];
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Copies the contents of one 4D vector to another.
///
/// \param res The destination vector.
/// \param src The source vector to copy.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_vec4_copy(bj_vec4 res, const bj_vec4 src) {
    res[0] = src[0];
    res[1] = src[1];
    res[2] = src[2];
    res[3] = src[3];
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Computes the 3D cross product of two 4D vectors, assuming w = 1.0.
///
/// Only the x, y, and z components are used for the cross product. The w component
/// of the result is set to 1.0.
///
/// \param res The resulting vector.
/// \param l The left-hand input vector.
/// \param r The right-hand input vector.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_vec4_cross(bj_vec4 res, const bj_vec4 l, const bj_vec4 r)
{
    res[0] = l[1] * r[2] - l[2] * r[1];
    res[1] = l[2] * r[0] - l[0] * r[2];
    res[2] = l[0] * r[1] - l[1] * r[0];
    res[3] = BJ_F(1.0);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Reflects a 4D vector around a given normal.
///
/// Computes the reflection of vector \a v across the normal vector \a n.
/// Assumes both vectors are 4D, and uses all four components.
///
/// \param res The reflected vector.
/// \param v The incident vector.
/// \param n The normal vector (should be normalized).
////////////////////////////////////////////////////////////////////////////////
static inline void bj_vec4_reflect(bj_vec4 res, const bj_vec4 v, const bj_vec4 n)
{
    bj_real p = BJ_F(2.0) * bj_vec4_dot(v, n);
    for (int i = 0; i < 4; ++i) {
        res[i] = v[i] - p * n[i];
    }
}

/// \}

#endif

