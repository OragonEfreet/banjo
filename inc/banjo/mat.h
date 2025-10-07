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
/// bj_mat3x2: 3×2 column-major matrix backed by bj_vec2. (2D affine: 2×2 linear block plus translation column).
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
#define BJ_M4(c,r)    ((c)*4 + (r))   /* 0<=c,r<4 */

////////////////////////////////////////////////////////////////////////////////
/// bj_mat4: Alias for bj_mat4x4
////////////////////////////////////////////////////////////////////////////////
typedef bj_mat4x4 bj_mat4;

////////////////////////////////////////////////////////////////////////////////
/// bj_mat4x3: 4×3 column-major matrix backed by bj_vec3. (3D affine: 3×3 linear block plus translation row).
/// Columns are arrays of 3 components.
////////////////////////////////////////////////////////////////////////////////
struct bj_mat4x3_t { bj_real m[12]; };
typedef struct bj_mat4x3_t bj_mat4x3;
#define BJ_M43(c,r)   ((c)*3 + (r))   /* 0<=c<4, 0<=r<3 */

////////////////////////////////////////////////////////////////////////////////
/// Set to identity matrix.
/// \param m Output buffer.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat3_identity(bj_mat3* BJ_RESTRICT M) {
    bj_real* m = M->m;
    m[BJ_M3(0, 0)] = BJ_F(1.0); m[BJ_M3(0, 1)] = BJ_FZERO;  m[BJ_M3(0, 2)] = BJ_FZERO;
    m[BJ_M3(1, 0)] = BJ_FZERO;  m[BJ_M3(1, 1)] = BJ_F(1.0); m[BJ_M3(1, 2)] = BJ_FZERO;
    m[BJ_M3(2, 0)] = BJ_FZERO;  m[BJ_M3(2, 1)] = BJ_FZERO;  m[BJ_M3(2, 2)] = BJ_F(1.0);
}

////////////////////////////////////////////////////////////////////////////////
/// Copy matrix contents.
/// \param to Output matrix.
/// \param from Input matrix.
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
/// Extract a row as a vector (row index into second subscript).
/// \param res Output vector.
/// \param m Input matrix.
/// \param r Row index.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec3 bj_mat3_row(const bj_mat3* BJ_RESTRICT M, int r) {
    const bj_real* m = M->m;
    return (bj_vec3) { 
        m[BJ_M3(0, r)], m[BJ_M3(1, r)], m[BJ_M3(2, r)] 
    };
}

////////////////////////////////////////////////////////////////////////////////
/// Extract a column as a vector (column index into first subscript).
/// \param res Output vector.
/// \param m Input matrix.
/// \param c Column index.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec3 bj_mat3_col(const bj_mat3* BJ_RESTRICT M, int c) {
    const bj_real* m = M->m;
    return (bj_vec3) { m[BJ_M3(c, 0)], m[BJ_M3(c, 1)], m[BJ_M3(c, 2)] };
}

////////////////////////////////////////////////////////////////////////////////
/// Transpose the matrix.
/// \param res Output matrix.
/// \param m Input matrix.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat3_transpose(
    bj_mat3* BJ_RESTRICT       out,
    const bj_mat3* BJ_RESTRICT A
){
    const bj_real* a = A->m; bj_real* o = out->m;
    o[BJ_M3(0,0)] = a[BJ_M3(0,0)];
    o[BJ_M3(0,1)] = a[BJ_M3(1,0)];
    o[BJ_M3(0,2)] = a[BJ_M3(2,0)];
    o[BJ_M3(1,0)] = a[BJ_M3(0,1)];
    o[BJ_M3(1,1)] = a[BJ_M3(1,1)];
    o[BJ_M3(1,2)] = a[BJ_M3(2,1)];
    o[BJ_M3(2,0)] = a[BJ_M3(0,2)];
    o[BJ_M3(2,1)] = a[BJ_M3(1,2)];
    o[BJ_M3(2,2)] = a[BJ_M3(2,2)];
}

