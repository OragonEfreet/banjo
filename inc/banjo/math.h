#pragma once

#include <math.h>

#define bj_real_t float
#define real_sqrt sqrtf
#define real_acos acosf

#define BJ_INLINE static inline

typedef bj_real_t bj_vec2[2];
typedef bj_real_t bj_vec3[3];
typedef bj_real_t bj_vec4[4];
typedef bj_vec4 bj_mat4[4];
typedef bj_real_t bj_quat[4];

////// VEC2 //////

BJ_INLINE void bj_vec2_set(bj_vec2 res, bj_real_t a, bj_real_t b) {
    res[0] = a; res[1] = b;
}

BJ_INLINE void bj_vec2_apply(bj_vec2 res, const bj_vec2 a, bj_real_t(*f)(bj_real_t)) {
    res[0] = f(a[0]);
    res[1] = f(a[1]);
}

BJ_INLINE void bj_vec2_add(bj_vec2 res, const bj_vec2 lhs, const bj_vec2 rhs) {
    res[0] = lhs[0] + rhs[0];
    res[1] = lhs[1] + rhs[1];
}

BJ_INLINE void bj_vec2_sub(bj_vec2 res, const bj_vec2 lhs, const bj_vec2 rhs) {
    res[0] = lhs[0] - rhs[0];
    res[1] = lhs[1] - rhs[1];
}

BJ_INLINE void bj_vec2_scale(bj_vec2 res, const bj_vec2 v, bj_real_t s) {
    res[0] = v[0] * s;
    res[1] = v[1] * s;
}

BJ_INLINE void bj_vec2_scale_each(bj_vec2 res, const bj_vec2 v, const bj_vec2 s) {
    res[0] = v[0] * s[0];
    res[1] = v[1] * s[1];
}

BJ_INLINE bj_real_t bj_vec2_dot(const bj_vec2 a, const bj_vec2 b) {
    return a[0] * b[0] + a[1] * b[1];
}

BJ_INLINE bj_real_t bj_vec2_len(const bj_vec2 v) {
    return real_sqrt(v[0] * v[0] + v[1] * v[1]);
}

BJ_INLINE void bj_vec2_normalize(bj_vec2 res, const bj_vec2 v) {
    bj_real_t inv_len = 1.0f / real_sqrt(v[0] * v[0] + v[1] * v[1]);
    res[0] = v[0] * inv_len;
    res[1] = v[1] * inv_len;
}

BJ_INLINE void bj_vec2_min(bj_vec2 res, const bj_vec2 a, const bj_vec2 b) {
    res[0] = a[0] < b[0] ? a[0] : b[0];
    res[1] = a[1] < b[1] ? a[1] : b[1];
}

BJ_INLINE void bj_vec2_max(bj_vec2 res, const bj_vec2 a, const bj_vec2 b) {
    res[0] = a[0] > b[0] ? a[0] : b[0];
    res[1] = a[1] > b[1] ? a[1] : b[1];
}

BJ_INLINE void bj_vec2_copy(bj_vec2 res, const bj_vec2 src) {
    res[0] = src[0];
    res[1] = src[1];
}

////// VEC3 //////

BJ_INLINE void bj_vec3_set(bj_vec2 res, bj_real_t a, bj_real_t b, bj_real_t c) {
    res[0] = a; res[1] = b; res[2] = c;
}

BJ_INLINE void bj_vec3_apply(bj_vec3 res, const bj_vec3 a, bj_real_t(*f)(bj_real_t)) {
    res[0] = f(a[0]);
    res[1] = f(a[1]);
    res[2] = f(a[2]);
}


BJ_INLINE void bj_vec3_add(bj_vec3 res, const bj_vec3 lhs, const bj_vec3 rhs) {
    res[0] = lhs[0] + rhs[0];
    res[1] = lhs[1] + rhs[1];
    res[2] = lhs[2] + rhs[2];
}

BJ_INLINE void bj_vec3_sub(bj_vec3 res, const bj_vec3 lhs, const bj_vec3 rhs) {
    res[0] = lhs[0] - rhs[0];
    res[1] = lhs[1] - rhs[1];
    res[2] = lhs[2] - rhs[2];
}

BJ_INLINE void bj_vec3_scale(bj_vec3 res, const bj_vec3 v, bj_real_t s) {
    res[0] = v[0] * s;
    res[1] = v[1] * s;
    res[2] = v[2] * s;
}

