#include <banjo/geometry_2d.h>

BANJO_EXPORT bj_bool bj_check_segments_hit(
    bj_vec2 a0,
    bj_vec2 a1,
    bj_vec2 b0,
    bj_vec2 b1
) {
    bj_vec2 r, s, qp;
    bj_vec2_sub(&r, a1, a0);
    bj_vec2_sub(&s, b1, b0);
    bj_vec2_sub(&qp, b0, a0);

    const bj_real rxs   = bj_vec2_cross(r, s);
    const bj_real qpxr  = bj_vec2_cross(qp, r);

    if (bj_real_is_zero(rxs) && bj_real_is_zero(qpxr)) {
        const bj_real rr = bj_vec2_dot(r, r);
        if (bj_real_is_zero(rr)) {
            bj_vec2 ab0; bj_vec2_sub(&ab0, a0, b0);
            bj_real ss = bj_vec2_dot(s, s);
            if (bj_real_is_zero(ss)) {
                return bj_vec2_dot(ab0, ab0) <= BJ_EPSILON*BJ_EPSILON;
            }
            bj_real t = bj_vec2_dot(ab0, s) / ss;
            return bj_real_lte(BJ_F(0.0), t) && bj_real_lte(t, BJ_F(1.0));
        }

        const bj_real t0 = bj_vec2_dot(qp, r) / rr;
        const bj_real t1 = t0 + bj_vec2_dot(s, r) / rr;
        const bj_real lo = (t0 < t1) ? t0 : t1;
        const bj_real hi = (t0 < t1) ? t1 : t0;
        return bj_real_lte(BJ_F(0.0), hi) && bj_real_lte(lo, BJ_F(1.0)); }

    if (bj_real_is_zero(rxs)) return 0;

    const bj_real inv = BJ_F(1.0) / rxs;
    const bj_real t = bj_vec2_cross(qp, s) * inv;
    const bj_real u = qpxr * inv;

    return bj_real_lte(BJ_F(0.0), t) && bj_real_lte(t, BJ_F(1.0)) &&
           bj_real_lte(BJ_F(0.0), u) && bj_real_lte(u, BJ_F(1.0));
}

bj_bool bj_check_circle_segment_hit(
    bj_vec2 c,
    bj_real r,
    bj_vec2 l0,
    bj_vec2 l1
) {

    bj_vec2 d;  bj_vec2_sub(&d, l1, l0);
    bj_real dd = bj_vec2_dot(d, d);
    if (dd <= BJ_EPSILON) { // segment dégénéré -> point
        bj_vec2 v; bj_vec2_sub(&v, l0, c);
        return bj_vec2_dot(v, v) <= r*r;
    }

    // t = clamp( dot(c-l0, d) / dot(d,d), 0, 1 )
    bj_vec2 m;  bj_vec2_sub(&m, c, l0);
    bj_real t = bj_vec2_dot(m, d) / dd;
         if (t < BJ_F(0.0)) t = BJ_F(0.0);
    else if (t > BJ_F(1.0)) t = BJ_F(1.0);

    // point le plus proche sur le segment
    bj_vec2 p;  bj_vec2_add_scaled(&p, l0, d, t);

    // collision si la distance au centre ≤ r
    bj_vec2 pc; bj_vec2_sub(&pc, p, c);
    bj_real dist2 = bj_vec2_dot(pc, pc);

    // petite tolérance numérique optionnelle
    bj_real tol = BJ_F(1e-6);
    bj_real r2  = (r + tol)*(r + tol);

    return dist2 <= r2;
}



