#include "bitmap_t.h"
#include "check.h"

// Note: This file is AI-generated.
// TODO: I'll remake it myself later on.

/* ---------- Public: color key ---------- */
void bj_set_bitmap_colorkey(bj_bitmap* bmp, bj_bool enabled, uint32_t key_value) {
    bj_check(bmp);
    bmp->colorkey_enabled = enabled;
    bmp->colorkey = key_value;
}

/* ---------- Bit helpers for sub-byte formats (1/4/8bpp) ---------- */
/* Read-modify-write into arbitrary bit offset; optimized enough for rare paths. */
static inline uint32_t mask_for_bpp(size_t bpp) {
    return (bpp >= 32) ? 0xFFFFFFFFu : ((1u << bpp) - 1u);
}

static inline uint32_t buffer_get_pixel_bits(
    size_t x, size_t y, size_t stride, const void* buffer, size_t bpp)
{
    const size_t bit_offset  = y * stride * 8u + x * bpp;
    const size_t byte_offset = bit_offset >> 3;
    const size_t bit_in_byte = bit_offset & 7u;
    const size_t span_bytes  = (bit_in_byte + bpp + 7u) >> 3;
    uint64_t window = 0;
    bj_memcpy(&window, (const uint8_t*)buffer + byte_offset, span_bytes);
    window >>= bit_in_byte;
    return (uint32_t)(window & mask_for_bpp(bpp));
}

static inline void buffer_set_pixel_bits(
    size_t x, size_t y, size_t stride, void* buffer, uint32_t value, size_t bpp)
{
    const size_t bit_offset  = y * stride * 8u + x * bpp;
    const size_t byte_offset = bit_offset >> 3;
    const size_t bit_in_byte = bit_offset & 7u;
    const size_t span_bytes  = (bit_in_byte + bpp + 7u) >> 3;

    const uint64_t field_mask = (uint64_t)mask_for_bpp(bpp) << bit_in_byte;

    uint64_t window = 0;
    bj_memcpy(&window, (const uint8_t*)buffer + byte_offset, span_bytes);
    window = (window & ~field_mask) | (((uint64_t)(value & mask_for_bpp(bpp)) << bit_in_byte) & field_mask);
    bj_memcpy((uint8_t*)buffer + byte_offset, &window, span_bytes);
}

/* ---------- Canonical RGB (8:8:8) converters via bj_pixel_* ---------- */

static inline void unpack_rgb_from_native(bj_pixel_mode mode, uint32_t native, uint8_t* r, uint8_t* g, uint8_t* b) {
    bj_make_pixel_rgb(mode, native, r, g, b);
}

static inline uint32_t pack_rgb_to_native(bj_pixel_mode mode, uint8_t r, uint8_t g, uint8_t b) {
    return bj_get_pixel_value(mode, r, g, b);
}

/* Optional: you can provide direct fast paths for known modes:
   e.g., RGB565/XRGB8888/BGR24 to/from RGB components for extra speed. */

/* ---------- ROPs on packed values ---------- */

static inline uint32_t rop_apply_u32(uint32_t dst, uint32_t src, bj_blit_op op) {
    switch (op) {
        case BJ_BLIT_OP_COPY:    return src;
        case BJ_BLIT_OP_XOR:     return dst ^ src;
        case BJ_BLIT_OP_OR:      return dst | src;
        case BJ_BLIT_OP_AND:     return dst & src;
        case BJ_BLIT_OP_ADD_SAT: {
            /* per-channel saturating add on 8:8:8 (ignore X) */
            uint32_t rb = (dst & 0x00FF00FFu) + (src & 0x00FF00FFu);
            uint32_t g  = ((dst & 0x0000FF00u) >> 8) + ((src & 0x0000FF00u) >> 8);
            rb = (rb & 0x01000100u) ? 0x00FF00FFu : (rb & 0x00FF00FFu);
            g  = (g  & 0x00000100u) ? 0x000000FFu : (g  & 0x000000FFu);
            return (rb & 0x00FF00FFu) | ((g & 0xFFu) << 8);
        }
        case BJ_BLIT_OP_SUB_SAT: {
            uint32_t rb = (dst & 0x00FF00FFu) - (src & 0x00FF00FFu);
            uint32_t g  = ((dst & 0x0000FF00u) >> 8) - ((src & 0x0000FF00u) >> 8);
            rb = (rb & 0x01000100u) ? 0u : (rb & 0x00FF00FFu);
            g  = (g  & 0x00000100u) ? 0u : (g  & 0x000000FFu);
            return (rb & 0x00FF00FFu) | ((g & 0xFFu) << 8);
        }
        default: return src;
    }
}

