////////////////////////////////////////////////////////////////////////////////
/// \file mat.h
/// C99 math shim with bj_real precision type and scalar utilities.
////////////////////////////////////////////////////////////////////////////////
/// Matrix utilities for 2D and 3D transforms using \c bj_real and \c bj_vec*.
///
/// \file mat.h
/// Column-major storage: elements are addressed as \c m[col][row].
/// Vectors are treated as column vectors and are multiplied on the right: \c res = M * v.
///
/// Provides creation, copy, transpose, arithmetic, products, inversion,
/// orthonormalization, and common projection/viewport helpers for 3×3, 3×2, 4×4, 4×3.
///
/// \addtogroup math
/// \{
////////////////////////////////////////////////////////////////////////////////
#ifndef BJ_MAT_H
#define BJ_MAT_H

#include <banjo/api.h>
#include <banjo/math.h>
#include <banjo/vec.h>


struct bj_mat3x3_t { bj_real m[9];  };
typedef struct bj_mat3x3_t bj_mat3x3;
typedef struct bj_mat3x3_t bj_mat3;
#define BJ_M3(c,r)    ((c)*3 + (r))   /* 0<=c,r<3 */

////////////////////////////////////////////////////////////////////////////////
/// bj_mat3x2: 3×2 column-major matrix backed by bj_vec2.
/// (2D affine: 2×2 linear block plus translation column).
/// Columns are arrays of 2 components.
////////////////////////////////////////////////////////////////////////////////
struct bj_mat3x2_t { bj_real m[6];  };
typedef struct bj_mat3x2_t bj_mat3x2;
#define BJ_M32(c,r)   ((c)*2 + (r))   /* 0<=c<3, 0<=r<2 */

////////////////////////////////////////////////////////////////////////////////
/// bj_mat4x4: 4×4 column-major matrix backed by bj_vec4.
/// Columns are arrays of 4 components.
////////////////////////////////////////////////////////////////////////////////
struct bj_mat4x4_t { bj_real m[16]; };
typedef struct bj_mat4x4_t bj_mat4x4;
typedef struct bj_mat4x4_t bj_mat4;
#define BJ_M4(c,r)    ((c)*4 + (r))   /* 0<=c,r<4) */

////////////////////////////////////////////////////////////////////////////////
/// bj_mat4x3: 4×3 column-major matrix backed by bj_vec3.
/// (3D affine: 3×3 linear block plus translation row).
/// Columns are arrays of 3 components.
////////////////////////////////////////////////////////////////////////////////
struct bj_mat4x3_t { bj_real m[12]; };
typedef struct bj_mat4x3_t bj_mat4x3;
#define BJ_M43(c,r)   ((c)*3 + (r))   /* 0<=c<4, 0<=r<3 */

////////////////////////////////////////////////////////////////////////////////
/// Set a 3×3 matrix to identity.
/// \param M Output 3×3 matrix.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat3_set_identity(bj_mat3* BJ_RESTRICT M) {
    bj_real* m = M->m;
    m[BJ_M3(0, 0)] = BJ_F(1.0); m[BJ_M3(0, 1)] = BJ_FZERO;  m[BJ_M3(0, 2)] = BJ_FZERO;
    m[BJ_M3(1, 0)] = BJ_FZERO;  m[BJ_M3(1, 1)] = BJ_F(1.0); m[BJ_M3(1, 2)] = BJ_FZERO;
    m[BJ_M3(2, 0)] = BJ_FZERO;  m[BJ_M3(2, 1)] = BJ_FZERO;  m[BJ_M3(2, 2)] = BJ_F(1.0);
}

////////////////////////////////////////////////////////////////////////////////
/// Copy a matrix.
/// \param dst Output 3×3 matrix.
/// \param src Input 3×3 matrix.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat3_copy(
    bj_mat3* BJ_RESTRICT       dst,
    const bj_mat3* BJ_RESTRICT src
) {
    for (int i = 0 ; i < 9 ; ++i) {
        dst->m[i] = src->m[i];
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Extract a matrix row as a vector.
/// \param M Input 3×3 matrix.
/// \param r index (0-based).
/// \return 3D vector result.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec3 bj_mat3_row(const bj_mat3* BJ_RESTRICT M, int r) {
    const bj_real* m = M->m;
    return (bj_vec3) { 
        m[BJ_M3(0, r)], m[BJ_M3(1, r)], m[BJ_M3(2, r)] 
    };
}

////////////////////////////////////////////////////////////////////////////////
/// Extract a matrix column as a vector.
/// \param M Input 3×3 matrix.
/// \param c index (0-based).
/// \return 3D vector result.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec3 bj_mat3_col(const bj_mat3* BJ_RESTRICT M, int c) {
    const bj_real* m = M->m;
    return (bj_vec3) { m[BJ_M3(c, 0)], m[BJ_M3(c, 1)], m[BJ_M3(c, 2)] };
}

////////////////////////////////////////////////////////////////////////////////
/// Transpose a matrix.
/// \param out Output 3×3 matrix.
/// \param A Input 3×3 matrix.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat3_transpose(bj_mat3* BJ_RESTRICT out, const bj_mat3* BJ_RESTRICT A) {
    const bj_real* a = A->m;
    bj_real r[9];
    r[BJ_M3(0,0)] = a[BJ_M3(0,0)];
    r[BJ_M3(0,1)] = a[BJ_M3(1,0)];
    r[BJ_M3(0,2)] = a[BJ_M3(2,0)];
    r[BJ_M3(1,0)] = a[BJ_M3(0,1)];
    r[BJ_M3(1,1)] = a[BJ_M3(1,1)];
    r[BJ_M3(1,2)] = a[BJ_M3(2,1)];
    r[BJ_M3(2,0)] = a[BJ_M3(0,2)];
    r[BJ_M3(2,1)] = a[BJ_M3(1,2)];
    r[BJ_M3(2,2)] = a[BJ_M3(2,2)];
    for (int i = 0 ; i<9 ; ++i) {
        out->m[i] = r[i];
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Component-wise addition: out = A + B.
/// \param out Output 3×3 matrix.
/// \param A Input 3×3 matrix.
/// \param B Input 3×3 matrix.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat3_add(
    bj_mat3* BJ_RESTRICT       out,
    const bj_mat3* BJ_RESTRICT A,
    const bj_mat3* BJ_RESTRICT B
) {
    for (int i = 0 ; i < 9 ; ++i) {
        out->m[i] = A->m[i] + B->m[i];
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Component-wise subtraction: out = A - B.
/// \param out Output 3×3 matrix.
/// \param A Input 3×3 matrix.
/// \param B Input 3×3 matrix.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat3_sub(
    bj_mat3* BJ_RESTRICT       out,
    const bj_mat3* BJ_RESTRICT A,
    const bj_mat3* BJ_RESTRICT B
) {
    for (int i = 0 ; i < 9 ; ++i) {
        out->m[i] = A->m[i] - B->m[i];
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Scalar multiply: out = A * k.
/// \param out Output 3×3 matrix.
/// \param A Input 3×3 matrix.
/// \param k Uniform scale factor.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat3_mul_scalar(
    bj_mat3* BJ_RESTRICT       out,
    const bj_mat3* BJ_RESTRICT A,
    bj_real                    k
) {
    for (int i = 0 ; i < 9 ; ++i) {
        out->m[i] = A->m[i] * k;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Matrix product: out = A * B.
/// \param out Output 3×3 matrix.
/// \param A Input 3×3 matrix.
/// \param B Input 3×3 matrix.
/// \note Column-major storage. Vectors are columns. Right-multiply: r = M * v.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat3_mul(
    bj_mat3* BJ_RESTRICT out,
    const bj_mat3* BJ_RESTRICT A,
    const bj_mat3* BJ_RESTRICT B
) {
    const bj_real* a = A->m;
    const bj_real* b = B->m;
    bj_real* o       = out->m;

    for (int c = 0; c < 3; ++c) {
        const bj_real b0 = b[BJ_M3(c, 0)];
        const bj_real b1 = b[BJ_M3(c, 1)];
        const bj_real b2 = b[BJ_M3(c, 2)];
        o[BJ_M3(c,0)] = a[BJ_M3(0,0)] * b0 + a[BJ_M3(1,0)] * b1 + a[BJ_M3(2,0)] * b2;
        o[BJ_M3(c,1)] = a[BJ_M3(0,1)] * b0 + a[BJ_M3(1,1)] * b1 + a[BJ_M3(2,1)] * b2;
        o[BJ_M3(c,2)] = a[BJ_M3(0,2)] * b0 + a[BJ_M3(1,2)] * b1 + a[BJ_M3(2,2)] * b2;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Multiply a 3×3 matrix by a 3D vector: r = M * v.
/// \param M Input 3×3 matrix.
/// \param v Input vector.
/// \return 3D vector result.
/// \note Column-major storage. Vectors are columns. Right-multiply: r = M * v.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec3 bj_mat3_transform_vec3(
    const bj_mat3* BJ_RESTRICT M,
    bj_vec3                    v
) {
    const bj_real* m = M->m;
    return (bj_vec3) {
        m[BJ_M3(0,0)] * v.x + m[BJ_M3(1,0)] * v.y + m[BJ_M3(2,0)] * v.z,
        m[BJ_M3(0,1)] * v.x + m[BJ_M3(1,1)] * v.y + m[BJ_M3(2,1)] * v.z,
        m[BJ_M3(0,2)] * v.x + m[BJ_M3(1,2)] * v.y + m[BJ_M3(2,2)] * v.z
    };
}

////////////////////////////////////////////////////////////////////////////////
/// Transform a 2D point by a 3×3 homogeneous transform.
/// \param M Input 3×3 matrix.
/// \param p Input point.
/// \return 2D vector result.
/// \note Homogeneous 2D. Applies projective divide if w ≠ 0.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec2 bj_mat3_transform_point(
    const bj_mat3* BJ_RESTRICT M,
    bj_vec2                    p
) {
    const bj_real* m = M->m;
    const bj_real x  = p.x;
    const bj_real y  = p.y;
    const bj_real w  = m[BJ_M3(0,2)] * x + m[BJ_M3(1,2)] * y + m[BJ_M3(2,2)];
    const bj_real rx = m[BJ_M3(0,0)] * x + m[BJ_M3(1,0)] * y + m[BJ_M3(2,0)];
    const bj_real ry = m[BJ_M3(0,1)] * x + m[BJ_M3(1,1)] * y + m[BJ_M3(2,1)];
    if (w !=  BJ_FZERO) {
        return (bj_vec2) { rx / w, ry / w };
    }
    return (bj_vec2) { rx, ry };
}

////////////////////////////////////////////////////////////////////////////////
/// Build a 3×3 translation matrix.
/// \param M Output 3×3 matrix.
/// \param tx Translation along x.
/// \param ty Translation along y.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat3_set_translation(
    bj_mat3* BJ_RESTRICT M,
    bj_real              tx,
    bj_real              ty
) {
    bj_mat3_set_identity(M);
    M->m[BJ_M3(2,0)] = tx;
    M->m[BJ_M3(2,1)] = ty;
}

////////////////////////////////////////////////////////////////////////////////
/// Right-multiply by a translation: M = M * T(tx,ty).
/// \param M Output 3×3 matrix.
/// \param tx Translation along x.
/// \param ty Translation along y.
/// \note Right-multiply in place.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat3_translate(
    bj_mat3* BJ_RESTRICT M,
    bj_real              tx,
    bj_real              ty
) {
    const bj_vec3 t = { tx, ty, BJ_FZERO };
    for (int r = 0 ; r<3 ; ++r) {
        const bj_vec3 row = bj_mat3_row(M, r);
        M->m[BJ_M3(2,r)] +=  row.x * t.x + row.y * t.y + row.z * t.z;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Apply non-uniform XY scale to a 3×3 matrix.
/// \param M Output 3×3 matrix.
/// \param sx Scale on x.
/// \param sy Scale on y.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat3_set_scaling_xy(
    bj_mat3* BJ_RESTRICT M,
    bj_real              sx,
    bj_real              sy
) {
    bj_mat3_set_identity(M);
    M->m[BJ_M3(0,0)] = sx;
    M->m[BJ_M3(1,1)] = sy;
}

////////////////////////////////////////////////////////////////////////////////
/// Build an XY shear into a 3×3 matrix.
/// \param M Output 3×3 matrix.
/// \param shx Input scalar.
/// \param shy Input scalar.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat3_set_shear_xy(
    bj_mat3* BJ_RESTRICT M,
    bj_real              shx,
    bj_real              shy
) {
    bj_mat3_set_identity(M);
    M->m[BJ_M3(1,0)] = shy;
    M->m[BJ_M3(0,1)] = shx;
}

////////////////////////////////////////////////////////////////////////////////
/// Matrix operation.
/// \param M Output 3×3 matrix.
/// \param angle Rotation angle in radians.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat3_set_rotation_z(
    bj_mat3* BJ_RESTRICT M,
    bj_real              angle
) {
    const bj_real s = bj_sin(angle);
    const bj_real c = bj_cos(angle);
    bj_real* m = M->m;
    m[BJ_M3(0,0)] =  c;
    m[BJ_M3(0,1)] =  s;
    m[BJ_M3(0,2)] = BJ_FZERO;
    m[BJ_M3(1,0)] = -s;
    m[BJ_M3(1,1)] =  c;
    m[BJ_M3(1,2)] = BJ_FZERO;
    m[BJ_M3(2,0)] = BJ_FZERO;
    m[BJ_M3(2,1)] = BJ_FZERO;
    m[BJ_M3(2,2)] = BJ_F(1.0);
}

