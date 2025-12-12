////////////////////////////////////////////////////////////////////////////////
/// \file quat.h
/// \brief Quaternion manipulation API (by-value, struct bj_vec4-based).
///
/// \details
/// Quaternions are stored in \c {x,y,z,w} layout where the vector part is
/// (x,y,z) and the scalar part is w.
///
/// This header defines \c struct bj_vec4 as an alias of the 4D vector struct type
/// \c struct bj_vec4 and exposes a by-value API: all quaternion arguments and return
/// values use pass-by-value semantics for clarity and inlining friendliness.
///
/// Unless stated otherwise, angles are in radians, matrices are column-major
/// as in \c struct bj_mat4x4, and inputs are not implicitly normalized except where
/// explicitly noted.
///
/// Provided operations:
///  - construction: identity, from axis-angle, from 4×4 rotation matrix
///  - algebra: dot, norm, normalize, conjugate, inverse, Hamilton product, slerp
///  - application: rotate 3D/4D vectors
///  - conversion: to 4×4 rotation matrix
///
/// Numerical notes:
///  - \ref BJ_EPSILON is used to guard zero-length normalization and inversion.
///  - \ref bj_quat_slerp clamps inputs near ±1 and falls back to nlerp if needed.
///
/// \addtogroup math
/// \{
////////////////////////////////////////////////////////////////////////////////
#ifndef BJ_QUAT_H
#define BJ_QUAT_H

#include <banjo/api.h>
#include <banjo/math.h>
#include <banjo/vec.h>
#include <banjo/mat.h>

////////////////////////////////////////////////////////////////////////////////
/// \typedef struct bj_vec4
/// \brief Quaternion type alias based on the 4D vector struct.
/// 
/// \details
/// The layout matches \c struct bj_vec4 fields:
/// \code
///   q.x, q.y, q.z  // vector part
///   q.w            // scalar part
/// \endcode
/// 
/// The alias preserves binary compatibility with \c struct bj_vec4 and allows
/// using quaternion values wherever a 4D vector is accepted, when meaningful.
////////////////////////////////////////////////////////////////////////////////
struct bj_vec4;

////////////////////////////////////////////////////////////////////////////////
/// \brief Return the identity quaternion.
/// 
/// \details
/// Represents a no-rotation. Equivalent to \c {0,0,0,1}.
/// 
/// \return Identity quaternion.
/// 
/// \sa bj_quat_normalize
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE struct bj_vec4 bj_quat_identity(
    void
) {
    return (struct bj_vec4){ BJ_FZERO, BJ_FZERO, BJ_FZERO, BJ_F(1.0) };
}

