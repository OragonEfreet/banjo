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

////////////////////////////////////////////////////////////////////////////////
/// bj_mat3x3: 3×3 column-major matrix backed by bj_vec3.
/// Columns are arrays of 3 components.
////////////////////////////////////////////////////////////////////////////////
typedef bj_vec3 bj_mat3x3[3];

////////////////////////////////////////////////////////////////////////////////
/// bj_mat3: Alias for bj_mat3x3
////////////////////////////////////////////////////////////////////////////////
typedef bj_mat3x3 bj_mat3;

////////////////////////////////////////////////////////////////////////////////
/// bj_mat3x2: 3×2 column-major matrix backed by bj_vec2. (2D affine: 2×2 linear block plus translation column).
/// Columns are arrays of 2 components.
////////////////////////////////////////////////////////////////////////////////
typedef bj_vec2 bj_mat3x2[3];

////////////////////////////////////////////////////////////////////////////////
/// bj_mat4x4: 4×4 column-major matrix backed by bj_vec4.
/// Columns are arrays of 4 components.
////////////////////////////////////////////////////////////////////////////////
typedef bj_vec4 bj_mat4x4[4];

////////////////////////////////////////////////////////////////////////////////
/// bj_mat4: Alias for bj_mat4x4
////////////////////////////////////////////////////////////////////////////////
typedef bj_mat4x4 bj_mat4;

////////////////////////////////////////////////////////////////////////////////
/// bj_mat4x3: 4×3 column-major matrix backed by bj_vec3. (3D affine: 3×3 linear block plus translation row).
/// Columns are arrays of 3 components.
////////////////////////////////////////////////////////////////////////////////
typedef bj_vec3 bj_mat4x3[4];

////////////////////////////////////////////////////////////////////////////////
/// Set to identity matrix.
/// \param m Output buffer.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3_identity(bj_mat3x3 m) {
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            m[i][j] = (i == j) ? BJ_F(1.0) : BJ_F(0.0);
}

////////////////////////////////////////////////////////////////////////////////
/// Copy matrix contents.
/// \param to Output matrix.
/// \param from Input matrix.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3_copy(bj_mat3x3 to, const bj_mat3x3 from) {
    for (int i = 0; i < 3; ++i) {
        to[i][0] = from[i][0];
        to[i][1] = from[i][1];
        to[i][2] = from[i][2];
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Extract a row as a vector (row index into second subscript).
/// \param res Output matrix.
/// \param m Output buffer.
/// \param r Right plane.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3_row(bj_vec3 res, const bj_mat3x3 m, int r) {
    for (int k = 0; k < 3; ++k) res[k] = m[k][r];
}

////////////////////////////////////////////////////////////////////////////////
/// Extract a column as a vector (column index into first subscript).
/// \param res Output matrix.
/// \param m Output buffer.
/// \param c Parameter.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3_col(bj_vec3 res, const bj_mat3x3 m, int c) {
    for (int k = 0; k < 3; ++k) res[k] = m[c][k];
}

////////////////////////////////////////////////////////////////////////////////
/// Transpose the matrix.
/// \param res Output matrix.
/// \param m Output buffer.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3_transpose(bj_mat3x3 res, const bj_mat3x3 m) {
    for (int j = 0; j < 3; ++j)
        for (int i = 0; i < 3; ++i)
            res[i][j] = m[j][i];
}

////////////////////////////////////////////////////////////////////////////////
/// Component-wise addition: res = a + b.
/// \param res Output matrix.
/// \param a Parameter.
/// \param b Bottom plane.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3_add(bj_mat3x3 res, const bj_mat3x3 a, const bj_mat3x3 b) {
    for (int i = 0; i < 3; ++i) {
        res[i][0] = a[i][0] + b[i][0];
        res[i][1] = a[i][1] + b[i][1];
        res[i][2] = a[i][2] + b[i][2];
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Component-wise subtraction: res = a - b.
/// \param res Output matrix.
/// \param a Parameter.
/// \param b Bottom plane.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3_sub(bj_mat3x3 res, const bj_mat3x3 a, const bj_mat3x3 b) {
    for (int i = 0; i < 3; ++i) {
        res[i][0] = a[i][0] - b[i][0];
        res[i][1] = a[i][1] - b[i][1];
        res[i][2] = a[i][2] - b[i][2];
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Uniformly scale all elements by scalar k.
/// \param res Output matrix.
/// \param m Output buffer.
/// \param k Parameter.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3_scale(bj_mat3x3 res, const bj_mat3x3 m, bj_real k) {
    for (int i = 0; i < 3; ++i) {
        res[i][0] = m[i][0] * k;
        res[i][1] = m[i][1] * k;
        res[i][2] = m[i][2] * k;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Matrix multiplication: res = lhs * rhs (column-major).
/// \param 3][3] Parameter.
/// \param 3][3] Parameter.
/// \param 3][3] Parameter.
/// \note Column-major convention: \c res = lhs * rhs.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3_mul(
    bj_real       res[restrict 3][3],
    const bj_real lhs[restrict 3][3],
    const bj_real rhs[restrict 3][3]
) {
#ifdef BJ_MAT_NO_UNROLL
    bj_mat3 tmp;
    for (int c = 0; c < 3; ++c) {
        for (int r = 0; r < 3; ++r) {
            tmp[c][r] = BJ_F(0.0);
            for (int k = 0; k < 3; ++k)
                tmp[c][r] += lhs[k][r] * rhs[c][k];
        }
    }
    bj_mat3_copy(res, tmp);
#else
    const bj_real a0 = lhs[0][0], b0 = lhs[1][0], c0 = lhs[2][0];
    const bj_real a1 = lhs[0][1], b1 = lhs[1][1], c1 = lhs[2][1];
    const bj_real a2 = lhs[0][2], b2 = lhs[1][2], c2 = lhs[2][2];

    /* column 0 of rhs */
    {
        const bj_real r0 = rhs[0][0], r1 = rhs[0][1], r2 = rhs[0][2];
        res[0][0] = a0*r0 + b0*r1 + c0*r2;
        res[0][1] = a1*r0 + b1*r1 + c1*r2;
        res[0][2] = a2*r0 + b2*r1 + c2*r2;
    }
    /* column 1 */
    {
        const bj_real r0 = rhs[1][0], r1 = rhs[1][1], r2 = rhs[1][2];
        res[1][0] = a0*r0 + b0*r1 + c0*r2;
        res[1][1] = a1*r0 + b1*r1 + c1*r2;
        res[1][2] = a2*r0 + b2*r1 + c2*r2;
    }
    /* column 2 */
    {
        const bj_real r0 = rhs[2][0], r1 = rhs[2][1], r2 = rhs[2][2];
        res[2][0] = a0*r0 + b0*r1 + c0*r2;
        res[2][1] = a1*r0 + b1*r1 + c1*r2;
        res[2][2] = a2*r0 + b2*r1 + c2*r2;
    }
#endif
}

////////////////////////////////////////////////////////////////////////////////
/// Multiply 3×3 matrix by a 3D column vector: res = M * v.
/// \param 3] Parameter.
/// \param 3][3] Parameter.
/// \param 3] Parameter.
/// \note Column-major convention: \c res = lhs * rhs.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3_mul_vec3(
    bj_real       res[restrict 3],
    const bj_real m[restrict 3][3],
    const bj_real v[restrict 3]
) {
#ifdef BJ_MAT_NO_UNROLL
    for (int j = 0; j < 3; ++j) {
        res[j] = BJ_F(0.0);
        for (int i = 0; i < 3; ++i)
            res[j] += m[i][j] * v[i];
    }
#else
    const bj_real vx = v[0], vy = v[1], vz = v[2];
    res[0] = m[0][0]*vx + m[1][0]*vy + m[2][0]*vz;
    res[1] = m[0][1]*vx + m[1][1]*vy + m[2][1]*vz;
    res[2] = m[0][2]*vx + m[1][2]*vy + m[2][2]*vz;
#endif
}

////////////////////////////////////////////////////////////////////////////////
/// Transform a point (homogeneous w=1 for 2D/3D variants).
/// \param res Output matrix.
/// \param m Output buffer.
/// \param p Input vector.
/// \note Column-major convention: \c res = lhs * rhs.
/// \note Uses homogeneous coordinates with w=1.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3_mul_point(bj_vec2 res, const bj_mat3x3 m, const bj_vec2 p) {
    bj_vec3 v = { p[0], p[1], BJ_F(1.0) };
    bj_vec3 o;
    bj_mat3_mul_vec3(o, m, v);
    bj_real w = o[2];
    if (w != BJ_F(0.0)) { res[0] = o[0] / w; res[1] = o[1] / w; }
    else                { res[0] = o[0];     res[1] = o[1];     }
}

