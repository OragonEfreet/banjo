////////////////////////////////////////////////////////////////////////////////
/// \file linmath.h
/// Linear math library.
////////////////////////////////////////////////////////////////////////////////
/// \defgroup math Math
/// \ingroup core
///
/// \brief Linear math function
///
/// The Math group provides usual facilities for linear math, including
/// - vector types: \ref bj_vec2, \ref bj_vec3 and \ref bj_vec4
/// - 3x3 matrix with \ref bj_mat3
/// - 4x4 matrix with \ref bj_mat4
/// - Quaternion with \ref bj_quat
///
/// This library is a _direct_ conversion of 
/// [linmath.h](https://github.com/datenwolf/linmath.h) by Wolfgang Draxinger.
///
/// \{
////////////////////////////////////////////////////////////////////////////////
#ifndef BJ_LINMATH_H
#define BJ_LINMATH_H

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
/// \brief Defines a 3x3 matrix type.
///
/// This type represents a 3x3 matrix, where each column is a `bj_vec3` vector.
/// It is commonly used for transformations such as rotations, scaling, and translations.
///
/// \note The matrix is represented as an array of four `bj_vec3` values.
/// \see bj_vec3
////////////////////////////////////////////////////////////////////////////////
typedef bj_vec3 bj_mat3[3];

////////////////////////////////////////////////////////////////////////////////
/// \brief Defines a 4x4 matrix type.
///
/// This type represents a 4x4 matrix, where each column is a `bj_vec4` vector.
/// It is commonly used for transformations such as rotations, scaling, and translations.
///
/// \note The matrix is represented as an array of four `bj_vec4` values.
/// \see bj_vec4
////////////////////////////////////////////////////////////////////////////////
typedef bj_vec4 bj_mat4[4];

////////////////////////////////////////////////////////////////////////////////
/// \brief Defines a quaternion type.
///
/// This type represents a quaternion with four components, each of type `bj_real`.
/// Quaternions are used for representing rotations in 3D space.
///
/// \note The quaternion is represented as an array of four `bj_real` values.
/// \see bj_real
////////////////////////////////////////////////////////////////////////////////
typedef bj_real bj_quat[4];

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