////////////////////////////////////////////////////////////////////////////////
/// Determinant of a 3×3 matrix.
/// \param A Input 3×3 matrix.
/// \return bj_real.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_real bj_mat3_determinant(
    const bj_mat3* BJ_RESTRICT A
) {
    const bj_real* m = A->m;
    return
      m[BJ_M3(0,0)] * (m[BJ_M3(1,1)] * m[BJ_M3(2,2)]-m[BJ_M3(2,1)] * m[BJ_M3(1,2)])
    - m[BJ_M3(1,0)] * (m[BJ_M3(0,1)] * m[BJ_M3(2,2)]-m[BJ_M3(2,1)] * m[BJ_M3(0,2)])
    + m[BJ_M3(2,0)] * (m[BJ_M3(0,1)] * m[BJ_M3(1,2)]-m[BJ_M3(1,1)] * m[BJ_M3(0,2)]);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Invert a 3×3 matrix (safe, adjugate).
/// 
/// \details Computes A^{-1} = adj(A) / det(A). If det(A) is zero by
/// bj_real_is_zero, returns 0 and leaves \p out unspecified.
/// 
/// \param out Output 3×3 matrix.
/// \param A   Input 3×3 matrix.
/// \return \ref BJ_TRUE on success, \ref BJ_FALSE if singular or 
///         near-singular by bj_real_is_zero(det).
/// 
/// \warning Adj/Det method is numerically unstable for ill-conditioned A.
/// \see bj_mat3_invert_unsafe
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_bool bj_mat3_invert(
    bj_mat3* BJ_RESTRICT       out,
    const bj_mat3* BJ_RESTRICT A
) {
    const bj_real* m = A->m;
    const bj_real c00 =  (m[BJ_M3(1,1)] * m[BJ_M3(2,2)] - m[BJ_M3(2,1)] * m[BJ_M3(1,2)]);
    const bj_real c01 = -(m[BJ_M3(0,1)] * m[BJ_M3(2,2)] - m[BJ_M3(2,1)] * m[BJ_M3(0,2)]);
    const bj_real c02 =  (m[BJ_M3(0,1)] * m[BJ_M3(1,2)] - m[BJ_M3(1,1)] * m[BJ_M3(0,2)]);
    const bj_real c10 = -(m[BJ_M3(1,0)] * m[BJ_M3(2,2)] - m[BJ_M3(2,0)] * m[BJ_M3(1,2)]);
    const bj_real c11 =  (m[BJ_M3(0,0)] * m[BJ_M3(2,2)] - m[BJ_M3(2,0)] * m[BJ_M3(0,2)]);
    const bj_real c12 = -(m[BJ_M3(0,0)] * m[BJ_M3(1,2)] - m[BJ_M3(1,0)] * m[BJ_M3(0,2)]);
    const bj_real c20 =  (m[BJ_M3(1,0)] * m[BJ_M3(2,1)] - m[BJ_M3(2,0)] * m[BJ_M3(1,1)]);
    const bj_real c21 = -(m[BJ_M3(0,0)] * m[BJ_M3(2,1)] - m[BJ_M3(2,0)] * m[BJ_M3(0,1)]);
    const bj_real c22 =  (m[BJ_M3(0,0)] * m[BJ_M3(1,1)] - m[BJ_M3(1,0)] * m[BJ_M3(0,1)]);
    const bj_real det = m[BJ_M3(0,0)] * c00 + m[BJ_M3(1,0)] * c01 + m[BJ_M3(2,0)] * c02;

    if (bj_real_is_zero(det)) {
        return BJ_FALSE;
    }
    const bj_real id = BJ_F(1.0) / det;
    out->m[BJ_M3(0,0)] = c00 * id; out->m[BJ_M3(1,0)] = c01 * id; out->m[BJ_M3(2,0)] = c02 * id;
    out->m[BJ_M3(0,1)] = c10 * id; out->m[BJ_M3(1,1)] = c11 * id; out->m[BJ_M3(2,1)] = c12 * id;
    out->m[BJ_M3(0,2)] = c20 * id; out->m[BJ_M3(1,2)] = c21 * id; out->m[BJ_M3(2,2)] = c22 * id;
    return BJ_TRUE;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Invert a 3×3 matrix (unsafe, adjugate).
/// 
/// \details Same as bj_mat3_invert but skips the singularity check and divides
/// by det(A) unconditionally. Faster when invertibility is guaranteed.
/// 
/// \param out Output 3×3 matrix.
/// \param A   Input 3×3 matrix (must be nonsingular).
/// \pre det(A) != 0 and A is well-conditioned for numerical stability.
/// \warning Division by zero if det(A) == 0. Consider bj_mat3_invert instead.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat3_invert_unsafe(
    bj_mat3* BJ_RESTRICT       out,
    const bj_mat3* BJ_RESTRICT A
) {
    const bj_real* m = A->m;
    const bj_real c00 =  (m[BJ_M3(1,1)] * m[BJ_M3(2,2)] - m[BJ_M3(2,1)] * m[BJ_M3(1,2)]);
    const bj_real c01 = -(m[BJ_M3(0,1)] * m[BJ_M3(2,2)] - m[BJ_M3(2,1)] * m[BJ_M3(0,2)]);
    const bj_real c02 =  (m[BJ_M3(0,1)] * m[BJ_M3(1,2)] - m[BJ_M3(1,1)] * m[BJ_M3(0,2)]);
    const bj_real c10 = -(m[BJ_M3(1,0)] * m[BJ_M3(2,2)] - m[BJ_M3(2,0)] * m[BJ_M3(1,2)]);
    const bj_real c11 =  (m[BJ_M3(0,0)] * m[BJ_M3(2,2)] - m[BJ_M3(2,0)] * m[BJ_M3(0,2)]);
    const bj_real c12 = -(m[BJ_M3(0,0)] * m[BJ_M3(1,2)] - m[BJ_M3(1,0)] * m[BJ_M3(0,2)]);
    const bj_real c20 =  (m[BJ_M3(1,0)] * m[BJ_M3(2,1)] - m[BJ_M3(2,0)] * m[BJ_M3(1,1)]);
    const bj_real c21 = -(m[BJ_M3(0,0)] * m[BJ_M3(2,1)] - m[BJ_M3(2,0)] * m[BJ_M3(0,1)]);
    const bj_real c22 =  (m[BJ_M3(0,0)] * m[BJ_M3(1,1)] - m[BJ_M3(1,0)] * m[BJ_M3(0,1)]);
    const bj_real id  = BJ_F(1.0) / (m[BJ_M3(0,0)] * c00 + m[BJ_M3(1,0)] * c01 + m[BJ_M3(2,0)] * c02);

    out->m[BJ_M3(0,0)] = c00 * id; out->m[BJ_M3(1,0)] = c01 * id; out->m[BJ_M3(2,0)] = c02 * id;
    out->m[BJ_M3(0,1)] = c10 * id; out->m[BJ_M3(1,1)] = c11 * id; out->m[BJ_M3(2,1)] = c12 * id;
    out->m[BJ_M3(0,2)] = c20 * id; out->m[BJ_M3(1,2)] = c21 * id; out->m[BJ_M3(2,2)] = c22 * id;
}


////////////////////////////////////////////////////////////////////////////////
/// Build a 2D orthographic projection into a 3×3 matrix.
/// \param M Output 3×3 matrix.
/// \param l Input scalar.
/// \param r scalar (0-based).
/// \param b Input scalar.
/// \param t Input scalar.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat3_set_ortho(
    bj_mat3* BJ_RESTRICT M,
    bj_real l, bj_real r,
    bj_real b, bj_real t
) {
    bj_real* m = M->m;
    m[BJ_M3(0,0)] = BJ_F(2.0)/(r-l);
    m[BJ_M3(0,1)] = BJ_FZERO;
    m[BJ_M3(0,2)] = BJ_FZERO;

    m[BJ_M3(1,0)] = BJ_FZERO;
    m[BJ_M3(1,1)] = BJ_F(-2.0)/(t-b);
    m[BJ_M3(1,2)] = BJ_FZERO;

    m[BJ_M3(2,0)] = -(r+l)/(r-l);
    m[BJ_M3(2,1)] =  (t+b)/(t-b);
    m[BJ_M3(2,2)] = BJ_F(1.0);
}