////////////////////////////////////////////////////////////////////////////////
/// Matrix operation.
/// \param res Output matrix.
/// \param m Output buffer.
/// \param v2 Input vector.
/// \note Column-major convention: \c res = lhs * rhs.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3_mul_vector2(bj_vec2 res, const bj_mat3x3 m, const bj_vec2 v2) {
    bj_vec3 v = { v2[0], v2[1], BJ_F(0.0) };
    bj_vec3 o; bj_mat3_mul_vec3(o, m, v);
    res[0] = o[0]; res[1] = o[1];
}

////////////////////////////////////////////////////////////////////////////////
/// Create a translation matrix.
/// \param res Output matrix.
/// \param tx Translation along X.
/// \param ty Translation along Y.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3_translation(bj_mat3x3 res, bj_real tx, bj_real ty) {
    bj_mat3_identity(res);
    res[2][0] = tx;
    res[2][1] = ty;
}

////////////////////////////////////////////////////////////////////////////////
/// Apply a translation to an existing matrix in-place.
/// \param M Output buffer.
/// \param tx Translation along X.
/// \param ty Translation along Y.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3_translation_inplace(bj_mat3x3 M, bj_real tx, bj_real ty) {
    bj_vec3 t = { tx, ty, BJ_F(0.0) };
    bj_vec3 r;
    for (int i = 0; i < 3; ++i) {
        bj_mat3_row(r, M, i);
        M[2][i] += r[0]*t[0] + r[1]*t[1] + r[2]*t[2];
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Create 2D scaling matrix with independent X/Y scales.
/// \param res Output matrix.
/// \param sx Scale along X.
/// \param sy Scale along Y.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3_scale_xy(bj_mat3x3 res, bj_real sx, bj_real sy) {
    bj_mat3_identity(res);
    res[0][0] = sx;
    res[1][1] = sy;
}

////////////////////////////////////////////////////////////////////////////////
/// Create 2D shear matrix.
/// \param res Output matrix.
/// \param shx Shear along X (xy).
/// \param shy Shear along Y (yx).
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3_shear(bj_mat3x3 res, bj_real shx, bj_real shy) {
    bj_mat3_identity(res);
    res[1][0] = shy;
    res[0][1] = shx;
}

////////////////////////////////////////////////////////////////////////////////
/// Rotate about arbitrary axis, post-multiplied.
/// \param res Output matrix.
/// \param angle Rotation angle in radians.
/// \param angle Angle in radians.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3_rotate(bj_mat3x3 res, bj_real angle) {
    bj_real s = bj_sin(angle), c = bj_cos(angle);
    /* [ c  s  0 ]
       [-s  c  0 ]
       [ 0  0  1 ] */
    res[0][0] =  c;  res[0][1] =  s;  res[0][2] = BJ_F(0.0);
    res[1][0] = -s;  res[1][1] =  c;  res[1][2] = BJ_F(0.0);
    res[2][0] = BJ_F(0.0); res[2][1] = BJ_F(0.0); res[2][2] = BJ_F(1.0);
}

////////////////////////////////////////////////////////////////////////////////
/// Invert the matrix.
/// \param res Output matrix.
/// \param m Output buffer.
/// \warning Undefined if the matrix is singular (determinant is zero).
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3_inverse(bj_mat3x3 res, const bj_mat3x3 m) {
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
/// Orthographic projection matrix.
/// \param 3][3] Parameter.
/// \param l Left plane.
/// \param r Right plane.
/// \param b Bottom plane.
/// \param t Top plane.
/// \note NDC convention: +Z forward, depth in [0,1], Y-down as implemented.
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
/// Viewport transform matrix.
/// \param 3][3] Parameter.
/// \param x X value.
/// \param y Y value.
/// \param w Parameter.
/// \param h Parameter.
/// \note NDC convention: +Z forward, depth in [0,1], Y-down as implemented.
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
/// Set to identity matrix.
/// \param 3] Parameter.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3x2_identity(bj_vec2 m[restrict 3]) {
    m[0][0]=BJ_F(1.0); m[0][1]=BJ_F(0.0); // col 0: [1,0]
    m[1][0]=BJ_F(0.0); m[1][1]=BJ_F(1.0); // col 1: [0,1]
    m[2][0]=BJ_F(0.0); m[2][1]=BJ_F(0.0); // col 2: [tx,ty] = [0,0]
}

