#include <banjo/memory.h>

#include <bitmap.h>
#include <check.h>

// Note: This file is AI-generated.

// ---------- Public: color properties ----------
void bj_set_bitmap_color(struct bj_bitmap* bmp, uint32_t color, uint8_t roles) {
    bj_check(bmp);
    if (roles & BJ_BITMAP_CLEAR_COLOR) {
        bmp->clear_color = color;
    }
    if (roles & BJ_BITMAP_COLORKEY) {
        bmp->colorkey = color;
        bmp->colorkey_enabled = BJ_TRUE;  // Auto-enable when setting colorkey
    }
}

void bj_enable_colorkey(struct bj_bitmap* bmp, bj_bool enabled) {
    bj_check(bmp);
    bmp->colorkey_enabled = enabled;
}

// ---------- Bit helpers for sub-byte formats (1/4/8bpp) ----------
// Read-modify-write into arbitrary bit offset; optimized enough for rare paths.
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
    uint64_t bit_window = 0;
    bj_memcpy(&bit_window, (const uint8_t*)buffer + byte_offset, span_bytes);
    bit_window >>= bit_in_byte;
    return (uint32_t)(bit_window & mask_for_bpp(bpp));
}

static inline void buffer_set_pixel_bits(
    size_t x, size_t y, size_t stride, void* buffer, uint32_t value, size_t bpp)
{
    const size_t bit_offset  = y * stride * 8u + x * bpp;
    const size_t byte_offset = bit_offset >> 3;
    const size_t bit_in_byte = bit_offset & 7u;
    const size_t span_bytes  = (bit_in_byte + bpp + 7u) >> 3;

    const uint64_t field_mask = (uint64_t)mask_for_bpp(bpp) << bit_in_byte;

    uint64_t bit_window = 0;
    bj_memcpy(&bit_window, (const uint8_t*)buffer + byte_offset, span_bytes);
    bit_window = (bit_window & ~field_mask) | (((uint64_t)(value & mask_for_bpp(bpp)) << bit_in_byte) & field_mask);
    bj_memcpy((uint8_t*)buffer + byte_offset, &bit_window, span_bytes);
}

// ---------- Canonical RGB (8:8:8) converters - inlined for hot loops ----------

// Direct inline unpack functions to avoid function call overhead.
// bj_make_pixel_rgb() has ~10-15 cycles overhead from:
//   - function call (not inlineable across translation units)
//   - switch on pixel type
//   - array lookup into bitfields[]
//
// These inline versions eliminate that overhead for the common formats.

static inline void unpack_rgb_from_native(enum bj_pixel_mode mode, uint32_t native, uint8_t* r, uint8_t* g, uint8_t* b) {
    switch (mode) {
    case BJ_PIXEL_MODE_XRGB8888:
        // 0x00RRGGBB
        *r = (uint8_t)(native >> 16);
        *g = (uint8_t)(native >> 8);
        *b = (uint8_t)(native);
        break;
    case BJ_PIXEL_MODE_BGR24:
        // Stored as BB GG RR in memory, packed as 0x00RRGGBB
        *b = (uint8_t)(native);
        *g = (uint8_t)(native >> 8);
        *r = (uint8_t)(native >> 16);
        break;
    case BJ_PIXEL_MODE_RGB565:
        // RRRRRGGGGGGBBBBB
        *r = (uint8_t)((native >> 11) << 3);
        *g = (uint8_t)(((native >> 5) & 0x3F) << 2);
        *b = (uint8_t)((native & 0x1F) << 3);
        break;
    case BJ_PIXEL_MODE_XRGB1555:
        // XRRRRRGGGGGBBBBB
        *r = (uint8_t)(((native >> 10) & 0x1F) << 3);
        *g = (uint8_t)(((native >> 5) & 0x1F) << 3);
        *b = (uint8_t)((native & 0x1F) << 3);
        break;
    default:
        // Indexed formats - fall back to API
        bj_make_pixel_rgb(mode, native, r, g, b);
        break;
    }
}