////////////////////////////////////////////////////////////////////////////////
/// Component-wise addition: res = a + b.
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
/// Component-wise subtraction: res = a - b.
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
/// Uniformly scale all elements by scalar k.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat3_scale(
    bj_mat3* BJ_RESTRICT       out,
    const bj_mat3* BJ_RESTRICT A,
    bj_real                    k
) {
    for (int i = 0 ; i < 9 ; ++i) {
        out->m[i] = A->m[i] * k;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Matrix multiplication: res = lhs * rhs (column-major).
/// \note Column-major convention: \c res = lhs * rhs.
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
/// Multiply 3×3 matrix by a 3D column vector: res = M * v.
/// \note Column-major convention: \c res = lhs * rhs.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec3 bj_mat3_mul_vec3(
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
/// Transform a point (homogeneous w=1).
/// \note Column-major convention: \c res = m * p.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec2 bj_mat3_mul_point(
    const bj_mat3* BJ_RESTRICT M,
    bj_vec2                    p
) {
    const bj_real* m = M->m;
    const bj_real x  = p.x;
    const bj_real y  = p.y;
    const bj_real w  = m[BJ_M3(0,2)] * x + m[BJ_M3(1,2)] * y + m[BJ_M3(2,2)];
    bj_real rx = m[BJ_M3(0,0)] * x + m[BJ_M3(1,0)] * y + m[BJ_M3(2,0)];
    bj_real ry = m[BJ_M3(0,1)] * x + m[BJ_M3(1,1)] * y + m[BJ_M3(2,1)];
    if (!bj_real_is_zero(w)) { 
        rx /= w;
        ry /= w; 
    }
    return (bj_vec2) { rx, ry };
}

////////////////////////////////////////////////////////////////////////////////
/// Create a translation matrix.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat3_translation(
    bj_mat3* BJ_RESTRICT M,
    bj_real              tx,
    bj_real              ty
) {
    bj_mat3_identity(M);
    M->m[BJ_M3(2,0)] = tx;
    M->m[BJ_M3(2,1)] = ty;
}

////////////////////////////////////////////////////////////////////////////////
/// Apply a translation in-place.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat3_translation_inplace(
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
/// Create 2D scaling matrix with independent X/Y scales.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat3_scale_xy(
    bj_mat3* BJ_RESTRICT M,
    bj_real              sx,
    bj_real              sy
) {
    bj_mat3_identity(M);
    M->m[BJ_M3(0,0)] = sx;
    M->m[BJ_M3(1,1)] = sy;
}

////////////////////////////////////////////////////////////////////////////////
/// Create 2D shear matrix.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat3_shear(
    bj_mat3* BJ_RESTRICT M,
    bj_real              shx,
    bj_real              shy
) {
    bj_mat3_identity(M);
    M->m[BJ_M3(1,0)] = shy;
    M->m[BJ_M3(0,1)] = shx;
}

////////////////////////////////////////////////////////////////////////////////
/// Rotate 2D about Z.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat3_rotate(
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
/// Invert a 3×3.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE int bj_mat3_inverse(
    bj_mat3* BJ_RESTRICT out,
    const bj_mat3* BJ_RESTRICT A
) {
    const bj_real* m = A->m;
    const bj_real c00 =  (m[BJ_M3(1,1)] * m[BJ_M3(2,2)]-m[BJ_M3(2,1)] * m[BJ_M3(1,2)]);
    const bj_real c01 = -(m[BJ_M3(0,1)] * m[BJ_M3(2,2)]-m[BJ_M3(2,1)] * m[BJ_M3(0,2)]);
    const bj_real c02 =  (m[BJ_M3(0,1)] * m[BJ_M3(1,2)]-m[BJ_M3(1,1)] * m[BJ_M3(0,2)]);
    const bj_real c10 = -(m[BJ_M3(1,0)] * m[BJ_M3(2,2)]-m[BJ_M3(2,0)] * m[BJ_M3(1,2)]);
    const bj_real c11 =  (m[BJ_M3(0,0)] * m[BJ_M3(2,2)]-m[BJ_M3(2,0)] * m[BJ_M3(0,2)]);
    const bj_real c12 = -(m[BJ_M3(0,0)] * m[BJ_M3(1,2)]-m[BJ_M3(1,0)] * m[BJ_M3(0,2)]);
    const bj_real c20 =  (m[BJ_M3(1,0)] * m[BJ_M3(2,1)]-m[BJ_M3(2,0)] * m[BJ_M3(1,1)]);
    const bj_real c21 = -(m[BJ_M3(0,0)] * m[BJ_M3(2,1)]-m[BJ_M3(2,0)] * m[BJ_M3(0,1)]);
    const bj_real c22 =  (m[BJ_M3(0,0)] * m[BJ_M3(1,1)]-m[BJ_M3(1,0)] * m[BJ_M3(0,1)]);
    const bj_real det = m[BJ_M3(0,0)] * c00 + m[BJ_M3(1,0)] * c01 + m[BJ_M3(2,0)]*c02;
    if (det ==  BJ_FZERO) {
        return 0;
    }
    const bj_real id = BJ_F(1.0)/det;
    out->m[BJ_M3(0,0)] = c00 * id;
    out->m[BJ_M3(1,0)] = c10 * id;
    out->m[BJ_M3(2,0)] = c20 * id;
    out->m[BJ_M3(0,1)] = c01 * id;
    out->m[BJ_M3(1,1)] = c11 * id;
    out->m[BJ_M3(2,1)] = c21 * id;
    out->m[BJ_M3(0,2)] = c02 * id;
    out->m[BJ_M3(1,2)] = c12 * id;
    out->m[BJ_M3(2,2)] = c22 * id;
    return 1;
}