////////////////////////////////////////////////////////////////////////////////
/// Build a 2D viewport transform into a 3×3 matrix.
/// \param M Output 3×3 matrix.
/// \param x Viewport X origin in pixels.
/// \param y Viewport Y origin in pixels.
/// \param w Viewport width in pixels.
/// \param h Viewport height in pixels.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat3_set_viewport(
    bj_mat3* BJ_RESTRICT M,
    bj_real              x,
    bj_real              y,
    bj_real              w,
    bj_real              h
) {
    bj_real* m = M->m;
    m[BJ_M3(0,0)] = BJ_F(0.5) * w;
    m[BJ_M3(0,1)] = BJ_FZERO;
    m[BJ_M3(0,2)] = BJ_FZERO;
    m[BJ_M3(1,0)] = BJ_FZERO;
    m[BJ_M3(1,1)] = BJ_F(0.5) * h;
    m[BJ_M3(1,2)] = BJ_FZERO;
    m[BJ_M3(2,0)] = x + BJ_F(0.5) * w;
    m[BJ_M3(2,1)] = y + BJ_F(0.5) * h;
    m[BJ_M3(2,2)] = BJ_F(1.0);
}

////////////////////////////////////////////////////////////////////////////////
/// Set a 3×2 affine matrix to identity.
/// \param M Output 3×2 matrix.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat3x2_set_identity(
    bj_mat3x2* BJ_RESTRICT M
) {
    bj_real* m = M->m;
    m[BJ_M32(0,0)] = BJ_F(1.0);
    m[BJ_M32(0,1)] = BJ_FZERO;
    m[BJ_M32(1,0)] = BJ_FZERO;
    m[BJ_M32(1,1)] = BJ_F(1.0);
    m[BJ_M32(2,0)] = BJ_FZERO;
    m[BJ_M32(2,1)] = BJ_FZERO;
}

////////////////////////////////////////////////////////////////////////////////
/// Matrix operation.
/// \param M Output 3×2 matrix.
/// \param tx Translation along x.
/// \param ty Translation along y.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat3x2_set_translation(
    bj_mat3x2* BJ_RESTRICT M,
    bj_real                tx,
    bj_real                ty
) {
    bj_mat3x2_set_identity(M);
    M->m[BJ_M32(2,0)] = tx;
    M->m[BJ_M32(2,1)] = ty;
}

////////////////////////////////////////////////////////////////////////////////
/// Scalar multiply: M = sx * k.
/// \param M Output 3×2 matrix.
/// \param sx Scale on x.
/// \param sy Scale on y.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat3x2_set_scaling_xy(
    bj_mat3x2* BJ_RESTRICT M,
    bj_real                sx,
    bj_real                sy
) {
    bj_mat3x2_set_identity(M);
    M->m[BJ_M32(0,0)] = sx;
    M->m[BJ_M32(1,1)] = sy;
}

////////////////////////////////////////////////////////////////////////////////
/// Matrix operation.
/// \param M Output 3×2 matrix.
/// \param angle Rotation angle in radians.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat3x2_set_rotation_z(
    bj_mat3x2* BJ_RESTRICT M,
    bj_real                angle
) {
    const bj_real c = bj_cos(angle);
    const bj_real s = bj_sin(angle);
    bj_real* m = M->m;
    m[BJ_M32(0,0)] =  c;
    m[BJ_M32(0,1)] =  s;
    m[BJ_M32(1,0)] = -s;
    m[BJ_M32(1,1)] =  c;
    m[BJ_M32(2,0)] = BJ_FZERO;
    m[BJ_M32(2,1)] = BJ_FZERO;
}

////////////////////////////////////////////////////////////////////////////////
/// Matrix product: out = A * B.
/// \param out Output 3×2 matrix.
/// \param A Input 3×2 matrix.
/// \param B Input 3×2 matrix.
/// \note Column-major storage. Vectors are columns. Right-multiply: r = M * v.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat3x2_mul(
    bj_mat3x2* BJ_RESTRICT       out,
    const bj_mat3x2* BJ_RESTRICT A,
    const bj_mat3x2* BJ_RESTRICT B
) {
    const bj_real a00 = A->m[BJ_M32(0,0)];
    const bj_real a10 = A->m[BJ_M32(0,1)];
    const bj_real a01 = A->m[BJ_M32(1,0)];
    const bj_real a11 = A->m[BJ_M32(1,1)];
    const bj_real a02 = A->m[BJ_M32(2,0)];
    const bj_real a12 = A->m[BJ_M32(2,1)];

    const bj_real b00 = B->m[BJ_M32(0,0)];
    const bj_real b10 = B->m[BJ_M32(0,1)];
    const bj_real b01 = B->m[BJ_M32(1,0)];
    const bj_real b11 = B->m[BJ_M32(1,1)];
    const bj_real b02 = B->m[BJ_M32(2,0)];
    const bj_real b12 = B->m[BJ_M32(2,1)];

    out->m[BJ_M32(0,0)] = a00*b00 + a01*b10;
    out->m[BJ_M32(0,1)] = a10*b00 + a11*b10;
    out->m[BJ_M32(1,0)] = a00*b01 + a01*b11;
    out->m[BJ_M32(1,1)] = a10*b01 + a11*b11;
    out->m[BJ_M32(2,0)] = a00*b02 + a01*b12 + a02;
    out->m[BJ_M32(2,1)] = a10*b02 + a11*b12 + a12;
}


////////////////////////////////////////////////////////////////////////////////
/// Transform a 2D point by a 3×2 affine matrix.
/// \param M Input 3×2 matrix.
/// \param p Input point.
/// \return 2D vector result.
/// \note Homogeneous 2D. Applies projective divide if w ≠ 0.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec2 bj_mat3x2_transform_point(
    const bj_mat3x2* BJ_RESTRICT M,
    bj_vec2                      p
) {
    const bj_real* m = M->m;
    const bj_real x = p.x;
    const bj_real y = p.y;
    return (bj_vec2) {
        m[BJ_M32(0,0)] * x + m[BJ_M32(1,0)] * y + m[BJ_M32(2,0)],
        m[BJ_M32(0,1)] * x + m[BJ_M32(1,1)] * y + m[BJ_M32(2,1)]
    };
}

////////////////////////////////////////////////////////////////////////////////
/// Transform a direction vector (ignoring translation).
/// \param M Input 3×2 matrix.
/// \param v Input vector.
/// \return 2D vector result.
/// \note Homogeneous 2D. Applies projective divide if w ≠ 0.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec2 bj_mat3x2_transform_dir(
    const bj_mat3x2* BJ_RESTRICT M,
    bj_vec2                      v
) {
    const bj_real* m = M->m;
    const bj_real x  = v.x;
    const bj_real y  = v.y;
    return (bj_vec2) {
        m[BJ_M32(0,0)] * x + m[BJ_M32(1,0)] * y,
        m[BJ_M32(0,1)] * x + m[BJ_M32(1,1)] * y
    };
}

////////////////////////////////////////////////////////////////////////////////
/// Promote a 3×2 affine matrix to 3×3.
/// \param M Output 3×3 matrix.
/// \param A Input 3×2 matrix.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat3_from_mat3x2(
    bj_mat3* BJ_RESTRICT         M,
    const bj_mat3x2* BJ_RESTRICT A
) {
    bj_real* o = M->m;
    const bj_real* a = A->m;
    o[BJ_M3(0,0)] = a[BJ_M32(0,0)];
    o[BJ_M3(0,1)] = a[BJ_M32(0,1)];
    o[BJ_M3(0,2)] = BJ_FZERO;
    o[BJ_M3(1,0)] = a[BJ_M32(1,0)];
    o[BJ_M3(1,1)] = a[BJ_M32(1,1)];
    o[BJ_M3(1,2)] = BJ_FZERO;
    o[BJ_M3(2,0)] = a[BJ_M32(2,0)];
    o[BJ_M3(2,1)] = a[BJ_M32(2,1)];
    o[BJ_M3(2,2)] = BJ_F(1.0);
}

////////////////////////////////////////////////////////////////////////////////
/// Demote a 3×3 matrix to 3×2 (drop projective terms).
/// \param M Output 3×2 matrix.
/// \param A Input 3×3 matrix.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat3x2_from_mat3(
    bj_mat3x2* BJ_RESTRICT     M,
    const bj_mat3* BJ_RESTRICT A
) {
    const bj_real* a = A->m;
    bj_real* o = M->m;
    o[BJ_M32(0,0)] = a[BJ_M3(0,0)];
    o[BJ_M32(0,1)] = a[BJ_M3(0,1)];
    o[BJ_M32(1,0)] = a[BJ_M3(1,0)];
    o[BJ_M32(1,1)] = a[BJ_M3(1,1)];
    o[BJ_M32(2,0)] = a[BJ_M3(2,0)];
    o[BJ_M32(2,1)] = a[BJ_M3(2,1)];
}

////////////////////////////////////////////////////////////////////////////////
/// Set a 4×4 matrix to identity.
/// \param M Output 4×4 matrix.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4_set_identity(
    bj_mat4* BJ_RESTRICT M
) {
    bj_real* m = M->m;
    m[BJ_M4(0,0)] = BJ_F(1.0);
    m[BJ_M4(0,1)] = BJ_FZERO;
    m[BJ_M4(0,2)] = BJ_FZERO;
    m[BJ_M4(0,3)] = BJ_FZERO;
    m[BJ_M4(1,0)] = BJ_FZERO;
    m[BJ_M4(1,1)] = BJ_F(1.0);
    m[BJ_M4(1,2)] = BJ_FZERO;
    m[BJ_M4(1,3)] = BJ_FZERO;
    m[BJ_M4(2,0)] = BJ_FZERO;
    m[BJ_M4(2,1)] = BJ_FZERO;
    m[BJ_M4(2,2)] = BJ_F(1.0);
    m[BJ_M4(2,3)] = BJ_FZERO;
    m[BJ_M4(3,0)] = BJ_FZERO;
    m[BJ_M4(3,1)] = BJ_FZERO;
    m[BJ_M4(3,2)] = BJ_FZERO;
    m[BJ_M4(3,3)] = BJ_F(1.0);
}

