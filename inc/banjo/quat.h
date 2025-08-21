////////////////////////////////////////////////////////////////////////////////
/// \file quat.h
/// Math quaternion library.
////////////////////////////////////////////////////////////////////////////////
/// \addtogroup math
///
/// \{
////////////////////////////////////////////////////////////////////////////////
#ifndef BJ_QUAT_H
#define BJ_QUAT_H

#include <banjo/api.h>
#include <banjo/mat.h>
#include <banjo/math.h>
#include <banjo/vec.h>

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