////////////////////////////////////////////////////////////////////////////////
/// Orthographic projection matrix (3×3).
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat3_ortho(
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
/// Viewport transform matrix (3×3).
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat3_viewport(
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
/// Set 3×2 to identity.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat3x2_identity(
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
/// Build 3×2 translation.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat3x2_translate(
    bj_mat3x2* BJ_RESTRICT M,
    bj_real                tx,
    bj_real                ty
) {
    bj_mat3x2_identity(M);
    M->m[BJ_M32(2,0)] = tx;
    M->m[BJ_M32(2,1)] = ty;
}

////////////////////////////////////////////////////////////////////////////////
/// Build 3×2 scale.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat3x2_scale(
    bj_mat3x2* BJ_RESTRICT M,
    bj_real                sx,
    bj_real                sy
) {
    bj_mat3x2_identity(M);
    M->m[BJ_M32(0,0)] = sx;
    M->m[BJ_M32(1,1)] = sy;
}

////////////////////////////////////////////////////////////////////////////////
/// Build 3×2 rotation.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat3x2_rotate(
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
/// 3×2 multiplication: res = A * B.
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
/// 3×2 transform point.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec2 bj_mat3x2_mul_point(
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
/// 3×2 transform direction (no translation).
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec2 bj_mat3x2_mul_dir(
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
/// Promote 3×2 to 3×3 homogeneous.
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
/// Extract 3×2 from 3×3.
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
/// Set 4×4 to identity.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4_identity(
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
/// Copy 4×4.
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
/// Extract 4×4 row.
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
/// Extract 4×4 column.
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
/// Transpose 4×4.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4_transpose(
    bj_mat4* BJ_RESTRICT       out,
    const bj_mat4* BJ_RESTRICT A
){
    const bj_real* a = A->m; bj_real* o = out->m;
    o[BJ_M4(0,0)] = a[BJ_M4(0,0)];
    o[BJ_M4(0,1)] = a[BJ_M4(1,0)];
    o[BJ_M4(0,2)] = a[BJ_M4(2,0)];
    o[BJ_M4(0,3)] = a[BJ_M4(3,0)];
    o[BJ_M4(1,0)] = a[BJ_M4(0,1)];
    o[BJ_M4(1,1)] = a[BJ_M4(1,1)];
    o[BJ_M4(1,2)] = a[BJ_M4(2,1)];
    o[BJ_M4(1,3)] = a[BJ_M4(3,1)];
    o[BJ_M4(2,0)] = a[BJ_M4(0,2)];
    o[BJ_M4(2,1)] = a[BJ_M4(1,2)];
    o[BJ_M4(2,2)] = a[BJ_M4(2,2)];
    o[BJ_M4(2,3)] = a[BJ_M4(3,2)];
    o[BJ_M4(3,0)] = a[BJ_M4(0,3)];
    o[BJ_M4(3,1)] = a[BJ_M4(1,3)];
    o[BJ_M4(3,2)] = a[BJ_M4(2,3)];
    o[BJ_M4(3,3)] = a[BJ_M4(3,3)];
}

////////////////////////////////////////////////////////////////////////////////
/// Add 4×4.
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
/// Sub 4×4.
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

static BJ_INLINE void bj_mat4_scale(
    bj_mat4* BJ_RESTRICT       out,
    const bj_mat4* BJ_RESTRICT A,
    bj_real                    k
) {

    for(int i = 0;i<16;++i) {
        out->m[i] = A->m[i] * k;
    }
}