/* For 16bpp (RGB565/XRGB1555), XOR/OR/AND are native-OK; ADD/SUB need per-channel unpack.
   We’ll do simple bitwise for those ops where possible. */

/* ---------- Fast row kernels (same-format) ---------- */

static inline bj_bool same_format_fastcopy_possible(const bj_bitmap* s, const bj_bitmap* d, bj_blit_op op, bj_bool use_key) {
    return (s->mode == d->mode) && (op == BJ_BLIT_OP_COPY) && !use_key;
}

static inline bj_bool is_32bpp(bj_pixel_mode m) { return BJ_PIXEL_GET_BPP(m) == 32; }
static inline bj_bool is_24bpp(bj_pixel_mode m) { return BJ_PIXEL_GET_BPP(m) == 24; }
static inline bj_bool is_16bpp(bj_pixel_mode m) { return BJ_PIXEL_GET_BPP(m) == 16; }
static inline bj_bool is_subbyte(bj_pixel_mode m){ size_t b=BJ_PIXEL_GET_BPP(m); return (b==1||b==4||b==8); }

/* bj_memcpy/bj_memmove whole rows (direction-aware for overlap) */
static void blit_rows_mem(const uint8_t* s, size_t s_stride, uint8_t* d, size_t d_stride, size_t rowbytes, size_t rows, bj_bool overlap) {
    if (!overlap || d < s) {
        for (size_t y=0; y<rows; ++y) bj_memcpy(d + y*d_stride, s + y*s_stride, rowbytes);
    } else {
        /* bottom-up to handle overlap */
        for (size_t y=rows; y>0; --y) bj_memmove(d + (y-1)*d_stride, s + (y-1)*s_stride, rowbytes);
    }
}

/* 32bpp same-mode ROP (XOR/OR/AND); colorkey optional */
static void blit_row_32_rop(const uint32_t* src, uint32_t* dst, size_t pixels, bj_bool use_key, uint32_t key, bj_blit_op op) {
    if (!use_key) {
        if (op == BJ_BLIT_OP_COPY) {
            bj_memcpy(dst, src, pixels * 4u);
            return;
        }
        if (op == BJ_BLIT_OP_XOR) { for (size_t i=0;i<pixels;++i) dst[i]^=src[i]; return; }
        if (op == BJ_BLIT_OP_OR)  { for (size_t i=0;i<pixels;++i) dst[i]|=src[i]; return; }
        if (op == BJ_BLIT_OP_AND) { for (size_t i=0;i<pixels;++i) dst[i]&=src[i]; return; }
    }
    /* general 32bpp path with key and extended ops */
    for (size_t i=0;i<pixels;++i) {
        uint32_t s = src[i];
        if (use_key && s == key) continue;
        if (op == BJ_BLIT_OP_COPY) { dst[i] = s; }
        else { dst[i] = rop_apply_u32(dst[i], s, op); }
    }
}

/* 16bpp same-mode fast bitwise (XOR/OR/AND); COPY → bj_memcpy; key compare in native */
static void blit_row_16_fast(const uint16_t* src, uint16_t* dst, size_t pixels, bj_bool use_key, uint16_t key, bj_blit_op op) {
    if (!use_key) {
        if (op == BJ_BLIT_OP_COPY) { bj_memcpy(dst, src, pixels * 2u); return; }
        if (op == BJ_BLIT_OP_XOR)  { for (size_t i=0;i<pixels;++i) dst[i]^=src[i]; return; }
        if (op == BJ_BLIT_OP_OR)   { for (size_t i=0;i<pixels;++i) dst[i]|=src[i]; return; }
        if (op == BJ_BLIT_OP_AND)  { for (size_t i=0;i<pixels;++i) dst[i]&=src[i]; return; }
    }
    for (size_t i=0;i<pixels;++i) { uint16_t s=src[i]; if (use_key && s==key) continue; dst[i] = (op==BJ_BLIT_OP_COPY)? s : (uint16_t)rop_apply_u32(dst[i], s, op); }
}