////////////////////////////////////////////////////////////////////////////////
/// Matrix operation.
/// \param 3] Parameter.
/// \param tx Translation along X.
/// \param ty Translation along Y.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3x2_translate(bj_vec2 m[restrict 3], bj_real tx, bj_real ty) {
    m[0][0]=BJ_F(1.0); m[0][1]=BJ_F(0.0);
    m[1][0]=BJ_F(0.0); m[1][1]=BJ_F(1.0);
    m[2][0]=tx;        m[2][1]=ty;
}

////////////////////////////////////////////////////////////////////////////////
/// Uniformly scale all elements by scalar k.
/// \param 3] Parameter.
/// \param sx Scale along X.
/// \param sy Scale along Y.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3x2_scale(bj_vec2 m[restrict 3], bj_real sx, bj_real sy) {
    m[0][0]=sx;        m[0][1]=BJ_F(0.0);
    m[1][0]=BJ_F(0.0); m[1][1]=sy;
    m[2][0]=BJ_F(0.0); m[2][1]=BJ_F(0.0);
}

////////////////////////////////////////////////////////////////////////////////
/// Rotate about arbitrary axis, post-multiplied.
/// \param 3] Parameter.
/// \param angle Rotation angle in radians.
/// \param angle Angle in radians.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3x2_rotate(bj_vec2 m[restrict 3], bj_real angle) {
    const bj_real c = bj_cosf(angle), s = bj_sinf(angle);
    m[0][0]= c;  m[0][1]= s;  // col 0 = ( a, c )
    m[1][0]=-s;  m[1][1]= c;  // col 1 = ( b, d )
    m[2][0]=BJ_F(0.0); m[2][1]=BJ_F(0.0); // translation
}

////////////////////////////////////////////////////////////////////////////////
/// Matrix multiplication: res = lhs * rhs (column-major).
/// \param 3] Parameter.
/// \param 3] Parameter.
/// \param 3] Parameter.
/// \note Column-major convention: \c res = lhs * rhs.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3x2_mul(bj_vec2 res[restrict 3],
                                 const bj_vec2 A  [restrict 3],
                                 const bj_vec2 B  [restrict 3]) {
#ifdef BJ_MAT_NO_UNROLL
    // 2×2 linear block: R = A.R * B.R
    res[0][0] = A[0][0]*B[0][0] + A[1][0]*B[0][1];
    res[0][1] = A[0][1]*B[0][0] + A[1][1]*B[0][1];

    res[1][0] = A[0][0]*B[1][0] + A[1][0]*B[1][1];
    res[1][1] = A[0][1]*B[1][0] + A[1][1]*B[1][1];

    // translation: T = A.R * B.T + A.T
    res[2][0] = A[0][0]*B[2][0] + A[1][0]*B[2][1] + A[2][0];
    res[2][1] = A[0][1]*B[2][0] + A[1][1]*B[2][1] + A[2][1];
#else
    const bj_real a00=A[0][0], a10=A[0][1];
    const bj_real a01=A[1][0], a11=A[1][1];
    const bj_real a02=A[2][0], a12=A[2][1];

    const bj_real b00=B[0][0], b10=B[0][1];
    const bj_real b01=B[1][0], b11=B[1][1];
    const bj_real b02=B[2][0], b12=B[2][1];

    res[0][0] = a00*b00 + a01*b10;
    res[0][1] = a10*b00 + a11*b10;

    res[1][0] = a00*b01 + a01*b11;
    res[1][1] = a10*b01 + a11*b11;

    res[2][0] = a00*b02 + a01*b12 + a02;
    res[2][1] = a10*b02 + a11*b12 + a12;
#endif
}

////////////////////////////////////////////////////////////////////////////////
/// Transform a point (homogeneous w=1 for 2D/3D variants).
/// \param 2] Parameter.
/// \param 3] Parameter.
/// \param 2] Parameter.
/// \note Column-major convention: \c res = lhs * rhs.
/// \note Uses homogeneous coordinates with w=1.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3x2_mul_point(bj_real r[restrict 2],
                                       const bj_vec2 m[restrict 3],
                                       const bj_real p[restrict 2]) {
#ifdef BJ_MAT_NO_UNROLL
    const bj_real x=p[0], y=p[1];
    r[0] = m[0][0]*x + m[1][0]*y + m[2][0];
    r[1] = m[0][1]*x + m[1][1]*y + m[2][1];
#else
    const bj_real x=p[0], y=p[1];
    r[0] = m[0][0]*x + m[1][0]*y + m[2][0];
    r[1] = m[0][1]*x + m[1][1]*y + m[2][1];
#endif
}

////////////////////////////////////////////////////////////////////////////////
/// Transform a direction (homogeneous w=0; ignores translation).
/// \param 2] Parameter.
/// \param 3] Parameter.
/// \param 2] Parameter.
/// \note Column-major convention: \c res = lhs * rhs.
/// \note Uses homogeneous coordinates with w=0 (no translation).
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3x2_mul_dir(bj_real r[restrict 2],
                                     const bj_vec2 m[restrict 3],
                                     const bj_real v[restrict 2]) {
#ifdef BJ_MAT_NO_UNROLL
    const bj_real x=v[0], y=v[1];
    r[0] = m[0][0]*x + m[1][0]*y;
    r[1] = m[0][1]*x + m[1][1]*y;
#else
    const bj_real x=v[0], y=v[1];
    r[0] = m[0][0]*x + m[1][0]*y;
    r[1] = m[0][1]*x + m[1][1]*y;
#endif
}

////////////////////////////////////////////////////////////////////////////////
/// Promote 2D affine (3×2) to 3×3 homogeneous.
/// \param 3][3] Parameter.
/// \param 3] Parameter.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3_from_mat3x2(bj_real out3x3[restrict 3][3],
                                       const bj_vec2 a[restrict 3]) {
    out3x3[0][0]=a[0][0]; out3x3[0][1]=a[0][1]; out3x3[0][2]=BJ_F(0.0);
    out3x3[1][0]=a[1][0]; out3x3[1][1]=a[1][1]; out3x3[1][2]=BJ_F(0.0);
    out3x3[2][0]=a[2][0]; out3x3[2][1]=a[2][1]; out3x3[2][2]=BJ_F(1.0);
}