static BJ_INLINE void bj_mat4_scale_xyz(
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
/// 4×4 multiplication: res = lhs * rhs.
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
/// 4×4 multiply by vec4.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec4 bj_mat4_mul_vec4(
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
/// Create 4×4 translation matrix.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4_translation(
    bj_mat4* BJ_RESTRICT M,
    bj_real              x,
    bj_real              y,
    bj_real              z
) {
    bj_mat4_identity(M);
    M->m[BJ_M4(3,0)] = x;
    M->m[BJ_M4(3,1)] = y;
    M->m[BJ_M4(3,2)] = z;
}

////////////////////////////////////////////////////////////////////////////////
/// Apply 4×4 translation in-place.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4_translation_inplace(
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
/// Outer product to upper-left 3×3 block of 4×4.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4_mul_outer(
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
/// Rotate about axis, post-multiply.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4_rotate_axis(
    bj_mat4* BJ_RESTRICT       out,
    const bj_mat4* BJ_RESTRICT M,
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
/// Rotate about X/Y/Z, post-multiply.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4_rotate_x(
    bj_mat4* BJ_RESTRICT       out,
    const bj_mat4* BJ_RESTRICT M,
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

static BJ_INLINE void bj_mat4_rotate_y(
    bj_mat4* BJ_RESTRICT       out,
    const bj_mat4* BJ_RESTRICT M,
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

static BJ_INLINE void bj_mat4_rotate_z(
    bj_mat4* BJ_RESTRICT       out,
    const bj_mat4* BJ_RESTRICT M,
    bj_real                    a
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
/// Arcball rotation.
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
    bj_mat4_rotate_axis(R, M, C, ang);
}

////////////////////////////////////////////////////////////////////////////////
/// Invert 4×4.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE int bj_mat4_inverse(
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
    if (det ==  BJ_FZERO) {
        return 0;
    }
    const bj_real id = BJ_F(1.0)/det;
    bj_real* o = out->m;

    o[BJ_M4(0,0)] = (m[BJ_M4(1,1)] * c[5] - m[BJ_M4(1,2)] * c[4] + m[BJ_M4(1,3)] * c[3]) * id;
    o[BJ_M4(0,1)] = (-m[BJ_M4(0,1)] * c[5] + m[BJ_M4(0,2)] * c[4] - m[BJ_M4(0,3)] * c[3]) * id;
    o[BJ_M4(0,2)] = (m[BJ_M4(3,1)] * s[5] - m[BJ_M4(3,2)] * s[4] + m[BJ_M4(3,3)] * s[3]) * id;
    o[BJ_M4(0,3)] = (-m[BJ_M4(2,1)] * s[5] + m[BJ_M4(2,2)] * s[4] - m[BJ_M4(2,3)] * s[3]) * id;

    o[BJ_M4(1,0)] = (-m[BJ_M4(1,0)] * c[5] + m[BJ_M4(1,2)] * c[2] - m[BJ_M4(1,3)] * c[1]) * id;
    o[BJ_M4(1,1)] = (m[BJ_M4(0,0)] * c[5] - m[BJ_M4(0,2)] * c[2] + m[BJ_M4(0,3)] * c[1]) * id;
    o[BJ_M4(1,2)] = (-m[BJ_M4(3,0)] * s[5] + m[BJ_M4(3,2)] * s[2] - m[BJ_M4(3,3)] * s[1]) * id;
    o[BJ_M4(1,3)] = (m[BJ_M4(2,0)] * s[5] - m[BJ_M4(2,2)] * s[2] + m[BJ_M4(2,3)] * s[1]) * id;

    o[BJ_M4(2,0)] = (m[BJ_M4(1,0)] * c[4] - m[BJ_M4(1,1)] * c[2] + m[BJ_M4(1,3)] * c[0]) * id;
    o[BJ_M4(2,1)] = (-m[BJ_M4(0,0)] * c[4] + m[BJ_M4(0,1)] * c[2] - m[BJ_M4(0,3)] * c[0]) * id;
    o[BJ_M4(2,2)] = (m[BJ_M4(3,0)] * s[4] - m[BJ_M4(3,1)] * s[2] + m[BJ_M4(3,3)] * s[0]) * id;
    o[BJ_M4(2,3)] = (-m[BJ_M4(2,0)] * s[4] + m[BJ_M4(2,1)] * s[2] - m[BJ_M4(2,3)] * s[0]) * id;

    o[BJ_M4(3,0)] = (-m[BJ_M4(1,0)] * c[3] + m[BJ_M4(1,1)] * c[1] - m[BJ_M4(1,2)] * c[0]) * id;
    o[BJ_M4(3,1)] = (m[BJ_M4(0,0)] * c[3] - m[BJ_M4(0,1)] * c[1] + m[BJ_M4(0,2)] * c[0]) * id;
    o[BJ_M4(3,2)] = (-m[BJ_M4(3,0)] * s[3] + m[BJ_M4(3,1)] * s[1] - m[BJ_M4(3,2)] * s[0]) * id;
    o[BJ_M4(3,3)] = (m[BJ_M4(2,0)] * s[3] - m[BJ_M4(2,1)] * s[1] + m[BJ_M4(2,2)] * s[0]) * id;
    return 1;
}