BJ_INLINE bj_real_t bj_vec3_dot(const bj_vec3 a, const bj_vec3 b) {
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

BJ_INLINE bj_real_t bj_vec3_len(const bj_vec3 v) {
    return real_sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

BJ_INLINE void bj_vec3_normalize(bj_vec3 res, const bj_vec3 v) {
    bj_real_t inv_len = 1.0f / real_sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    res[0] = v[0] * inv_len;
    res[1] = v[1] * inv_len;
    res[2] = v[2] * inv_len;
}

BJ_INLINE void bj_vec3_min(bj_vec3 res, const bj_vec3 a, const bj_vec3 b) {
    res[0] = a[0] < b[0] ? a[0] : b[0];
    res[1] = a[1] < b[1] ? a[1] : b[1];
    res[2] = a[2] < b[2] ? a[2] : b[2];
}

BJ_INLINE void bj_vec3_max(bj_vec3 res, const bj_vec3 a, const bj_vec3 b) {
    res[0] = a[0] > b[0] ? a[0] : b[0];
    res[1] = a[1] > b[1] ? a[1] : b[1];
    res[2] = a[2] > b[2] ? a[2] : b[2];
}

BJ_INLINE void bj_vec3_copy(bj_vec3 res, const bj_vec3 src) {
    res[0] = src[0];
    res[1] = src[1];
    res[2] = src[2];
}

////// VEC4 //////

BJ_INLINE void bj_vec4_set(bj_vec2 res, bj_real_t a, bj_real_t b, bj_real_t c, bj_real_t d) {
    res[0] = a; res[1] = b; res[2] = c; res[3] = d;
}

BJ_INLINE void bj_vec4_apply(bj_vec4 res, const bj_vec4 a, bj_real_t(*f)(bj_real_t)) {
    res[0] = f(a[0]);
    res[1] = f(a[1]);
    res[2] = f(a[2]);
    res[3] = f(a[4]);
}

BJ_INLINE void bj_vec4_add(bj_vec4 res, const bj_vec4 lhs, const bj_vec4 rhs) {
    res[0] = lhs[0] + rhs[0];
    res[1] = lhs[1] + rhs[1];
    res[2] = lhs[2] + rhs[2];
    res[3] = lhs[3] + rhs[3];
}

BJ_INLINE void bj_vec4_sub(bj_vec4 res, const bj_vec4 lhs, const bj_vec4 rhs) {
    res[0] = lhs[0] - rhs[0];
    res[1] = lhs[1] - rhs[1];
    res[2] = lhs[2] - rhs[2];
    res[3] = lhs[3] - rhs[3];
}

BJ_INLINE void bj_vec4_scale(bj_vec4 res, const bj_vec4 v, bj_real_t s) {
    res[0] = v[0] * s;
    res[1] = v[1] * s;
    res[2] = v[2] * s;
    res[3] = v[3] * s;
}

BJ_INLINE bj_real_t bj_vec4_dot(const bj_vec4 a, const bj_vec4 b) {
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3];
}

BJ_INLINE bj_real_t bj_vec4_len(const bj_vec4 v) {
    return real_sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3]);
}

BJ_INLINE void bj_vec4_normalize(bj_vec4 res, const bj_vec4 v) {
    bj_real_t inv_len = 1.0f / real_sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3]);
    res[0] = v[0] * inv_len;
    res[1] = v[1] * inv_len;
    res[2] = v[2] * inv_len;
    res[3] = v[3] * inv_len;
}

BJ_INLINE void bj_vec4_min(bj_vec4 res, const bj_vec4 a, const bj_vec4 b) {
    res[0] = a[0] < b[0] ? a[0] : b[0];
    res[1] = a[1] < b[1] ? a[1] : b[1];
    res[2] = a[2] < b[2] ? a[2] : b[2];
    res[3] = a[3] < b[3] ? a[3] : b[3];
}

BJ_INLINE void bj_vec4_max(bj_vec4 res, const bj_vec4 a, const bj_vec4 b) {
    res[0] = a[0] > b[0] ? a[0] : b[0];
    res[1] = a[1] > b[1] ? a[1] : b[1];
    res[2] = a[2] > b[2] ? a[2] : b[2];
    res[3] = a[3] > b[3] ? a[3] : b[3];
}

BJ_INLINE void bj_vec4_copy(bj_vec4 res, const bj_vec4 src) {
    res[0] = src[0];
    res[1] = src[1];
    res[2] = src[2];
    res[3] = src[3];
}

BJ_INLINE void bj_vec3_cross(bj_vec3 res, const bj_vec3 l, const bj_vec3 r)
{
    res[0] = l[1] * r[2] - l[2] * r[1];
    res[1] = l[2] * r[0] - l[0] * r[2];
    res[2] = l[0] * r[1] - l[1] * r[0];
}

BJ_INLINE void bj_vec3_reflect(bj_vec3 res, const bj_vec3 v, const bj_vec3 n)
{
    const bj_real_t p = 2.f * bj_vec3_dot(v, n);
    for (int i = 0; i < 3; ++i) {
        res[i] = v[i] - p * n[i];
    }
}