/* 24bpp BGR same-mode; do it channel-wise; COPY can use bj_memcpy; key compare needs 3-byte compare */
static void blit_row_24_fast(const uint8_t* s, uint8_t* d, size_t pixels, bj_bool use_key, const uint8_t key[3], bj_blit_op op) {
    if (!use_key && op == BJ_BLIT_OP_COPY) { bj_memcpy(d, s, pixels*3u); return; }
    for (size_t i=0;i<pixels;++i) {
        const uint8_t sr = s[i*3+2], sg = s[i*3+1], sb = s[i*3+0];
        if (use_key && sb==key[0] && sg==key[1] && sr==key[2]) continue;
        if (op == BJ_BLIT_OP_COPY) { d[i*3+0]=sb; d[i*3+1]=sg; d[i*3+2]=sr; }
        else {
            uint8_t dr = d[i*3+2], dg = d[i*3+1], db = d[i*3+0];
            switch(op){
                case BJ_BLIT_OP_XOR: dr^=sr; dg^=sg; db^=sb; break;
                case BJ_BLIT_OP_OR:  dr|=sr; dg|=sg; db|=sb; break;
                case BJ_BLIT_OP_AND: dr&=sr; dg&=sg; db&=sb; break;
                case BJ_BLIT_OP_ADD_SAT:{
                    int r=dr+sr, g=dg+sg, b=db+sb;
                    d[i*3+2]=(uint8_t)(r>255?255:r);
                    d[i*3+1]=(uint8_t)(g>255?255:g);
                    d[i*3+0]=(uint8_t)(b>255?255:b);
                    continue;
                }
                case BJ_BLIT_OP_SUB_SAT:{
                    int r=dr-sr, g=dg-sg, b=db-sb;
                    d[i*3+2]=(uint8_t)(r<0?0:r);
                    d[i*3+1]=(uint8_t)(g<0?0:g);
                    d[i*3+0]=(uint8_t)(b<0?0:b);
                    continue;
                }
                default: break;
            }
            d[i*3+2]=dr; d[i*3+1]=dg; d[i*3+0]=db;
        }
    }
}

/* ---------- General per-pixel kernel (any format combo) ---------- */

