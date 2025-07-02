////////////////////////////////////////////////////////////////////////////////
/// \file math.h
/// Linear math library.
////////////////////////////////////////////////////////////////////////////////
/// \defgroup math Math
/// \ingroup core
///
/// \brief Linear math function
///
/// The Math group provides usual facilities for linear math, including
/// - vector types: \ref bj_vec2, \ref bj_vec3 and \ref bj_vec4
/// - 4x4 matrix with \ref bj_mat4
/// - Quaternion with \ref bj_quat
///
/// The scalar type is `float` by is aliased as `bj_real_t` using a typedef.
///
/// This library is a _direct_ conversion of 
/// [linmath.h](https://github.com/datenwolf/linmath.h) by Wolfgang Draxinger.
///
/// \{
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <banjo/math.h>

#define bj_real_t float //!< Alias for `float` type

#ifndef BJ_COMPILER_DOXYGEN
#   define real_sqrt bj_sqrtf
#   define real_acos bj_acosf
#   define BJ_INLINE static inline
#endif

////////////////////////////////////////////////////////////////////////////////
/// \brief Defines a 2D vector type.
///
/// This type represents a 2D vector with two components, each of type `bj_real_t`.
/// It is commonly used for storing 2D coordinates, directions, or other 2D data.
///
/// \note The vector is represented as an array of two `bj_real_t` values.
/// \see bj_real_t
////////////////////////////////////////////////////////////////////////////////
typedef bj_real_t bj_vec2[2];

////////////////////////////////////////////////////////////////////////////////
/// \brief Defines a 3D vector type.
///
/// This type represents a 3D vector with three components, each of type `bj_real_t`.
/// It is commonly used for storing 3D coordinates, directions, or other 3D data.
///
/// \note The vector is represented as an array of three `bj_real_t` values.
/// \see bj_real_t
////////////////////////////////////////////////////////////////////////////////
typedef bj_real_t bj_vec3[3];

////////////////////////////////////////////////////////////////////////////////
/// \brief Defines a 4D vector type.
///
/// This type represents a 4D vector with four components, each of type `bj_real_t`.
/// It is commonly used for storing 4D homogeneous coordinates or other 4D data.
///
/// \note The vector is represented as an array of four `bj_real_t` values.
/// \see bj_real_t
////////////////////////////////////////////////////////////////////////////////
typedef bj_real_t bj_vec4[4];

////////////////////////////////////////////////////////////////////////////////
/// \brief Defines a 4x4 matrix type.
///
/// This type represents a 4x4 matrix, where each row is a `bj_vec4` vector.
/// It is commonly used for transformations such as rotations, scaling, and translations.
///
/// \note The matrix is represented as an array of four `bj_vec4` values.
/// \see bj_vec4
////////////////////////////////////////////////////////////////////////////////
typedef bj_vec4 bj_mat4[4];

////////////////////////////////////////////////////////////////////////////////
/// \brief Defines a quaternion type.
///
/// This type represents a quaternion with four components, each of type `bj_real_t`.
/// Quaternions are used for representing rotations in 3D space.
///
/// \note The quaternion is represented as an array of four `bj_real_t` values.
/// \see bj_real_t
////////////////////////////////////////////////////////////////////////////////
typedef bj_real_t bj_quat[4];