BJ_INLINE void bj_vec4_cross(bj_vec4 res, const bj_vec4 l, const bj_vec4 r)
{
    res[0] = l[1] * r[2] - l[2] * r[1];
    res[1] = l[2] * r[0] - l[0] * r[2];
    res[2] = l[0] * r[1] - l[1] * r[0];
    res[3] = 1.f;
}

BJ_INLINE void bj_vec4_reflect(bj_vec4 res, const bj_vec4 v, const bj_vec4 n)
{
    bj_real_t p = 2.f * bj_vec4_dot(v, n);
    for (int i = 0; i < 4; ++i) {
        res[i] = v[i] - p * n[i];
    }
}


BJ_INLINE void bj_mat4_identity(bj_mat4 mat)
{
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            mat[i][j] = i == j ? 1.f : 0.f;
        }
    }
}

BJ_INLINE void bj_mat4_copy(bj_mat4 to, const bj_mat4 from)
{
    for (int i = 0; i < 4; ++i) {
        bj_vec4_copy(to[i], from[i]);
    }
}

BJ_INLINE void bj_mat4_row(bj_vec4 res, const bj_mat4 mat, int r)
{
    for (int k = 0; k < 4; ++k) {
        res[k] = mat[k][r];
    }
}

BJ_INLINE void bj_mat4_col(bj_vec4 res, const bj_mat4 mat, int c)
{
    for (int k = 0; k < 4; ++k) {
        res[k] = mat[c][k];
    }
}

BJ_INLINE void bj_mat4_transpose(bj_mat4 res, const bj_mat4 mat)
{
    for (int j = 0; j < 4; ++j) {
        for (int i = 0; i < 4; ++i) {
            res[i][j] = mat[j][i];
        }
    }
}

BJ_INLINE void bj_mat4_add(bj_mat4 res, const bj_mat4 lhs, const bj_mat4 rhs)
{
    for (int i = 0; i < 4; ++i) {
        bj_vec4_add(res[i], lhs[i], rhs[i]);
    }
}

BJ_INLINE void bj_mat4_sub(bj_mat4 res, const bj_mat4 lhs, const bj_mat4 rhs)
{
    for (int i = 0; i < 4; ++i) {
        bj_vec4_sub(res[i], lhs[i], rhs[i]);
    }
}

BJ_INLINE void bj_mat4_scale(bj_mat4 res, const bj_mat4 lhs, bj_real_t k)
{
    for (int i = 0; i < 4; ++i) {
        bj_vec4_scale(res[i], lhs[i], k);
    }
}

BJ_INLINE void bj_mat4_scale_xyz(bj_mat4 res, const bj_mat4 mat, bj_real_t x, bj_real_t y, bj_real_t z)
{
    bj_vec4_scale(res[0], mat[0], x);
    bj_vec4_scale(res[1], mat[1], y);
    bj_vec4_scale(res[2], mat[2], z);
    bj_vec4_copy(res[3], mat[3]);
}

BJ_INLINE void bj_mat4_mul(bj_mat4 res, const bj_mat4 lhs, const bj_mat4 rhs)
{
    bj_mat4 temp;;
    for (int c = 0; c < 4; ++c) {
        for (int r = 0; r < 4; ++r) {
            temp[c][r] = 0.f;
            for (int k = 0; k < 4; ++k)
                temp[c][r] += lhs[k][r] * rhs[c][k];
        }
    }
    bj_mat4_copy(res, temp);
}

BJ_INLINE void bj_mat4_mul_vec4(bj_vec4 res, const bj_mat4 mat, const bj_vec4 v)
{
    for (int j = 0; j < 4; ++j) {
        res[j] = 0.f;
        for (int i = 0; i < 4; ++i) {
            res[j] += mat[i][j] * v[i];
        }
    }
}

BJ_INLINE void bj_mat4_translation(bj_mat4 res, bj_real_t x, bj_real_t y, bj_real_t z)
{
    bj_mat4_identity(res);
    res[3][0] = x;
    res[3][1] = y;
    res[3][2] = z;
}

BJ_INLINE void bj_mat4_translation_inplace(bj_mat4 M, bj_real_t x, bj_real_t y, bj_real_t z)
{
    bj_vec4 t = { x, y, z, 0 };
    bj_vec4 r;
    for (int i = 0; i < 4; ++i) {
        bj_mat4_row(r, M, i);
        M[3][i] += bj_vec4_dot(r, t);
    }
}

BJ_INLINE void bj_mat4_mul_outer(bj_mat4 res, const bj_vec3 a, const bj_vec3 b)
{
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            res[i][j] = i < 3 && j < 3 ? a[i] * b[j] : 0.f;
        }
    }
}