////////////////////////////////////////////////////////////////////////////////
/// Copy a matrix.
/// \param dst Output 4×4 matrix.
/// \param src Input 4×4 matrix.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4_copy(
    bj_mat4* BJ_RESTRICT       dst,
    const bj_mat4* BJ_RESTRICT src
) {
    for(int i = 0 ; i < 16 ; ++i) {
        dst->m[i] = src->m[i];
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Extract a matrix row as a vector.
/// \param M Input 4×4 matrix.
/// \param r index (0-based).
/// \return 4D vector result.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec4 bj_mat4_row(
    const bj_mat4* BJ_RESTRICT M,
    int                        r
) {
    const bj_real* m = M->m;
    return (bj_vec4) { 
        m[BJ_M4(0,r)],
        m[BJ_M4(1,r)],
        m[BJ_M4(2,r)],
        m[BJ_M4(3,r)] 
    };
}

////////////////////////////////////////////////////////////////////////////////
/// Extract a matrix column as a vector.
/// \param M Input 4×4 matrix.
/// \param c index (0-based).
/// \return 4D vector result.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec4 bj_mat4_col(
    const bj_mat4* BJ_RESTRICT M,
    int                        c
) {
    const bj_real* m = M->m;
    return (bj_vec4) { 
        m[BJ_M4(c,0)],
        m[BJ_M4(c,1)],
        m[BJ_M4(c,2)],
        m[BJ_M4(c,3)] 
    };
}