////////////////////////////////////////////////////////////////////////////////
/// Extract 3×2 affine block from 3×3.
/// \param 3] Parameter.
/// \param 3][3] Parameter.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3x2_from_mat3(bj_vec2 out3x2[restrict 3],
                                       const bj_real m[restrict 3][3]) {
    out3x2[0][0]=m[0][0]; out3x2[0][1]=m[0][1];
    out3x2[1][0]=m[1][0]; out3x2[1][1]=m[1][1];
    out3x2[2][0]=m[2][0]; out3x2[2][1]=m[2][1];
}

////////////////////////////////////////////////////////////////////////////////
/// Orthographic projection matrix.
/// \param 3] Parameter.
/// \param l Left plane.
/// \param r Right plane.
/// \param b Bottom plane.
/// \param t Top plane.
/// \note NDC convention: +Z forward, depth in [0,1], Y-down as implemented.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3x2_ortho(bj_vec2 omat[restrict 3],
                                   bj_real l, bj_real r,
                                   bj_real b, bj_real t)
{
    const bj_real sx = BJ_F(2.0) / (r - l);
    const bj_real sy = -BJ_F(2.0) / (t - b);   // Y-down
    const bj_real tx = -(r + l) / (r - l);
    const bj_real ty =  (t + b) / (t - b);

    omat[0][0] = sx; omat[0][1] = BJ_F(0.0);  // col0 = (a,c)
    omat[1][0] = BJ_F(0.0); omat[1][1] = sy;  // col1 = (b,d)
    omat[2][0] = tx; omat[2][1] = ty;         // col2 = (tx,ty)
}

////////////////////////////////////////////////////////////////////////////////
/// Viewport transform matrix.
/// \param 3] Parameter.
/// \param x X value.
/// \param y Y value.
/// \param w Parameter.
/// \param h Parameter.
/// \note NDC convention: +Z forward, depth in [0,1], Y-down as implemented.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat3x2_viewport(bj_vec2 vpmat[restrict 3],
                                      bj_real x, bj_real y,
                                      bj_real w, bj_real h)
{
    const bj_real sx = w * BJ_F(0.5);
    const bj_real sy = h * BJ_F(0.5);
    const bj_real tx = x + sx;
    const bj_real ty = y + sy;

    vpmat[0][0] = sx; vpmat[0][1] = BJ_F(0.0); // col0
    vpmat[1][0] = BJ_F(0.0); vpmat[1][1] = sy; // col1
    vpmat[2][0] = tx; vpmat[2][1] = ty;        // col2
}