static void blit_general_any(
    const bj_bitmap* s, const bj_rect* sr,
    bj_bitmap* d, const bj_rect* dr,
    bj_blit_op op)
{
    const size_t bpp_s = BJ_PIXEL_GET_BPP(s->mode);
    const size_t bpp_d = BJ_PIXEL_GET_BPP(d->mode);

    for (uint16_t r = 0; r < dr->h; ++r) {
        size_t sy = (size_t)sr->y + r;
        size_t dy = (size_t)dr->y + r;

        const uint8_t* srow = (const uint8_t*)s->buffer + sy * s->stride;
        uint8_t*       drow = (uint8_t*)d->buffer + dy * d->stride;

        for (uint16_t c = 0; c < dr->w; ++c) {
            size_t sx = (size_t)sr->x + c;
            size_t dx = (size_t)dr->x + c;

            uint32_t sval; /* source in native */
            if (is_subbyte(s->mode)) {
                sval = buffer_get_pixel_bits(sx, sy, s->stride, s->buffer, bpp_s);
            } else if (is_16bpp(s->mode)) {
                sval = *(const uint16_t*)(srow + (sx * 2u));
            } else if (is_24bpp(s->mode)) {
                const uint8_t* sp = srow + (sx * 3u);
                sval = (uint32_t)sp[0] | ((uint32_t)sp[1] << 8) | ((uint32_t)sp[2] << 16);
            } else { /* 32bpp */
                sval = *(const uint32_t*)(srow + (sx * 4u));
            }

            if (s->colorkey_enabled && sval == s->colorkey) continue;

            if (d->mode == s->mode) {
                /* Same-mode, any op */
                if (is_subbyte(d->mode)) {
                    uint32_t dval = buffer_get_pixel_bits(dx, dy, d->stride, d->buffer, bpp_d);
                    uint32_t res = (op==BJ_BLIT_OP_COPY)? sval : rop_apply_u32(dval, sval, op);
                    buffer_set_pixel_bits(dx, dy, d->stride, d->buffer, res, bpp_d);
                } else if (is_16bpp(d->mode)) {
                    uint16_t* dp = (uint16_t*)(drow + dx * 2u);
                    if (op==BJ_BLIT_OP_COPY) *dp = (uint16_t)sval;
                    else *dp = (uint16_t)rop_apply_u32(*dp, (uint16_t)sval, op);
                } else if (is_24bpp(d->mode)) {
                    uint8_t* dp = drow + dx * 3u;
                    if (op==BJ_BLIT_OP_COPY) { dp[0]=(uint8_t)(sval); dp[1]=(uint8_t)(sval>>8); dp[2]=(uint8_t)(sval>>16); }
                    else {
                        uint8_t br=dp[2], bg=dp[1], bb=dp[0];
                        uint8_t sr=(uint8_t)(sval>>16), sg=(uint8_t)(sval>>8), sb=(uint8_t)sval;
                        switch(op){
                            case BJ_BLIT_OP_XOR: br^=sr; bg^=sg; bb^=sb; break;
                            case BJ_BLIT_OP_OR:  br|=sr; bg|=sg; bb|=sb; break;
                            case BJ_BLIT_OP_AND: br&=sr; bg&=sg; bb&=sb; break;
                            case BJ_BLIT_OP_ADD_SAT: {
                                int r=br+sr,g=bg+sg,b=bb+sb;
                                br=(uint8_t)(r>255?255:r);
                                bg=(uint8_t)(g>255?255:g);
                                bb=(uint8_t)(b>255?255:b);
                            } break;
                            case BJ_BLIT_OP_SUB_SAT: {
                                int r=br-sr,g=bg-sg,b=bb-sb;
                                br=(uint8_t)(r<0?0:r);
                                bg=(uint8_t)(g<0?0:g);
                                bb=(uint8_t)(b<0?0:b);
                            } break;
                            default: break;
                        }
                        dp[2]=br; dp[1]=bg; dp[0]=bb;
                    }
                } else { /* 32bpp */
                    uint32_t* dp = (uint32_t*)(drow + dx * 4u);
                    *dp = (op==BJ_BLIT_OP_COPY)? sval : rop_apply_u32(*dp, sval, op);
                }
                continue;
            }

            /* Different formats: convert via RGB components */
            uint8_t r8,g8,b8;
            unpack_rgb_from_native(s->mode, sval, &r8,&g8,&b8);

            /* current dst value for ROPs other than COPY */
            if (op != BJ_BLIT_OP_COPY) {
                uint32_t dval;
                if (is_subbyte(d->mode)) dval = buffer_get_pixel_bits(dx, dy, d->stride, d->buffer, bpp_d);
                else if (is_16bpp(d->mode)) dval = *(uint16_t*)(drow + dx*2u);
                else if (is_24bpp(d->mode)) {
                    uint8_t* dp = drow + dx*3u; dval = (uint32_t)dp[0] | ((uint32_t)dp[1]<<8) | ((uint32_t)dp[2]<<16);
                } else dval = *(uint32_t*)(drow + dx*4u);

                /* Apply op in RGB space by converting dval to rgb, applying op per channel, then pack */
                uint8_t dr,dg,db; unpack_rgb_from_native(d->mode, dval, &dr,&dg,&db);
                switch(op){
                    case BJ_BLIT_OP_XOR:    r8 ^= dr; g8 ^= dg; b8 ^= db; break;
                    case BJ_BLIT_OP_OR:     r8 |= dr; g8 |= dg; b8 |= db; break;
                    case BJ_BLIT_OP_AND:    r8 &= dr; g8 &= dg; b8 &= db; break;
                    case BJ_BLIT_OP_ADD_SAT:{ int R=r8+dr,G=g8+dg,B=b8+db; r8=(uint8_t)(R>255?255:R); g8=(uint8_t)(G>255?255:G); b8=(uint8_t)(B>255?255:B); } break;
                    case BJ_BLIT_OP_SUB_SAT:{ int R=dr-r8,G=dg-g8,B=db-b8; r8=(uint8_t)(R<0?0:R); g8=(uint8_t)(G<0?0:G); b8=(uint8_t)(B<0?0:B); } break;
                    default: break;
                }
            }

            const uint32_t out_native = pack_rgb_to_native(d->mode, r8,g8,b8);

            if (is_subbyte(d->mode)) {
                buffer_set_pixel_bits(dx, dy, d->stride, d->buffer, out_native, bpp_d);
            } else if (is_16bpp(d->mode)) {
                *(uint16_t*)(drow + dx*2u) = (uint16_t)out_native;
            } else if (is_24bpp(d->mode)) {
                uint8_t* dp = drow + dx*3u;
                dp[0]=(uint8_t)out_native; dp[1]=(uint8_t)(out_native>>8); dp[2]=(uint8_t)(out_native>>16);
            } else {
                *(uint32_t*)(drow + dx*4u) = out_native;
            }
        }
    }
}