////////////////////////////////////////////////////////////////////////////////
/// Transpose a matrix.
/// \param out Output 4×4 matrix.
/// \param A Input 4×4 matrix.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4_transpose(
    bj_mat4* BJ_RESTRICT       out,
    const bj_mat4* BJ_RESTRICT A
) {
    const bj_real* a = A->m;
    bj_real r[16];
    for(int c = 0 ; c < 4; ++c) {
        for(int r0 = 0 ; r0 < 4; ++r0) {
            r[BJ_M4(r0,c)] = a[BJ_M4(c,r0)];
        }
    }
    for(int i = 0 ; i < 16 ; ++i) {
        out->m[i] = r[i];
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Component-wise addition: out = A + B.
/// \param out Output 4×4 matrix.
/// \param A Input 4×4 matrix.
/// \param B Input 4×4 matrix.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4_add(
    bj_mat4* BJ_RESTRICT       out,
    const bj_mat4* BJ_RESTRICT A,
    const bj_mat4* BJ_RESTRICT B
) {

    for(int i = 0 ; i < 16 ; ++i) {
        out->m[i] = A->m[i] + B->m[i];
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Component-wise subtraction: out = A - B.
/// \param out Output 4×4 matrix.
/// \param A Input 4×4 matrix.
/// \param B Input 4×4 matrix.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4_sub(
    bj_mat4* BJ_RESTRICT       out,
    const bj_mat4* BJ_RESTRICT A,
    const bj_mat4* BJ_RESTRICT B
) {

    for(int i = 0 ; i < 16 ; ++i) {
        out->m[i] = A->m[i] - B->m[i];
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Scalar multiply: out = A * k.
/// \param out Output 4×4 matrix.
/// \param A Input 4×4 matrix.
/// \param k Uniform scale factor.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4_mul_scalar(
    bj_mat4* BJ_RESTRICT       out,
    const bj_mat4* BJ_RESTRICT A,
    bj_real                    k
) {

    for(int i = 0;i<16;++i) {
        out->m[i] = A->m[i] * k;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Scale basis vectors of a 4×4 matrix by per-axis factors.
/// \param out Output 4×4 matrix.
/// \param A Input 4×4 matrix.
/// \param sx Scale on x.
/// \param sy Scale on y.
/// \param sz Scale on z.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4_scale_axes(
    bj_mat4* BJ_RESTRICT       out,
    const bj_mat4* BJ_RESTRICT A,
    bj_real                    sx,
    bj_real                    sy,
    bj_real                    sz
) {

    const bj_real* a = A->m;
    bj_real* m = out->m;
    m[BJ_M4(0,0)] = a[BJ_M4(0,0)] * sx;
    m[BJ_M4(0,1)] = a[BJ_M4(0,1)] * sx;
    m[BJ_M4(0,2)] = a[BJ_M4(0,2)] * sx;
    m[BJ_M4(0,3)] = a[BJ_M4(0,3)] * sx;
    m[BJ_M4(1,0)] = a[BJ_M4(1,0)] * sy;
    m[BJ_M4(1,1)] = a[BJ_M4(1,1)] * sy;
    m[BJ_M4(1,2)] = a[BJ_M4(1,2)] * sy;
    m[BJ_M4(1,3)] = a[BJ_M4(1,3)] * sy;
    m[BJ_M4(2,0)] = a[BJ_M4(2,0)] * sz;
    m[BJ_M4(2,1)] = a[BJ_M4(2,1)] * sz;
    m[BJ_M4(2,2)] = a[BJ_M4(2,2)] * sz;
    m[BJ_M4(2,3)] = a[BJ_M4(2,3)] * sz;
    m[BJ_M4(3,0)] = a[BJ_M4(3,0)];
    m[BJ_M4(3,1)] = a[BJ_M4(3,1)];
    m[BJ_M4(3,2)] = a[BJ_M4(3,2)];
    m[BJ_M4(3,3)] = a[BJ_M4(3,3)];
}

////////////////////////////////////////////////////////////////////////////////
/// Matrix product: out = A * B.
/// \param out Output 4×4 matrix.
/// \param A Input 4×4 matrix.
/// \param B Input 4×4 matrix.
/// \note Column-major storage. Vectors are columns. Right-multiply: r = M * v.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4_mul(
    bj_mat4* BJ_RESTRICT       out,
    const bj_mat4* BJ_RESTRICT A,
    const bj_mat4* BJ_RESTRICT B
)
{
    const bj_real* a = A->m;
    const bj_real* b = B->m;
    bj_real* o = out->m;

    for (int c = 0; c < 4; ++c) {
        const bj_real b0 = b[BJ_M4(c,0)];
        const bj_real b1 = b[BJ_M4(c,1)];
        const bj_real b2 = b[BJ_M4(c,2)];
        const bj_real b3 = b[BJ_M4(c,3)];

        o[BJ_M4(c,0)] = a[BJ_M4(0,0)] * b0 
                      + a[BJ_M4(1,0)] * b1 
                      + a[BJ_M4(2,0)] * b2 
                      + a[BJ_M4(3,0)] * b3;
        o[BJ_M4(c,1)] = a[BJ_M4(0,1)] * b0 
                      + a[BJ_M4(1,1)] * b1 
                      + a[BJ_M4(2,1)] * b2 
                      + a[BJ_M4(3,1)] * b3;
        o[BJ_M4(c,2)] = a[BJ_M4(0,2)] * b0 
                      + a[BJ_M4(1,2)] * b1 
                      + a[BJ_M4(2,2)] * b2 
                      + a[BJ_M4(3,2)] * b3;
        o[BJ_M4(c,3)] = a[BJ_M4(0,3)] * b0 
                      + a[BJ_M4(1,3)] * b1 
                      + a[BJ_M4(2,3)] * b2 
                      + a[BJ_M4(3,3)] * b3;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Multiply a 4×4 matrix by a 4D vector: r = M * v.
/// \param M Input 4×4 matrix.
/// \param v Input vector.
/// \return 4D vector result.
/// \note Column-major storage. Vectors are columns. Right-multiply: r = M * v.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec4 bj_mat4_transform_vec4(
    const bj_mat4* BJ_RESTRICT M,
    bj_vec4                    v
) {
    const bj_real* m = M->m;
    return (bj_vec4) {
        m[BJ_M4(0,0)] * v.x + m[BJ_M4(1,0)] * v.y + 
        m[BJ_M4(2,0)] * v.z + m[BJ_M4(3,0)] * v.w,
        m[BJ_M4(0,1)] * v.x + m[BJ_M4(1,1)] * v.y + 
        m[BJ_M4(2,1)] * v.z + m[BJ_M4(3,1)] * v.w,
        m[BJ_M4(0,2)] * v.x + m[BJ_M4(1,2)] * v.y + 
        m[BJ_M4(2,2)] * v.z + m[BJ_M4(3,2)] * v.w,
        m[BJ_M4(0,3)] * v.x + m[BJ_M4(1,3)] * v.y + 
        m[BJ_M4(2,3)] * v.z + m[BJ_M4(3,3)] * v.w
    };
}

////////////////////////////////////////////////////////////////////////////////
/// Build a 4×4 translation matrix.
/// \param M Output 4×4 matrix.
/// \param x Viewport X origin in pixels.
/// \param y Viewport Y origin in pixels.
/// \param z Input scalar.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4_set_translation(
    bj_mat4* BJ_RESTRICT M,
    bj_real              x,
    bj_real              y,
    bj_real              z
) {
    bj_mat4_set_identity(M);
    M->m[BJ_M4(3,0)] = x;
    M->m[BJ_M4(3,1)] = y;
    M->m[BJ_M4(3,2)] = z;
}

////////////////////////////////////////////////////////////////////////////////
/// Right-multiply by a translation: M = M * T(tx,ty,tz).
/// \param M Output 4×4 matrix.
/// \param x Viewport X origin in pixels.
/// \param y Viewport Y origin in pixels.
/// \param z Input scalar.
/// \note Right-multiply in place.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4_translate(
    bj_mat4* BJ_RESTRICT M,
    bj_real x,
    bj_real y,
    bj_real z
) {
    const bj_vec4 t = { x, y, z, BJ_FZERO };
    for (int r = 0 ; r<4 ; ++r) {
        const bj_vec4 row = bj_mat4_row(M, r);
        M->m[BJ_M4(3,r)] +=  bj_vec4_dot(row, t);
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Add outer product r += s * v^T to a 4×4 matrix.
/// \param out Output 4×4 matrix.
/// \param a Input 3D vector.
/// \param b Input 3D vector.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4_set_outer_product(
    bj_mat4* BJ_RESTRICT out,
    bj_vec3              a,
    bj_vec3              b
) {
    bj_real* m = out->m;
    m[BJ_M4(0,0)] = a.x*b.x;
    m[BJ_M4(1,0)] = a.y*b.x;
    m[BJ_M4(2,0)] = a.z*b.x;
    m[BJ_M4(3,0)] = BJ_FZERO;
    m[BJ_M4(0,1)] = a.x*b.y;
    m[BJ_M4(1,1)] = a.y*b.y;
    m[BJ_M4(2,1)] = a.z*b.y;
    m[BJ_M4(3,1)] = BJ_FZERO;
    m[BJ_M4(0,2)] = a.x*b.z;
    m[BJ_M4(1,2)] = a.y*b.z;
    m[BJ_M4(2,2)] = a.z*b.z;
    m[BJ_M4(3,2)] = BJ_FZERO;
    m[BJ_M4(0,3)] = BJ_FZERO;
    m[BJ_M4(1,3)] = BJ_FZERO;
    m[BJ_M4(2,3)] = BJ_FZERO;
    m[BJ_M4(3,3)] = BJ_FZERO;
}

////////////////////////////////////////////////////////////////////////////////
/// Right-multiply a 4×4 by a rotation around an arbitrary axis.
/// \param out Output 4×4 matrix.
/// \param M Input 4×4 matrix.
/// \param axis Unit axis of rotation.
/// \param angle Rotation angle in radians.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4_rotate_axis_andle(
    bj_mat4* restrict       out,
    const bj_mat4* restrict M,
    bj_vec3                 axis,
    bj_real                 angle
) {
    const bj_real s = bj_sin(angle);
    const bj_real c = bj_cos(angle);
    const bj_real len = bj_vec3_len(axis);
    if (len <= BJ_F(1e-4)) { 
        bj_mat4_copy(out, M);
        return; 
    }

    const bj_real inv = BJ_F(1.0) / len;
    const bj_real ux = axis.x * inv, uy = axis.y * inv, uz = axis.z * inv;
    const bj_real t = BJ_F(1.0) - c;

    /* 3x3 rotation block */
    const bj_real r00 = c + t*ux*ux;
    const bj_real r01 = t*ux*uy + s*uz;
    const bj_real r02 = t*ux*uz - s*uy;

    const bj_real r10 = t*uy*ux - s*uz;
    const bj_real r11 = c + t*uy*uy;
    const bj_real r12 = t*uy*uz + s*ux;

    const bj_real r20 = t*uz*ux + s*uy;
    const bj_real r21 = t*uz*uy - s*ux;
    const bj_real r22 = c + t*uz*uz;

    const bj_real* m = M->m;
    bj_real* o = out->m;

    /* Columns of M (column-major) */
    const bj_real* m0 = &m[BJ_M4(0,0)];
    const bj_real* m1 = &m[BJ_M4(1,0)];
    const bj_real* m2 = &m[BJ_M4(2,0)];
    const bj_real* m3 = &m[BJ_M4(3,0)];

    bj_real* o0 = &o[BJ_M4(0,0)];
    bj_real* o1 = &o[BJ_M4(1,0)];
    bj_real* o2 = &o[BJ_M4(2,0)];
    bj_real* o3 = &o[BJ_M4(3,0)];

    for (int r = 0; r < 4; ++r) {
        const bj_real M0 = m0[r], M1 = m1[r], M2 = m2[r];
        o0[r] = M0*r00 + M1*r10 + M2*r20;
        o1[r] = M0*r01 + M1*r11 + M2*r21;
        o2[r] = M0*r02 + M1*r12 + M2*r22;
        o3[r] = m3[r];
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Right-multiply by an X-axis rotation.
/// \param out Output 4×4 matrix.
/// \param M Input 4×4 matrix.
/// \param a Input scalar.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4_rotate_x(
    bj_mat4* restrict       out,
    const bj_mat4* restrict M,
    bj_real                 a
) {

    const bj_real s = bj_sin(a), c = bj_cos(a);
    const bj_real* m = M->m; bj_real* o = out->m;

    const bj_real* m0 = &m[BJ_M4(0,0)];
    const bj_real* m1 = &m[BJ_M4(1,0)];
    const bj_real* m2 = &m[BJ_M4(2,0)];
    const bj_real* m3 = &m[BJ_M4(3,0)];

    bj_real* o0 = &o[BJ_M4(0,0)];
    bj_real* o1 = &o[BJ_M4(1,0)];
    bj_real* o2 = &o[BJ_M4(2,0)];
    bj_real* o3 = &o[BJ_M4(3,0)];

    for (int r = 0; r < 4; ++r) {
        const bj_real M1 = m1[r], M2 = m2[r];
        o0[r] = m0[r];
        o1[r] =  c*M1 - s*M2;
        o2[r] =  s*M1 + c*M2;
        o3[r] = m3[r];
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Right-multiply by a Y-axis rotation.
/// \param out Output 4×4 matrix.
/// \param M Input 4×4 matrix.
/// \param a Input scalar.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4_rotate_y(
    bj_mat4* restrict       out,
    const bj_mat4* restrict M,
    bj_real                 a
) {
    const bj_real s = bj_sin(a), c = bj_cos(a);
    const bj_real* m = M->m; bj_real* o = out->m;

    const bj_real* m0 = &m[BJ_M4(0,0)];
    const bj_real* m1 = &m[BJ_M4(1,0)];
    const bj_real* m2 = &m[BJ_M4(2,0)];
    const bj_real* m3 = &m[BJ_M4(3,0)];

    bj_real* o0 = &o[BJ_M4(0,0)];
    bj_real* o1 = &o[BJ_M4(1,0)];
    bj_real* o2 = &o[BJ_M4(2,0)];
    bj_real* o3 = &o[BJ_M4(3,0)];

    for (int r = 0; r < 4; ++r) {
        const bj_real M0 = m0[r], M2 = m2[r];
        o0[r] =  c*M0 + s*M2;
        o1[r] =  m1[r];
        o2[r] = -s*M0 + c*M2;
        o3[r] =  m3[r];
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Right-multiply by a Z-axis rotation.
/// \param out Output 4×4 matrix.
/// \param M Input 4×4 matrix.
/// \param a Input scalar.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4_rotate_z(
    bj_mat4* restrict       out,
    const bj_mat4* restrict M,
    bj_real                 a
) {
    const bj_real s = bj_sin(a), c = bj_cos(a);
    const bj_real* m = M->m; bj_real* o = out->m;

    const bj_real* m0 = &m[BJ_M4(0,0)];
    const bj_real* m1 = &m[BJ_M4(1,0)];
    const bj_real* m2 = &m[BJ_M4(2,0)];
    const bj_real* m3 = &m[BJ_M4(3,0)];

    bj_real* o0 = &o[BJ_M4(0,0)];
    bj_real* o1 = &o[BJ_M4(1,0)];
    bj_real* o2 = &o[BJ_M4(2,0)];
    bj_real* o3 = &o[BJ_M4(3,0)];

    for (int r = 0; r < 4; ++r) {
        const bj_real M0 = m0[r], M1 = m1[r];
        o0[r] =  c*M0 - s*M1;
        o1[r] =  s*M0 + c*M1;
        o2[r] =  m2[r];
        o3[r] =  m3[r];
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Build a 4×4 rotation from two unit vectors (arcball).
/// \param R Output 4×4 matrix.
/// \param M Input 4×4 matrix.
/// \param a Input 2D vector.
/// \param b Input 2D vector.
/// \param s Scale factors as a vector.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4_rotate_arcball(
    bj_mat4* BJ_RESTRICT       R,
    const bj_mat4* BJ_RESTRICT M,
    bj_vec2                    a,
    bj_vec2                    b,
    bj_real                    s
) {
    bj_real z_a = BJ_FZERO;
    bj_real z_b = BJ_FZERO;
    const bj_real la = bj_vec2_len(a);
    const bj_real lb = bj_vec2_len(b);
    if (la < BJ_F(1.0)) {
        z_a = bj_sqrt(BJ_F(1.0) - bj_vec2_dot(a,a));
    } else {
        a = bj_vec2_normalize(a);
    }
    if (lb < BJ_F(1.0)) {
        z_b = bj_sqrt(BJ_F(1.0) - bj_vec2_dot(b,b));
    } else {
        b = bj_vec2_normalize(b);
    }
    const bj_vec3 A = { a.x, a.y, z_a };
    const bj_vec3 B = { b.x, b.y, z_b };
    const bj_vec3 C = bj_vec3_cross(A,B);
    const bj_real ang = bj_acos(bj_vec3_dot(A,B)) * s;
    bj_mat4_rotate_axis_andle(R, M, C, ang);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Invert a 4×4 matrix (safe).
/// 
/// \details Computes A^{-1} via cofactors. If det(A) is zero by
/// bj_real_is_zero, returns 0 and leaves \p out unspecified.
/// 
/// \param out Output 4×4 matrix.
/// \param M   Input 4×4 matrix.
/// \return \ref BJ_TRUE on success, \ref BJ_FALSE if singular or 
///         near-singular by bj_real_is_zero(det).
/// 
/// \warning Cofactor-based inversion is costly and can be unstable for
/// ill-conditioned matrices. Prefer affine/orthonormal specializations when possible.
/// \see bj_mat4_invert_unsafe
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_bool bj_mat4_invert(
    bj_mat4* BJ_RESTRICT       out,
    const bj_mat4* BJ_RESTRICT M
) {
    const bj_real* m = M->m;
    bj_real s[6], c[6];

    s[0] = m[BJ_M4(0,0)] * m[BJ_M4(1,1)] - m[BJ_M4(1,0)] * m[BJ_M4(0,1)];
    s[1] = m[BJ_M4(0,0)] * m[BJ_M4(1,2)] - m[BJ_M4(1,0)] * m[BJ_M4(0,2)];
    s[2] = m[BJ_M4(0,0)] * m[BJ_M4(1,3)] - m[BJ_M4(1,0)] * m[BJ_M4(0,3)];
    s[3] = m[BJ_M4(0,1)] * m[BJ_M4(1,2)] - m[BJ_M4(1,1)] * m[BJ_M4(0,2)];
    s[4] = m[BJ_M4(0,1)] * m[BJ_M4(1,3)] - m[BJ_M4(1,1)] * m[BJ_M4(0,3)];
    s[5] = m[BJ_M4(0,2)] * m[BJ_M4(1,3)] - m[BJ_M4(1,2)] * m[BJ_M4(0,3)];

    c[0] = m[BJ_M4(2,0)] * m[BJ_M4(3,1)] - m[BJ_M4(3,0)] * m[BJ_M4(2,1)];
    c[1] = m[BJ_M4(2,0)] * m[BJ_M4(3,2)] - m[BJ_M4(3,0)] * m[BJ_M4(2,2)];
    c[2] = m[BJ_M4(2,0)] * m[BJ_M4(3,3)] - m[BJ_M4(3,0)] * m[BJ_M4(2,3)];
    c[3] = m[BJ_M4(2,1)] * m[BJ_M4(3,2)] - m[BJ_M4(3,1)] * m[BJ_M4(2,2)];
    c[4] = m[BJ_M4(2,1)] * m[BJ_M4(3,3)] - m[BJ_M4(3,1)] * m[BJ_M4(2,3)];
    c[5] = m[BJ_M4(2,2)] * m[BJ_M4(3,3)] - m[BJ_M4(3,2)] * m[BJ_M4(2,3)];

    const bj_real det = s[0]*c[5] - s[1]*c[4] + s[2]*c[3]
                      + s[3]*c[2] - s[4]*c[1] + s[5]*c[0];

    if (bj_real_is_zero(det)) {
        return BJ_FALSE;
    }
    const bj_real id = BJ_F(1.0) / det;
    bj_real* o = out->m;

    o[BJ_M4(0,0)] = ( m[BJ_M4(1,1)] * c[5] - m[BJ_M4(1,2)] * c[4] + m[BJ_M4(1,3)] * c[3]) * id;
    o[BJ_M4(0,1)] = (-m[BJ_M4(0,1)] * c[5] + m[BJ_M4(0,2)] * c[4] - m[BJ_M4(0,3)] * c[3]) * id;
    o[BJ_M4(0,2)] = ( m[BJ_M4(3,1)] * s[5] - m[BJ_M4(3,2)] * s[4] + m[BJ_M4(3,3)] * s[3]) * id;
    o[BJ_M4(0,3)] = (-m[BJ_M4(2,1)] * s[5] + m[BJ_M4(2,2)] * s[4] - m[BJ_M4(2,3)] * s[3]) * id;

    o[BJ_M4(1,0)] = (-m[BJ_M4(1,0)] * c[5] + m[BJ_M4(1,2)] * c[2] - m[BJ_M4(1,3)] * c[1]) * id;
    o[BJ_M4(1,1)] = ( m[BJ_M4(0,0)] * c[5] - m[BJ_M4(0,2)] * c[2] + m[BJ_M4(0,3)] * c[1]) * id;
    o[BJ_M4(1,2)] = (-m[BJ_M4(3,0)] * s[5] + m[BJ_M4(3,2)] * s[2] - m[BJ_M4(3,3)] * s[1]) * id;
    o[BJ_M4(1,3)] = ( m[BJ_M4(2,0)] * s[5] - m[BJ_M4(2,2)] * s[2] + m[BJ_M4(2,3)] * s[1]) * id;

    o[BJ_M4(2,0)] = ( m[BJ_M4(1,0)] * c[4] - m[BJ_M4(1,1)] * c[2] + m[BJ_M4(1,3)] * c[0]) * id;
    o[BJ_M4(2,1)] = (-m[BJ_M4(0,0)] * c[4] + m[BJ_M4(0,1)] * c[2] - m[BJ_M4(0,3)] * c[0]) * id;
    o[BJ_M4(2,2)] = ( m[BJ_M4(3,0)] * s[4] - m[BJ_M4(3,1)] * s[2] + m[BJ_M4(3,3)] * s[0]) * id;
    o[BJ_M4(2,3)] = (-m[BJ_M4(2,0)] * s[4] + m[BJ_M4(2,1)] * s[2] - m[BJ_M4(2,3)] * s[0]) * id;

    o[BJ_M4(3,0)] = (-m[BJ_M4(1,0)] * c[3] + m[BJ_M4(1,1)] * c[1] - m[BJ_M4(1,2)] * c[0]) * id;
    o[BJ_M4(3,1)] = ( m[BJ_M4(0,0)] * c[3] - m[BJ_M4(0,1)] * c[1] + m[BJ_M4(0,2)] * c[0]) * id;
    o[BJ_M4(3,2)] = (-m[BJ_M4(3,0)] * s[3] + m[BJ_M4(3,1)] * s[1] - m[BJ_M4(3,2)] * s[0]) * id;
    o[BJ_M4(3,3)] = ( m[BJ_M4(2,0)] * s[3] - m[BJ_M4(2,1)] * s[1] + m[BJ_M4(2,2)] * s[0]) * id;

    return BJ_TRUE;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Invert a 4×4 matrix (unsafe).
/// 
/// \details Same as bj_mat4_invert but skips the singularity check and divides
/// by det(A) unconditionally.
/// 
/// \param out Output 4×4 matrix.
/// \param M   Input 4×4 matrix (must be nonsingular).
/// \pre det(M) != 0 and M is well-conditioned for numerical stability.
/// \warning Division by zero if det(M) == 0. Consider bj_mat4_invert instead.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4_invert_unsafe(
    bj_mat4* BJ_RESTRICT       out,
    const bj_mat4* BJ_RESTRICT M
) {
    const bj_real* m = M->m;
    bj_real s[6], c[6];

    s[0] = m[BJ_M4(0,0)] * m[BJ_M4(1,1)] - m[BJ_M4(1,0)] * m[BJ_M4(0,1)];
    s[1] = m[BJ_M4(0,0)] * m[BJ_M4(1,2)] - m[BJ_M4(1,0)] * m[BJ_M4(0,2)];
    s[2] = m[BJ_M4(0,0)] * m[BJ_M4(1,3)] - m[BJ_M4(1,0)] * m[BJ_M4(0,3)];
    s[3] = m[BJ_M4(0,1)] * m[BJ_M4(1,2)] - m[BJ_M4(1,1)] * m[BJ_M4(0,2)];
    s[4] = m[BJ_M4(0,1)] * m[BJ_M4(1,3)] - m[BJ_M4(1,1)] * m[BJ_M4(0,3)];
    s[5] = m[BJ_M4(0,2)] * m[BJ_M4(1,3)] - m[BJ_M4(1,2)] * m[BJ_M4(0,3)];

    c[0] = m[BJ_M4(2,0)] * m[BJ_M4(3,1)] - m[BJ_M4(3,0)] * m[BJ_M4(2,1)];
    c[1] = m[BJ_M4(2,0)] * m[BJ_M4(3,2)] - m[BJ_M4(3,0)] * m[BJ_M4(2,2)];
    c[2] = m[BJ_M4(2,0)] * m[BJ_M4(3,3)] - m[BJ_M4(3,0)] * m[BJ_M4(2,3)];
    c[3] = m[BJ_M4(2,1)] * m[BJ_M4(3,2)] - m[BJ_M4(3,1)] * m[BJ_M4(2,2)];
    c[4] = m[BJ_M4(2,1)] * m[BJ_M4(3,3)] - m[BJ_M4(3,1)] * m[BJ_M4(2,3)];
    c[5] = m[BJ_M4(2,2)] * m[BJ_M4(3,3)] - m[BJ_M4(3,2)] * m[BJ_M4(2,3)];

    const bj_real id = BJ_F(1.0) /
        (s[0]*c[5] - s[1]*c[4] + s[2]*c[3] + s[3]*c[2] - s[4]*c[1] + s[5]*c[0]);

    bj_real* o = out->m;

    o[BJ_M4(0,0)] = ( m[BJ_M4(1,1)] * c[5] - m[BJ_M4(1,2)] * c[4] + m[BJ_M4(1,3)] * c[3]) * id;
    o[BJ_M4(0,1)] = (-m[BJ_M4(0,1)] * c[5] + m[BJ_M4(0,2)] * c[4] - m[BJ_M4(0,3)] * c[3]) * id;
    o[BJ_M4(0,2)] = ( m[BJ_M4(3,1)] * s[5] - m[BJ_M4(3,2)] * s[4] + m[BJ_M4(3,3)] * s[3]) * id;
    o[BJ_M4(0,3)] = (-m[BJ_M4(2,1)] * s[5] + m[BJ_M4(2,2)] * s[4] - m[BJ_M4(2,3)] * s[3]) * id;

    o[BJ_M4(1,0)] = (-m[BJ_M4(1,0)] * c[5] + m[BJ_M4(1,2)] * c[2] - m[BJ_M4(1,3)] * c[1]) * id;
    o[BJ_M4(1,1)] = ( m[BJ_M4(0,0)] * c[5] - m[BJ_M4(0,2)] * c[2] + m[BJ_M4(0,3)] * c[1]) * id;
    o[BJ_M4(1,2)] = (-m[BJ_M4(3,0)] * s[5] + m[BJ_M4(3,2)] * s[2] - m[BJ_M4(3,3)] * s[1]) * id;
    o[BJ_M4(1,3)] = ( m[BJ_M4(2,0)] * s[5] - m[BJ_M4(2,2)] * s[2] + m[BJ_M4(2,3)] * s[1]) * id;

    o[BJ_M4(2,0)] = ( m[BJ_M4(1,0)] * c[4] - m[BJ_M4(1,1)] * c[2] + m[BJ_M4(1,3)] * c[0]) * id;
    o[BJ_M4(2,1)] = (-m[BJ_M4(0,0)] * c[4] + m[BJ_M4(0,1)] * c[2] - m[BJ_M4(0,3)] * c[0]) * id;
    o[BJ_M4(2,2)] = ( m[BJ_M4(3,0)] * s[4] - m[BJ_M4(3,1)] * s[2] + m[BJ_M4(3,3)] * s[0]) * id;
    o[BJ_M4(2,3)] = (-m[BJ_M4(2,0)] * s[4] + m[BJ_M4(2,1)] * s[2] - m[BJ_M4(2,3)] * s[0]) * id;

    o[BJ_M4(3,0)] = (-m[BJ_M4(1,0)] * c[3] + m[BJ_M4(1,1)] * c[1] - m[BJ_M4(1,2)] * c[0]) * id;
    o[BJ_M4(3,1)] = ( m[BJ_M4(0,0)] * c[3] - m[BJ_M4(0,1)] * c[1] + m[BJ_M4(0,2)] * c[0]) * id;
    o[BJ_M4(3,2)] = (-m[BJ_M4(3,0)] * s[3] + m[BJ_M4(3,1)] * s[1] - m[BJ_M4(3,2)] * s[0]) * id;
    o[BJ_M4(3,3)] = ( m[BJ_M4(2,0)] * s[3] - m[BJ_M4(2,1)] * s[1] + m[BJ_M4(2,2)] * s[0]) * id;
}

////////////////////////////////////////////////////////////////////////////////
/// Orthonormalize the 3×3 linear part of a 4×4 matrix.
/// \param out Output 4×4 matrix.
/// \param A Input 4×4 matrix.
/// \warning If the 3×3 block is degenerate the result may contain NaNs.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4_orthonormalize(
    bj_mat4* BJ_RESTRICT       out,
    const bj_mat4* BJ_RESTRICT A
) {
    bj_mat4_copy(out, A);
    bj_vec3 z = { 
        out->m[BJ_M4(2,0)],
        out->m[BJ_M4(2,1)],
        out->m[BJ_M4(2,2)] 
    };
    z = bj_vec3_normalize(z);

    bj_vec3 y = { 
        out->m[BJ_M4(1,0)],
        out->m[BJ_M4(1,1)],
        out->m[BJ_M4(1,2)] 
    };
    y = bj_vec3_sub(y, bj_vec3_scale(z, bj_vec3_dot(y,z)));
    y = bj_vec3_normalize(y);

    bj_vec3 x = {
        out->m[BJ_M4(0,0)],
        out->m[BJ_M4(0,1)],
        out->m[BJ_M4(0,2)] 
    };
    x = bj_vec3_sub(x, bj_vec3_scale(z, bj_vec3_dot(x,z)));
    x = bj_vec3_sub(x, bj_vec3_scale(y, bj_vec3_dot(x,y)));
    x = bj_vec3_normalize(x);

    out->m[BJ_M4(0,0)] = x.x;
    out->m[BJ_M4(0,1)] = x.y;
    out->m[BJ_M4(0,2)] = x.z;
    out->m[BJ_M4(1,0)] = y.x;
    out->m[BJ_M4(1,1)] = y.y;
    out->m[BJ_M4(1,2)] = y.z;
    out->m[BJ_M4(2,0)] = z.x;
    out->m[BJ_M4(2,1)] = z.y;
    out->m[BJ_M4(2,2)] = z.z;
}

////////////////////////////////////////////////////////////////////////////////
/// Build a perspective frustum into a 4×4 matrix.
/// \param M Output 4×4 matrix.
/// \param l Input scalar.
/// \param r scalar (0-based).
/// \param b Input scalar.
/// \param t Input scalar.
/// \param n Near plane distance.
/// \param f Far plane distance.
/// \warning Requires r>l, t>b, f>n. Depth maps to [0,1]. Y is inverted.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4_set_frustum(
    bj_mat4* BJ_RESTRICT M,
    bj_real              l,
    bj_real              r,
    bj_real              b,
    bj_real              t,
    bj_real              n,
    bj_real              f
) {

    bj_real* m = M->m;
    m[BJ_M4(0,0)] = BJ_F(2.0)*n/(r-l);
    m[BJ_M4(0,1)] = m[BJ_M4(0,2)] = m[BJ_M4(0,3)] = BJ_FZERO;

    m[BJ_M4(1,0)] = BJ_FZERO;
    m[BJ_M4(1,1)] = BJ_F(-2.0)*n/(t-b);
    m[BJ_M4(1,2)] = m[BJ_M4(1,3)] = BJ_FZERO;

    m[BJ_M4(2,0)] = (r+l)/(r-l);
    m[BJ_M4(2,1)] = (t+b)/(t-b);
    m[BJ_M4(2,2)] =  f/(f-n);
    m[BJ_M4(2,3)] =  BJ_F(1.0);

    m[BJ_M4(3,0)] = BJ_FZERO; m[BJ_M4(3,1)] = BJ_FZERO;
    m[BJ_M4(3,2)] = -(f*n)/(f-n);
    m[BJ_M4(3,3)] = BJ_FZERO;
}

////////////////////////////////////////////////////////////////////////////////
/// Build a 3D orthographic projection into a 4×4 matrix.
/// \param M Output 4×4 matrix.
/// \param l Input scalar.
/// \param r scalar (0-based).
/// \param b Input scalar.
/// \param t Input scalar.
/// \param n Near plane distance.
/// \param f Far plane distance.
/// \warning Requires r!=l, t!=b, f!=n. Depth maps to [0,1]. Y is inverted.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4_set_ortho(
    bj_mat4* BJ_RESTRICT M,
    bj_real              l,
    bj_real              r,
    bj_real              b,
    bj_real              t,
    bj_real              n,
    bj_real              f
) {

    bj_real* m = M->m;
    m[BJ_M4(0,0)] = BJ_F(2.0)/(r-l);
    m[BJ_M4(0,1)] = m[BJ_M4(0,2)] = m[BJ_M4(0,3)] = BJ_FZERO;

    m[BJ_M4(1,0)] = m[BJ_M4(1,2)] = m[BJ_M4(1,3)] = BJ_FZERO;
    m[BJ_M4(1,1)] = BJ_F(-2.0)/(t-b);

    m[BJ_M4(2,0)] = m[BJ_M4(2,1)] = m[BJ_M4(2,3)] = BJ_FZERO;
    m[BJ_M4(2,2)] = BJ_F(1.0)/(f-n);

    m[BJ_M4(3,0)] = -(r+l)/(r-l);
    m[BJ_M4(3,1)] =  (t+b)/(t-b);
    m[BJ_M4(3,2)] = -n/(f-n);
    m[BJ_M4(3,3)] = BJ_F(1.0);
}


////////////////////////////////////////////////////////////////////////////////
/// Build a 4×4 perspective projection from vertical FOV.
/// \param M Output 4×4 matrix.
/// \param y_fov Vertical field of view in radians.
/// \param aspect Aspect ratio width/height.
/// \param n Near plane distance.
/// \param f Far plane distance.
/// \warning Requires f > n and aspect > 0. Depth maps to [0,1]. Y is inverted.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4_set_perspective(
    bj_mat4* BJ_RESTRICT M,
    bj_real              y_fov,
    bj_real              aspect,
    bj_real              n,
    bj_real              f
) {

    const bj_real a = BJ_F(1.0)/bj_tan(y_fov/BJ_F(2.0));
    bj_real* m = M->m;
    m[BJ_M4(0,0)] = a/aspect;
    m[BJ_M4(0,1)] = m[BJ_M4(0,2)] = m[BJ_M4(0,3)] = BJ_FZERO;

    m[BJ_M4(1,0)] = BJ_FZERO;
    m[BJ_M4(1,1)] = -a;
    m[BJ_M4(1,2)] = m[BJ_M4(1,3)] = BJ_FZERO;

    m[BJ_M4(2,0)] = BJ_FZERO; m[BJ_M4(2,1)] = BJ_FZERO;
    m[BJ_M4(2,2)] = f/(f-n);
    m[BJ_M4(2,3)] = BJ_F(1.0);

    m[BJ_M4(3,0)] = BJ_FZERO; m[BJ_M4(3,1)] = BJ_FZERO;
    m[BJ_M4(3,2)] = -(f*n)/(f-n);
    m[BJ_M4(3,3)] = BJ_FZERO;
}

////////////////////////////////////////////////////////////////////////////////
/// Build a 3D viewport transform into a 4×4 matrix.
/// \param M Output 4×4 matrix.
/// \param x Viewport X origin in pixels.
/// \param y Viewport Y origin in pixels.
/// \param w Viewport width in pixels.
/// \param h Viewport height in pixels.
/// \note Maps NDC x,y ∈ [-1,1] and z ∈ [0,1] to window coordinates.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4_set_viewport(
    bj_mat4* BJ_RESTRICT M,
    bj_real              x,
    bj_real              y,
    bj_real              w,
    bj_real              h
) {
    const bj_real zmin = BJ_FZERO;
    const bj_real zmax = BJ_F(1.0);
    bj_real* m = M->m;
    m[BJ_M4(0,0)] = BJ_F(0.5) * w;
    m[BJ_M4(0,1)] = BJ_FZERO;
    m[BJ_M4(0,2)] = BJ_FZERO;
    m[BJ_M4(0,3)] = BJ_FZERO;
    m[BJ_M4(1,0)] = BJ_FZERO;
    m[BJ_M4(1,1)] = BJ_F(0.5) * h;
    m[BJ_M4(1,2)] = BJ_FZERO;
    m[BJ_M4(1,3)] = BJ_FZERO;
    m[BJ_M4(2,0)] = BJ_FZERO;
    m[BJ_M4(2,1)] = BJ_FZERO;
    m[BJ_M4(2,2)] = (zmax - zmin);
    m[BJ_M4(2,3)] = BJ_FZERO;
    m[BJ_M4(3,0)] = x + BJ_F(0.5) * w;
    m[BJ_M4(3,1)] = y + BJ_F(0.5) * h;
    m[BJ_M4(3,2)] = zmin;
    m[BJ_M4(3,3)] = BJ_F(1.0);
}

////////////////////////////////////////////////////////////////////////////////
/// Build a right-handed look-at view matrix.
/// \param M Output 4×4 matrix.
/// \param eye Camera position.
/// \param center Target point the camera looks at.
/// \param up Approximate up direction.
/// \note Right-handed view: +Z looks from eye to center.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4_set_lookat(
    bj_mat4* BJ_RESTRICT M,
    bj_vec3              eye,
    bj_vec3              center,
    bj_vec3              up
) {
    bj_vec3 f = bj_vec3_normalize(bj_vec3_sub(center, eye));
    bj_vec3 s = bj_vec3_normalize(bj_vec3_cross(up, f));
    bj_vec3 t = bj_vec3_cross(f, s);

    bj_mat4_set_identity(M);
    M->m[BJ_M4(0,0)] = s.x;
    M->m[BJ_M4(0,1)] = t.x;
    M->m[BJ_M4(0,2)] = f.x;
    M->m[BJ_M4(1,0)] = s.y;
    M->m[BJ_M4(1,1)] = t.y;
    M->m[BJ_M4(1,2)] = f.y;
    M->m[BJ_M4(2,0)] = s.z;
    M->m[BJ_M4(2,1)] = t.z;
    M->m[BJ_M4(2,2)] = f.z;
    bj_mat4_translate(M, -eye.x, -eye.y, -eye.z);
}

////////////////////////////////////////////////////////////////////////////////
/// Set a 4×3 affine matrix to identity.
/// \param M Output 4×3 matrix.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4x3_set_identity(
    bj_mat4x3* BJ_RESTRICT M
) {
    bj_real* m = M->m;
    m[BJ_M43(0,0)] = BJ_F(1.0);
    m[BJ_M43(0,1)] = BJ_FZERO;
    m[BJ_M43(0,2)] = BJ_FZERO;
    m[BJ_M43(1,0)] = BJ_FZERO;
    m[BJ_M43(1,1)] = BJ_F(1.0);
    m[BJ_M43(1,2)] = BJ_FZERO;
    m[BJ_M43(2,0)] = BJ_FZERO;
    m[BJ_M43(2,1)] = BJ_FZERO;
    m[BJ_M43(2,2)] = BJ_F(1.0);
    m[BJ_M43(3,0)] = BJ_FZERO;
    m[BJ_M43(3,1)] = BJ_FZERO;
    m[BJ_M43(3,2)] = BJ_FZERO;
}