////////////////////////////////////////////////////////////////////////////////
/// Directly set the scalar values of a \ref bj_vec2 object.
///
/// \param res A location to the target vec4
/// \param a   The first scalar value
/// \param b   The second scalar value
///
////////////////////////////////////////////////////////////////////////////////
BJ_INLINE void bj_vec2_set(bj_vec2 res, bj_real_t a, bj_real_t b) {
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
BJ_INLINE void bj_vec2_apply(bj_vec2 res, const bj_vec2 a, bj_real_t(*f)(bj_real_t)) {
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
BJ_INLINE void bj_vec2_add(bj_vec2 res, const bj_vec2 lhs, const bj_vec2 rhs) {
    res[0] = lhs[0] + rhs[0];
    res[1] = lhs[1] + rhs[1];
}

////////////////////////////////////////////////////////////////////////////////
/// Set `res` to the result of `lhs`-`rhs`.
///
/// \param res The destination vec2
/// \param lhs The first source vec2
/// \param rhs The second source vec2
////////////////////////////////////////////////////////////////////////////////
BJ_INLINE void bj_vec2_sub(bj_vec2 res, const bj_vec2 lhs, const bj_vec2 rhs) {
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
BJ_INLINE void bj_vec2_scale(bj_vec2 res, const bj_vec2 v, bj_real_t s) {
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
BJ_INLINE void bj_vec2_scale_each(bj_vec2 res, const bj_vec2 v, const bj_vec2 s) {
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
BJ_INLINE bj_real_t bj_vec2_dot(const bj_vec2 a, const bj_vec2 b) {
    return a[0] * b[0] + a[1] * b[1];
}

////////////////////////////////////////////////////////////////////////////////
/// Computes the length of the vec2
///
/// \param v   The vec2
/// \return    The length
////////////////////////////////////////////////////////////////////////////////
BJ_INLINE bj_real_t bj_vec2_len(const bj_vec2 v) {
    return real_sqrt(v[0] * v[0] + v[1] * v[1]);
}

////////////////////////////////////////////////////////////////////////////////
/// Normalizes the provided vec2.
///
/// \param res   The resulting vec2
/// \param v     The vec2
////////////////////////////////////////////////////////////////////////////////
BJ_INLINE void bj_vec2_normalize(bj_vec2 res, const bj_vec2 v) {
    bj_real_t inv_len = 1.0f / real_sqrt(v[0] * v[0] + v[1] * v[1]);
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
BJ_INLINE void bj_vec2_min(bj_vec2 res, const bj_vec2 a, const bj_vec2 b) {
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
BJ_INLINE void bj_vec2_max(bj_vec2 res, const bj_vec2 a, const bj_vec2 b) {
    res[0] = a[0] > b[0] ? a[0] : b[0];
    res[1] = a[1] > b[1] ? a[1] : b[1];
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Copies the contents of one 2D vector to another.
///
/// \param res The destination vector.
/// \param src The source vector to copy.
////////////////////////////////////////////////////////////////////////////////
BJ_INLINE void bj_vec2_copy(bj_vec2 res, const bj_vec2 src) {
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
BJ_INLINE void bj_vec3_set(bj_vec2 res, bj_real_t a, bj_real_t b, bj_real_t c) {
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
BJ_INLINE void bj_vec3_apply(bj_vec3 res, const bj_vec3 a, bj_real_t(*f)(bj_real_t)) {
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
BJ_INLINE void bj_vec3_add(bj_vec3 res, const bj_vec3 lhs, const bj_vec3 rhs) {
    res[0] = lhs[0] + rhs[0];
    res[1] = lhs[1] + rhs[1];
    res[2] = lhs[2] + rhs[2];
}

////////////////////////////////////////////////////////////////////////////////
/// Set `res` to the result of `lhs`-`rhs`.
///
/// \param res The destination vec3
/// \param lhs The first source vec3
/// \param rhs The second source vec3
////////////////////////////////////////////////////////////////////////////////
BJ_INLINE void bj_vec3_sub(bj_vec3 res, const bj_vec3 lhs, const bj_vec3 rhs) {
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
BJ_INLINE void bj_vec3_scale(bj_vec3 res, const bj_vec3 v, bj_real_t s) {
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
BJ_INLINE bj_real_t bj_vec3_dot(const bj_vec3 a, const bj_vec3 b) {
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

////////////////////////////////////////////////////////////////////////////////
/// Computes the length of the vec3
///
/// \param v   The vec3
/// \return    The length
////////////////////////////////////////////////////////////////////////////////
BJ_INLINE bj_real_t bj_vec3_len(const bj_vec3 v) {
    return real_sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

////////////////////////////////////////////////////////////////////////////////
/// Normalizes the provided vec3.
///
/// \param res   The resulting vec3
/// \param v     The vec3
////////////////////////////////////////////////////////////////////////////////
BJ_INLINE void bj_vec3_normalize(bj_vec3 res, const bj_vec3 v) {
    bj_real_t inv_len = 1.0f / real_sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
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
BJ_INLINE void bj_vec3_min(bj_vec3 res, const bj_vec3 a, const bj_vec3 b) {
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
BJ_INLINE void bj_vec3_max(bj_vec3 res, const bj_vec3 a, const bj_vec3 b) {
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
BJ_INLINE void bj_vec3_copy(bj_vec3 res, const bj_vec3 src) {
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
BJ_INLINE void bj_vec3_cross(bj_vec3 res, const bj_vec3 l, const bj_vec3 r)
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
BJ_INLINE void bj_vec3_reflect(bj_vec3 res, const bj_vec3 v, const bj_vec3 n)
{
    const bj_real_t p = 2.f * bj_vec3_dot(v, n);
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
BJ_INLINE void bj_vec4_set(bj_vec2 res, bj_real_t a, bj_real_t b, bj_real_t c, bj_real_t d) {
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
BJ_INLINE void bj_vec4_apply(bj_vec4 res, const bj_vec4 a, bj_real_t(*f)(bj_real_t)) {
    res[0] = f(a[0]);
    res[1] = f(a[1]);
    res[2] = f(a[2]);
    res[3] = f(a[4]);
}

////////////////////////////////////////////////////////////////////////////////
/// Set `res` to the result of `lhs`+`rhs`.
///
/// \param res The destination vec4
/// \param lhs The first source vec4
/// \param rhs The second source vec4
////////////////////////////////////////////////////////////////////////////////
BJ_INLINE void bj_vec4_add(bj_vec4 res, const bj_vec4 lhs, const bj_vec4 rhs) {
    res[0] = lhs[0] + rhs[0];
    res[1] = lhs[1] + rhs[1];
    res[2] = lhs[2] + rhs[2];
    res[3] = lhs[3] + rhs[3];
}

////////////////////////////////////////////////////////////////////////////////
/// Set `res` to the result of `lhs`-`rhs`.
///
/// \param res The destination vec4
/// \param lhs The first source vec4
/// \param rhs The second source vec4
////////////////////////////////////////////////////////////////////////////////
BJ_INLINE void bj_vec4_sub(bj_vec4 res, const bj_vec4 lhs, const bj_vec4 rhs) {
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
BJ_INLINE void bj_vec4_scale(bj_vec4 res, const bj_vec4 v, bj_real_t s) {
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
BJ_INLINE bj_real_t bj_vec4_dot(const bj_vec4 a, const bj_vec4 b) {
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3];
}

////////////////////////////////////////////////////////////////////////////////
/// Computes the length of the vec4
///
/// \param v   The vec4
/// \return    The length
////////////////////////////////////////////////////////////////////////////////
BJ_INLINE bj_real_t bj_vec4_len(const bj_vec4 v) {
    return real_sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3]);
}

////////////////////////////////////////////////////////////////////////////////
/// Normalizes the provided vec4.
///
/// \param res   The resulting vec4
/// \param v     The vec4
////////////////////////////////////////////////////////////////////////////////
BJ_INLINE void bj_vec4_normalize(bj_vec4 res, const bj_vec4 v) {
    bj_real_t inv_len = 1.0f / real_sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3]);
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
BJ_INLINE void bj_vec4_min(bj_vec4 res, const bj_vec4 a, const bj_vec4 b) {
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
BJ_INLINE void bj_vec4_max(bj_vec4 res, const bj_vec4 a, const bj_vec4 b) {
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
BJ_INLINE void bj_vec4_copy(bj_vec4 res, const bj_vec4 src) {
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
BJ_INLINE void bj_vec4_cross(bj_vec4 res, const bj_vec4 l, const bj_vec4 r)
{
    res[0] = l[1] * r[2] - l[2] * r[1];
    res[1] = l[2] * r[0] - l[0] * r[2];
    res[2] = l[0] * r[1] - l[1] * r[0];
    res[3] = 1.f;
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
BJ_INLINE void bj_vec4_reflect(bj_vec4 res, const bj_vec4 v, const bj_vec4 n)
{
    bj_real_t p = 2.f * bj_vec4_dot(v, n);
    for (int i = 0; i < 4; ++i) {
        res[i] = v[i] - p * n[i];
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Initializes a 4x4 matrix to the identity matrix.
///
/// Sets all diagonal elements of the matrix to 1.0 and all off-diagonal elements to 0.0.
///
/// \param mat The matrix to be initialized as the identity matrix.
////////////////////////////////////////////////////////////////////////////////
BJ_INLINE void bj_mat4_identity(bj_mat4 mat)
{
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            mat[i][j] = i == j ? 1.f : 0.f;
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
BJ_INLINE void bj_mat4_copy(bj_mat4 to, const bj_mat4 from)
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
BJ_INLINE void bj_mat4_row(bj_vec4 res, const bj_mat4 mat, int r)
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
BJ_INLINE void bj_mat4_col(bj_vec4 res, const bj_mat4 mat, int c)
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
BJ_INLINE void bj_mat4_transpose(bj_mat4 res, const bj_mat4 mat)
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
BJ_INLINE void bj_mat4_add(bj_mat4 res, const bj_mat4 lhs, const bj_mat4 rhs)
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
BJ_INLINE void bj_mat4_sub(bj_mat4 res, const bj_mat4 lhs, const bj_mat4 rhs)
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
BJ_INLINE void bj_mat4_scale(bj_mat4 res, const bj_mat4 lhs, bj_real_t k)
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
BJ_INLINE void bj_mat4_scale_xyz(bj_mat4 res, const bj_mat4 mat, bj_real_t x, bj_real_t y, bj_real_t z)
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
BJ_INLINE void bj_mat4_mul(bj_mat4 res, const bj_mat4 lhs, const bj_mat4 rhs)
{
    bj_mat4 temp;
    for (int c = 0; c < 4; ++c) {
        for (int r = 0; r < 4; ++r) {
            temp[c][r] = 0.f;
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
BJ_INLINE void bj_mat4_mul_vec4(bj_vec4 res, const bj_mat4 mat, const bj_vec4 v)
{
    for (int j = 0; j < 4; ++j) {
        res[j] = 0.f;
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
BJ_INLINE void bj_mat4_translation(bj_mat4 res, bj_real_t x, bj_real_t y, bj_real_t z)
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
BJ_INLINE void bj_mat4_translation_inplace(bj_mat4 M, bj_real_t x, bj_real_t y, bj_real_t z)
{
    bj_vec4 t = { x, y, z, 0 };
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
BJ_INLINE void bj_mat4_mul_outer(bj_mat4 res, const bj_vec3 a, const bj_vec3 b)
{
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            res[i][j] = i < 3 && j < 3 ? a[i] * b[j] : 0.f;
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
BJ_INLINE void bj_mat4_rotate(bj_mat4 res, const bj_mat4 mat, bj_real_t x, bj_real_t y, bj_real_t z, bj_real_t angle)
{
    bj_real_t s = bj_sinf(angle);
    bj_real_t c = bj_cosf(angle);
    bj_vec3 u = { x, y, z };

    if (bj_vec3_len(u) > 1e-4) {
        bj_vec3_normalize(u, u);
        bj_mat4 T;
        bj_mat4_mul_outer(T, u, u);

        bj_mat4 S = {
            {    0,  u[2], -u[1], 0},
            {-u[2],     0,  u[0], 0},
            { u[1], -u[0],     0, 0},
            {    0,     0,     0, 0}
        };
        bj_mat4_scale(S, S, s);

        bj_mat4 C;
        bj_mat4_identity(C);
        bj_mat4_sub(C, C, T);

        bj_mat4_scale(C, C, c);

        bj_mat4_add(T, T, C);
        bj_mat4_add(T, T, S);

        T[3][3] = 1.f;
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
BJ_INLINE void bj_ma4_rotate_x(bj_mat4 res, const bj_mat4 mat, bj_real_t angle)
{
    bj_real_t s = bj_sinf(angle);
    bj_real_t c = bj_cosf(angle);
    bj_mat4 R = {
        {1.f, 0.f, 0.f, 0.f},
        {0.f,   c,   s, 0.f},
        {0.f,  -s,   c, 0.f},
        {0.f, 0.f, 0.f, 1.f}
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
BJ_INLINE void bj_ma4_rotate_y(bj_mat4 res, const bj_mat4 mat, bj_real_t angle)
{
    bj_real_t s = bj_sinf(angle);
    bj_real_t c = bj_cosf(angle);
    bj_mat4 R = {
        {   c, 0.f,  -s, 0.f},
        { 0.f, 1.f, 0.f, 0.f},
        {   s, 0.f,   c, 0.f},
        { 0.f, 0.f, 0.f, 1.f}
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
BJ_INLINE void bj_ma4_rotate_z(bj_mat4 res, const bj_mat4 mat, bj_real_t angle)
{
    bj_real_t s = bj_sinf(angle);
    bj_real_t c = bj_cosf(angle);
    bj_mat4 R = {
        {   c,   s, 0.f, 0.f},
        {  -s,   c, 0.f, 0.f},
        { 0.f, 0.f, 1.f, 0.f},
        { 0.f, 0.f, 0.f, 1.f}
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
BJ_INLINE void bj_mat4_rotate_arcball(bj_mat4 R, const bj_mat4 M, bj_vec2 const _a, bj_vec2 const _b, bj_real_t s)
{
    bj_vec2 a = { _a[0], _a[1] };
    bj_vec2 b = { _b[0], _b[1] };;

    bj_real_t z_a = 0.;
    bj_real_t z_b = 0.;

    if (bj_vec2_len(a) < 1.) {
        z_a = real_sqrt(1.f - bj_vec2_dot(a, a));
    }
    else {
        bj_vec2_normalize(a, a);
    }

    if (bj_vec2_len(b) < 1.) {
        z_b = real_sqrt(1.f - bj_vec2_dot(b, b));
    }
    else {
        bj_vec2_normalize(b, b);
    }

    bj_vec3 a_ = { a[0], a[1], z_a };
    bj_vec3 b_ = { b[0], b[1], z_b };

    bj_vec3 c_;
    bj_vec3_cross(c_, a_, b_);

    bj_real_t const angle = real_acos(bj_vec3_dot(a_, b_)) * s;
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
BJ_INLINE void bj_mat4_inverse(bj_mat4 res, const bj_mat4 mat) {
    bj_real_t s[6];
    bj_real_t c[6];
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

    bj_real_t idet = 1.0f / (s[0] * c[5] - s[1] * c[4] + s[2] * c[3] + s[3] * c[2] - s[4] * c[1] + s[5] * c[0]);

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
BJ_INLINE void bj_mat4_orthonormalize(bj_mat4 res, const bj_mat4 mat) {
    bj_mat4_copy(res, mat);
    bj_real_t s = 1.f;
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
BJ_INLINE void bj_mat4_frustum(bj_mat4 fmat, bj_real_t l, bj_real_t r, bj_real_t b, bj_real_t t, bj_real_t n, bj_real_t f) {
    fmat[0][0] = 2.f * n / (r - l);
    fmat[0][1] = fmat[0][2] = fmat[0][3] = 0.f;
    fmat[1][1] = 2.f * n / (t - b);
    fmat[1][0] = fmat[1][2] = fmat[1][3] = 0.f;
    fmat[2][0] = (r + l) / (r - l);
    fmat[2][1] = (t + b) / (t - b);
    fmat[2][2] = -(f + n) / (f - n);
    fmat[2][3] = -1.f;
    fmat[3][2] = -2.f * (f * n) / (f - n);
    fmat[3][0] = fmat[3][1] = fmat[3][3] = 0.f;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Generates an orthographic projection matrix.
///
/// This function generates an orthographic projection matrix for the specified
/// left, right, bottom, top, near, and far clipping planes.
///
/// \param l The left plane of the orthographic projection.
/// \param r The right plane of the orthographic projection.
/// \param b The bottom plane of the orthographic projection.
/// \param t The top plane of the orthographic projection.
/// \param n The near plane of the orthographic projection.
/// \param f The far plane of the orthographic projection.
/// \param omat The resulting orthographic projection matrix.
////////////////////////////////////////////////////////////////////////////////
BJ_INLINE void bj_mat4_ortho(bj_mat4 omat, bj_real_t l, bj_real_t r, bj_real_t b, bj_real_t t, bj_real_t n, bj_real_t f) {
    omat[0][0] = 2.f / (r - l);
    omat[0][1] = omat[0][2] = omat[0][3] = 0.f;
    omat[1][1] = 2.f / (t - b);
    omat[1][0] = omat[1][2] = omat[1][3] = 0.f;
    omat[2][2] = -2.f / (f - n);
    omat[2][0] = omat[2][1] = omat[2][3] = 0.f;
    omat[3][0] = -(r + l) / (r - l);
    omat[3][1] = -(t + b) / (t - b);
    omat[3][2] = -(f + n) / (f - n);
    omat[3][3] = 1.f;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Generates a perspective projection matrix based on field of view.
///
/// This function generates a perspective projection matrix based on the field
/// of view (y_fov), aspect ratio, and near and far clipping planes.
///
/// \param y_fov The vertical field of view in radians.
/// \param aspect The aspect ratio (width / height).
/// \param n The near clipping plane.
/// \param f The far clipping plane.
/// \param pmat The resulting perspective projection matrix.
////////////////////////////////////////////////////////////////////////////////
BJ_INLINE void bj_mat4_perspective(bj_mat4 pmat, bj_real_t y_fov, bj_real_t aspect, bj_real_t n, bj_real_t f) {
    const bj_real_t a = 1.f / bj_tanf(y_fov / 2.f);
    pmat[0][0] = a / aspect;
    pmat[0][1] = 0.f;
    pmat[0][2] = 0.f;
    pmat[0][3] = 0.f;
    pmat[1][0] = 0.f;
    pmat[1][1] = a;
    pmat[1][2] = 0.f;
    pmat[1][3] = 0.f;
    pmat[2][0] = 0.f;
    pmat[2][1] = 0.f;
    pmat[2][2] = -((f + n) / (f - n));
    pmat[2][3] = -1.f;
    pmat[3][0] = 0.f;
    pmat[3][1] = 0.f;
    pmat[3][2] = -((2.f * f * n) / (f - n));
    pmat[3][3] = 0.f;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Generates a look-at matrix.
///
/// This function generates a view matrix that transforms coordinates from world
/// space to camera space based on the eye position, center (target) position, and
/// up vector.
///
/// \param eye The position of the camera.
/// \param center The target position the camera is looking at.
/// \param up The up vector of the camera.
/// \param m The resulting look-at matrix.
////////////////////////////////////////////////////////////////////////////////
BJ_INLINE void bj_mat4_lookat(bj_mat4 m, const bj_vec3 eye, const bj_vec3 center, const bj_vec3 up) {
    bj_vec3 f;
    bj_vec3_sub(f, center, eye);
    bj_vec3_normalize(f, f);

    bj_vec3 s;
    bj_vec3_cross(s, f, up);
    bj_vec3_normalize(s, s);

    bj_vec3 t;
    bj_vec3_cross(t, s, f);

    m[0][0] = s[0];
    m[0][1] = t[0];
    m[0][2] = -f[0];
    m[0][3] = 0.f;

    m[1][0] = s[1];
    m[1][1] = t[1];
    m[1][2] = -f[1];
    m[1][3] = 0.f;

    m[2][0] = s[2];
    m[2][1] = t[2];
    m[2][2] = -f[2];
    m[2][3] = 0.f;

    m[3][0] = 0.f;
    m[3][1] = 0.f;
    m[3][2] = 0.f;
    m[3][3] = 1.f;

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
BJ_INLINE void bj_quat_identity(bj_quat q)
{
    q[0] = q[1] = q[2] = 0.f;
    q[3] = 1.f;
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
BJ_INLINE void bj_quat_mul(bj_quat res, const  bj_quat p, const bj_quat q)
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
BJ_INLINE void bj_quat_conjugate(bj_quat res, const bj_quat q)
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
BJ_INLINE void bj_quat_rotation(bj_quat res, bj_real_t angle, const bj_vec3 axis) {
    bj_vec3 axis_norm;
    bj_vec3_normalize(axis_norm, axis);
    const bj_real_t s = bj_sinf(angle / 2);
    const bj_real_t c = bj_cosf(angle / 2);
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
BJ_INLINE void bj_quat_mul_vec3(bj_vec3 res, const bj_quat q, const bj_vec3 v) {
    bj_vec3 t;
    bj_vec3 q_xyz = { q[0], q[1], q[2] };
    bj_vec3 u = { q[0], q[1], q[2] };

    bj_vec3_cross(t, q_xyz, v);
    bj_vec3_scale(t, t, 2);

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
BJ_INLINE void bj_mat4_from_quat(bj_mat4 res, const bj_quat q) {
    bj_real_t a = q[3];
    bj_real_t b = q[0];
    bj_real_t c = q[1];
    bj_real_t d = q[2];
    bj_real_t a2 = a * a;
    bj_real_t b2 = b * b;
    bj_real_t c2 = c * c;
    bj_real_t d2 = d * d;

    res[0][0] = a2 + b2 - c2 - d2;
    res[0][1] = 2.f * (b * c + a * d);
    res[0][2] = 2.f * (b * d - a * c);
    res[0][3] = 0.f;

    res[1][0] = 2 * (b * c - a * d);
    res[1][1] = a2 - b2 + c2 - d2;
    res[1][2] = 2.f * (c * d + a * b);
    res[1][3] = 0.f;

    res[2][0] = 2.f * (b * d + a * c);
    res[2][1] = 2.f * (c * d - a * b);
    res[2][2] = a2 - b2 - c2 + d2;
    res[2][3] = 0.f;

    res[3][0] = res[3][1] = res[3][2] = 0.f;
    res[3][3] = 1.f;
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
BJ_INLINE void bj_mat4_rotate_from_quat(bj_mat4 R, const bj_mat4 M, const bj_quat q) {
    bj_quat_mul_vec3(R[0], q, M[0]);
    bj_quat_mul_vec3(R[1], q, M[1]);
    bj_quat_mul_vec3(R[2], q, M[2]);

    R[3][0] = R[3][1] = R[3][2] = 0.f;
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
BJ_INLINE void bj_quat_from_mat4(bj_quat q, const bj_mat4 M) {
    bj_real_t r = 0.f;
    int i, j, k;

    int perm[] = { 0, 1, 2, 0, 1 };
    int* p = perm;

    for (i = 0; i < 3; i++) {
        bj_real_t m = M[i][i];
        if (m > r) {
            r = m;
            p = &perm[i];
        }
    }

    i = p[0];
    j = p[1];
    k = p[2];

    r = real_sqrt(1.f + M[i][i] - M[j][j] - M[k][k]);

    if (r < 1e-6f) {
        q[0] = 0.f;
        q[1] = 0.f;
        q[2] = 0.f;
        q[3] = 1.f;
        return;
    }

    bj_real_t inv = 0.5f / r;

    q[i] = 0.5f * r;
    q[j] = (M[i][j] + M[j][i]) * inv;
    q[k] = (M[k][i] + M[i][k]) * inv;
    q[3] = (M[k][j] - M[j][k]) * inv;
}