////////////////////////////////////////////////////////////////////////////////
/// Orthonormalize upper-left 3×3 of 4×4.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4_orthonormalize(
    bj_mat4* BJ_RESTRICT       out,
    const bj_mat4* BJ_RESTRICT A
){
    const bj_real* m = A->m; bj_real* o = out->m;

    bj_vec3 z = { m[BJ_M4(2,0)], m[BJ_M4(2,1)], m[BJ_M4(2,2)] };
    z = bj_vec3_normalize(z);

    bj_vec3 y = { m[BJ_M4(1,0)], m[BJ_M4(1,1)], m[BJ_M4(1,2)] };
    y = bj_vec3_sub(y, bj_vec3_scale(z, bj_vec3_dot(y,z)));
    y = bj_vec3_normalize(y);

    bj_vec3 x = { m[BJ_M4(0,0)], m[BJ_M4(0,1)], m[BJ_M4(0,2)] };
    x = bj_vec3_sub(x, bj_vec3_scale(z, bj_vec3_dot(x,z)));
    x = bj_vec3_sub(x, bj_vec3_scale(y, bj_vec3_dot(x,y)));
    x = bj_vec3_normalize(x);

    o[BJ_M4(0,0)] = x.x; o[BJ_M4(0,1)] = x.y; o[BJ_M4(0,2)] = x.z;
    o[BJ_M4(1,0)] = y.x; o[BJ_M4(1,1)] = y.y; o[BJ_M4(1,2)] = y.z;
    o[BJ_M4(2,0)] = z.x; o[BJ_M4(2,1)] = z.y; o[BJ_M4(2,2)] = z.z;

    /* preserve translation column from A */
    o[BJ_M4(3,0)] = m[BJ_M4(3,0)];
    o[BJ_M4(3,1)] = m[BJ_M4(3,1)];
    o[BJ_M4(3,2)] = m[BJ_M4(3,2)];
    o[BJ_M4(3,3)] = m[BJ_M4(3,3)];
}