////////////////////////////////////////////////////////////////////////////////
/// Matrix operation.
/// \param M Output 4×3 matrix.
/// \param tx Translation along x.
/// \param ty Translation along y.
/// \param tz Translation along z.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4x3_set_translation(
    bj_mat4x3* BJ_RESTRICT M,
    bj_real                tx,
    bj_real                ty,
    bj_real                tz
) {
    bj_mat4x3_set_identity(M);
    M->m[BJ_M43(3,0)] = tx;
    M->m[BJ_M43(3,1)] = ty;
    M->m[BJ_M43(3,2)] = tz;
}

////////////////////////////////////////////////////////////////////////////////
/// Scalar multiply: M = sx * k.
/// \param M Output 4×3 matrix.
/// \param sx Scale on x.
/// \param sy Scale on y.
/// \param sz Scale on z.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4x3_set_scaling_xyz(
    bj_mat4x3* BJ_RESTRICT M,
    bj_real                sx,
    bj_real                sy,
    bj_real                sz
) {
    bj_mat4x3_set_identity(M);
    M->m[BJ_M43(0,0)] = sx;
    M->m[BJ_M43(1,1)] = sy;
    M->m[BJ_M43(2,2)] = sz;
}

////////////////////////////////////////////////////////////////////////////////
/// Right-multiply by an X-axis rotation.
/// \param M Output 4×3 matrix.
/// \param a Input scalar.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4x3_set_rotation_x(
    bj_mat4x3* BJ_RESTRICT M,
    bj_real                a
) {
    const bj_real c = bj_cos(a), s = bj_sin(a);
    bj_mat4x3_set_identity(M);
    M->m[BJ_M43(1,1)] = c; M->m[BJ_M43(1,2)] = s;
    M->m[BJ_M43(2,1)]  = -s; M->m[BJ_M43(2,2)] = c;
}