static inline uint32_t pack_rgb_to_native(enum bj_pixel_mode mode, uint8_t r, uint8_t g, uint8_t b) {
    switch (mode) {
    case BJ_PIXEL_MODE_XRGB8888:
        return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
    case BJ_PIXEL_MODE_BGR24:
        return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
    case BJ_PIXEL_MODE_RGB565:
        return (uint32_t)(((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3));
    case BJ_PIXEL_MODE_XRGB1555:
        return (uint32_t)(((r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3));
    default:
        // Indexed formats - fall back to API
        return bj_get_pixel_value(mode, r, g, b);
    }
}

// ---------- ROPs on packed values ----------

static inline uint32_t rop_apply_u32(uint32_t dst, uint32_t src, enum bj_blit_op op) {
    switch (op) {
        case BJ_BLIT_OP_COPY:    return src;
        case BJ_BLIT_OP_XOR:     return dst ^ src;
        case BJ_BLIT_OP_OR:      return dst | src;
        case BJ_BLIT_OP_AND:     return dst & src;
        case BJ_BLIT_OP_ADD_SAT: {
            // per-channel saturating add on 8:8:8 (ignore X)
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

// 16-bit saturating add/sub requires proper channel extraction.
// RGB565:   RRRRRGGGGGGBBBBB (5:6:5)
// XRGB1555: XRRRRRGGGGGBBBBB (1:5:5:5)
//
// For XOR/OR/AND, bitwise ops work directly on native values.
// For ADD_SAT/SUB_SAT, we must unpack channels, apply saturating op, repack.

// RGB565 saturating operations
static inline uint16_t rop_add_sat_rgb565(uint16_t dst, uint16_t src) {
    // Extract channels
    int dr = (dst >> 11) & 0x1F, dg = (dst >> 5) & 0x3F, db = dst & 0x1F;
    int sr = (src >> 11) & 0x1F, sg = (src >> 5) & 0x3F, sb = src & 0x1F;
    // Saturating add
    int r = dr + sr; if (r > 31) r = 31;
    int g = dg + sg; if (g > 63) g = 63;
    int b = db + sb; if (b > 31) b = 31;
    return (uint16_t)((r << 11) | (g << 5) | b);
}

static inline uint16_t rop_sub_sat_rgb565(uint16_t dst, uint16_t src) {
    int dr = (dst >> 11) & 0x1F, dg = (dst >> 5) & 0x3F, db = dst & 0x1F;
    int sr = (src >> 11) & 0x1F, sg = (src >> 5) & 0x3F, sb = src & 0x1F;
    int r = dr - sr; if (r < 0) r = 0;
    int g = dg - sg; if (g < 0) g = 0;
    int b = db - sb; if (b < 0) b = 0;
    return (uint16_t)((r << 11) | (g << 5) | b);
}

// XRGB1555 saturating operations
static inline uint16_t rop_add_sat_xrgb1555(uint16_t dst, uint16_t src) {
    int dr = (dst >> 10) & 0x1F, dg = (dst >> 5) & 0x1F, db = dst & 0x1F;
    int sr = (src >> 10) & 0x1F, sg = (src >> 5) & 0x1F, sb = src & 0x1F;
    int r = dr + sr; if (r > 31) r = 31;
    int g = dg + sg; if (g > 31) g = 31;
    int b = db + sb; if (b > 31) b = 31;
    return (uint16_t)((r << 10) | (g << 5) | b);
}

static inline uint16_t rop_sub_sat_xrgb1555(uint16_t dst, uint16_t src) {
    int dr = (dst >> 10) & 0x1F, dg = (dst >> 5) & 0x1F, db = dst & 0x1F;
    int sr = (src >> 10) & 0x1F, sg = (src >> 5) & 0x1F, sb = src & 0x1F;
    int r = dr - sr; if (r < 0) r = 0;
    int g = dg - sg; if (g < 0) g = 0;
    int b = db - sb; if (b < 0) b = 0;
    return (uint16_t)((r << 10) | (g << 5) | b);
}

// ---------- Fast row kernels (same-format) ----------

static inline bj_bool same_format_fastcopy_possible(const struct bj_bitmap* s, const struct bj_bitmap* d, enum bj_blit_op op, bj_bool use_key) {
    return (s->mode == d->mode) && (op == BJ_BLIT_OP_COPY) && !use_key;
}

static inline bj_bool is_32bpp(enum bj_pixel_mode m) { return BJ_PIXEL_GET_BPP(m) == 32; }
static inline bj_bool is_24bpp(enum bj_pixel_mode m) { return BJ_PIXEL_GET_BPP(m) == 24; }
static inline bj_bool is_16bpp(enum bj_pixel_mode m) { return BJ_PIXEL_GET_BPP(m) == 16; }
static inline bj_bool is_subbyte(enum bj_pixel_mode m){ size_t b=BJ_PIXEL_GET_BPP(m); return (b==1||b==4||b==8); }

// bj_memcpy/bj_memmove whole rows (direction-aware for overlap)
static void blit_rows_mem(const uint8_t* restrict s, size_t s_stride, uint8_t* restrict d, size_t d_stride, size_t rowbytes, size_t rows, bj_bool overlap) {
    if (!overlap || d < s) {
        for (size_t y=0; y<rows; ++y) bj_memcpy(d + y*d_stride, s + y*s_stride, rowbytes);
    } else {
        // bottom-up to handle overlap
        for (size_t y=rows; y>0; --y) bj_memmove(d + (y-1)*d_stride, s + (y-1)*s_stride, rowbytes);
    }
}

// 32bpp same-mode ROP (XOR/OR/AND); colorkey optional
static void blit_row_32_rop(const uint32_t* restrict src, uint32_t* restrict dst, size_t pixels, bj_bool use_key, uint32_t key, enum bj_blit_op op) {
    if (!use_key) {
        if (op == BJ_BLIT_OP_COPY) {
            bj_memcpy(dst, src, pixels * 4u);
            return;
        }
        if (op == BJ_BLIT_OP_XOR) { for (size_t i=0;i<pixels;++i) dst[i]^=src[i]; return; }
        if (op == BJ_BLIT_OP_OR)  { for (size_t i=0;i<pixels;++i) dst[i]|=src[i]; return; }
        if (op == BJ_BLIT_OP_AND) { for (size_t i=0;i<pixels;++i) dst[i]&=src[i]; return; }
    }
    // general 32bpp path with key and extended ops
    for (size_t i=0;i<pixels;++i) {
        uint32_t s = src[i];
        if (use_key && s == key) continue;
        if (op == BJ_BLIT_OP_COPY) { dst[i] = s; }
        else { dst[i] = rop_apply_u32(dst[i], s, op); }
    }
}

// 16bpp same-mode fast bitwise (XOR/OR/AND); COPY → bj_memcpy; key compare in native
// For ADD_SAT/SUB_SAT, we need to know the pixel mode for correct channel extraction
static void blit_row_16_fast(const uint16_t* restrict src, uint16_t* restrict dst, size_t pixels, bj_bool use_key, uint16_t key, enum bj_blit_op op, enum bj_pixel_mode mode) {
    if (!use_key) {
        if (op == BJ_BLIT_OP_COPY) { bj_memcpy(dst, src, pixels * 2u); return; }
        if (op == BJ_BLIT_OP_XOR)  { for (size_t i = 0; i < pixels; ++i) dst[i] ^= src[i]; return; }
        if (op == BJ_BLIT_OP_OR)   { for (size_t i = 0; i < pixels; ++i) dst[i] |= src[i]; return; }
        if (op == BJ_BLIT_OP_AND)  { for (size_t i = 0; i < pixels; ++i) dst[i] &= src[i]; return; }
        // ADD_SAT/SUB_SAT without colorkey - fast path with correct channel handling
        if (mode == BJ_PIXEL_MODE_RGB565) {
            if (op == BJ_BLIT_OP_ADD_SAT) { for (size_t i = 0; i < pixels; ++i) dst[i] = rop_add_sat_rgb565(dst[i], src[i]); return; }
            if (op == BJ_BLIT_OP_SUB_SAT) { for (size_t i = 0; i < pixels; ++i) dst[i] = rop_sub_sat_rgb565(dst[i], src[i]); return; }
        } else { // XRGB1555
            if (op == BJ_BLIT_OP_ADD_SAT) { for (size_t i = 0; i < pixels; ++i) dst[i] = rop_add_sat_xrgb1555(dst[i], src[i]); return; }
            if (op == BJ_BLIT_OP_SUB_SAT) { for (size_t i = 0; i < pixels; ++i) dst[i] = rop_sub_sat_xrgb1555(dst[i], src[i]); return; }
        }
    }
    // Fallback with colorkey check
    for (size_t i = 0; i < pixels; ++i) {
        uint16_t s = src[i];
        if (use_key && s == key) continue;
        if (op == BJ_BLIT_OP_COPY) {
            dst[i] = s;
        } else if (op == BJ_BLIT_OP_XOR) {
            dst[i] ^= s;
        } else if (op == BJ_BLIT_OP_OR) {
            dst[i] |= s;
        } else if (op == BJ_BLIT_OP_AND) {
            dst[i] &= s;
        } else if (mode == BJ_PIXEL_MODE_RGB565) {
            dst[i] = (op == BJ_BLIT_OP_ADD_SAT) ? rop_add_sat_rgb565(dst[i], s) : rop_sub_sat_rgb565(dst[i], s);
        } else {
            dst[i] = (op == BJ_BLIT_OP_ADD_SAT) ? rop_add_sat_xrgb1555(dst[i], s) : rop_sub_sat_xrgb1555(dst[i], s);
        }
    }
}

// 24bpp BGR same-mode; do it channel-wise; COPY can use bj_memcpy; key compare needs 3-byte compare
static void blit_row_24_fast(const uint8_t* restrict s, uint8_t* restrict d, size_t pixels, bj_bool use_key, const uint8_t key[3], enum bj_blit_op op) {
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

// ---------- General per-pixel kernel (any format combo) ----------

static void blit_general_any(
    const struct bj_bitmap* s, const struct bj_rect* sr,
    struct bj_bitmap* d, const struct bj_rect* dr,
    enum bj_blit_op op)
{
    // Cache BPP and format checks - avoid per-pixel is_*bpp() calls
    const size_t bpp_s = BJ_PIXEL_GET_BPP(s->mode);
    const size_t bpp_d = BJ_PIXEL_GET_BPP(d->mode);
    const bj_bool same_mode = (d->mode == s->mode);
    const bj_bool use_colorkey = s->colorkey_enabled;
    const uint32_t colorkey = s->colorkey;

    for (uint16_t r = 0; r < dr->h; ++r) {
        const size_t sy = (size_t)sr->y + r;
        const size_t dy = (size_t)dr->y + r;

        const uint8_t* srow = (const uint8_t*)s->buffer + sy * s->stride;
        uint8_t*       drow = (uint8_t*)d->buffer + dy * d->stride;

        for (uint16_t c = 0; c < dr->w; ++c) {
            const size_t sx = (size_t)sr->x + c;
            const size_t dx = (size_t)dr->x + c;

            // Fetch source native using cached bpp_s
            uint32_t sval;
            if (bpp_s <= 8) {
                sval = buffer_get_pixel_bits(sx, sy, s->stride, s->buffer, bpp_s);
            } else if (bpp_s == 16) {
                sval = ((const uint16_t*)srow)[sx];
            } else if (bpp_s == 24) {
                const uint8_t* sp = srow + sx * 3u;
                sval = (uint32_t)sp[0] | ((uint32_t)sp[1] << 8) | ((uint32_t)sp[2] << 16);
            } else { // 32bpp
                sval = ((const uint32_t*)srow)[sx];
            }

            if (use_colorkey && sval == colorkey) continue;

            if (same_mode) {
                // Same-mode, any op - use cached bpp_d
                if (bpp_d <= 8) {
                    uint32_t dval = buffer_get_pixel_bits(dx, dy, d->stride, d->buffer, bpp_d);
                    uint32_t res = (op == BJ_BLIT_OP_COPY) ? sval : rop_apply_u32(dval, sval, op);
                    buffer_set_pixel_bits(dx, dy, d->stride, d->buffer, res, bpp_d);
                } else if (bpp_d == 16) {
                    uint16_t* dp = (uint16_t*)drow + dx;
                    if (op == BJ_BLIT_OP_COPY) *dp = (uint16_t)sval;
                    else *dp = (uint16_t)rop_apply_u32(*dp, (uint16_t)sval, op);
                } else if (bpp_d == 24) {
                    uint8_t* dp = drow + dx * 3u;
                    if (op == BJ_BLIT_OP_COPY) {
                        dp[0] = (uint8_t)(sval);
                        dp[1] = (uint8_t)(sval >> 8);
                        dp[2] = (uint8_t)(sval >> 16);
                    } else {
                        uint8_t br = dp[2], bg = dp[1], bb = dp[0];
                        uint8_t sr8 = (uint8_t)(sval >> 16), sg = (uint8_t)(sval >> 8), sb = (uint8_t)sval;
                        switch (op) {
                            case BJ_BLIT_OP_XOR: br ^= sr8; bg ^= sg; bb ^= sb; break;
                            case BJ_BLIT_OP_OR:  br |= sr8; bg |= sg; bb |= sb; break;
                            case BJ_BLIT_OP_AND: br &= sr8; bg &= sg; bb &= sb; break;
                            case BJ_BLIT_OP_ADD_SAT: {
                                int rv = br + sr8, gv = bg + sg, bv = bb + sb;
                                br = (uint8_t)(rv > 255 ? 255 : rv);
                                bg = (uint8_t)(gv > 255 ? 255 : gv);
                                bb = (uint8_t)(bv > 255 ? 255 : bv);
                            } break;
                            case BJ_BLIT_OP_SUB_SAT: {
                                int rv = br - sr8, gv = bg - sg, bv = bb - sb;
                                br = (uint8_t)(rv < 0 ? 0 : rv);
                                bg = (uint8_t)(gv < 0 ? 0 : gv);
                                bb = (uint8_t)(bv < 0 ? 0 : bv);
                            } break;
                            default: break;
                        }
                        dp[2] = br; dp[1] = bg; dp[0] = bb;
                    }
                } else { // 32bpp
                    uint32_t* dp = (uint32_t*)drow + dx;
                    *dp = (op == BJ_BLIT_OP_COPY) ? sval : rop_apply_u32(*dp, sval, op);
                }
                continue;
            }

            // Different formats: convert via RGB components
            uint8_t r8, g8, b8;
            unpack_rgb_from_native(s->mode, sval, &r8, &g8, &b8);

            // current dst value for ROPs other than COPY
            if (op != BJ_BLIT_OP_COPY) {
                uint32_t dval;
                if (bpp_d <= 8) {
                    dval = buffer_get_pixel_bits(dx, dy, d->stride, d->buffer, bpp_d);
                } else if (bpp_d == 16) {
                    dval = ((const uint16_t*)drow)[dx];
                } else if (bpp_d == 24) {
                    const uint8_t* dp = drow + dx * 3u;
                    dval = (uint32_t)dp[0] | ((uint32_t)dp[1] << 8) | ((uint32_t)dp[2] << 16);
                } else {
                    dval = ((const uint32_t*)drow)[dx];
                }

                // Apply op in RGB space by converting dval to rgb, applying op per channel, then pack
                uint8_t dr8, dg8, db8;
                unpack_rgb_from_native(d->mode, dval, &dr8, &dg8, &db8);
                switch (op) {
                    case BJ_BLIT_OP_XOR:    r8 ^= dr8; g8 ^= dg8; b8 ^= db8; break;
                    case BJ_BLIT_OP_OR:     r8 |= dr8; g8 |= dg8; b8 |= db8; break;
                    case BJ_BLIT_OP_AND:    r8 &= dr8; g8 &= dg8; b8 &= db8; break;
                    case BJ_BLIT_OP_ADD_SAT: {
                        int R = r8 + dr8, G = g8 + dg8, B = b8 + db8;
                        r8 = (uint8_t)(R > 255 ? 255 : R);
                        g8 = (uint8_t)(G > 255 ? 255 : G);
                        b8 = (uint8_t)(B > 255 ? 255 : B);
                    } break;
                    case BJ_BLIT_OP_SUB_SAT: {
                        int R = dr8 - r8, G = dg8 - g8, B = db8 - b8;
                        r8 = (uint8_t)(R < 0 ? 0 : R);
                        g8 = (uint8_t)(G < 0 ? 0 : G);
                        b8 = (uint8_t)(B < 0 ? 0 : B);
                    } break;
                    default: break;
                }
            }

            const uint32_t out_native = pack_rgb_to_native(d->mode, r8, g8, b8);

            if (bpp_d <= 8) {
                buffer_set_pixel_bits(dx, dy, d->stride, d->buffer, out_native, bpp_d);
            } else if (bpp_d == 16) {
                ((uint16_t*)drow)[dx] = (uint16_t)out_native;
            } else if (bpp_d == 24) {
                uint8_t* dp = drow + dx * 3u;
                dp[0] = (uint8_t)out_native;
                dp[1] = (uint8_t)(out_native >> 8);
                dp[2] = (uint8_t)(out_native >> 16);
            } else {
                ((uint32_t*)drow)[dx] = out_native;
            }
        }
    }
}

// ---------- Core clipped blit dispatcher (no scaling) ----------

static bj_bool do_blit_dispatch(
    const struct bj_bitmap* src, const struct bj_rect* sr,
    struct bj_bitmap* dst, const struct bj_rect* dr,
    enum bj_blit_op op)
{
    bj_check_or_0(src && dst && sr && dr);
    if (!sr->w || !sr->h || !dr->w || !dr->h) return BJ_FALSE;

    // Same format fast paths
    if (src->mode == dst->mode) {
        const size_t bpp = BJ_PIXEL_GET_BPP(src->mode);
        const size_t rowbytes = (size_t)dr->w * (bpp >> 3); // only for 8+ bpp
        const uint8_t* sbase = (const uint8_t*)src->buffer + (size_t)sr->y*src->stride + ((size_t)sr->x * (bpp>>3));
        uint8_t*       dbase = (uint8_t*)dst->buffer       + (size_t)dr->y*dst->stride + ((size_t)dr->x * (bpp>>3));

        const bj_bool overlap =
            (src == dst) &&
            !(dbase + dst->stride*dr->h <= sbase || sbase + src->stride*sr->h <= dbase);

        // COPY, no key: bj_memcpy/bj_memmove
        if (same_format_fastcopy_possible(src, dst, op, src->colorkey_enabled)) {
            if (bpp >= 8) { blit_rows_mem(sbase, src->stride, dbase, dst->stride, rowbytes, dr->h, overlap); return BJ_TRUE; }
            // sub-byte falls through to general
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
                blit_row_16_fast(srow, drow, dr->w, src->colorkey_enabled, key16, op, src->mode);
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
        // sub-byte or exotic layouts → fall through
    }

    // General any→any path with converters (supports sub-byte, mismatched modes)
    blit_general_any(src, sr, dst, dr, op);
    return BJ_TRUE;
}

// ---------- Public: clipped blit (no scaling) using existing clipper ----------

bj_bool bj_blit(
    const struct bj_bitmap* p_src, const struct bj_rect* p_src_area,
    struct bj_bitmap* p_dst, const struct bj_rect* p_dst_area,
    enum bj_blit_op op)
{
    bj_check_or_0(p_src && p_dst);

    // Build default rects & clip like current bj_blit
    struct bj_rect src_rect = {0,0,(uint16_t)p_src->width,(uint16_t)p_src->height};
    struct bj_rect dst_rect = {0,0,0,0};

    if (p_dst_area) { dst_rect.x = p_dst_area->x; dst_rect.y = p_dst_area->y; }
    if (p_src_area) {
        struct bj_rect tmp;
        if (bj_rect_intersection(p_src_area, &src_rect, &tmp) == 0) return BJ_FALSE;
        dst_rect.x += (int16_t)(tmp.x - p_src_area->x);
        dst_rect.y += (int16_t)(tmp.y - p_src_area->y);
        src_rect = tmp;
    }
    dst_rect.w = src_rect.w; dst_rect.h = src_rect.h;

    struct bj_rect dst_bounds = (struct bj_rect){0,0,(uint16_t)p_dst->width,(uint16_t)p_dst->height};
    struct bj_rect inter;
    if (bj_rect_intersection(&dst_rect, &dst_bounds, &inter) == 0) return BJ_FALSE;

    // Adjust source accordingly
    src_rect.x += (int16_t)(inter.x - dst_rect.x);
    src_rect.y += (int16_t)(inter.y - dst_rect.y);
    src_rect.w  = inter.w;
    src_rect.h  = inter.h;
    dst_rect    = inter;

    if (!dst_rect.w || !dst_rect.h) return BJ_FALSE;

    return do_blit_dispatch(p_src, &src_rect, p_dst, &dst_rect, op);
}

// ---------- Stretched blit (nearest) with same fast paths ----------

// Fixed-point coordinate mapping for stretched blits.
//
// Instead of computing (i * src_len / dst_len) per pixel (expensive division),
// we use 16.16 fixed-point arithmetic with incremental accumulation:
//   - Compute step = (src_len << 16) / dst_len once before the loop
//   - Each iteration: coord = accum >> 16; accum += step;
//
// This replaces ~40-cycle division with ~2-cycle shift+add per pixel.
#define FRAC_BITS 16
#define FRAC_ONE  (1u << FRAC_BITS)

bj_bool bj_blit_stretched(
    const struct bj_bitmap* src, const struct bj_rect* src_area,
    struct bj_bitmap* dst, const struct bj_rect* dst_area,
    enum bj_blit_op op)
{
    bj_check_or_0(src && dst);

    // Determine rectangles (default to full)
    struct bj_rect s = {0,0,(uint16_t)src->width,(uint16_t)src->height};
    struct bj_rect d = {0,0,(uint16_t)dst->width,(uint16_t)dst->height};
    if (src_area) s = *src_area;
    if (dst_area) d.x = dst_area->x, d.y = dst_area->y, d.w = dst_area->w, d.h = dst_area->h;
    if (!s.w || !s.h || !d.w || !d.h) return BJ_FALSE;

    struct bj_rect sbounds = (struct bj_rect){0,0,(uint16_t)src->width,(uint16_t)src->height};
    struct bj_rect dbounds = (struct bj_rect){0,0,(uint16_t)dst->width,(uint16_t)dst->height};
    if (bj_rect_intersection(&s, &sbounds, &s) == 0) return BJ_FALSE;
    if (bj_rect_intersection(&d, &dbounds, &d) == 0) return BJ_FALSE;

    // If sizes match, delegate to non-stretched fast path
    if (s.w == d.w && s.h == d.h) {
        struct bj_rect s_adj = s, d_adj = d;
        return do_blit_dispatch(src, &s_adj, dst, &d_adj, op);
    }

    // Stretched: row-by-row map, using same-format fast row kernels where possible
    const size_t bpp_s = BJ_PIXEL_GET_BPP(src->mode);
    const size_t bpp_d = BJ_PIXEL_GET_BPP(dst->mode);

    // Fixed-point step values: computed ONCE before the loops.
    // y_step = (src_height << 16) / dst_height
    // x_step = (src_width << 16) / dst_width
    const uint32_t y_step = ((uint32_t)s.h << FRAC_BITS) / (uint32_t)d.h;
    const uint32_t x_step = ((uint32_t)s.w << FRAC_BITS) / (uint32_t)d.w;

    // Cache format checks - avoid per-pixel is_*bpp() calls
    const bj_bool src_subbyte = is_subbyte(src->mode);
    const bj_bool dst_subbyte = is_subbyte(dst->mode);
    const bj_bool same_mode_copy = (src->mode == dst->mode) && (op == BJ_BLIT_OP_COPY) && !dst_subbyte;

    uint32_t y_accum = 0;

    for (uint16_t dy = 0; dy < d.h; ++dy) {
        const size_t sy = (size_t)s.y + (y_accum >> FRAC_BITS);
        y_accum += y_step;

        // Cache row pointers - computed once per row, not per pixel
        const uint8_t* src_row = (const uint8_t*)src->buffer + sy * src->stride;
        const size_t outy = (size_t)d.y + dy;
        uint8_t* dst_row = (uint8_t*)dst->buffer + outy * dst->stride;

        uint32_t x_accum = 0;

        for (uint16_t dx = 0; dx < d.w; ++dx) {
            const size_t sx = (size_t)s.x + (x_accum >> FRAC_BITS);
            x_accum += x_step;

            // Fetch source native using cached row pointer
            uint32_t sval;
            if (src_subbyte) {
                sval = buffer_get_pixel_bits(sx, sy, src->stride, src->buffer, bpp_s);
            } else if (bpp_s == 16) {
                sval = ((const uint16_t*)src_row)[sx];
            } else if (bpp_s == 24) {
                const uint8_t* p = src_row + sx * 3u;
                sval = (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16);
            } else {
                sval = ((const uint32_t*)src_row)[sx];
            }

            if (src->colorkey_enabled && sval == src->colorkey) continue;

            // Convert/apply op and store using cached row pointer
            const size_t outx = (size_t)d.x + dx;

            if (same_mode_copy) {
                // fast store of native value
                if (bpp_d == 16) {
                    ((uint16_t*)dst_row)[outx] = (uint16_t)sval;
                } else if (bpp_d == 24) {
                    uint8_t* dp = dst_row + outx * 3u;
                    dp[0] = (uint8_t)sval;
                    dp[1] = (uint8_t)(sval >> 8);
                    dp[2] = (uint8_t)(sval >> 16);
                } else {
                    ((uint32_t*)dst_row)[outx] = sval;
                }
                continue;
            }

            uint8_t r, g, b;
            unpack_rgb_from_native(src->mode, sval, &r, &g, &b);

            if (op != BJ_BLIT_OP_COPY) {
                uint32_t dval;
                if (dst_subbyte) {
                    dval = buffer_get_pixel_bits(outx, outy, dst->stride, dst->buffer, bpp_d);
                } else if (bpp_d == 16) {
                    dval = ((const uint16_t*)dst_row)[outx];
                } else if (bpp_d == 24) {
                    const uint8_t* dp = dst_row + outx * 3u;
                    dval = (uint32_t)dp[0] | ((uint32_t)dp[1] << 8) | ((uint32_t)dp[2] << 16);
                } else {
                    dval = ((const uint32_t*)dst_row)[outx];
                }

                uint8_t dr, dg, db;
                unpack_rgb_from_native(dst->mode, dval, &dr, &dg, &db);

                switch (op) {
                    case BJ_BLIT_OP_XOR: r ^= dr; g ^= dg; b ^= db; break;
                    case BJ_BLIT_OP_OR:  r |= dr; g |= dg; b |= db; break;
                    case BJ_BLIT_OP_AND: r &= dr; g &= dg; b &= db; break;
                    case BJ_BLIT_OP_ADD_SAT: {
                        int R = r + dr, G = g + dg, B = b + db;
                        r = (uint8_t)(R > 255 ? 255 : R);
                        g = (uint8_t)(G > 255 ? 255 : G);
                        b = (uint8_t)(B > 255 ? 255 : B);
                    } break;
                    case BJ_BLIT_OP_SUB_SAT: {
                        int R = dr - r, G = dg - g, B = db - b;
                        r = (uint8_t)(R < 0 ? 0 : R);
                        g = (uint8_t)(G < 0 ? 0 : G);
                        b = (uint8_t)(B < 0 ? 0 : B);
                    } break;
                    default: break;
                }
            }

            uint32_t out = pack_rgb_to_native(dst->mode, r, g, b);

            if (dst_subbyte) {
                buffer_set_pixel_bits(outx, outy, dst->stride, dst->buffer, out, bpp_d);
            } else if (bpp_d == 16) {
                ((uint16_t*)dst_row)[outx] = (uint16_t)out;
            } else if (bpp_d == 24) {
                uint8_t* dp = dst_row + outx * 3u;
                dp[0] = (uint8_t)out;
                dp[1] = (uint8_t)(out >> 8);
                dp[2] = (uint8_t)(out >> 16);
            } else {
                ((uint32_t*)dst_row)[outx] = out;
            }
        }
    }
    return BJ_TRUE;
}