/* ---------- Core clipped blit dispatcher (no scaling) ---------- */

static bj_bool do_blit_dispatch(
    const bj_bitmap* src, const bj_rect* sr,
    bj_bitmap* dst, const bj_rect* dr,
    bj_blit_op op)
{
    bj_check_or_0(src && dst && sr && dr);
    if (!sr->w || !sr->h || !dr->w || !dr->h) return BJ_FALSE;

    /* Same format fast paths */
    if (src->mode == dst->mode) {
        const size_t bpp = BJ_PIXEL_GET_BPP(src->mode);
        const size_t rowbytes = (size_t)dr->w * (bpp >> 3); /* only for 8+ bpp */
        const uint8_t* sbase = (const uint8_t*)src->buffer + sr->y*src->stride + (sr->x * (bpp>>3));
        uint8_t*       dbase = (uint8_t*)dst->buffer       + dr->y*dst->stride + (dr->x * (bpp>>3));

        const bj_bool overlap =
            (src == dst) &&
            !(dbase + dst->stride*dr->h <= sbase || sbase + src->stride*sr->h <= dbase);

        /* COPY, no key: bj_memcpy/bj_memmove */
        if (same_format_fastcopy_possible(src, dst, op, src->colorkey_enabled)) {
            if (bpp >= 8) { blit_rows_mem(sbase, src->stride, dbase, dst->stride, rowbytes, dr->h, overlap); return BJ_TRUE; }
            /* sub-byte falls through to general */
        }

        if (is_32bpp(src->mode)) {
            for (uint16_t y=0; y<dr->h; ++y) {
                const uint32_t* srow = (const uint32_t*)(sbase + y*src->stride);
                uint32_t*       drow = (uint32_t*)(dbase + y*dst->stride);
                blit_row_32_rop(srow, drow, dr->w, src->colorkey_enabled, src->colorkey, op);
            }
            return BJ_TRUE;
        } else if (is_16bpp(src->mode)) {
            uint16_t key16 = (uint16_t)src->colorkey;
            for (uint16_t y=0; y<dr->h; ++y) {
                const uint16_t* srow = (const uint16_t*)(sbase + y*src->stride);
                uint16_t*       drow = (uint16_t*)(dbase + y*dst->stride);
                blit_row_16_fast(srow, drow, dr->w, src->colorkey_enabled, key16, op);
            }
            return BJ_TRUE;
        } else if (is_24bpp(src->mode)) {
            uint8_t key24[3] = { (uint8_t)src->colorkey, (uint8_t)(src->colorkey>>8), (uint8_t)(src->colorkey>>16) };
            for (uint16_t y=0; y<dr->h; ++y) {
                const uint8_t* srow = sbase + y*src->stride;
                uint8_t*       drow = dbase + y*dst->stride;
                blit_row_24_fast(srow, drow, dr->w, src->colorkey_enabled, key24, op);
            }
            return BJ_TRUE;
        }
        /* sub-byte or exotic layouts → fall through */
    }

    /* General any→any path with converters (supports sub-byte, mismatched modes) */
    blit_general_any(src, sr, dst, dr, op);
    return BJ_TRUE;
}