////////////////////////////////////////////////////////////////////////////////
/// Right-multiply by a Y-axis rotation.
/// \param M Output 4×3 matrix.
/// \param a Input scalar.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4x3_set_rotation_y(
    bj_mat4x3* BJ_RESTRICT M,
    bj_real                a
) {
    const bj_real c = bj_cos(a), s = bj_sin(a);
    bj_mat4x3_set_identity(M);
    M->m[BJ_M43(0,0)] = c; M->m[BJ_M43(0,2)] = -s;
    M->m[BJ_M43(2,0)] = s; M->m[BJ_M43(2,2)] =  c;
}

////////////////////////////////////////////////////////////////////////////////
/// Right-multiply by a Z-axis rotation.
/// \param M Output 4×3 matrix.
/// \param a Input scalar.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4x3_set_rotation_z(
    bj_mat4x3* BJ_RESTRICT M,
    bj_real                a
) {
    const bj_real c = bj_cos(a), s = bj_sin(a);
    bj_mat4x3_set_identity(M);
    M->m[BJ_M43(0,0)] = c; M->m[BJ_M43(0,1)] = s;
    M->m[BJ_M43(1,0)]  = -s; M->m[BJ_M43(1,1)] = c;
}

////////////////////////////////////////////////////////////////////////////////
/// Matrix product: out = A * B.
/// \param out Output 4×3 matrix.
/// \param A Input 4×3 matrix.
/// \param B Input 4×3 matrix.
/// \note Column-major storage. Vectors are columns. Right-multiply: r = M * v.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4x3_mul(
    bj_mat4x3* BJ_RESTRICT       out,
    const bj_mat4x3* BJ_RESTRICT A,
    const bj_mat4x3* BJ_RESTRICT B
){

    const bj_real a00 = A->m[BJ_M43(0,0)];
    const bj_real a10 = A->m[BJ_M43(0,1)];
    const bj_real a20 = A->m[BJ_M43(0,2)];
    const bj_real a30 = A->m[BJ_M43(3,0)];
    const bj_real a01 = A->m[BJ_M43(1,0)];
    const bj_real a11 = A->m[BJ_M43(1,1)];
    const bj_real a21 = A->m[BJ_M43(1,2)];
    const bj_real a31 = A->m[BJ_M43(3,1)];
    const bj_real a02 = A->m[BJ_M43(2,0)];
    const bj_real a12 = A->m[BJ_M43(2,1)];
    const bj_real a22 = A->m[BJ_M43(2,2)];
    const bj_real a32 = A->m[BJ_M43(3,2)];

    const bj_real b00 = B->m[BJ_M43(0,0)];
    const bj_real b01 = B->m[BJ_M43(1,0)];
    const bj_real bt0 = B->m[BJ_M43(3,0)];
    const bj_real b10 = B->m[BJ_M43(0,1)];
    const bj_real b11 = B->m[BJ_M43(1,1)];
    const bj_real bt1 = B->m[BJ_M43(3,1)];
    const bj_real b20 = B->m[BJ_M43(0,2)];
    const bj_real b21 = B->m[BJ_M43(1,2)];
    const bj_real b22 = B->m[BJ_M43(2,2)];
    const bj_real bt2 = B->m[BJ_M43(3,2)];

    out->m[BJ_M43(0,0)] = a00 * b00 + a01 * b10 + a02 * b20;
    out->m[BJ_M43(0,1)] = a10 * b00 + a11 * b10 + a12 * b20;
    out->m[BJ_M43(0,2)] = a20 * b00 + a21 * b10 + a22 * b20;

    out->m[BJ_M43(1,0)] = a00 * b01 + a01 * b11 + a02 * b21;
    out->m[BJ_M43(1,1)] = a10 * b01 + a11 * b11 + a12 * b21;
    out->m[BJ_M43(1,2)] = a20 * b01 + a21 * b11 + a22 * b21;

    out->m[BJ_M43(2,0)] = a00 * b20 + a01 * b21 + a02 * b22;
    out->m[BJ_M43(2,1)] = a10 * b20 + a11 * b21 + a12 * b22;
    out->m[BJ_M43(2,2)] = a20 * b20 + a21 * b21 + a22 * b22;

    out->m[BJ_M43(3,0)] = a00 * bt0 + a01 * bt1 + a02 * bt2 + a30;
    out->m[BJ_M43(3,1)] = a10 * bt0 + a11 * bt1 + a12 * bt2 + a31;
    out->m[BJ_M43(3,2)] = a20 * bt0 + a21 * bt1 + a22 * bt2 + a32;
}