////////////////////////////////////////////////////////////////////////////////
/// Set to identity matrix.
/// \param mat Input matrix.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_identity(bj_mat4x4 mat)
{
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            mat[i][j] = i == j ? BJ_F(1.0) : BJ_F(0.0);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Copy matrix contents.
/// \param to Output matrix.
/// \param from Input matrix.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_copy(bj_mat4x4 to, const bj_mat4x4 from)
{
    for (int i = 0; i < 4; ++i) {
        bj_vec4_copy(to[i], from[i]);
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Extract a row as a vector (row index into second subscript).
/// \param res Output matrix.
/// \param mat Input matrix.
/// \param r Right plane.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_row(bj_vec4 res, const bj_mat4x4 mat, int r)
{
    for (int k = 0; k < 4; ++k) {
        res[k] = mat[k][r];
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Extract a column as a vector (column index into first subscript).
/// \param res Output matrix.
/// \param mat Input matrix.
/// \param c Parameter.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_col(bj_vec4 res, const bj_mat4x4 mat, int c)
{
    for (int k = 0; k < 4; ++k) {
        res[k] = mat[c][k];
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Transpose the matrix.
/// \param res Output matrix.
/// \param mat Input matrix.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_transpose(bj_mat4x4 res, const bj_mat4x4 mat)
{
    for (int j = 0; j < 4; ++j) {
        for (int i = 0; i < 4; ++i) {
            res[i][j] = mat[j][i];
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Component-wise addition: res = a + b.
/// \param res Output matrix.
/// \param lhs Input matrix.
/// \param rhs Input matrix.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_add(bj_mat4x4 res, const bj_mat4x4 lhs, const bj_mat4x4 rhs)
{
    for (int i = 0; i < 4; ++i) {
        bj_vec4_add(res[i], lhs[i], rhs[i]);
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Component-wise subtraction: res = a - b.
/// \param res Output matrix.
/// \param lhs Input matrix.
/// \param rhs Input matrix.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_sub(bj_mat4x4 res, const bj_mat4x4 lhs, const bj_mat4x4 rhs)
{
    for (int i = 0; i < 4; ++i) {
        bj_vec4_sub(res[i], lhs[i], rhs[i]);
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Uniformly scale all elements by scalar k.
/// \param res Output matrix.
/// \param lhs Input matrix.
/// \param k Parameter.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_scale(bj_mat4x4 res, const bj_mat4x4 lhs, bj_real k)
{
    for (int i = 0; i < 4; ++i) {
        bj_vec4_scale(res[i], lhs[i], k);
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Scale each basis vector by given X/Y/Z factors.
/// \param res Output matrix.
/// \param mat Input matrix.
/// \param x X value.
/// \param y Y value.
/// \param z Z value.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_scale_xyz(bj_mat4x4 res, const bj_mat4x4 mat, bj_real x, bj_real y, bj_real z)
{
    bj_vec4_scale(res[0], mat[0], x);
    bj_vec4_scale(res[1], mat[1], y);
    bj_vec4_scale(res[2], mat[2], z);
    bj_vec4_copy(res[3], mat[3]);
}

////////////////////////////////////////////////////////////////////////////////
/// Matrix multiplication: res = lhs * rhs (column-major).
/// \param 4][4] Parameter.
/// \param 4][4] Parameter.
/// \param 4][4] Parameter.
/// \note Column-major convention: \c res = lhs * rhs.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_mul(
    bj_real       res[restrict 4][4],
    const bj_real lhs[restrict 4][4],
    const bj_real rhs[restrict 4][4]
) {
#ifdef BJ_MAT_NO_UNROLL
    bj_mat4x4 temp;
    for (int c = 0; c < 4; ++c) {
        for (int r = 0; r < 4; ++r) {
            temp[c][r] = BJ_F(0.0);
            for (int k = 0; k < 4; ++k)
                temp[c][r] += lhs[k][r] * rhs[c][k];
        }
    }
    bj_mat4_copy(res, temp);
#else
    const bj_real a0 = lhs[0][0], b0 = lhs[1][0], c0 = lhs[2][0], d0 = lhs[3][0];
    const bj_real a1 = lhs[0][1], b1 = lhs[1][1], c1 = lhs[2][1], d1 = lhs[3][1];
    const bj_real a2 = lhs[0][2], b2 = lhs[1][2], c2 = lhs[2][2], d2 = lhs[3][2];
    const bj_real a3 = lhs[0][3], b3 = lhs[1][3], c3 = lhs[2][3], d3 = lhs[3][3];

    /* column 0 */
    {
        const bj_real r0 = rhs[0][0], r1 = rhs[0][1], r2 = rhs[0][2], r3 = rhs[0][3];
        res[0][0] = a0*r0 + b0*r1 + c0*r2 + d0*r3;
        res[0][1] = a1*r0 + b1*r1 + c1*r2 + d1*r3;
        res[0][2] = a2*r0 + b2*r1 + c2*r2 + d2*r3;
        res[0][3] = a3*r0 + b3*r1 + c3*r2 + d3*r3;
    }
    /* column 1 */
    {
        const bj_real r0 = rhs[1][0], r1 = rhs[1][1], r2 = rhs[1][2], r3 = rhs[1][3];
        res[1][0] = a0*r0 + b0*r1 + c0*r2 + d0*r3;
        res[1][1] = a1*r0 + b1*r1 + c1*r2 + d1*r3;
        res[1][2] = a2*r0 + b2*r1 + c2*r2 + d2*r3;
        res[1][3] = a3*r0 + b3*r1 + c3*r2 + d3*r3;
    }
    /* column 2 */
    {
        const bj_real r0 = rhs[2][0], r1 = rhs[2][1], r2 = rhs[2][2], r3 = rhs[2][3];
        res[2][0] = a0*r0 + b0*r1 + c0*r2 + d0*r3;
        res[2][1] = a1*r0 + b1*r1 + c1*r2 + d1*r3;
        res[2][2] = a2*r0 + b2*r1 + c2*r2 + d2*r3;
        res[2][3] = a3*r0 + b3*r1 + c3*r2 + d3*r3;
    }
    /* column 3 */
    {
        const bj_real r0 = rhs[3][0], r1 = rhs[3][1], r2 = rhs[3][2], r3 = rhs[3][3];
        res[3][0] = a0*r0 + b0*r1 + c0*r2 + d0*r3;
        res[3][1] = a1*r0 + b1*r1 + c1*r2 + d1*r3;
        res[3][2] = a2*r0 + b2*r1 + c2*r2 + d2*r3;
        res[3][3] = a3*r0 + b3*r1 + c3*r2 + d3*r3;
    }
#endif
}

////////////////////////////////////////////////////////////////////////////////
/// Multiply matrix by a 4D column vector: res = M * v.
/// \param 4] Parameter.
/// \param 4][4] Parameter.
/// \param 4] Parameter.
/// \note Column-major convention: \c res = lhs * rhs.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_mul_vec4(
    bj_real       res[restrict 4],
    const bj_real mat[restrict 4][4],
    const bj_real v[restrict 4]
) {
#ifdef BJ_MAT_NO_UNROLL
    for (int j = 0; j < 4; ++j) {
        res[j] = BJ_F(0.0);
        for (int i = 0; i < 4; ++i) {
            res[j] += mat[i][j] * v[i];
        }
    }
#else
    const bj_real vx = v[0], vy = v[1], vz = v[2], vw = v[3];
    res[0] = mat[0][0]*vx + mat[1][0]*vy + mat[2][0]*vz + mat[3][0]*vw;
    res[1] = mat[0][1]*vx + mat[1][1]*vy + mat[2][1]*vz + mat[3][1]*vw;
    res[2] = mat[0][2]*vx + mat[1][2]*vy + mat[2][2]*vz + mat[3][2]*vw;
    res[3] = mat[0][3]*vx + mat[1][3]*vy + mat[2][3]*vz + mat[3][3]*vw;
#endif
}

////////////////////////////////////////////////////////////////////////////////
/// Create a translation matrix.
/// \param res Output matrix.
/// \param x X value.
/// \param y Y value.
/// \param z Z value.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_translation(bj_mat4x4 res, bj_real x, bj_real y, bj_real z)
{
    bj_mat4_identity(res);
    res[3][0] = x;
    res[3][1] = y;
    res[3][2] = z;
}

////////////////////////////////////////////////////////////////////////////////
/// Apply a translation to an existing matrix in-place.
/// \param M Output buffer.
/// \param x X value.
/// \param y Y value.
/// \param z Z value.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_translation_inplace(bj_mat4x4 M, bj_real x, bj_real y, bj_real z)
{
    bj_vec4 t = { x, y, z, BJ_F(0.0) };
    bj_vec4 r;
    for (int i = 0; i < 4; ++i) {
        bj_mat4_row(r, M, i);
        M[3][i] += bj_vec4_dot(r, t);
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Outer product of two 3D vectors placed in the upper-left 3×3 block.
/// \param res Output matrix.
/// \param a Parameter.
/// \param b Bottom plane.
/// \note Column-major convention: \c res = lhs * rhs.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_mul_outer(bj_mat4x4 res, const bj_vec3 a, const bj_vec3 b)
{
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            res[i][j] = i < 3 && j < 3 ? a[i] * b[j] : BJ_F(0.0);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Rotate about arbitrary axis, post-multiplied.
/// \param res Output matrix.
/// \param mat Input matrix.
/// \param x X value.
/// \param y Y value.
/// \param z Z value.
/// \param angle Rotation angle in radians.
/// \param angle Angle in radians.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_rotate(bj_mat4x4 res, const bj_mat4x4 mat, bj_real x, bj_real y, bj_real z, bj_real angle)
{
    bj_real s = bj_sin(angle);
    bj_real c = bj_cos(angle);
    bj_vec3 u = { x, y, z };

    if (bj_vec3_len(u) > BJ_F(1e-4)) {
        bj_vec3_normalize(u, u);
        bj_mat4x4 T;
        bj_mat4_mul_outer(T, u, u);

        bj_mat4x4 S = {
            {    BJ_F(0),  u[2], -u[1], BJ_F(0)},
            {-u[2],     BJ_F(0),  u[0], BJ_F(0)},
            { u[1], -u[0],     BJ_F(0), BJ_F(0)},
            {    BJ_F(0),     BJ_F(0),     BJ_F(0), BJ_F(0)}
        };
        bj_mat4_scale(S, S, s);

        bj_mat4x4 C;
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
/// Rotate about X axis and post-multiply: res = mat * R_x.
/// \param res Output matrix.
/// \param mat Input matrix.
/// \param angle Rotation angle in radians.
/// \param angle Angle in radians.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_rotate_x(bj_mat4x4 res, const bj_mat4x4 mat, bj_real angle)
{
    bj_real s = bj_sin(angle);
    bj_real c = bj_cos(angle);
    bj_mat4x4 R = {
        {BJ_F(1.0), BJ_F(0.0), BJ_F(0.0), BJ_F(0.0)},
        {BJ_F(0.0),   c,   s, BJ_F(0.0)},
        {BJ_F(0.0),  -s,   c, BJ_F(0.0)},
        {BJ_F(0.0), BJ_F(0.0), BJ_F(0.0), BJ_F(1.0)}
    };
    bj_mat4_mul(res, mat, R);
}

////////////////////////////////////////////////////////////////////////////////
/// Rotate about Y axis and post-multiply: res = mat * R_y.
/// \param res Output matrix.
/// \param mat Input matrix.
/// \param angle Rotation angle in radians.
/// \param angle Angle in radians.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_rotate_y(bj_mat4x4 res, const bj_mat4x4 mat, bj_real angle)
{
    bj_real s = bj_sin(angle);
    bj_real c = bj_cos(angle);
    bj_mat4x4 R = {
        {   c, BJ_F(0.0),  -s, BJ_F(0.0)},
        { BJ_F(0.0), BJ_F(1.0), BJ_F(0.0), BJ_F(0.0)},
        {   s, BJ_F(0.0),   c, BJ_F(0.0)},
        { BJ_F(0.0), BJ_F(0.0), BJ_F(0.0), BJ_F(1.0)}
    };
    bj_mat4_mul(res, mat, R);
}

////////////////////////////////////////////////////////////////////////////////
/// Rotate about Z axis and post-multiply: res = mat * R_z.
/// \param res Output matrix.
/// \param mat Input matrix.
/// \param angle Rotation angle in radians.
/// \param angle Angle in radians.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_rotate_z(bj_mat4x4 res, const bj_mat4x4 mat, bj_real angle)
{
    bj_real s = bj_sin(angle);
    bj_real c = bj_cos(angle);
    bj_mat4x4 R = {
        {   c,   s, BJ_F(0.0), BJ_F(0.0)},
        {  -s,   c, BJ_F(0.0), BJ_F(0.0)},
        { BJ_F(0.0), BJ_F(0.0), BJ_F(1.0), BJ_F(0.0)},
        { BJ_F(0.0), BJ_F(0.0), BJ_F(0.0), BJ_F(1.0)}
    };
    bj_mat4_mul(res, mat, R);
}


////////////////////////////////////////////////////////////////////////////////
/// Trackball (arcball) rotation from two 2D positions.
/// \param R Output matrix.
/// \param M Output buffer.
/// \param _a Parameter.
/// \param _b Parameter.
/// \param s Scale or sensitivity.
/// \note Inputs are normalized to the unit sphere if inside the unit disk.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_rotate_arcball(bj_mat4x4 R, const bj_mat4x4 M, bj_vec2 const _a, bj_vec2 const _b, bj_real s)
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
/// Invert the matrix.
/// \param res Output matrix.
/// \param mat Input matrix.
/// \warning Undefined if the matrix is singular (determinant is zero).
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_inverse(bj_mat4x4 res, const bj_mat4x4 mat) {
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
/// Orthonormalize the upper-left 3×3 (Gram–Schmidt).
/// \param res Output matrix.
/// \param mat Input matrix.
/// \note Operates on the upper-left 3×3; translation (last row/column) is preserved.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_orthonormalize(bj_mat4x4 res, const bj_mat4x4 mat) {
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
/// Off-center perspective frustum matrix.
/// \param 4][4] Parameter.
/// \param l Left plane.
/// \param r Right plane.
/// \param b Bottom plane.
/// \param t Top plane.
/// \param n Near plane (> 0).
/// \param f Far plane (> near).
/// \note NDC convention: +Z forward, depth in [0,1], Y-down as implemented.
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
/// Orthographic projection matrix.
/// \param 4][4] Parameter.
/// \param l Left plane.
/// \param r Right plane.
/// \param b Bottom plane.
/// \param t Top plane.
/// \param n Near plane (> 0).
/// \param f Far plane (> near).
/// \note NDC convention: +Z forward, depth in [0,1], Y-down as implemented.
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
/// Symmetric perspective projection matrix from FOV and aspect.
/// \param 4][4] Parameter.
/// \param y_fov Vertical field of view in radians.
/// \param aspect Aspect ratio width/height.
/// \param n Near plane (> 0).
/// \param f Far plane (> near).
/// \note NDC convention: +Z forward, depth in [0,1], Y-down as implemented.
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
/// Viewport transform matrix.
/// \param 4][4] Parameter.
/// \param x X value.
/// \param y Y value.
/// \param w Parameter.
/// \param h Parameter.
/// \note NDC convention: +Z forward, depth in [0,1], Y-down as implemented.
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
/// Look-at view matrix builder.
/// \param 4][4] Parameter.
/// \param eye Input vector.
/// \param center Input vector.
/// \param up Input vector.
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
/// Set to identity matrix.
/// \param 4] Parameter.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4x3_identity(bj_vec3 m[restrict 4]) {
    m[0][0]=BJ_F(1.0); m[0][1]=BJ_F(0.0); m[0][2]=BJ_F(0.0);
    m[1][0]=BJ_F(0.0); m[1][1]=BJ_F(1.0); m[1][2]=BJ_F(0.0);
    m[2][0]=BJ_F(0.0); m[2][1]=BJ_F(0.0); m[2][2]=BJ_F(1.0);
    m[3][0]=BJ_F(0.0); m[3][1]=BJ_F(0.0); m[3][2]=BJ_F(0.0);
}

////////////////////////////////////////////////////////////////////////////////
/// Matrix operation.
/// \param 4] Parameter.
/// \param tx Translation along X.
/// \param ty Translation along Y.
/// \param tz Translation along Z.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4x3_translate(bj_vec3 m[restrict 4],
                                       bj_real tx, bj_real ty, bj_real tz) {
    m[0][0]=BJ_F(1.0); m[0][1]=BJ_F(0.0); m[0][2]=BJ_F(0.0);
    m[1][0]=BJ_F(0.0); m[1][1]=BJ_F(1.0); m[1][2]=BJ_F(0.0);
    m[2][0]=BJ_F(0.0); m[2][1]=BJ_F(0.0); m[2][2]=BJ_F(1.0);
    m[3][0]=tx;        m[3][1]=ty;        m[3][2]=tz;
}

////////////////////////////////////////////////////////////////////////////////
/// Uniformly scale all elements by scalar k.
/// \param 4] Parameter.
/// \param sx Scale along X.
/// \param sy Scale along Y.
/// \param sz Scale along Z.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4x3_scale(bj_vec3 m[restrict 4],
                                   bj_real sx, bj_real sy, bj_real sz) {
    m[0][0]=sx;        m[0][1]=BJ_F(0.0); m[0][2]=BJ_F(0.0);
    m[1][0]=BJ_F(0.0); m[1][1]=sy;        m[1][2]=BJ_F(0.0);
    m[2][0]=BJ_F(0.0); m[2][1]=BJ_F(0.0); m[2][2]=sz;
    m[3][0]=BJ_F(0.0); m[3][1]=BJ_F(0.0); m[3][2]=BJ_F(0.0);
}

////////////////////////////////////////////////////////////////////////////////
/// Rotate about X axis and post-multiply: res = mat * R_x.
/// \param 4] Parameter.
/// \param angle Rotation angle in radians.
/// \param angle Angle in radians.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4x3_rotate_x(bj_vec3 m[restrict 4], bj_real angle) {
    const bj_real c = bj_cosf(angle), s = bj_sinf(angle);
    m[0][0]=BJ_F(1.0); m[0][1]=BJ_F(0.0); m[0][2]=BJ_F(0.0);
    m[1][0]=BJ_F(0.0); m[1][1]= c;        m[1][2]= s;
    m[2][0]=BJ_F(0.0); m[2][1]=-s;        m[2][2]= c;
    m[3][0]=BJ_F(0.0); m[3][1]=BJ_F(0.0); m[3][2]=BJ_F(0.0);
}

////////////////////////////////////////////////////////////////////////////////
/// Rotate about Y axis and post-multiply: res = mat * R_y.
/// \param 4] Parameter.
/// \param angle Rotation angle in radians.
/// \param angle Angle in radians.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4x3_rotate_y(bj_vec3 m[restrict 4], bj_real angle) {
    const bj_real c = bj_cosf(angle), s = bj_sinf(angle);
    m[0][0]= c;        m[0][1]=BJ_F(0.0); m[0][2]=-s;
    m[1][0]=BJ_F(0.0); m[1][1]=BJ_F(1.0); m[1][2]=BJ_F(0.0);
    m[2][0]= s;        m[2][1]=BJ_F(0.0); m[2][2]= c;
    m[3][0]=BJ_F(0.0); m[3][1]=BJ_F(0.0); m[3][2]=BJ_F(0.0);
}

////////////////////////////////////////////////////////////////////////////////
/// Rotate about Z axis and post-multiply: res = mat * R_z.
/// \param 4] Parameter.
/// \param angle Rotation angle in radians.
/// \param angle Angle in radians.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4x3_rotate_z(bj_vec3 m[restrict 4], bj_real angle) {
    const bj_real c = bj_cosf(angle), s = bj_sinf(angle);
    m[0][0]= c;        m[0][1]= s;        m[0][2]=BJ_F(0.0);
    m[1][0]=-s;        m[1][1]= c;        m[1][2]=BJ_F(0.0);
    m[2][0]=BJ_F(0.0); m[2][1]=BJ_F(0.0); m[2][2]=BJ_F(1.0);
    m[3][0]=BJ_F(0.0); m[3][1]=BJ_F(0.0); m[3][2]=BJ_F(0.0);
}

////////////////////////////////////////////////////////////////////////////////
/// Matrix multiplication: res = lhs * rhs (column-major).
/// \param 4] Parameter.
/// \param 4] Parameter.
/// \param 4] Parameter.
/// \note Column-major convention: \c res = lhs * rhs.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4x3_mul(bj_vec3 res[restrict 4],
                                 const bj_vec3 A  [restrict 4],
                                 const bj_vec3 B  [restrict 4]) {
#ifdef BJ_MAT_NO_UNROLL
    for (int r=0; r<3; ++r) {
        res[0][r] = A[0][r]*B[0][0] + A[1][r]*B[1][0] + A[2][r]*B[2][0];
        res[1][r] = A[0][r]*B[0][1] + A[1][r]*B[1][1] + A[2][r]*B[2][1];
        res[2][r] = A[0][r]*B[0][2] + A[1][r]*B[1][2] + A[2][r]*B[2][2];
        res[3][r] = A[0][r]*B[3][0] + A[1][r]*B[3][1] + A[2][r]*B[3][2] + A[3][r];
    }
#else
    // A rows (gives best locality in column-major)
    const bj_real a00=A[0][0], a10=A[1][0], a20=A[2][0], a30=A[3][0];
    const bj_real a01=A[0][1], a11=A[1][1], a21=A[2][1], a31=A[3][1];
    const bj_real a02=A[0][2], a12=A[1][2], a22=A[2][2], a32=A[3][2];

    // B columns
    const bj_real b00=B[0][0], b01=B[1][0], b02=B[2][0];
    const bj_real b10=B[0][1], b11=B[1][1], b12=B[2][1];
    const bj_real b20=B[0][2], b21=B[1][2], b22=B[2][2];
    const bj_real bt0=B[3][0], bt1=B[3][1], bt2=B[3][2];

    // res col 0
    res[0][0] = a00*b00 + a10*b01 + a20*b02;
    res[0][1] = a01*b00 + a11*b01 + a21*b02;
    res[0][2] = a02*b00 + a12*b01 + a22*b02;
    // res col 1
    res[1][0] = a00*b10 + a10*b11 + a20*b12;
    res[1][1] = a01*b10 + a11*b11 + a21*b12;
    res[1][2] = a02*b10 + a12*b11 + a22*b12;
    // res col 2
    res[2][0] = a00*b20 + a10*b21 + a20*b22;
    res[2][1] = a01*b20 + a11*b21 + a21*b22;
    res[2][2] = a02*b20 + a12*b21 + a22*b22;
    // translation
    res[3][0] = a00*bt0 + a10*bt1 + a20*bt2 + a30;
    res[3][1] = a01*bt0 + a11*bt1 + a21*bt2 + a31;
    res[3][2] = a02*bt0 + a12*bt1 + a22*bt2 + a32;
#endif
}

////////////////////////////////////////////////////////////////////////////////
/// Transform a point (homogeneous w=1 for 2D/3D variants).
/// \param 3] Parameter.
/// \param 4] Parameter.
/// \param 3] Parameter.
/// \note Column-major convention: \c res = lhs * rhs.
/// \note Uses homogeneous coordinates with w=1.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4x3_mul_point(bj_real r[restrict 3],
                                       const bj_vec3 m[restrict 4],
                                       const bj_real p[restrict 3]) {
#ifdef BJ_MAT_NO_UNROLL
    const bj_real x=p[0], y=p[1], z=p[2];
    r[0] = m[0][0]*x + m[1][0]*y + m[2][0]*z + m[3][0];
    r[1] = m[0][1]*x + m[1][1]*y + m[2][1]*z + m[3][1];
    r[2] = m[0][2]*x + m[1][2]*y + m[2][2]*z + m[3][2];
#else
    const bj_real x=p[0], y=p[1], z=p[2];
    r[0] = m[0][0]*x + m[1][0]*y + m[2][0]*z + m[3][0];
    r[1] = m[0][1]*x + m[1][1]*y + m[2][1]*z + m[3][1];
    r[2] = m[0][2]*x + m[1][2]*y + m[2][2]*z + m[3][2];
#endif
}

////////////////////////////////////////////////////////////////////////////////
/// Transform a direction (homogeneous w=0; ignores translation).
/// \param 3] Parameter.
/// \param 4] Parameter.
/// \param 3] Parameter.
/// \note Column-major convention: \c res = lhs * rhs.
/// \note Uses homogeneous coordinates with w=0 (no translation).
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4x3_mul_dir(bj_real r[restrict 3],
                                     const bj_vec3 m[restrict 4],
                                     const bj_real v[restrict 3]) {
#ifdef BJ_MAT_NO_UNROLL
    const bj_real x=v[0], y=v[1], z=v[2];
    r[0] = m[0][0]*x + m[1][0]*y + m[2][0]*z;
    r[1] = m[0][1]*x + m[1][1]*y + m[2][1]*z;
    r[2] = m[0][2]*x + m[1][2]*y + m[2][2]*z;
#else
    const bj_real x=v[0], y=v[1], z=v[2];
    r[0] = m[0][0]*x + m[1][0]*y + m[2][0]*z;
    r[1] = m[0][1]*x + m[1][1]*y + m[2][1]*z;
    r[2] = m[0][2]*x + m[1][2]*y + m[2][2]*z;
#endif
}

////////////////////////////////////////////////////////////////////////////////
/// Promote 3D affine (4×3) to 4×4 homogeneous.
/// \param out Output matrix.
/// \param 4] Parameter.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4_from_mat4x3(bj_mat4x4 out, const bj_vec3 a[restrict 4]) {
    out[0][0]=a[0][0]; out[0][1]=a[0][1]; out[0][2]=a[0][2]; out[0][3]=BJ_F(0.0);
    out[1][0]=a[1][0]; out[1][1]=a[1][1]; out[1][2]=a[1][2]; out[1][3]=BJ_F(0.0);
    out[2][0]=a[2][0]; out[2][1]=a[2][1]; out[2][2]=a[2][2]; out[2][3]=BJ_F(0.0);
    out[3][0]=a[3][0]; out[3][1]=a[3][1]; out[3][2]=a[3][2]; out[3][3]=BJ_F(1.0);
}

////////////////////////////////////////////////////////////////////////////////
/// Extract upper 4×3 affine block from 4×4.
/// \param 4] Parameter.
/// \param m Output buffer.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4x3_from_mat4(bj_vec3 out43[restrict 4], const bj_mat4x4 m) {
    out43[0][0]=m[0][0]; out43[0][1]=m[0][1]; out43[0][2]=m[0][2];
    out43[1][0]=m[1][0]; out43[1][1]=m[1][1]; out43[1][2]=m[1][2];
    out43[2][0]=m[2][0]; out43[2][1]=m[2][1]; out43[2][2]=m[2][2];
    out43[3][0]=m[3][0]; out43[3][1]=m[3][1]; out43[3][2]=m[3][2];
}

////////////////////////////////////////////////////////////////////////////////
/// Look-at view matrix builder.
/// \param 4] Parameter.
/// \param 3] Parameter.
/// \param 3] Parameter.
/// \param 3] Parameter.
////////////////////////////////////////////////////////////////////////////////
static inline void bj_mat4x3_lookat(bj_vec3 m[restrict 4],
                                    const bj_real eye[restrict 3],
                                    const bj_real center[restrict 3],
                                    const bj_real up[restrict 3])
{
    bj_real f[3] = { center[0]-eye[0], center[1]-eye[1], center[2]-eye[2] };
    // normalize f
    {
        const bj_real len2 = f[0]*f[0]+f[1]*f[1]+f[2]*f[2];
        const bj_real inv  = BJ_F(1.0)/bj_sqrt(len2);
        f[0]*=inv; f[1]*=inv; f[2]*=inv;
    }
    // s = normalize(up × f)
    bj_real s[3] = {
        up[1]*f[2] - up[2]*f[1],
        up[2]*f[0] - up[0]*f[2],
        up[0]*f[1] - up[1]*f[0]
    };
    {
        const bj_real len2 = s[0]*s[0]+s[1]*s[1]+s[2]*s[2];
        const bj_real inv  = BJ_F(1.0)/bj_sqrt(len2);
        s[0]*=inv; s[1]*=inv; s[2]*=inv;
    }
    // t = f × s
    const bj_real t[3] = {
        f[1]*s[2] - f[2]*s[1],
        f[2]*s[0] - f[0]*s[2],
        f[0]*s[1] - f[1]*s[0]
    };

    // columns {s, t, f}
    m[0][0]=s[0]; m[0][1]=s[1]; m[0][2]=s[2];
    m[1][0]=t[0]; m[1][1]=t[1]; m[1][2]=t[2];
    m[2][0]=f[0]; m[2][1]=f[1]; m[2][2]=f[2];

    // translation = -eye expressed in this basis
    m[3][0]=-(s[0]*eye[0] + s[1]*eye[1] + s[2]*eye[2]);
    m[3][1]=-(t[0]*eye[0] + t[1]*eye[1] + t[2]*eye[2]);
    m[3][2]=-(f[0]*eye[0] + f[1]*eye[1] + f[2]*eye[2]);
}


#endif

/// \}

