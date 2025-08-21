////////////////////////////////////////////////////////////////////////////////
/// Quaternion utilities using \c bj_real with \c {x,y,z,w} layout.
///
/// \file quat.h
/// Quaternions are stored as a 4-tuple where the vector part is (x,y,z) and scalar part is w.
/// The functions here support identity, multiplication, conjugation, axis-angle creation,
/// rotating vectors, and conversions to/from 4×4 rotation matrices.
///
/// \addtogroup math
/// \{
////////////////////////////////////////////////////////////////////////////////
#ifndef BJ_QUAT_H
#define BJ_QUAT_H

#include <banjo/api.h>
#include <banjo/mat.h>
#include <banjo/math.h>
#include <banjo/vec.h>

////////////////////////////////////////////////////////////////////////////////
/// bj_quat: Quaternion stored as {x, y, z, w} with \c bj_real components.
/// Multiplication composes rotations; unit quaternions represent pure rotations.
////////////////////////////////////////////////////////////////////////////////
typedef bj_real bj_quat[4];

////////////////////////////////////////////////////////////////////////////////
/// Alias to vector add (component-wise).
/// \see bj_vec4_add
////////////////////////////////////////////////////////////////////////////////
#define bj_quat_add bj_vec4_add

////////////////////////////////////////////////////////////////////////////////
/// Alias to vector subtract (component-wise).
/// \see bj_vec4_sub
////////////////////////////////////////////////////////////////////////////////
#define bj_quat_sub bj_vec4_sub

////////////////////////////////////////////////////////////////////////////////
/// Alias to normalize quaternion (scales to unit length).
/// \see bj_vec4_normalize
////////////////////////////////////////////////////////////////////////////////
#define bj_quat_norm bj_vec4_normalize

////////////////////////////////////////////////////////////////////////////////
/// Alias to uniform scale all four components.
/// \see bj_vec4_scale
////////////////////////////////////////////////////////////////////////////////
#define bj_quat_scale bj_vec4_scale

////////////////////////////////////////////////////////////////////////////////
/// Alias to 4D dot product.
/// \see bj_vec4_dot
////////////////////////////////////////////////////////////////////////////////
#define bj_quat_dot bj_vec4_dot

////////////////////////////////////////////////////////////////////////////////
/// Set quaternion to identity (no rotation).
/// \param q Input quaternion.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_quat_identity(bj_quat q)
{
    q[0] = q[1] = q[2] = BJ_F(0.0);
    q[3] = BJ_F(1.0);
}

////////////////////////////////////////////////////////////////////////////////
/// Hamilton product: res = p * q.
/// \param res Output quaternion.
/// \param p Input quaternion.
/// \param q Input quaternion.
/// \note Assumes unit-length quaternions for pure rotations.
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
/// Conjugate quaternion: (x,y,z,w) -> (-x,-y,-z,w).
/// \param res Output quaternion.
/// \param q Input quaternion.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_quat_conjugate(bj_quat res, const bj_quat q)
{
    for (int i = 0; i < 3; ++i) {
        res[i] = -q[i];
    }
    res[3] = q[3];
}

////////////////////////////////////////////////////////////////////////////////
/// Build a unit quaternion from axis-angle.
/// \param res Output quaternion.
/// \param angle Rotation angle in radians.
/// \param axis 3D vector.
/// \note Assumes unit-length quaternions for pure rotations.
/// \warning If \p axis is near zero length, the result is implementation-defined.
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
/// Rotate a 3D vector by a unit quaternion.
/// \param res Output quaternion.
/// \param q Input quaternion.
/// \param v 3D vector.
/// \note Assumes unit-length quaternions for pure rotations.
/// \returns The rotated vector in \p res.
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
/// Convert unit quaternion to a 4×4 rotation matrix.
/// \param res Output quaternion.
/// \param q Input quaternion.
/// \note Assumes unit-length quaternions for pure rotations.
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
/// Post-multiply matrix by rotation from quaternion: R = M * rot(q).
/// \param R Output matrix.
/// \param M Input 4×4 matrix.
/// \param q Input quaternion.
/// \note Assumes unit-length quaternions for pure rotations.
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
/// Extract a unit quaternion from a 4×4 rotation matrix.
/// \param q Input quaternion.
/// \param M Input 4×4 matrix.
/// \note Assumes unit-length quaternions for pure rotations.
/// \note Uses a numerically stable branch based on the dominant diagonal term.
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



#endif

/// \}