////////////////////////////////////////////////////////////////////////////////
/// Transform a 3D point by a 4×3 affine matrix.
/// \param M Input 4×3 matrix.
/// \param p Input point.
/// \return 3D vector result.
/// \note Affine 3D. Ignores projective terms; no divide.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec3 bj_mat4x3_transform_point(
    const bj_mat4x3* BJ_RESTRICT M,
    bj_vec3                      p
){
    const bj_real* m = M->m; const bj_real x = p.x,y = p.y,z = p.z;
    return (bj_vec3){
        m[BJ_M43(0,0)]*x + m[BJ_M43(1,0)]*y + m[BJ_M43(2,0)]*z + m[BJ_M43(3,0)],
        m[BJ_M43(0,1)]*x + m[BJ_M43(1,1)]*y + m[BJ_M43(2,1)]*z + m[BJ_M43(3,1)],
        m[BJ_M43(0,2)]*x + m[BJ_M43(1,2)]*y + m[BJ_M43(2,2)]*z + m[BJ_M43(3,2)]
    };
}

////////////////////////////////////////////////////////////////////////////////
/// Transform a direction vector (ignoring translation).
/// \param M Input 4×3 matrix.
/// \param v Input vector.
/// \return 3D vector result.
/// \note Affine 3D. Ignores projective terms; no divide.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec3 bj_mat4x3_transform_dir(
    const bj_mat4x3* BJ_RESTRICT M,
    bj_vec3                      v
){
    const bj_real* m = M->m; const bj_real x = v.x,y = v.y,z = v.z;
    return (bj_vec3){
        m[BJ_M43(0,0)]*x + m[BJ_M43(1,0)]*y + m[BJ_M43(2,0)]*z,
        m[BJ_M43(0,1)]*x + m[BJ_M43(1,1)]*y + m[BJ_M43(2,1)]*z,
        m[BJ_M43(0,2)]*x + m[BJ_M43(1,2)]*y + m[BJ_M43(2,2)]*z
    };
}

////////////////////////////////////////////////////////////////////////////////
/// Promote a 4×3 affine matrix to 4×4.
/// \param M Output 4×4 matrix.
/// \param A Input 4×3 matrix.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4_from_mat4x3(
    bj_mat4* BJ_RESTRICT         M,
    const bj_mat4x3* BJ_RESTRICT A
){
    const bj_real* a = A->m; bj_real* o = M->m;
    o[BJ_M4(0,0)] = a[BJ_M43(0,0)];
    o[BJ_M4(0,1)] = a[BJ_M43(0,1)];
    o[BJ_M4(0,2)] = a[BJ_M43(0,2)];
    o[BJ_M4(0,3)] = BJ_FZERO;
    o[BJ_M4(1,0)] = a[BJ_M43(1,0)];
    o[BJ_M4(1,1)] = a[BJ_M43(1,1)];
    o[BJ_M4(1,2)] = a[BJ_M43(1,2)];
    o[BJ_M4(1,3)] = BJ_FZERO;
    o[BJ_M4(2,0)] = a[BJ_M43(2,0)];
    o[BJ_M4(2,1)] = a[BJ_M43(2,1)];
    o[BJ_M4(2,2)] = a[BJ_M43(2,2)];
    o[BJ_M4(2,3)] = BJ_FZERO;
    o[BJ_M4(3,0)] = a[BJ_M43(3,0)];
    o[BJ_M4(3,1)] = a[BJ_M43(3,1)];
    o[BJ_M4(3,2)] = a[BJ_M43(3,2)];
    o[BJ_M4(3,3)] = BJ_F(1.0);
}

////////////////////////////////////////////////////////////////////////////////
/// Demote a 4×4 matrix to 4×3 (drop projective terms).
/// \param M Output 4×3 matrix.
/// \param A Input 4×4 matrix.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4x3_from_mat4(
    bj_mat4x3* BJ_RESTRICT     M,
    const bj_mat4* BJ_RESTRICT A
){
    const bj_real* a = A->m; bj_real* o = M->m;
    o[BJ_M43(0,0)] = a[BJ_M4(0,0)];
    o[BJ_M43(0,1)] = a[BJ_M4(0,1)];
    o[BJ_M43(0,2)] = a[BJ_M4(0,2)];
    o[BJ_M43(1,0)] = a[BJ_M4(1,0)];
    o[BJ_M43(1,1)] = a[BJ_M4(1,1)];
    o[BJ_M43(1,2)] = a[BJ_M4(1,2)];
    o[BJ_M43(2,0)] = a[BJ_M4(2,0)];
    o[BJ_M43(2,1)] = a[BJ_M4(2,1)];
    o[BJ_M43(2,2)] = a[BJ_M4(2,2)];
    o[BJ_M43(3,0)] = a[BJ_M4(3,0)];
    o[BJ_M43(3,1)] = a[BJ_M4(3,1)];
    o[BJ_M43(3,2)] = a[BJ_M4(3,2)];
}

#endif
/// \}