////////////////////////////////////////////////////////////////////////////////
/// \brief Initializes a 3x3 matrix to the identity matrix.
///
/// Sets all diagonal elements to 1 and off-diagonal to 0.
///
/// \param m The matrix to initialize.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3_identity(bj_mat3 m) {
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            m[i][j] = (i == j) ? BJ_F(1.0) : BJ_F(0.0);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Copies a 3x3 matrix.
///
/// \param to   Destination matrix.
/// \param from Source matrix.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3_copy(bj_mat3 to, const bj_mat3 from) {
    for (int i = 0; i < 3; ++i) {
        to[i][0] = from[i][0];
        to[i][1] = from[i][1];
        to[i][2] = from[i][2];
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Extracts a row from a 3x3 matrix.
///
/// Column-major convention: row r contains elements m[0..2][r].
///
/// \param res Output row (vec3).
/// \param m   Source matrix.
/// \param r   Row index in [0,2].
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3_row(bj_vec3 res, const bj_mat3 m, int r) {
    for (int k = 0; k < 3; ++k) res[k] = m[k][r];
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Extracts a column from a 3x3 matrix.
///
/// \param res Output column (vec3).
/// \param m   Source matrix.
/// \param c   Column index in [0,2].
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3_col(bj_vec3 res, const bj_mat3 m, int c) {
    for (int k = 0; k < 3; ++k) res[k] = m[c][k];
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Transposes a 3x3 matrix.
///
/// \param res Output transposed matrix.
/// \param m   Input matrix.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3_transpose(bj_mat3 res, const bj_mat3 m) {
    for (int j = 0; j < 3; ++j)
        for (int i = 0; i < 3; ++i)
            res[i][j] = m[j][i];
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Element-wise addition of two 3x3 matrices.
///
/// \param res Output matrix (a + b).
/// \param a   Left operand.
/// \param b   Right operand.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3_add(bj_mat3 res, const bj_mat3 a, const bj_mat3 b) {
    for (int i = 0; i < 3; ++i) {
        res[i][0] = a[i][0] + b[i][0];
        res[i][1] = a[i][1] + b[i][1];
        res[i][2] = a[i][2] + b[i][2];
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Element-wise subtraction of two 3x3 matrices.
///
/// \param res Output matrix (a - b).
/// \param a   Left operand.
/// \param b   Right operand.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3_sub(bj_mat3 res, const bj_mat3 a, const bj_mat3 b) {
    for (int i = 0; i < 3; ++i) {
        res[i][0] = a[i][0] - b[i][0];
        res[i][1] = a[i][1] - b[i][1];
        res[i][2] = a[i][2] - b[i][2];
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Scales a 3x3 matrix by a scalar (element-wise).
///
/// \param res Output matrix.
/// \param m   Input matrix.
/// \param k   Scalar multiplier.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3_scale(bj_mat3 res, const bj_mat3 m, bj_real k) {
    for (int i = 0; i < 3; ++i) {
        res[i][0] = m[i][0] * k;
        res[i][1] = m[i][1] * k;
        res[i][2] = m[i][2] * k;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Multiplies two 3x3 matrices (res = lhs * rhs).
///
/// Column-major, column-vector convention (matches bj_mat4_mul).
///
/// \param res Output product.
/// \param lhs Left operand.
/// \param rhs Right operand.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3_mul(
    bj_real       res[restrict 3][3],
    const bj_real lhs[restrict 3][3],
    const bj_real rhs[restrict 3][3]
) {
    bj_mat3 tmp;
    for (int c = 0; c < 3; ++c) {
        for (int r = 0; r < 3; ++r) {
            tmp[c][r] = BJ_F(0.0);
            for (int k = 0; k < 3; ++k)
                tmp[c][r] += lhs[k][r] * rhs[c][k];
        }
    }
    bj_mat3_copy(res, tmp);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Multiplies a 3x3 matrix by a 3D vector.
///
/// \param res Output vector (m * v).
/// \param m   Matrix.
/// \param v   Vector.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3_mul_vec3(
    bj_real       res[restrict 3],
    const bj_real m[restrict 3][3],
    const bj_real v[restrict 3]
) {
    for (int j = 0; j < 3; ++j) {
        res[j] = BJ_F(0.0);
        for (int i = 0; i < 3; ++i)
            res[j] += m[i][j] * v[i];
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Transforms a 2D point by a 3x3 homogeneous matrix.
///
/// Uses (x,y,1) and performs homogeneous divide. If w==0, returns the
/// un-divided x,y.
///
/// \param res Output point (2D).
/// \param m   Matrix.
/// \param p   Input point (2D).
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3_mul_point(bj_vec2 res, const bj_mat3 m, const bj_vec2 p) {
    bj_vec3 v = { p[0], p[1], BJ_F(1.0) };
    bj_vec3 o;
    bj_mat3_mul_vec3(o, m, v);
    bj_real w = o[2];
    if (w != BJ_F(0.0)) { res[0] = o[0] / w; res[1] = o[1] / w; }
    else                { res[0] = o[0];     res[1] = o[1];     }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Transforms a 2D direction by a 3x3 homogeneous matrix.
///
/// Uses (x,y,0), which ignores translation.
///
/// \param res Output direction (2D).
/// \param m   Matrix.
/// \param v2  Input direction (2D).
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3_mul_vector2(bj_vec2 res, const bj_mat3 m, const bj_vec2 v2) {
    bj_vec3 v = { v2[0], v2[1], BJ_F(0.0) };
    bj_vec3 o; bj_mat3_mul_vec3(o, m, v);
    res[0] = o[0]; res[1] = o[1];
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Creates a 2D translation matrix.
///
/// \param res Output matrix.
/// \param tx  Translation along X.
/// \param ty  Translation along Y.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3_translation(bj_mat3 res, bj_real tx, bj_real ty) {
    bj_mat3_identity(res);
    res[2][0] = tx;
    res[2][1] = ty;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Applies a 2D translation in-place (M ← M * T(tx,ty)).
///
/// Column-major: updates the last column using current rows of M.
///
/// \param M  Matrix to modify.
/// \param tx Translation along X.
/// \param ty Translation along Y.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3_translation_inplace(bj_mat3 M, bj_real tx, bj_real ty) {
    bj_vec3 t = { tx, ty, BJ_F(0.0) };
    bj_vec3 r;
    for (int i = 0; i < 3; ++i) {
        bj_mat3_row(r, M, i);
        M[2][i] += r[0]*t[0] + r[1]*t[1] + r[2]*t[2];
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Creates a 2D scaling matrix.
///
/// \param res Output matrix.
/// \param sx  Scale along X.
/// \param sy  Scale along Y.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3_scale_xy(bj_mat3 res, bj_real sx, bj_real sy) {
    bj_mat3_identity(res);
    res[0][0] = sx;
    res[1][1] = sy;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Creates a 2D shear matrix.
///
/// shy applies y += shy * x, shx applies x += shx * y.
///
/// \param res Output matrix.
/// \param shx Shear factor in X (by Y).
/// \param shy Shear factor in Y (by X).
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3_shear(bj_mat3 res, bj_real shx, bj_real shy) {
    bj_mat3_identity(res);
    res[1][0] = shy;
    res[0][1] = shx;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Creates a 2D rotation matrix around the origin (CCW, radians).
///
/// \param res   Output matrix.
/// \param angle Angle in radians (counterclockwise).
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3_rotate(bj_mat3 res, bj_real angle) {
    bj_real s = bj_sin(angle), c = bj_cos(angle);
    /* [ c  s  0 ]
       [-s  c  0 ]
       [ 0  0  1 ] */
    res[0][0] =  c;  res[0][1] =  s;  res[0][2] = BJ_F(0.0);
    res[1][0] = -s;  res[1][1] =  c;  res[1][2] = BJ_F(0.0);
    res[2][0] = BJ_F(0.0); res[2][1] = BJ_F(0.0); res[2][2] = BJ_F(1.0);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Inverts a 3x3 matrix.
///
/// \warning The matrix must be invertible (determinant != 0).
///
/// \param res Output inverse matrix.
/// \param m   Input matrix.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3_inverse(bj_mat3 res, const bj_mat3 m) {
    bj_real a00 = m[0][0], a01 = m[0][1], a02 = m[0][2];
    bj_real a10 = m[1][0], a11 = m[1][1], a12 = m[1][2];
    bj_real a20 = m[2][0], a21 = m[2][1], a22 = m[2][2];

    bj_real b01 =  a22*a11 - a12*a21;
    bj_real b11 = -a22*a10 + a12*a20;
    bj_real b21 =  a21*a10 - a11*a20;

    bj_real det = a00*b01 + a01*b11 + a02*b21;
    bj_real inv_det = BJ_F(1.0) / det;

    res[0][0] = b01 * inv_det;
    res[0][1] = (-a22*a01 + a02*a21) * inv_det;
    res[0][2] = ( a12*a01 - a02*a11) * inv_det;

    res[1][0] = b11 * inv_det;
    res[1][1] = ( a22*a00 - a02*a20) * inv_det;
    res[1][2] = (-a12*a00 + a02*a10) * inv_det;

    res[2][0] = b21 * inv_det;
    res[2][1] = (-a21*a00 + a01*a20) * inv_det;
    res[2][2] = ( a11*a00 - a01*a10) * inv_det;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Generates a 2D orthographic transform to NDC [-1,1]^2 with Y down
///
/// Maps world box [l..r]×[b..t] to NDC such that:
///   x_ndc =  (2x - r - l) / (r - l)
///   y_ndc = -(2y - t - b) / (t - b)    // Y down: top -> -1, bottom -> +1
///
/// Column-major 3×3:
/// [  2/(r-l)        0              0 ]
/// [     0       -2/(t-b)           0 ]
/// [ -(r+l)/(r-l)  (t+b)/(t-b)      1 ]
///
/// \param omat Output 3x3 matrix
/// \param l    Left   world bound
/// \param r    Right  world bound
/// \param b    Bottom world bound
/// \param t    Top    world bound
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3_ortho(bj_real omat[restrict 3][3],
                                 bj_real l, bj_real r,
                                 bj_real b, bj_real t)
{
    omat[0][0] = BJ_F(2.0) / (r - l);
    omat[0][1] = BJ_F(0.0);
    omat[0][2] = BJ_F(0.0);

    omat[1][0] = BJ_F(0.0);
    omat[1][1] = BJ_F(-2.0) / (t - b);
    omat[1][2] = BJ_F(0.0);

    omat[2][0] = -(r + l) / (r - l);
    omat[2][1] =  (t + b) / (t - b);
    omat[2][2] = BJ_F(1.0);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Builds a 2D viewport transform (NDC -> window pixels; top-left origin)
///
/// Vulkan-like mapping with **no Y flip** here:
///   x_win = x + (x_ndc + 1) * (w/2)
///   y_win = y + (y_ndc + 1) * (h/2)
///
/// Column-major 3×3:
/// [  w/2    0     0 ]
/// [   0    h/2    0 ]
/// [ x+w/2  y+h/2  1 ]
///
/// \param vpmat Output 3x3 viewport matrix
/// \param x     Viewport origin X in pixels
/// \param y     Viewport origin Y in pixels
/// \param w     Viewport width  in pixels
/// \param h     Viewport height in pixels
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3_viewport(bj_real vpmat[restrict 3][3],
                                    bj_real x, bj_real y,
                                    bj_real w, bj_real h)
{
    vpmat[0][0] = BJ_F(0.5) * w;  vpmat[0][1] = BJ_F(0.0);       vpmat[0][2] = BJ_F(0.0);
    vpmat[1][0] = BJ_F(0.0);      vpmat[1][1] = BJ_F(0.5) * h;   vpmat[1][2] = BJ_F(0.0);
    vpmat[2][0] = x + BJ_F(0.5) * w;
    vpmat[2][1] = y + BJ_F(0.5) * h;
    vpmat[2][2] = BJ_F(1.0);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Initializes a 4x4 matrix to the identity matrix.
///
/// Sets all diagonal elements of the matrix to 1.0 and all off-diagonal elements to 0.0.
///
/// \param mat The matrix to be initialized as the identity matrix.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_identity(bj_mat4 mat)
{
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            mat[i][j] = i == j ? BJ_F(1.0) : BJ_F(0.0);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Copies a 4x4 matrix from one to another.
///
/// Copies the entire contents of the matrix \a from to the matrix \a to.
///
/// \param to The destination matrix.
/// \param from The source matrix to copy from.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_copy(bj_mat4 to, const bj_mat4 from)
{
    for (int i = 0; i < 4; ++i) {
        bj_vec4_copy(to[i], from[i]);
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Retrieves a specific row from a 4x4 matrix.
///
/// Extracts the row at index \a r from matrix \a mat and stores it in \a res.
///
/// \param res The resulting row vector.
/// \param mat The matrix from which to extract the row.
/// \param r The row index (0-based) to extract.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_row(bj_vec4 res, const bj_mat4 mat, int r)
{
    for (int k = 0; k < 4; ++k) {
        res[k] = mat[k][r];
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Retrieves a specific column from a 4x4 matrix.
///
/// Extracts the column at index \a c from matrix \a mat and stores it in \a res.
///
/// \param res The resulting column vector.
/// \param mat The matrix from which to extract the column.
/// \param c The column index (0-based) to extract.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_col(bj_vec4 res, const bj_mat4 mat, int c)
{
    for (int k = 0; k < 4; ++k) {
        res[k] = mat[c][k];
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Transposes a 4x4 matrix.
///
/// Computes the transpose of matrix \a mat and stores it in \a res.
/// The rows of \a mat become the columns of \a res.
///
/// \param res The transposed matrix.
/// \param mat The matrix to transpose.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_transpose(bj_mat4 res, const bj_mat4 mat)
{
    for (int j = 0; j < 4; ++j) {
        for (int i = 0; i < 4; ++i) {
            res[i][j] = mat[j][i];
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Adds two 4x4 matrices.
///
/// Computes the element-wise addition of two matrices \a lhs and \a rhs and stores the result in \a res.
///
/// \param res The resulting matrix after addition.
/// \param lhs The left-hand matrix to add.
/// \param rhs The right-hand matrix to add.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_add(bj_mat4 res, const bj_mat4 lhs, const bj_mat4 rhs)
{
    for (int i = 0; i < 4; ++i) {
        bj_vec4_add(res[i], lhs[i], rhs[i]);
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Subtracts one 4x4 matrix from another.
///
/// Computes the element-wise subtraction of two matrices \a lhs and \a rhs and stores the result in \a res.
///
/// \param res The resulting matrix after subtraction.
/// \param lhs The matrix to subtract from.
/// \param rhs The matrix to subtract.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_sub(bj_mat4 res, const bj_mat4 lhs, const bj_mat4 rhs)
{
    for (int i = 0; i < 4; ++i) {
        bj_vec4_sub(res[i], lhs[i], rhs[i]);
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Scales a 4x4 matrix by a scalar.
///
/// Multiplies each element of the matrix \a lhs by the scalar \a k and stores the result in \a res.
///
/// \param res The resulting scaled matrix.
/// \param lhs The matrix to scale.
/// \param k The scalar by which to scale the matrix.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_scale(bj_mat4 res, const bj_mat4 lhs, bj_real k)
{
    for (int i = 0; i < 4; ++i) {
        bj_vec4_scale(res[i], lhs[i], k);
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Scales the X, Y, and Z components of a 4x4 matrix.
///
/// Scales the X, Y, and Z components of the matrix \a mat by \a x, \a y, and \a z respectively.
/// The W component is left unchanged and copied directly from \a mat.
///
/// \param res The resulting scaled matrix.
/// \param mat The matrix to scale.
/// \param x The scaling factor for the X component.
/// \param y The scaling factor for the Y component.
/// \param z The scaling factor for the Z component.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_scale_xyz(bj_mat4 res, const bj_mat4 mat, bj_real x, bj_real y, bj_real z)
{
    bj_vec4_scale(res[0], mat[0], x);
    bj_vec4_scale(res[1], mat[1], y);
    bj_vec4_scale(res[2], mat[2], z);
    bj_vec4_copy(res[3], mat[3]);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Multiplies two 4x4 matrices.
///
/// Computes the matrix product of \a lhs and \a rhs and stores the result in \a res.
///
/// \param res The resulting matrix after multiplication.
/// \param lhs The left-hand matrix.
/// \param rhs The right-hand matrix.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_mul(
    bj_real       res[restrict 4][4],
    const bj_real lhs[restrict 4][4],
    const bj_real rhs[restrict 4][4]
) {
    bj_mat4 temp;
    for (int c = 0; c < 4; ++c) {
        for (int r = 0; r < 4; ++r) {
            temp[c][r] = BJ_F(0.0);
            for (int k = 0; k < 4; ++k)
                temp[c][r] += lhs[k][r] * rhs[c][k];
        }
    }
    bj_mat4_copy(res, temp);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Multiplies a 4x4 matrix by a 4D vector.
///
/// Computes the matrix-vector product of matrix \a mat and vector \a v and stores the result in \a res.
///
/// \param res The resulting vector after multiplication.
/// \param mat The matrix to multiply.
/// \param v The vector to multiply.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_mul_vec4(
    bj_real       res[restrict 4],
    const bj_real mat[restrict 4][4],
    const bj_real v[restrict 4]
) {
    for (int j = 0; j < 4; ++j) {
        res[j] = BJ_F(0.0);
        for (int i = 0; i < 4; ++i) {
            res[j] += mat[i][j] * v[i];
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Creates a 4x4 translation matrix.
///
/// Creates a translation matrix that translates a point by \a x, \a y, and \a z in 3D space.
///
/// \param res The resulting translation matrix.
/// \param x The translation distance along the X axis.
/// \param y The translation distance along the Y axis.
/// \param z The translation distance along the Z axis.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_translation(bj_mat4 res, bj_real x, bj_real y, bj_real z)
{
    bj_mat4_identity(res);
    res[3][0] = x;
    res[3][1] = y;
    res[3][2] = z;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Applies a translation transformation to a 4x4 matrix in-place.
///
/// This function adds a translation by \a x, \a y, and \a z to the matrix \a M in-place.
/// The translation vector is applied by modifying the last column of the matrix.
///
/// \param[in,out] M The matrix to apply the translation to (modified in-place).
/// \param x The translation distance along the X axis.
/// \param y The translation distance along the Y axis.
/// \param z The translation distance along the Z axis.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_translation_inplace(bj_mat4 M, bj_real x, bj_real y, bj_real z)
{
    bj_vec4 t = { x, y, z, BJ_F(0.0) };
    bj_vec4 r;
    for (int i = 0; i < 4; ++i) {
        bj_mat4_row(r, M, i);
        M[3][i] += bj_vec4_dot(r, t);
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Computes the outer product of two 3D vectors and stores the result in a 4x4 matrix.
///
/// Computes the outer product of vectors \a a and \a b and stores the result in matrix \a res.
/// The result is a matrix where elements are the products of corresponding components of the vectors.
///
/// \param res The resulting 4x4 matrix from the outer product.
/// \param a The first 3D vector.
/// \param b The second 3D vector.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_mul_outer(bj_mat4 res, const bj_vec3 a, const bj_vec3 b)
{
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            res[i][j] = i < 3 && j < 3 ? a[i] * b[j] : BJ_F(0.0);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Rotates a 4x4 matrix by a given axis and angle.
///
/// This function performs a 3D rotation on matrix \a mat around an arbitrary axis \a (x, y, z) by \a angle (in radians),
/// and stores the resulting matrix in \a res. The axis vector is normalized before the rotation.
///
/// \param res The resulting rotated matrix.
/// \param mat The matrix to rotate.
/// \param x The X component of the axis of rotation.
/// \param y The Y component of the axis of rotation.
/// \param z The Z component of the axis of rotation.
/// \param angle The angle to rotate by (in radians).
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_rotate(bj_mat4 res, const bj_mat4 mat, bj_real x, bj_real y, bj_real z, bj_real angle)
{
    bj_real s = bj_sin(angle);
    bj_real c = bj_cos(angle);
    bj_vec3 u = { x, y, z };

    if (bj_vec3_len(u) > BJ_F(1e-4)) {
        bj_vec3_normalize(u, u);
        bj_mat4 T;
        bj_mat4_mul_outer(T, u, u);

        bj_mat4 S = {
            {    BJ_F(0),  u[2], -u[1], BJ_F(0)},
            {-u[2],     BJ_F(0),  u[0], BJ_F(0)},
            { u[1], -u[0],     BJ_F(0), BJ_F(0)},
            {    BJ_F(0),     BJ_F(0),     BJ_F(0), BJ_F(0)}
        };
        bj_mat4_scale(S, S, s);

        bj_mat4 C;
        bj_mat4_identity(C);
        bj_mat4_sub(C, C, T);

        bj_mat4_scale(C, C, c);

        bj_mat4_add(T, T, C);
        bj_mat4_add(T, T, S);

        T[3][3] = BJ_F(1.0);
        bj_mat4_mul(res, mat, T);
    } else {
        bj_mat4_copy(res, mat);
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Rotates a 4x4 matrix around the X axis by a given angle.
///
/// This function performs a rotation of matrix \a mat around the X axis by \a angle (in radians),
/// and stores the resulting matrix in \a res.
///
/// \param res The resulting rotated matrix.
/// \param mat The matrix to rotate.
/// \param angle The angle to rotate by (in radians).
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_rotate_x(bj_mat4 res, const bj_mat4 mat, bj_real angle)
{
    bj_real s = bj_sin(angle);
    bj_real c = bj_cos(angle);
    bj_mat4 R = {
        {BJ_F(1.0), BJ_F(0.0), BJ_F(0.0), BJ_F(0.0)},
        {BJ_F(0.0),   c,   s, BJ_F(0.0)},
        {BJ_F(0.0),  -s,   c, BJ_F(0.0)},
        {BJ_F(0.0), BJ_F(0.0), BJ_F(0.0), BJ_F(1.0)}
    };
    bj_mat4_mul(res, mat, R);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Rotates a 4x4 matrix around the Y axis by a given angle.
///
/// This function performs a rotation of matrix \a mat around the Y axis by \a angle (in radians),
/// and stores the resulting matrix in \a res.
///
/// \param res The resulting rotated matrix.
/// \param mat The matrix to rotate.
/// \param angle The angle to rotate by (in radians).
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_rotate_y(bj_mat4 res, const bj_mat4 mat, bj_real angle)
{
    bj_real s = bj_sin(angle);
    bj_real c = bj_cos(angle);
    bj_mat4 R = {
        {   c, BJ_F(0.0),  -s, BJ_F(0.0)},
        { BJ_F(0.0), BJ_F(1.0), BJ_F(0.0), BJ_F(0.0)},
        {   s, BJ_F(0.0),   c, BJ_F(0.0)},
        { BJ_F(0.0), BJ_F(0.0), BJ_F(0.0), BJ_F(1.0)}
    };
    bj_mat4_mul(res, mat, R);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Rotates a 4x4 matrix around the Z-axis by a given angle.
///
/// This function applies a rotation to the input matrix around the Z-axis by
/// a specified angle and stores the result in the output matrix.
///
/// \param mat The 4x4 matrix to rotate.
/// \param angle The angle (in radians) to rotate the matrix around the Z-axis.
/// \param res The resulting rotated matrix.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_rotate_z(bj_mat4 res, const bj_mat4 mat, bj_real angle)
{
    bj_real s = bj_sin(angle);
    bj_real c = bj_cos(angle);
    bj_mat4 R = {
        {   c,   s, BJ_F(0.0), BJ_F(0.0)},
        {  -s,   c, BJ_F(0.0), BJ_F(0.0)},
        { BJ_F(0.0), BJ_F(0.0), BJ_F(1.0), BJ_F(0.0)},
        { BJ_F(0.0), BJ_F(0.0), BJ_F(0.0), BJ_F(1.0)}
    };
    bj_mat4_mul(res, mat, R);
}


////////////////////////////////////////////////////////////////////////////////
/// \brief Rotates a matrix based on arcball-style rotation from two 2D vectors.
///
/// This function computes the rotation matrix that represents the arcball
/// rotation based on two 2D input vectors (a, b), the input matrix, and a scaling factor.
/// It normalizes the vectors and calculates the rotation axis and angle,
/// applying the rotation to the matrix.
///
/// \param M The input matrix to rotate.
/// \param _a The first 2D vector for the arcball rotation.
/// \param _b The second 2D vector for the arcball rotation.
/// \param s A scaling factor for the rotation angle.
/// \param R The resulting rotation matrix.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_rotate_arcball(bj_mat4 R, const bj_mat4 M, bj_vec2 const _a, bj_vec2 const _b, bj_real s)
{
    bj_vec2 a = { _a[0], _a[1] };
    bj_vec2 b = { _b[0], _b[1] };;

    bj_real z_a = BJ_F(0.0);
    bj_real z_b = BJ_F(0.0);

    if (bj_vec2_len(a) < BJ_F(1.0)) {
        z_a = bj_sqrt(BJ_F(1.0) - bj_vec2_dot(a, a));
    }
    else {
        bj_vec2_normalize(a, a);
    }

    if (bj_vec2_len(b) < BJ_F(1.0)) {
        z_b = bj_sqrt(BJ_F(1.0) - bj_vec2_dot(b, b));
    }
    else {
        bj_vec2_normalize(b, b);
    }

    bj_vec3 a_ = { a[0], a[1], z_a };
    bj_vec3 b_ = { b[0], b[1], z_b };

    bj_vec3 c_;
    bj_vec3_cross(c_, a_, b_);

    bj_real const angle = bj_acos(bj_vec3_dot(a_, b_)) * s;
    bj_mat4_rotate(R, M, c_[0], c_[1], c_[2], angle);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Inverts a 4x4 matrix.
///
/// This function calculates the inverse of the given 4x4 matrix and stores it
/// in the result matrix. It uses a standard matrix inversion method for 4x4 matrices.
///
/// \param mat The matrix to invert.
/// \param res The resulting inverted matrix.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_inverse(bj_mat4 res, const bj_mat4 mat) {
    bj_real s[6];
    bj_real c[6];
    s[0] = mat[0][0] * mat[1][1] - mat[1][0] * mat[0][1];
    s[1] = mat[0][0] * mat[1][2] - mat[1][0] * mat[0][2];
    s[2] = mat[0][0] * mat[1][3] - mat[1][0] * mat[0][3];
    s[3] = mat[0][1] * mat[1][2] - mat[1][1] * mat[0][2];
    s[4] = mat[0][1] * mat[1][3] - mat[1][1] * mat[0][3];
    s[5] = mat[0][2] * mat[1][3] - mat[1][2] * mat[0][3];

    c[0] = mat[2][0] * mat[3][1] - mat[3][0] * mat[2][1];
    c[1] = mat[2][0] * mat[3][2] - mat[3][0] * mat[2][2];
    c[2] = mat[2][0] * mat[3][3] - mat[3][0] * mat[2][3];
    c[3] = mat[2][1] * mat[3][2] - mat[3][1] * mat[2][2];
    c[4] = mat[2][1] * mat[3][3] - mat[3][1] * mat[2][3];
    c[5] = mat[2][2] * mat[3][3] - mat[3][2] * mat[2][3];

    bj_real idet = BJ_F(1.0) / (s[0] * c[5] - s[1] * c[4] + s[2] * c[3] + s[3] * c[2] - s[4] * c[1] + s[5] * c[0]);

    res[0][0] = (mat[1][1] * c[5] - mat[1][2] * c[4] + mat[1][3] * c[3]) * idet;
    res[0][1] = (-mat[0][1] * c[5] + mat[0][2] * c[4] - mat[0][3] * c[3]) * idet;
    res[0][2] = (mat[3][1] * s[5] - mat[3][2] * s[4] + mat[3][3] * s[3]) * idet;
    res[0][3] = (-mat[2][1] * s[5] + mat[2][2] * s[4] - mat[2][3] * s[3]) * idet;

    res[1][0] = (-mat[1][0] * c[5] + mat[1][2] * c[2] - mat[1][3] * c[1]) * idet;
    res[1][1] = (mat[0][0] * c[5] - mat[0][2] * c[2] + mat[0][3] * c[1]) * idet;
    res[1][2] = (-mat[3][0] * s[5] + mat[3][2] * s[2] - mat[3][3] * s[1]) * idet;
    res[1][3] = (mat[2][0] * s[5] - mat[2][2] * s[2] + mat[2][3] * s[1]) * idet;

    res[2][0] = (mat[1][0] * c[4] - mat[1][1] * c[2] + mat[1][3] * c[0]) * idet;
    res[2][1] = (-mat[0][0] * c[4] + mat[0][1] * c[2] - mat[0][3] * c[0]) * idet;
    res[2][2] = (mat[3][0] * s[4] - mat[3][1] * s[2] + mat[3][3] * s[0]) * idet;
    res[2][3] = (-mat[2][0] * s[4] + mat[2][1] * s[2] - mat[2][3] * s[0]) * idet;

    res[3][0] = (-mat[1][0] * c[3] + mat[1][1] * c[1] - mat[1][2] * c[0]) * idet;
    res[3][1] = (mat[0][0] * c[3] - mat[0][1] * c[1] + mat[0][2] * c[0]) * idet;
    res[3][2] = (-mat[3][0] * s[3] + mat[3][1] * s[1] - mat[3][2] * s[0]) * idet;
    res[3][3] = (mat[2][0] * s[3] - mat[2][1] * s[1] + mat[2][2] * s[0]) * idet;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Orthonormalizes the rows of a 4x4 matrix.
///
/// This function orthonormalizes the rows of the matrix, ensuring that the 
/// matrix rows are orthogonal and normalized.
///
/// \param mat The 4x4 matrix to orthonormalize.
/// \param res The resulting orthonormalized 4x4 matrix.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_orthonormalize(bj_mat4 res, const bj_mat4 mat) {
    bj_mat4_copy(res, mat);
    bj_real s = BJ_F(1.0);
    bj_vec3 h;

    bj_vec3_normalize(res[2], res[2]);

    s = bj_vec3_dot(res[1], res[2]);
    bj_vec3_scale(h, res[2], s);
    bj_vec3_sub(res[1], res[1], h);
    bj_vec3_normalize(res[1], res[1]);

    s = bj_vec3_dot(res[0], res[2]);
    bj_vec3_scale(h, res[2], s);
    bj_vec3_sub(res[0], res[0], h);

    s = bj_vec3_dot(res[0], res[1]);
    bj_vec3_scale(h, res[1], s);
    bj_vec3_sub(res[0], res[0], h);
    bj_vec3_normalize(res[0], res[0]);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Generates a perspective projection matrix for a frustum.
///
/// This function generates a perspective projection matrix based on a frustum
/// defined by the left, right, bottom, top, near, and far clipping planes.
///
/// \param l The left plane of the frustum.
/// \param r The right plane of the frustum.
/// \param b The bottom plane of the frustum.
/// \param t The top plane of the frustum.
/// \param n The near plane of the frustum.
/// \param f The far plane of the frustum.
/// \param fmat The resulting perspective projection matrix.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_frustum(bj_real fmat[restrict 4][4],
                                   bj_real l, bj_real r,
                                   bj_real b, bj_real t,
                                   bj_real n, bj_real f)
{
    // X, Y scales (note Y negative for Y-down)
    fmat[0][0] = BJ_F(2.0) * n / (r - l);
    fmat[0][1] = fmat[0][2] = fmat[0][3] = BJ_F(0.0);

    fmat[1][0] = BJ_F(0.0);
    fmat[1][1] = BJ_F(-2.0) * n / (t - b);
    fmat[1][2] = fmat[1][3] = BJ_F(0.0);

    // Off-center terms
    fmat[2][0] = (r + l) / (r - l);
    fmat[2][1] = (t + b) / (t - b);

    // Depth in [0,1] with w=+z_eye
    fmat[2][2] =  f / (f - n);
    fmat[2][3] =  BJ_F(1.0);

    fmat[3][0] = BJ_F(0.0);
    fmat[3][1] = BJ_F(0.0);
    fmat[3][2] = -(f * n) / (f - n);
    fmat[3][3] = BJ_F(0.0);
}


////////////////////////////////////////////////////////////////////////////////
/// \brief Generates a Vulkan-style orthographic projection (Y down, Z in [0,1])
///
/// Maps world box [l..r]×[b..t]×[n..f] to NDC:
///   x_ndc =  (2x - r - l) / (r - l)
///   y_ndc = -(2y - t - b) / (t - b)
///   z_ndc =        (z - n) / (f - n)      // [0,1]
///
/// Column-major 4×4:
/// [ 2/(r-l)      0          0        0 ]
/// [   0       -2/(t-b)      0        0 ]
/// [   0          0        1/(f-n)    0 ]
/// [-(r+l)/(r-l) (t+b)/(t-b) -n/(f-n)  1 ]
///
/// \param omat Output 4x4 matrix
/// \param l,r,b,t World bounds
/// \param n,f     Near/Far plane distances
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_ortho(
    bj_real omat[restrict 4][4],
    bj_real l, 
    bj_real r,
    bj_real b, 
    bj_real t,
    bj_real n, 
    bj_real f
) {
    omat[0][0] = BJ_F(2.0) / (r - l);
    omat[0][1] = omat[0][2] = omat[0][3] = BJ_F(0.0);

    omat[1][0] = omat[1][2] = omat[1][3] = BJ_F(0.0);
    omat[1][1] = BJ_F(-2.0) / (t - b);

    omat[2][0] = omat[2][1] = omat[2][3] = BJ_F(0.0);
    omat[2][2] = BJ_F(1.0) / (f - n);

    omat[3][0] = -(r + l) / (r - l);
    omat[3][1] =  (t + b) / (t - b);
    omat[3][2] = -n / (f - n);
    omat[3][3] = BJ_F(1.0);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Generates a Vulkan-style perspective projection matrix.
///
/// Vulkan clip/NDC conventions:
/// - Y points **down** in NDC (so the Y scale term is negated),
/// - Z maps to **[0,1]** (near -> 0, far -> 1),
/// - Right-handed clip with **+Z forward** (w = +z_eye).
///
/// Column-major, column-vector convention.
///
/// \param pmat   Output 4x4 matrix
/// \param y_fov  Vertical field of view in radians
/// \param aspect Aspect ratio (width / height)
/// \param n      Near plane distance  (> 0)
/// \param f      Far  plane distance  (> n)
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_perspective(bj_real pmat[restrict 4][4],
                                       bj_real y_fov, bj_real aspect,
                                       bj_real n, bj_real f)
{
    const bj_real a = BJ_F(1.0) / bj_tan(y_fov / BJ_F(2.0));

    // X scale
    pmat[0][0] = a / aspect;
    pmat[0][1] = pmat[0][2] = pmat[0][3] = BJ_F(0.0);

    // Y scale (negated: Y-down NDC)
    pmat[1][0] = BJ_F(0.0);
    pmat[1][1] = -a;
    pmat[1][2] = pmat[1][3] = BJ_F(0.0);

    // Z in [0,1], +Z forward, w = +z_eye
    pmat[2][0] = BJ_F(0.0);
    pmat[2][1] = BJ_F(0.0);
    pmat[2][2] = f / (f - n);
    pmat[2][3] = BJ_F(1.0);

    pmat[3][0] = BJ_F(0.0);
    pmat[3][1] = BJ_F(0.0);
    pmat[3][2] = -(f * n) / (f - n);
    pmat[3][3] = BJ_F(0.0);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Builds a Vulkan-style viewport (NDC -> window pixels; no Y flip)
///
/// For Z in [0,1]:
///   x_win = x + (x_ndc + 1) * (w/2)
///   y_win = y + (y_ndc + 1) * (h/2)
///   z_win = z_min + z_ndc * (z_max - z_min)
///
/// Column-major 4×4:
/// [  w/2    0      0      0 ]
/// [   0    h/2     0      0 ]
/// [   0     0   z_max-z_min 0 ]
/// [ x+w/2 y+h/2   z_min   1 ]
///
/// \param vpmat Output 4x4 viewport matrix
/// \param x,y   Viewport origin in pixels
/// \param w,h   Viewport size in pixels
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_viewport(bj_real vpmat[restrict 4][4],
                                    bj_real x, bj_real y,
                                    bj_real w, bj_real h)
{
    const bj_real z_min = BJ_F(0.0);
    const bj_real z_max = BJ_F(1.0);

    const bj_real sx = BJ_F(0.50) * w;
    const bj_real sy = BJ_F(0.50) * h;
    const bj_real sz = (z_max - z_min);

    const bj_real tx = x + BJ_F(0.50) * w;
    const bj_real ty = y + BJ_F(0.50) * h;
    const bj_real tz = z_min;

    vpmat[0][0] = sx;   vpmat[0][1] = BJ_F(0.0);  vpmat[0][2] = BJ_F(0.0);  vpmat[0][3] = BJ_F(0.0);
    vpmat[1][0] = BJ_F(0.0);  vpmat[1][1] = sy;   vpmat[1][2] = BJ_F(0.0);  vpmat[1][3] = BJ_F(0.0);
    vpmat[2][0] = BJ_F(0.0);  vpmat[2][1] = BJ_F(0.0);  vpmat[2][2] = sz;   vpmat[2][3] = BJ_F(0.0);
    vpmat[3][0] = tx;   vpmat[3][1] = ty;   vpmat[3][2] = tz;               vpmat[3][3] = BJ_F(1.0);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Generates a right-handed view matrix with **+Z forward** (Vulkan-style).
///
/// Column-major, column-vector convention.
///
/// \param m      Output 4x4 view matrix
/// \param eye    Camera position
/// \param center Target point the camera looks at
/// \param up     World up
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_lookat(bj_real m[restrict 4][4],
                                  const bj_vec3 eye,
                                  const bj_vec3 center,
                                  const bj_vec3 up)
{
    bj_vec3 f;  bj_vec3_sub(f, center, eye);  bj_vec3_normalize(f, f);      // forward (+Z)
    bj_vec3 s;  bj_vec3_cross(s, up, f);      bj_vec3_normalize(s, s);      // right = up × f
    bj_vec3 t;  bj_vec3_cross(t, f, s);                                      // up'

    m[0][0]=s[0]; m[0][1]=t[0]; m[0][2]=f[0]; m[0][3]=BJ_F(0.0);
    m[1][0]=s[1]; m[1][1]=t[1]; m[1][2]=f[1]; m[1][3]=BJ_F(0.0);
    m[2][0]=s[2]; m[2][1]=t[2]; m[2][2]=f[2]; m[2][3]=BJ_F(0.0);
    m[3][0]=BJ_F(0.0); m[3][1]=BJ_F(0.0); m[3][2]=BJ_F(0.0); m[3][3]=BJ_F(1.0);

    bj_mat4_translation_inplace(m, -eye[0], -eye[1], -eye[2]);
}
////////////////////////////////////////////////////////////////////////////////
/// Alias for \ref bj_vec4_add, adds two quaternions.
#define bj_quat_add bj_vec4_add

////////////////////////////////////////////////////////////////////////////////
/// Alias for \ref bj_vec4_sub, subtracts two quaternions.
#define bj_quat_sub bj_vec4_sub

////////////////////////////////////////////////////////////////////////////////
/// Alias for \ref bj_vec4_normalize, normalizes a quaternion.
#define bj_quat_norm bj_vec4_normalize

////////////////////////////////////////////////////////////////////////////////
/// Alias for \ref bj_vec4_scale, scales a quaternion by a scalar.
#define bj_quat_scale bj_vec4_scale

////////////////////////////////////////////////////////////////////////////////
/// Alias for \ref bj_vec4_dot, computes the dot product of two quaternions.
#define bj_quat_dot bj_vec4_dot

////////////////////////////////////////////////////////////////////////////////
/// \brief Sets a quaternion to the identity quaternion.
///
/// This function sets the given quaternion to the identity quaternion (0, 0, 0, 1).
///
/// \param q The quaternion to set as the identity.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_quat_identity(bj_quat q)
{
    q[0] = q[1] = q[2] = BJ_F(0.0);
    q[3] = BJ_F(1.0);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Multiplies two quaternions.
///
/// This function computes the product of two quaternions, following the quaternion
/// multiplication rules, and stores the result in the output quaternion.
///
/// \param res The resulting quaternion after multiplication.
/// \param p The first quaternion.
/// \param q The second quaternion.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_quat_mul(bj_quat res, const  bj_quat p, const bj_quat q)
{
    bj_vec3 w, tmp;

    bj_vec3_cross(tmp, p, q);
    bj_vec3_scale(w, p, q[3]);
    bj_vec3_add(tmp, tmp, w);
    bj_vec3_scale(w, q, p[3]);
    bj_vec3_add(tmp, tmp, w);

    bj_vec3_copy(res, tmp);
    res[3] = p[3] * q[3] - bj_vec3_dot(p, q);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Computes the conjugate of a quaternion.
///
/// This function computes the conjugate of the given quaternion, which involves
/// negating the vector part (x, y, z) of the quaternion while keeping the scalar
/// part unchanged.
///
/// \param res The resulting conjugated quaternion.
/// \param q The quaternion to conjugate.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_quat_conjugate(bj_quat res, const bj_quat q)
{
    for (int i = 0; i < 3; ++i) {
        res[i] = -q[i];
    }
    res[3] = q[3];
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Creates a rotation quaternion based on an angle and axis.
///
/// This function generates a quaternion representing a rotation of the specified
/// angle (in radians) around the given axis.
///
/// \param res The resulting rotation quaternion.
/// \param angle The angle (in radians) of rotation.
/// \param axis The axis around which the rotation occurs.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_quat_rotation(bj_quat res, bj_real angle, const bj_vec3 axis) {
    bj_vec3 axis_norm;
    bj_vec3_normalize(axis_norm, axis);
    const bj_real s = bj_sin(angle / BJ_F(2.0));
    const bj_real c = bj_cos(angle / BJ_F(2.0));
    bj_vec3_scale(res, axis_norm, s);
    res[3] = c;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Applies a quaternion rotation to a 3D vector.
///
/// This function rotates a 3D vector using the given quaternion, which represents
/// the rotation.
///
/// \param res The resulting rotated vector.
/// \param q The quaternion representing the rotation.
/// \param v The 3D vector to rotate.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_quat_mul_vec3(bj_vec3 res, const bj_quat q, const bj_vec3 v) {
    bj_vec3 t;
    bj_vec3 q_xyz = { q[0], q[1], q[2] };
    bj_vec3 u = { q[0], q[1], q[2] };

    bj_vec3_cross(t, q_xyz, v);
    bj_vec3_scale(t, t, BJ_F(2));

    bj_vec3_cross(u, q_xyz, t);
    bj_vec3_scale(t, t, q[3]);

    bj_vec3_add(res, v, t);
    bj_vec3_add(res, res, u);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Converts a quaternion to a 4x4 rotation matrix.
///
/// This function converts the given quaternion into a 4x4 rotation matrix.
///
/// \param res The resulting 4x4 rotation matrix.
/// \param q The quaternion to convert.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_from_quat(bj_mat4 res, const bj_quat q) {
    bj_real a = q[3];
    bj_real b = q[0];
    bj_real c = q[1];
    bj_real d = q[2];
    bj_real a2 = a * a;
    bj_real b2 = b * b;
    bj_real c2 = c * c;
    bj_real d2 = d * d;

    res[0][0] = a2 + b2 - c2 - d2;
    res[0][1] = BJ_F(2.0) * (b * c + a * d);
    res[0][2] = BJ_F(2.0) * (b * d - a * c);
    res[0][3] = BJ_F(0.0);

    res[1][0] = BJ_F(2) * (b * c - a * d);
    res[1][1] = a2 - b2 + c2 - d2;
    res[1][2] = BJ_F(2.0) * (c * d + a * b);
    res[1][3] = BJ_F(0.0);

    res[2][0] = BJ_F(2.0) * (b * d + a * c);
    res[2][1] = BJ_F(2.0) * (c * d - a * b);
    res[2][2] = a2 - b2 - c2 + d2;
    res[2][3] = BJ_F(0.0);

    res[3][0] = res[3][1] = res[3][2] = BJ_F(0.0);
    res[3][3] = BJ_F(1.0);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Applies a quaternion rotation to each column of a matrix.
///
/// This function applies the rotation represented by the given quaternion to each
/// column of the given matrix, transforming it into the rotated matrix.
///
/// \param R The resulting rotated matrix.
/// \param M The matrix to rotate.
/// \param q The quaternion representing the rotation.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_rotate_from_quat(bj_mat4 R, const bj_mat4 M, const bj_quat q) {
    bj_quat_mul_vec3(R[0], q, M[0]);
    bj_quat_mul_vec3(R[1], q, M[1]);
    bj_quat_mul_vec3(R[2], q, M[2]);

    R[3][0] = R[3][1] = R[3][2] = BJ_F(0.0);
    R[0][3] = M[0][3];
    R[1][3] = M[1][3];
    R[2][3] = M[2][3];
    R[3][3] = M[3][3];
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Converts a 4x4 rotation matrix to a quaternion.
///
/// This function converts the given 4x4 rotation matrix into a quaternion.
///
/// \param q The resulting quaternion.
/// \param M The 4x4 rotation matrix to convert.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_quat_from_mat4(bj_quat q, const bj_mat4 M) {
    bj_real r = BJ_F(0.0);
    int i, j, k;

    int perm[] = { 0, 1, 2, 0, 1 };
    int* p = perm;

    for (i = 0; i < 3; i++) {
        bj_real m = M[i][i];
        if (m > r) {
            r = m;
            p = &perm[i];
        }
    }

    i = p[0];
    j = p[1];
    k = p[2];

    r = bj_sqrt(BJ_F(1.0) + M[i][i] - M[j][j] - M[k][k]);

    if (r < BJ_F(1e-6)) {
        q[0] = BJ_F(0.0);
        q[1] = BJ_F(0.0);
        q[2] = BJ_F(0.0);
        q[3] = BJ_F(1.0);
        return;
    }

    bj_real inv = BJ_F(0.5) / r;

    q[i] = BJ_F(0.5) * r;
    q[j] = (M[i][j] + M[j][i]) * inv;
    q[k] = (M[k][i] + M[i][k]) * inv;
    q[3] = (M[k][j] - M[j][k]) * inv;
}


/// \}

#endif