////////////////////////////////////////////////////////////////////////////////
/// \brief 4D dot product between two quaternions.
/// 
/// \param a First quaternion.
/// \param b Second quaternion.
/// \return \c a·b.
/// 
/// \note For unit quaternions this equals \c cos(theta) where \c theta is the
/// half-angle between orientations used by \c bj_quat_slerp.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_real bj_quat_dot(
    struct bj_vec4 a,
    struct bj_vec4 b
) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Squared Euclidean norm.
/// 
/// \param q Quaternion.
/// \return \c ||q||^2.
/// 
/// \sa bj_quat_norm, bj_quat_normalize
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_real bj_quat_norm2(
    struct bj_vec4 q
) {
    return bj_quat_dot(q, q);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Euclidean norm (length).
/// 
/// \param q Quaternion.
/// \return \c ||q||.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_real bj_quat_norm(
    struct bj_vec4 q
) {
    return bj_sqrt(bj_quat_norm2(q));
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Normalize a quaternion.
/// 
/// \details
/// Returns identity if the input length is near zero (<= \c BJ_EPSILON).
/// 
/// \param q Quaternion.
/// \return Unit-length quaternion.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE struct bj_vec4 bj_quat_normalize(
    struct bj_vec4 q
) {
    const bj_real n2 = bj_quat_norm2(q);
    if (n2 <= BJ_EPSILON) {
        return bj_quat_identity();
    }
    const bj_real inv = BJ_F(1.0) / bj_sqrt(n2);
    return (struct bj_vec4){ q.x * inv, q.y * inv, q.z * inv, q.w * inv };
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Conjugate of a quaternion.
/// 
/// \details
/// Negates the vector part and keeps the scalar part:
/// \c conj(q) = {-x,-y,-z,w}.
/// 
/// \param q Quaternion.
/// \return Conjugated quaternion.
/// 
/// \sa bj_quat_inverse
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE struct bj_vec4 bj_quat_conjugate(
    struct bj_vec4 q
) {
    return (struct bj_vec4){ -q.x, -q.y, -q.z, q.w };
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Multiplicative inverse of a quaternion.
/// 
/// \details
/// Returns identity if the squared norm is near zero (<= \c BJ_EPSILON).
/// Otherwise \c q^{-1} = conj(q) / ||q||^2.
/// 
/// \param q Quaternion.
/// \return Inverse quaternion.
/// 
/// \sa bj_quat_conjugate, bj_quat_normalize
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE struct bj_vec4 bj_quat_inverse(
    struct bj_vec4 q
) {
    const bj_real n2 = bj_quat_norm2(q);
    if (n2 <= BJ_EPSILON) {
        return bj_quat_identity();
    }
    const bj_real inv = BJ_F(1.0) / n2;
    return (struct bj_vec4){ -q.x * inv, -q.y * inv, -q.z * inv, q.w * inv };
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Hamilton product \c p * q.
/// 
/// \details
/// Composition order follows standard Hamilton convention. When used to rotate
/// vectors via \c v' = q * v * q^{-1}, apply \c q on the left.
/// 
/// \param p Left quaternion.
/// \param q Right quaternion.
/// \return Product \c p*q.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE struct bj_vec4 bj_quat_mul(
    struct bj_vec4 p,
    struct bj_vec4 q
) {
    return (struct bj_vec4){
        p.w*q.x + p.x*q.w + p.y*q.z - p.z*q.y,
        p.w*q.y - p.x*q.z + p.y*q.w + p.z*q.x,
        p.w*q.z + p.x*q.y - p.y*q.x + p.z*q.w,
        p.w*q.w - p.x*q.x - p.y*q.y - p.z*q.z
    };
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Spherical linear interpolation between two orientations.
/// 
/// \details
/// Interpolates along the shortest arc on S^3. If inputs are nearly parallel,
/// falls back to normalized linear interpolation to avoid divide-by-zero.
/// 
/// \param a Start quaternion.
/// \param b End quaternion.
/// \param t Interpolation factor in [0,1].
/// \return Interpolated quaternion.
/// 
/// \note Inputs need not be normalized; the result is normalized.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE struct bj_vec4 bj_quat_slerp(
    struct bj_vec4 a,
    struct bj_vec4 b,
    bj_real t
) {
    bj_real cos_omega = bj_quat_dot(a, b);
    struct bj_vec4 bb = b;
    if (cos_omega < BJ_FZERO) {
        cos_omega = -cos_omega;
        bb.x = -b.x;
        bb.y = -b.y;
        bb.z = -b.z;
        bb.w = -b.w;
    }

    /* Clamp for numerical safety */
    if (cos_omega > BJ_F(1.0)) cos_omega = BJ_F(1.0);
    if (cos_omega < -BJ_F(1.0)) cos_omega = -BJ_F(1.0);

    if (cos_omega > BJ_F(1.0) - BJ_EPSILON) {
        return bj_quat_normalize((struct bj_vec4){
            a.x + t*(bb.x - a.x),
            a.y + t*(bb.y - a.y),
            a.z + t*(bb.z - a.z),
            a.w + t*(bb.w - a.w)
        });
    }

    const bj_real omega = bj_acos(cos_omega);
    const bj_real sin_omega = bj_sin(omega);
    if (sin_omega <= BJ_EPSILON) {
        /* Fallback to nlerp */
        return bj_quat_normalize((struct bj_vec4){
            a.x + t*(bb.x - a.x),
            a.y + t*(bb.y - a.y),
            a.z + t*(bb.z - a.z),
            a.w + t*(bb.w - a.w)
        });
    }

    const bj_real wa = bj_sin((BJ_F(1.0) - t) * omega) / sin_omega;
    const bj_real wb = bj_sin(t * omega) / sin_omega;
    return (struct bj_vec4){
        wa * a.x + wb * bb.x,
        wa * a.y + wb * bb.y,
        wa * a.z + wb * bb.z,
        wa * a.w + wb * bb.w
    };
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Build a quaternion from a rotation axis and angle.
/// 
/// \param axis Rotation axis. Need not be unit length.
/// \param angle_rad Rotation angle in radians.
/// \return Quaternion representing the rotation.
/// 
/// \note Identity is returned if the axis length is near zero.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE struct bj_vec4 bj_quat_from_axis_angle(
    struct bj_vec3 axis,
    bj_real angle_rad
) {
    const bj_real alen2 = axis.x*axis.x + axis.y*axis.y + axis.z*axis.z;
    if (alen2 <= BJ_EPSILON) {
        return bj_quat_identity();
    }
    const bj_real invlen = BJ_F(1.0) / bj_sqrt(alen2);
    const struct bj_vec3 n = (struct bj_vec3){
        axis.x * invlen, axis.y * invlen, axis.z * invlen
    };
    const bj_real h = angle_rad * BJ_F(0.5);
    const bj_real s = bj_sin(h);
    const bj_real c = bj_cos(h);
    return (struct bj_vec4){ n.x * s, n.y * s, n.z * s, c };
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Rotate a 3D vector by a quaternion.
/// 
/// \param q Rotation quaternion. Expected to be unit length for pure rotation.
/// \param v Vector to rotate.
/// \return Rotated vector.
/// 
/// \note For performance, \c q is not normalized inside the function.
/// Call \c bj_quat_normalize beforehand if needed.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE struct bj_vec3 bj_quat_rotate_vec3(
    struct bj_vec4 q,
    struct bj_vec3 v
) {
    struct bj_vec3 u = (struct bj_vec3){ q.x, q.y, q.z };
    struct bj_vec3 t = bj_vec3_cross(u, v);
    t.x += t.x; t.y += t.y; t.z += t.z; /* 2*(u×v) */
    struct bj_vec3 r = (struct bj_vec3){
        v.x + q.w * t.x + (u.y * t.z - u.z * t.y),
        v.y + q.w * t.y + (u.z * t.x - u.x * t.z),
        v.z + q.w * t.z + (u.x * t.y - u.y * t.x)
    };
    return r;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Rotate a 4D vector by a quaternion, preserving \c w.
/// 
/// \param q Rotation quaternion. Expected to be unit length for pure rotation.
/// \param v Vector to rotate. Its \c w component is passed through unchanged.
/// \return Rotated vector with original \c w.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE struct bj_vec4 bj_quat_rotate_vec4(
    struct bj_vec4 q,
    struct bj_vec4 v
) {
    struct bj_vec3 r3 = bj_quat_rotate_vec3(q, (struct bj_vec3){ v.x, v.y, v.z });
    return (struct bj_vec4){ .x = r3.x, .y = r3.y, .z = r3.z, .w = v.w };
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Fill a 4×4 rotation matrix from a quaternion.
/// 
/// \param[out] M Destination matrix (column-major).
/// \param[in]  q Input quaternion. It is normalized internally.
/// 
/// \note The resulting matrix has the last row and column set to form a proper
/// rigid transform rotation block with bottom-right element equal to 1.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_quat_to_mat4(
    struct bj_mat4x4* BJ_RESTRICT M,
    struct bj_vec4              q
) {
    q = bj_quat_normalize(q);
    bj_real xx = q.x * q.x, yy = q.y * q.y, zz = q.z * q.z;
    bj_real xy = q.x * q.y, xz = q.x * q.z, yz = q.y * q.z;
    bj_real wx = q.w * q.x, wy = q.w * q.y, wz = q.w * q.z;

    bj_mat4_set_identity(M);

    bj_real* m = M->m;
    /* Column 0 */
    m[BJ_M4(0,0)] = BJ_F(1.0) - BJ_F(2.0) * (yy + zz);
    m[BJ_M4(0,1)] = BJ_F(2.0) * (xy + wz);
    m[BJ_M4(0,2)] = BJ_F(2.0) * (xz - wy);
    m[BJ_M4(0,3)] = BJ_FZERO;
    /* Column 1 */
    m[BJ_M4(1,0)] = BJ_F(2.0) * (xy - wz);
    m[BJ_M4(1,1)] = BJ_F(1.0) - BJ_F(2.0) * (xx + zz);
    m[BJ_M4(1,2)] = BJ_F(2.0) * (yz + wx);
    m[BJ_M4(1,3)] = BJ_FZERO;
    /* Column 2 */
    m[BJ_M4(2,0)] = BJ_F(2.0) * (xz + wy);
    m[BJ_M4(2,1)] = BJ_F(2.0) * (yz - wx);
    m[BJ_M4(2,2)] = BJ_F(1.0) - BJ_F(2.0) * (xx + yy);
    m[BJ_M4(2,3)] = BJ_FZERO;
    /* Column 3 */
    m[BJ_M4(3,0)] = BJ_FZERO;
    m[BJ_M4(3,1)] = BJ_FZERO;
    m[BJ_M4(3,2)] = BJ_FZERO;
    m[BJ_M4(3,3)] = BJ_F(1.0);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Build a quaternion from a 4×4 rotation matrix.
/// 
/// \details
/// Only the upper-left 3×3 block is used. Assumes it encodes a proper
/// rotation. The result is normalized.
/// 
/// \param M Source 4×4 matrix (column-major).
/// \return Quaternion representing the rotation.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE struct bj_vec4 bj_quat_from_mat4(
    const struct bj_mat4x4* BJ_RESTRICT M
) {
    const bj_real* m = M->m;
    bj_real m00 = m[BJ_M4(0,0)], m01 = m[BJ_M4(1,0)], m02 = m[BJ_M4(2,0)];
    bj_real m10 = m[BJ_M4(0,1)], m11 = m[BJ_M4(1,1)], m12 = m[BJ_M4(2,1)];
    bj_real m20 = m[BJ_M4(0,2)], m21 = m[BJ_M4(1,2)], m22 = m[BJ_M4(2,2)];

    const bj_real trace = m00 + m11 + m22;
    if (trace > BJ_FZERO) {
        const bj_real s = bj_sqrt(trace + BJ_F(1.0)) * BJ_F(2.0);
        const bj_real w = BJ_F(0.25) * s;
        const bj_real x = (m21 - m12) / s;
        const bj_real y = (m02 - m20) / s;
        const bj_real z = (m10 - m01) / s;
        return bj_quat_normalize((struct bj_vec4){ x, y, z, w });
    }

    if (m00 > m11 && m00 > m22) {
        const bj_real s = bj_sqrt(BJ_F(1.0) + m00 - m11 - m22) * BJ_F(2.0);
        const bj_real w = (m21 - m12) / s;
        const bj_real x = BJ_F(0.25) * s;
        const bj_real y = (m01 + m10) / s;
        const bj_real z = (m02 + m20) / s;
        return bj_quat_normalize((struct bj_vec4){ x, y, z, w });
    } else if (m11 > m22) {
        const bj_real s = bj_sqrt(BJ_F(1.0) - m00 + m11 - m22) * BJ_F(2.0);
        const bj_real w = (m02 - m20) / s;
        const bj_real x = (m01 + m10) / s;
        const bj_real y = BJ_F(0.25) * s;
        const bj_real z = (m12 + m21) / s;
        return bj_quat_normalize((struct bj_vec4){ x, y, z, w });
    } else {
        const bj_real s = bj_sqrt(BJ_F(1.0) - m00 - m11 + m22) * BJ_F(2.0);
        const bj_real w = (m10 - m01) / s;
        const bj_real x = (m02 + m20) / s;
        const bj_real y = (m12 + m21) / s;
        const bj_real z = BJ_F(0.25) * s;
        return bj_quat_normalize((struct bj_vec4){ x, y, z, w });
    }
}

#endif /* BJ_QUAT_H */

/// \}