/* ---------- Public: clipped blit (no scaling) using existing clipper ---------- */

bj_bool bj_blit(
    const bj_bitmap* p_src, const bj_rect* p_src_area,
    bj_bitmap* p_dst, const bj_rect* p_dst_area,
    bj_blit_op op)
{
    bj_check_or_0(p_src && p_dst);

    /* Build default rects & clip like current bj_blit */
    bj_rect src_rect = {0,0,(uint16_t)p_src->width,(uint16_t)p_src->height};
    bj_rect dst_rect = {0,0,0,0};

    if (p_dst_area) { dst_rect.x = p_dst_area->x; dst_rect.y = p_dst_area->y; }
    if (p_src_area) {
        bj_rect tmp;
        if (bj_rect_intersection(p_src_area, &src_rect, &tmp) == 0) return BJ_FALSE;
        dst_rect.x += tmp.x - p_src_area->x;
        dst_rect.y += tmp.y - p_src_area->y;
        src_rect = tmp;
    }
    dst_rect.w = src_rect.w; dst_rect.h = src_rect.h;

    bj_rect dst_bounds = (bj_rect){0,0,(uint16_t)p_dst->width,(uint16_t)p_dst->height};
    bj_rect inter;
    if (bj_rect_intersection(&dst_rect, &dst_bounds, &inter) == 0) return BJ_FALSE;

    /* Adjust source accordingly */
    src_rect.x += inter.x - dst_rect.x;
    src_rect.y += inter.y - dst_rect.y;
    src_rect.w  = inter.w;
    src_rect.h  = inter.h;
    dst_rect    = inter;

    if (!dst_rect.w || !dst_rect.h) return BJ_FALSE;

    return do_blit_dispatch(p_src, &src_rect, p_dst, &dst_rect, op);
}

/* ---------- Stretched blit (nearest) with same fast paths ---------- */

static inline size_t map_nn(size_t i, size_t src_len, size_t dst_len) {
    /* floor(i * src_len / dst_len) with 64-bit to avoid overflow */
    return (size_t)((uint64_t)i * (uint64_t)src_len / (uint64_t)dst_len);
}