////////////////////////////////////////////////////////////////////////////////
/// Off-center perspective frustum (4×4).
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4_frustum(
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
/// Orthographic projection (4×4).
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4_ortho(
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
/// Symmetric perspective from FOV+aspect (4×4).
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4_perspective(
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
/// Viewport transform (4×4).
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4_viewport(
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
/// Look-at view matrix builder (4×4).
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4_lookat(
    bj_mat4* BJ_RESTRICT M,
    bj_vec3              eye,
    bj_vec3              center,
    bj_vec3              up
) {
    bj_vec3 f = bj_vec3_normalize(bj_vec3_sub(center, eye));
    bj_vec3 s = bj_vec3_normalize(bj_vec3_cross(up, f));
    bj_vec3 t = bj_vec3_cross(f, s);

    bj_mat4_identity(M);
    M->m[BJ_M4(0,0)] = s.x;
    M->m[BJ_M4(0,1)] = t.x;
    M->m[BJ_M4(0,2)] = f.x;
    M->m[BJ_M4(1,0)] = s.y;
    M->m[BJ_M4(1,1)] = t.y;
    M->m[BJ_M4(1,2)] = f.y;
    M->m[BJ_M4(2,0)] = s.z;
    M->m[BJ_M4(2,1)] = t.z;
    M->m[BJ_M4(2,2)] = f.z;
    bj_mat4_translation_inplace(M, -eye.x, -eye.y, -eye.z);
}

////////////////////////////////////////////////////////////////////////////////
/// Set 4×3 to identity.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4x3_identity(
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
/// 4×3 translation.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4x3_translate(
    bj_mat4x3* BJ_RESTRICT M,
    bj_real                tx,
    bj_real                ty,
    bj_real                tz
) {
    bj_mat4x3_identity(M);
    M->m[BJ_M43(3,0)] = tx;
    M->m[BJ_M43(3,1)] = ty;
    M->m[BJ_M43(3,2)] = tz;
}

////////////////////////////////////////////////////////////////////////////////
/// 4×3 scale.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4x3_scale(
    bj_mat4x3* BJ_RESTRICT M,
    bj_real                sx,
    bj_real                sy,
    bj_real                sz
) {
    bj_mat4x3_identity(M);
    M->m[BJ_M43(0,0)] = sx;
    M->m[BJ_M43(1,1)] = sy;
    M->m[BJ_M43(2,2)] = sz;
}

////////////////////////////////////////////////////////////////////////////////
/// 4×3 rotate X/Y/Z.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_mat4x3_rotate_x(
    bj_mat4x3* BJ_RESTRICT M,
    bj_real                a
) {
    const bj_real c = bj_cos(a), s = bj_sin(a);
    bj_mat4x3_identity(M);
    M->m[BJ_M43(1,1)] = c; M->m[BJ_M43(1,2)] = s;
    M->m[BJ_M43(2,1)]  = -s; M->m[BJ_M43(2,2)] = c;
}

static BJ_INLINE void bj_mat4x3_rotate_y(
    bj_mat4x3* BJ_RESTRICT M,
    bj_real                a
) {
    const bj_real c = bj_cos(a), s = bj_sin(a);
    bj_mat4x3_identity(M);
    M->m[BJ_M43(0,0)] = c; M->m[BJ_M43(0,2)] = -s;
    M->m[BJ_M43(2,0)] = s; M->m[BJ_M43(2,2)] =  c;
}

static BJ_INLINE void bj_mat4x3_rotate_z(
    bj_mat4x3* BJ_RESTRICT M,
    bj_real                a
) {
    const bj_real c = bj_cos(a), s = bj_sin(a);
    bj_mat4x3_identity(M);
    M->m[BJ_M43(0,0)] = c; M->m[BJ_M43(0,1)] = s;
    M->m[BJ_M43(1,0)]  = -s; M->m[BJ_M43(1,1)] = c;
}

////////////////////////////////////////////////////////////////////////////////
/// 4×3 multiplication: res = A * B.
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
    const bj_real b10 = B->m[BJ_M43(0,1)];
    const bj_real b20 = B->m[BJ_M43(0,2)];

    const bj_real b01 = B->m[BJ_M43(1,0)];
    const bj_real b11 = B->m[BJ_M43(1,1)];
    const bj_real b21 = B->m[BJ_M43(1,2)];

    const bj_real b02 = B->m[BJ_M43(2,0)];
    const bj_real b12 = B->m[BJ_M43(2,1)];
    const bj_real b22 = B->m[BJ_M43(2,2)];

    const bj_real bt0 = B->m[BJ_M43(3,0)];
    const bj_real bt1 = B->m[BJ_M43(3,1)];
    const bj_real bt2 = B->m[BJ_M43(3,2)];

    out->m[BJ_M43(0,0)] = a00*b00 + a01*b10 + a02*b20;
    out->m[BJ_M43(0,1)] = a10*b00 + a11*b10 + a12*b20;
    out->m[BJ_M43(0,2)] = a20*b00 + a21*b10 + a22*b20;

    out->m[BJ_M43(1,0)] = a00*b01 + a01*b11 + a02*b21;
    out->m[BJ_M43(1,1)] = a10*b01 + a11*b11 + a12*b21;
    out->m[BJ_M43(1,2)] = a20*b01 + a21*b11 + a22*b21;

    out->m[BJ_M43(2,0)] = a00*b02 + a01*b12 + a02*b22;
    out->m[BJ_M43(2,1)] = a10*b02 + a11*b12 + a12*b22;
    out->m[BJ_M43(2,2)] = a20*b02 + a21*b12 + a22*b22;

    out->m[BJ_M43(3,0)] = a00*bt0 + a01*bt1 + a02*bt2 + a30;
    out->m[BJ_M43(3,1)] = a10*bt0 + a11*bt1 + a12*bt2 + a31;
    out->m[BJ_M43(3,2)] = a20*bt0 + a21*bt1 + a22*bt2 + a32;
}

////////////////////////////////////////////////////////////////////////////////
/// 4×3 transform point.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec3 bj_mat4x3_mul_point(
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
/// 4×3 transform direction (no translation).
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_vec3 bj_mat4x3_mul_dir(
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
/// Promote 4×3 to 4×4 homogeneous.
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
/// Extract 4×3 from 4×4.
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