BJ_INLINE void bj_mat4_rotate(bj_mat4 res, const bj_mat4 mat, bj_real_t x, bj_real_t y, bj_real_t z, bj_real_t angle)
{
    bj_real_t s = sinf(angle);
    bj_real_t c = cosf(angle);
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

BJ_INLINE void bj_ma4_rotate_x(bj_mat4 res, const bj_mat4 mat, bj_real_t angle)
{
    bj_real_t s = sinf(angle);
    bj_real_t c = cosf(angle);
    bj_mat4 R = {
        {1.f, 0.f, 0.f, 0.f},
        {0.f,   c,   s, 0.f},
        {0.f,  -s,   c, 0.f},
        {0.f, 0.f, 0.f, 1.f}
    };
    bj_mat4_mul(res, mat, R);
}

BJ_INLINE void bj_ma4_rotate_y(bj_mat4 res, const bj_mat4 mat, bj_real_t angle)
{
    bj_real_t s = sinf(angle);
    bj_real_t c = cosf(angle);
    bj_mat4 R = {
        {   c, 0.f,  -s, 0.f},
        { 0.f, 1.f, 0.f, 0.f},
        {   s, 0.f,   c, 0.f},
        { 0.f, 0.f, 0.f, 1.f}
    };
    bj_mat4_mul(res, mat, R);
}

BJ_INLINE void bj_ma4_rotate_z(bj_mat4 res, const bj_mat4 mat, bj_real_t angle)
{
    bj_real_t s = sinf(angle);
    bj_real_t c = cosf(angle);
    bj_mat4 R = {
        {   c,   s, 0.f, 0.f},
        {  -s,   c, 0.f, 0.f},
        { 0.f, 0.f, 1.f, 0.f},
        { 0.f, 0.f, 0.f, 1.f}
    };
    bj_mat4_mul(res, mat, R);
}


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

BJ_INLINE void bj_mat4_inverse(bj_mat4 res, const bj_mat4 mat)
{
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

BJ_INLINE void bj_mat4_orthonormalize(bj_mat4 res, const bj_mat4 mat)
{
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

BJ_INLINE void bj_mat4_frustum(bj_mat4 fmat, bj_real_t l, bj_real_t r, bj_real_t b, bj_real_t t, bj_real_t n, bj_real_t f)
{
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

BJ_INLINE void bj_mat4_ortho(bj_mat4 omat, bj_real_t l, bj_real_t r, bj_real_t b, bj_real_t t, bj_real_t n, bj_real_t f)
{
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

BJ_INLINE void bj_mat4_perspective(bj_mat4 pmat, bj_real_t y_fov, bj_real_t aspect, bj_real_t n, bj_real_t f)
{
    const bj_real_t a = 1.f / tanf(y_fov / 2.f);
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

BJ_INLINE void bj_mat4_lookat(bj_mat4 m, const bj_vec3 eye, const bj_vec3 center, const bj_vec3 up)
{
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

#define bj_quat_add bj_vec4_add
#define bj_quat_sub bj_vec4_sub
#define bj_quat_norm bj_vec4_normalize
#define bj_quat_scale bj_vec4_scale
#define bj_quat_dot bj_vec4_dot

BJ_INLINE void bj_quat_identity(bj_quat q)
{
    q[0] = q[1] = q[2] = 0.f;
    q[3] = 1.f;
}

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

BJ_INLINE void bj_quat_conjugate(bj_quat res, const bj_quat q)
{
    for (int i = 0; i < 3; ++i) {
        res[i] = -q[i];
    }
    res[3] = q[3];
}

BJ_INLINE void bj_quat_rotation(bj_quat res, bj_real_t angle, const bj_vec3 axis) {
    bj_vec3 axis_norm;
    bj_vec3_normalize(axis_norm, axis);
    const bj_real_t s = sinf(angle / 2);
    const bj_real_t c = cosf(angle / 2);
    bj_vec3_scale(res, axis_norm, s);
    res[3] = c;
}

BJ_INLINE void bj_quat_mul_vec3(bj_vec3 res, const bj_quat q, const bj_vec3 v)
{
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

BJ_INLINE void bj_mat4_from_quat(bj_mat4 res, const bj_quat q)
{
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

BJ_INLINE void bj_mat4_rotate_from_quat(bj_mat4 R, const bj_mat4 M, const bj_quat q)
{
    bj_quat_mul_vec3(R[0], q, M[0]);
    bj_quat_mul_vec3(R[1], q, M[1]);
    bj_quat_mul_vec3(R[2], q, M[2]);

    R[3][0] = R[3][1] = R[3][2] = 0.f;
    R[0][3] = M[0][3];
    R[1][3] = M[1][3];
    R[2][3] = M[2][3];
    R[3][3] = M[3][3];
}

BJ_INLINE void bj_quat_from_mat4(bj_quat q, const bj_mat4 M)
{
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