bj_bool bj_blit_stretched(
    const bj_bitmap* src, const bj_rect* src_area,
    bj_bitmap* dst, const bj_rect* dst_area,
    bj_blit_op op)
{
    bj_check_or_0(src && dst);

    /* Determine rectangles (default to full) */
    bj_rect s = {0,0,(uint16_t)src->width,(uint16_t)src->height};
    bj_rect d = {0,0,(uint16_t)dst->width,(uint16_t)dst->height};
    if (src_area) s = *src_area;
    if (dst_area) d.x = dst_area->x, d.y = dst_area->y, d.w = dst_area->w, d.h = dst_area->h;
    if (!s.w || !s.h || !d.w || !d.h) return BJ_FALSE;

    bj_rect sbounds = (bj_rect){0,0,(uint16_t)src->width,(uint16_t)src->height};
    bj_rect dbounds = (bj_rect){0,0,(uint16_t)dst->width,(uint16_t)dst->height};
    if (bj_rect_intersection(&s, &sbounds, &s) == 0) return BJ_FALSE;
    if (bj_rect_intersection(&d, &dbounds, &d) == 0) return BJ_FALSE;

    /* If sizes match, delegate to non-stretched fast path */
    if (s.w == d.w && s.h == d.h) {
        bj_rect s_adj = s, d_adj = d;
        return do_blit_dispatch(src, &s_adj, dst, &d_adj, op);
    }

    /* Stretched: row-by-row map, using same-format fast row kernels where possible */
    const size_t bpp_s = BJ_PIXEL_GET_BPP(src->mode);
    const size_t bpp_d = BJ_PIXEL_GET_BPP(dst->mode);

    for (uint16_t dy = 0; dy < d.h; ++dy) {
        size_t sy = (size_t)s.y + map_nn(dy, s.h, d.h);

        for (uint16_t dx = 0; dx < d.w; ++dx) {
            size_t sx = (size_t)s.x + map_nn(dx, s.w, d.w);

            /* Fetch source native */
            uint32_t sval;
            if (is_subbyte(src->mode))        sval = buffer_get_pixel_bits(sx, sy, src->stride, src->buffer, bpp_s);
            else if (is_16bpp(src->mode))     sval = *(uint16_t*)((uint8_t*)src->buffer + sy*src->stride + sx*2u);
            else if (is_24bpp(src->mode))   { const uint8_t* p=(const uint8_t*)src->buffer + sy*src->stride + sx*3u; sval = (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16); }
            else                              sval = *(uint32_t*)((uint8_t*)src->buffer + sy*src->stride + sx*4u);

            if (src->colorkey_enabled && sval == src->colorkey) continue;

            /* Convert/apply op and store */
            size_t outx = (size_t)d.x + dx;
            size_t outy = (size_t)d.y + dy;

            if (src->mode == dst->mode && op == BJ_BLIT_OP_COPY && !is_subbyte(dst->mode)) {
                /* fast store of native value */
                if (is_16bpp(dst->mode))     *(uint16_t*)((uint8_t*)dst->buffer + outy*dst->stride + outx*2u) = (uint16_t)sval;
                else if (is_24bpp(dst->mode)){ uint8_t* dp=(uint8_t*)dst->buffer + outy*dst->stride + outx*3u; dp[0]=(uint8_t)sval; dp[1]=(uint8_t)(sval>>8); dp[2]=(uint8_t)(sval>>16); }
                else                          *(uint32_t*)((uint8_t*)dst->buffer + outy*dst->stride + outx*4u) = sval;
                continue;
            }

            uint8_t r,g,b; unpack_rgb_from_native(src->mode, sval, &r,&g,&b);
            if (op != BJ_BLIT_OP_COPY) {
                uint32_t dval;
                if (is_subbyte(dst->mode))        dval = buffer_get_pixel_bits(outx, outy, dst->stride, dst->buffer, bpp_d);
                else if (is_16bpp(dst->mode))     dval = *(uint16_t*)((uint8_t*)dst->buffer + outy*dst->stride + outx*2u);
                else if (is_24bpp(dst->mode))   { uint8_t* dp=(uint8_t*)dst->buffer + outy*dst->stride + outx*3u; dval = (uint32_t)dp[0]|((uint32_t)dp[1]<<8)|((uint32_t)dp[2]<<16); }
                else                              dval = *(uint32_t*)((uint8_t*)dst->buffer + outy*dst->stride + outx*4u);
                uint8_t dr,dg,db; unpack_rgb_from_native(dst->mode, dval, &dr,&dg,&db);
                switch(op){
                    case BJ_BLIT_OP_XOR: r^=dr; g^=dg; b^=db; break;
                    case BJ_BLIT_OP_OR:  r|=dr; g|=dg; b|=db; break;
                    case BJ_BLIT_OP_AND: r&=dr; g&=dg; b&=db; break;
                    case BJ_BLIT_OP_ADD_SAT:{ int R=r+dr,G=g+dg,B=b+db; r=(uint8_t)(R>255?255:R); g=(uint8_t)(G>255?255:G); b=(uint8_t)(B>255?255:B);} break;
                    case BJ_BLIT_OP_SUB_SAT:{ int R=dr-r,G=dg-g,B=db-b; r=(uint8_t)(R<0?0:R); g=(uint8_t)(G<0?0:G); b=(uint8_t)(B<0?0:B);} break;
                    default: break;
                }
            }
            uint32_t out = pack_rgb_to_native(dst->mode, r,g,b);
            if (is_subbyte(dst->mode))        buffer_set_pixel_bits(outx, outy, dst->stride, dst->buffer, out, bpp_d);
            else if (is_16bpp(dst->mode))     *(uint16_t*)((uint8_t*)dst->buffer + outy*dst->stride + outx*2u) = (uint16_t)out;
            else if (is_24bpp(dst->mode))   { uint8_t* dp=(uint8_t*)dst->buffer + outy*dst->stride + outx*3u; dp[0]=(uint8_t)out; dp[1]=(uint8_t)(out>>8); dp[2]=(uint8_t)(out>>16); }
            else                              *(uint32_t*)((uint8_t*)dst->buffer + outy*dst->stride + outx*4u) = out;
        }
    }
    return BJ_TRUE;
}

