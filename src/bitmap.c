#include <banjo/math.h>
#include <banjo/memory.h>
#include <banjo/draw.h>

#include <bitmap.h>
#include <check.h>

#include <string.h>

struct bj_bitmap* dib_create_bitmap_from_stream(struct bj_stream*, struct bj_error**);

BANJO_EXPORT struct bj_bitmap* bj_allocate_bitmap(
    void
) {
    return bj_malloc(sizeof(struct bj_bitmap));
}

struct bj_bitmap* bj_init_bitmap(
    struct bj_bitmap*       bitmap,
    void*            pixels,
    size_t           width,
    size_t           height,
    enum bj_pixel_mode    mode,
    size_t           stride
) {
    const size_t computed_stride = bj_compute_bitmap_stride(width, mode);
    if(stride < computed_stride) {
        stride = computed_stride;
    }

    if(bitmap) {
        bj_memset(bitmap, 0, sizeof(struct bj_bitmap));

        if(stride > 0) {
            size_t bufsize = stride * height;

            bitmap->width       = width;
            bitmap->height      = height;
            bitmap->stride      = stride;
            bitmap->mode        = mode;
            bitmap->clear_color = 0x00000000;
            bitmap->weak        = (pixels != 0);

            if (bitmap->weak) {
                bitmap->buffer = pixels;
            } else {
                bitmap->buffer = bj_malloc(bufsize);
                if (bitmap->buffer == 0) {
                    return 0;
                }
                bj_memset(bitmap->buffer, 0x00, bufsize);
            }

        }
    }
    return bitmap;
}

void bj_reset_bitmap(
    struct bj_bitmap* bitmap
) {
    bj_check(bitmap);

    if(bitmap->weak == 0) {
        bj_free(bitmap->buffer);
    }
    bitmap->buffer = 0;
    bj_destroy_bitmap(bitmap->charset);
}

struct bj_bitmap* bj_create_bitmap(
    size_t           width,
    size_t           height,
    enum bj_pixel_mode    mode,
    size_t           stride
) {
    struct bj_bitmap temp_bitmap;
    if(bj_init_bitmap(&temp_bitmap, 0, width, height, mode, stride) == 0) {
        return 0;
    }
    struct bj_bitmap* new = bj_allocate_bitmap();
    if (new == 0) {
        bj_free(temp_bitmap.buffer);
        return 0;
    }
    return bj_memcpy(new, &temp_bitmap, sizeof(struct bj_bitmap));
}


void bj_assign_bitmap(
    struct bj_bitmap*  bitmap,
    void*              pixels,
    size_t             width,
    size_t             height,
    enum bj_pixel_mode mode,
    size_t             stride
) {
    bj_check(bitmap);
    bj_reset_bitmap(bitmap);
    bj_init_bitmap(bitmap, pixels, width, height, mode, stride);
}

struct bj_bitmap* bj_create_bitmap_from_pixels(
    void*              pixels,
    size_t             width,
    size_t             height,
    enum bj_pixel_mode mode,
    size_t             stride
) {
    bj_check_or_0(pixels);
    struct bj_bitmap temp_bitmap;
    if (bj_init_bitmap(&temp_bitmap, pixels, width, height, mode, stride) == 0) {
        return 0;
    }
    struct bj_bitmap* new = bj_allocate_bitmap();
    if (new == 0) {
        return 0;
    }
    return bj_memcpy(new, &temp_bitmap, sizeof(struct bj_bitmap));
}

struct bj_bitmap* bj_copy_bitmap(
    const struct bj_bitmap* bitmap
) {
    bj_check_or_0(bitmap);
    struct bj_bitmap temp_bitmap;
    if (bj_init_bitmap(&temp_bitmap, 0, bitmap->width, bitmap->height, bitmap->mode, bitmap->stride) == 0) {
        return 0;
    }
    bj_memcpy(temp_bitmap.buffer, bitmap->buffer, temp_bitmap.stride * temp_bitmap.height);
    struct bj_bitmap* new = bj_allocate_bitmap();
    if (new == 0) {
        bj_free(temp_bitmap.buffer);
        return 0;
    }
    return bj_memcpy(new, &temp_bitmap, sizeof(struct bj_bitmap));
}

// ============================================================================
// Format Conversion - Optimized Row Converters
// ============================================================================
//
// Instead of per-pixel format dispatch, we determine the conversion function
// once and apply it to entire rows. This eliminates millions of switch
// statements and function calls for large images.

// Forward declarations for sub-byte pixel access (defined later)
static void buffer_set_pixel_bits(size_t x, size_t y, size_t stride, void* buffer, uint32_t value, size_t bpp);
static uint32_t buffer_get_pixel_bits(size_t x, size_t y, size_t stride, const void* buffer, size_t bpp);

// --------------------------------------------------------------------------
// Inline unpack functions (native pixel -> RGB)
// --------------------------------------------------------------------------

static inline void unpack_32(uint32_t p, uint8_t* r, uint8_t* g, uint8_t* b) {
    // XRGB8888: 0x00RRGGBB
    *b = (uint8_t)(p);
    *g = (uint8_t)(p >> 8);
    *r = (uint8_t)(p >> 16);
}


static inline void unpack_565(uint16_t p, uint8_t* r, uint8_t* g, uint8_t* b) {
    // RGB565: RRRRRGGGGGGBBBBB
    *r = (uint8_t)((p >> 11) << 3);        // 5 bits -> 8 bits
    *g = (uint8_t)(((p >> 5) & 0x3F) << 2); // 6 bits -> 8 bits
    *b = (uint8_t)((p & 0x1F) << 3);        // 5 bits -> 8 bits
}

static inline void unpack_1555(uint16_t p, uint8_t* r, uint8_t* g, uint8_t* b) {
    // XRGB1555: XRRRRRGGGGGBBBBB
    *r = (uint8_t)(((p >> 10) & 0x1F) << 3); // 5 bits -> 8 bits
    *g = (uint8_t)(((p >> 5) & 0x1F) << 3);  // 5 bits -> 8 bits
    *b = (uint8_t)((p & 0x1F) << 3);          // 5 bits -> 8 bits
}

// --------------------------------------------------------------------------
// Inline pack functions (RGB -> native pixel)
// --------------------------------------------------------------------------

static inline uint32_t pack_32(uint8_t r, uint8_t g, uint8_t b) {
    return (uint32_t)b | ((uint32_t)g << 8) | ((uint32_t)r << 16);
}


static inline uint16_t pack_565(uint8_t r, uint8_t g, uint8_t b) {
    return (uint16_t)(((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3));
}

static inline uint16_t pack_1555(uint8_t r, uint8_t g, uint8_t b) {
    return (uint16_t)(((r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3));
}

// --------------------------------------------------------------------------
// Row converters: src format -> dst format
// --------------------------------------------------------------------------

// 32-bit source converters
static void convert_row_32_to_24(const uint8_t* restrict src, uint8_t* restrict dst, size_t width) {
    const uint32_t* s = (const uint32_t*)src;
    for (size_t x = 0; x < width; ++x) {
        uint32_t p = s[x];
        *dst++ = (uint8_t)(p);       // B
        *dst++ = (uint8_t)(p >> 8);  // G
        *dst++ = (uint8_t)(p >> 16); // R
    }
}

static void convert_row_32_to_565(const uint8_t* restrict src, uint8_t* restrict dst, size_t width) {
    const uint32_t* s = (const uint32_t*)src;
    uint16_t* d = (uint16_t*)dst;
    for (size_t x = 0; x < width; ++x) {
        uint8_t r, g, b;
        unpack_32(s[x], &r, &g, &b);
        d[x] = pack_565(r, g, b);
    }
}

static void convert_row_32_to_1555(const uint8_t* restrict src, uint8_t* restrict dst, size_t width) {
    const uint32_t* s = (const uint32_t*)src;
    uint16_t* d = (uint16_t*)dst;
    for (size_t x = 0; x < width; ++x) {
        uint8_t r, g, b;
        unpack_32(s[x], &r, &g, &b);
        d[x] = pack_1555(r, g, b);
    }
}

// 24-bit source converters
static void convert_row_24_to_32(const uint8_t* restrict src, uint8_t* restrict dst, size_t width) {
    uint32_t* d = (uint32_t*)dst;
    for (size_t x = 0; x < width; ++x) {
        uint8_t b = *src++;
        uint8_t g = *src++;
        uint8_t r = *src++;
        d[x] = pack_32(r, g, b);
    }
}

static void convert_row_24_to_565(const uint8_t* restrict src, uint8_t* restrict dst, size_t width) {
    uint16_t* d = (uint16_t*)dst;
    for (size_t x = 0; x < width; ++x) {
        uint8_t b = *src++;
        uint8_t g = *src++;
        uint8_t r = *src++;
        d[x] = pack_565(r, g, b);
    }
}

static void convert_row_24_to_1555(const uint8_t* restrict src, uint8_t* restrict dst, size_t width) {
    uint16_t* d = (uint16_t*)dst;
    for (size_t x = 0; x < width; ++x) {
        uint8_t b = *src++;
        uint8_t g = *src++;
        uint8_t r = *src++;
        d[x] = pack_1555(r, g, b);
    }
}

// 16-bit RGB565 source converters
static void convert_row_565_to_32(const uint8_t* restrict src, uint8_t* restrict dst, size_t width) {
    const uint16_t* s = (const uint16_t*)src;
    uint32_t* d = (uint32_t*)dst;
    for (size_t x = 0; x < width; ++x) {
        uint8_t r, g, b;
        unpack_565(s[x], &r, &g, &b);
        d[x] = pack_32(r, g, b);
    }
}

static void convert_row_565_to_24(const uint8_t* restrict src, uint8_t* restrict dst, size_t width) {
    const uint16_t* s = (const uint16_t*)src;
    for (size_t x = 0; x < width; ++x) {
        uint8_t r, g, b;
        unpack_565(s[x], &r, &g, &b);
        *dst++ = b;
        *dst++ = g;
        *dst++ = r;
    }
}

static void convert_row_565_to_1555(const uint8_t* restrict src, uint8_t* restrict dst, size_t width) {
    const uint16_t* s = (const uint16_t*)src;
    uint16_t* d = (uint16_t*)dst;
    for (size_t x = 0; x < width; ++x) {
        uint8_t r, g, b;
        unpack_565(s[x], &r, &g, &b);
        d[x] = pack_1555(r, g, b);
    }
}

// 16-bit XRGB1555 source converters
static void convert_row_1555_to_32(const uint8_t* restrict src, uint8_t* restrict dst, size_t width) {
    const uint16_t* s = (const uint16_t*)src;
    uint32_t* d = (uint32_t*)dst;
    for (size_t x = 0; x < width; ++x) {
        uint8_t r, g, b;
        unpack_1555(s[x], &r, &g, &b);
        d[x] = pack_32(r, g, b);
    }
}

static void convert_row_1555_to_24(const uint8_t* restrict src, uint8_t* restrict dst, size_t width) {
    const uint16_t* s = (const uint16_t*)src;
    for (size_t x = 0; x < width; ++x) {
        uint8_t r, g, b;
        unpack_1555(s[x], &r, &g, &b);
        *dst++ = b;
        *dst++ = g;
        *dst++ = r;
    }
}

static void convert_row_1555_to_565(const uint8_t* restrict src, uint8_t* restrict dst, size_t width) {
    const uint16_t* s = (const uint16_t*)src;
    uint16_t* d = (uint16_t*)dst;
    for (size_t x = 0; x < width; ++x) {
        uint8_t r, g, b;
        unpack_1555(s[x], &r, &g, &b);
        d[x] = pack_565(r, g, b);
    }
}

// --------------------------------------------------------------------------
// Row converter dispatch table
// --------------------------------------------------------------------------

typedef void (*row_converter_fn)(const uint8_t* restrict src, uint8_t* restrict dst, size_t width);

static row_converter_fn get_row_converter(enum bj_pixel_mode src_mode, enum bj_pixel_mode dst_mode) {
    // 32-bit source
    if (src_mode == BJ_PIXEL_MODE_XRGB8888) {
        if (dst_mode == BJ_PIXEL_MODE_BGR24)    return convert_row_32_to_24;
        if (dst_mode == BJ_PIXEL_MODE_RGB565)   return convert_row_32_to_565;
        if (dst_mode == BJ_PIXEL_MODE_XRGB1555) return convert_row_32_to_1555;
    }
    // 24-bit source
    if (src_mode == BJ_PIXEL_MODE_BGR24) {
        if (dst_mode == BJ_PIXEL_MODE_XRGB8888) return convert_row_24_to_32;
        if (dst_mode == BJ_PIXEL_MODE_RGB565)   return convert_row_24_to_565;
        if (dst_mode == BJ_PIXEL_MODE_XRGB1555) return convert_row_24_to_1555;
    }
    // 16-bit RGB565 source
    if (src_mode == BJ_PIXEL_MODE_RGB565) {
        if (dst_mode == BJ_PIXEL_MODE_XRGB8888) return convert_row_565_to_32;
        if (dst_mode == BJ_PIXEL_MODE_BGR24)    return convert_row_565_to_24;
        if (dst_mode == BJ_PIXEL_MODE_XRGB1555) return convert_row_565_to_1555;
    }
    // 16-bit XRGB1555 source
    if (src_mode == BJ_PIXEL_MODE_XRGB1555) {
        if (dst_mode == BJ_PIXEL_MODE_XRGB8888) return convert_row_1555_to_32;
        if (dst_mode == BJ_PIXEL_MODE_BGR24)    return convert_row_1555_to_24;
        if (dst_mode == BJ_PIXEL_MODE_RGB565)   return convert_row_1555_to_565;
    }
    return 0; // No optimized converter, use generic fallback
}

// --------------------------------------------------------------------------
// Generic fallback converter (for indexed and unsupported format pairs)
// --------------------------------------------------------------------------

static void convert_bitmap_generic(
    const struct bj_bitmap* src,
    struct bj_bitmap* dst
) {
    // Cache format info to avoid per-pixel extraction
    const enum bj_pixel_mode src_mode = src->mode;
    const enum bj_pixel_mode dst_mode = dst->mode;
    const size_t src_bpp = BJ_PIXEL_GET_BPP(src_mode);
    const size_t dst_bpp = BJ_PIXEL_GET_BPP(dst_mode);
    const size_t width = dst->width;
    const size_t height = dst->height;

    for (size_t y = 0; y < height; ++y) {
        const uint8_t* src_row = (const uint8_t*)src->buffer + y * src->stride;
        uint8_t* dst_row = (uint8_t*)dst->buffer + y * dst->stride;

        for (size_t x = 0; x < width; ++x) {
            // Fetch source pixel (inline switch, no function call)
            uint32_t sval;
            switch (src_bpp) {
            case 32: sval = ((const uint32_t*)src_row)[x]; break;
            case 24: {
                const uint8_t* p = src_row + x * 3;
                sval = (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16);
            } break;
            case 16: sval = ((const uint16_t*)src_row)[x]; break;
            case 8:  sval = src_row[x]; break;
            default: sval = buffer_get_pixel_bits(x, y, src->stride, src->buffer, src_bpp); break;
            }

            // Convert to RGB
            uint8_t r, g, b;
            bj_make_pixel_rgb(src_mode, sval, &r, &g, &b);

            // Convert to destination format
            uint32_t dval = bj_get_pixel_value(dst_mode, r, g, b);

            // Store destination pixel (inline switch, no function call)
            switch (dst_bpp) {
            case 32: ((uint32_t*)dst_row)[x] = dval; break;
            case 24: {
                uint8_t* p = dst_row + x * 3;
                p[0] = (uint8_t)(dval);
                p[1] = (uint8_t)(dval >> 8);
                p[2] = (uint8_t)(dval >> 16);
            } break;
            case 16: ((uint16_t*)dst_row)[x] = (uint16_t)dval; break;
            case 8:  dst_row[x] = (uint8_t)dval; break;
            default: buffer_set_pixel_bits(x, y, dst->stride, dst->buffer, dval, dst_bpp); break;
            }
        }
    }
}

// --------------------------------------------------------------------------
// Main conversion function
// --------------------------------------------------------------------------

struct bj_bitmap* bj_convert_bitmap(
    const struct bj_bitmap* src,
    enum bj_pixel_mode mode
) {
    bj_check_or_0(src);
    bj_check_or_0(mode);

    // Same format? Just copy.
    if (src->mode == mode) {
        return bj_copy_bitmap(src);
    }

    // Create destination bitmap with the TARGET mode (bug fix: was using src->mode)
    struct bj_bitmap dst;
    if (bj_init_bitmap(&dst, 0, src->width, src->height, mode, 0) == 0) {
        return 0;
    }

    // Try to get an optimized row converter
    row_converter_fn convert_row = get_row_converter(src->mode, mode);

    if (convert_row) {
        // Fast path: use optimized row converter
        for (size_t y = 0; y < dst.height; ++y) {
            const uint8_t* src_row = (const uint8_t*)src->buffer + y * src->stride;
            uint8_t* dst_row = (uint8_t*)dst.buffer + y * dst.stride;
            convert_row(src_row, dst_row, dst.width);
        }
    } else {
        // Slow path: generic pixel-by-pixel conversion
        convert_bitmap_generic(src, &dst);
    }

    struct bj_bitmap* result = bj_allocate_bitmap();
    if (result == 0) {
        bj_free(dst.buffer);
        return 0;
    }
    return bj_memcpy(result, &dst, sizeof(struct bj_bitmap));
}

void bj_destroy_bitmap(
    struct bj_bitmap*     bitmap
) {
    bj_reset_bitmap(bitmap);
    bj_free(bitmap);
}

size_t bj_bitmap_width(
    const struct bj_bitmap*     bitmap
) {
    bj_check_or_0(bitmap);
    return bitmap->width;
}

size_t bj_bitmap_height(
    const struct bj_bitmap*     bitmap
) {
    bj_check_or_0(bitmap);
    return bitmap->height;
}

int bj_bitmap_mode(
    struct bj_bitmap* bitmap
) {
    bj_check_or_0(bitmap);
    return bitmap->mode;
}

size_t bj_bitmap_stride(
    struct bj_bitmap* bitmap
) {
    bj_check_or_0(bitmap);
    return bitmap->stride;
}


void* bj_bitmap_pixels(
    struct bj_bitmap*     bitmap
) {
    bj_check_or_0(bitmap);
    return bitmap->buffer;
}

uint32_t bj_make_bitmap_pixel(
    struct bj_bitmap* bitmap,
    uint8_t red,
    uint8_t green,
    uint8_t blue
) {
    bj_check_or_0(bitmap);
    return bj_get_pixel_value(bitmap->mode, red, green, blue);
}

// ----------------------------------------------------------------------------
// Pixel Accessors - Optimized for common formats
// ----------------------------------------------------------------------------

// Sub-byte pixel access (1/4/8 bpp) - slow path, bit manipulation required.
// Only used for indexed/paletted formats.
static void buffer_set_pixel_bits(size_t x, size_t y, size_t stride, void* buffer, uint32_t value, size_t bpp) {
    const size_t   bit_offset        = y * stride * 8 + x * bpp;
    const size_t   byte_offset       = bit_offset / 8;
    const size_t   bit_in_first_byte = bit_offset % 8;
    const uint32_t aligned_value     = value << bit_in_first_byte;
    const size_t   bytes_to_copy     = (bit_in_first_byte + bpp + 7) / 8;
    bj_memcpy((uint8_t*)buffer + byte_offset, &aligned_value, bytes_to_copy);
}

static uint32_t buffer_get_pixel_bits(size_t x, size_t y, size_t stride, const void* buffer, size_t bpp) {
    const size_t bit_offset = y * stride * 8 + x * bpp;
    const size_t byte_offset = bit_offset / 8;
    const size_t bit_in_first_byte = bit_offset % 8;
    const size_t bytes_to_copy = (bit_in_first_byte + bpp + 7) / 8;
    uint32_t pixel_value = 0;
    bj_memcpy(&pixel_value, (const uint8_t*)buffer + byte_offset, bytes_to_copy);
    pixel_value >>= bit_in_first_byte;
    return pixel_value;
}

void bj_put_pixel(
    struct bj_bitmap* bitmap,
    size_t x,
    size_t y,
    uint32_t pixel
) {
    bj_check(bitmap);
    bj_check(x < bitmap->width && y < bitmap->height);

    uint8_t* row = (uint8_t*)bitmap->buffer + y * bitmap->stride;
    const size_t bpp = BJ_PIXEL_GET_BPP(bitmap->mode);

    // Fast paths for common formats - no function call, direct memory access
    switch (bpp) {
    case 32:
        ((uint32_t*)row)[x] = pixel;
        return;
    case 24: {
        uint8_t* p = row + x * 3;
        p[0] = (uint8_t)(pixel);        // Blue
        p[1] = (uint8_t)(pixel >> 8);   // Green
        p[2] = (uint8_t)(pixel >> 16);  // Red
        return;
    }
    case 16:
        ((uint16_t*)row)[x] = (uint16_t)pixel;
        return;
    case 8:
        row[x] = (uint8_t)pixel;
        return;
    default:
        // Sub-byte formats (1/4 bpp) - slow path
        buffer_set_pixel_bits(x, y, bitmap->stride, bitmap->buffer, pixel, bpp);
        return;
    }
}

uint32_t bj_bitmap_pixel(
    const struct bj_bitmap* bitmap,
    size_t           x,
    size_t           y
) {
    bj_check_or_0(bitmap);
    bj_check_or_0(x < bitmap->width && y < bitmap->height);

    const uint8_t* row = (const uint8_t*)bitmap->buffer + y * bitmap->stride;
    const size_t bpp = BJ_PIXEL_GET_BPP(bitmap->mode);

    // Fast paths for common formats
    switch (bpp) {
    case 32:
        return ((const uint32_t*)row)[x];
    case 24: {
        const uint8_t* p = row + x * 3;
        return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16);
    }
    case 16:
        return ((const uint16_t*)row)[x];
    case 8:
        return row[x];
    default:
        // Sub-byte formats (1/4 bpp) - slow path
        return buffer_get_pixel_bits(x, y, bitmap->stride, bitmap->buffer, bpp);
    }
}

struct bj_bitmap* bj_create_bitmap_from_file(
    const char*       path,
    struct bj_error**        error
) {
    struct bj_error* inner_error = 0;

    struct bj_stream* stream = bj_open_stream_file(path, &inner_error);
    if(inner_error) {
        bj_propagate_prefixed_error(error, inner_error,
            "Loading bitmap '%s': ", path);
        return 0;
    }

    struct bj_bitmap* bitmap = dib_create_bitmap_from_stream(stream, error);
    bj_close_stream(stream);
    return bitmap;
}

void bj_clear_bitmap(struct bj_bitmap* bitmap) {
    bj_check(bitmap);

    const size_t bpp = BJ_PIXEL_GET_BPP(bitmap->mode);
    const int w = (int)bitmap->width;
    const int h = (int)bitmap->height;

    // Dispatch to format-specific fill for maximum speed
    switch (bpp) {
    case 32:
        bj__fill_rect_32(bitmap, 0, 0, w, h, bitmap->clear_color);
        break;
    case 24:
        bj__fill_rect_24(bitmap, 0, 0, w, h, bitmap->clear_color);
        break;
    case 16:
        bj__fill_rect_16(bitmap, 0, 0, w, h, bitmap->clear_color);
        break;
    default:
        bj__fill_rect_generic(bitmap, 0, 0, w, h, bitmap->clear_color);
        break;
    }
}



void bj_make_bitmap_rgb(
    const struct bj_bitmap* bitmap,
    size_t           x,
    size_t           y,
    uint8_t*         red,
    uint8_t*         green,
    uint8_t*         blue
) {
    bj_check(bitmap);
    bj_make_pixel_rgb(
        bitmap->mode,
        bj_bitmap_pixel(bitmap, x, y),
        red,
        green,
        blue
    );
}

// ============================================================================
// GENERIC FALLBACK IMPLEMENTATIONS
// ============================================================================
// These implementations work with ANY pixel format by using the public
// bj_put_pixel/bj_bitmap_pixel API. Slower than format-specific implementations
// due to per-pixel function call overhead and runtime format detection.
// Used as fallback for indexed formats (1/4/8 bpp) and future formats.

// Fixed-point constants for stretched blits.
// We use 16.16 fixed-point for coordinate interpolation to avoid per-pixel division.
#define FRAC_BITS 16

// ----------------------------------------------------------------------------
// Alpha Blending (generic)
// ----------------------------------------------------------------------------

static inline uint8_t generic_mix_u8(uint16_t alpha, uint8_t src, uint8_t dst) {
    uint32_t x = (uint32_t)dst * (255u - alpha) + (uint32_t)src * alpha;
    x += 128u + (x >> 8);
    return (uint8_t)(x >> 8);
}

static inline void generic_src_over_rgb(
    uint8_t alpha,
    uint8_t src_r, uint8_t src_g, uint8_t src_b,
    uint8_t dst_r, uint8_t dst_g, uint8_t dst_b,
    uint8_t* out_r, uint8_t* out_g, uint8_t* out_b
) {
    *out_r = generic_mix_u8(alpha, src_r, dst_r);
    *out_g = generic_mix_u8(alpha, src_g, dst_g);
    *out_b = generic_mix_u8(alpha, src_b, dst_b);
}

// ----------------------------------------------------------------------------
// Mask Blit (non-stretched) - Generic
// ----------------------------------------------------------------------------

void bj__blit_mask_generic(
    const struct bj_bitmap* mask,
    const struct bj_rect*   ms,
    struct bj_bitmap*       dst,
    const struct bj_rect*   ds,
    uint32_t                fg_native,
    uint32_t                bg_native,
    uint8_t fr, uint8_t fg, uint8_t fb,
    uint8_t br, uint8_t bg, uint8_t bb,
    bj_mask_bg_mode         mode
) {
    for (uint16_t row = 0; row < ds->h; ++row) {
        const size_t my = (size_t)ms->y + row;
        const size_t dy = (size_t)ds->y + row;
        // Optimize mask access at least - mask is always 8bpp
        const uint8_t* mrow = bj__row_ptr(mask, my);

        for (uint16_t col = 0; col < ds->w; ++col) {
            const size_t mx = (size_t)ms->x + col;
            const size_t dx = (size_t)ds->x + col;
            const uint8_t alpha = mrow[mx];

            switch (mode) {
            case BJ_MASK_BG_TRANSPARENT:
                if (alpha == 0) continue;
                if (alpha == 255) {
                    bj_put_pixel(dst, dx, dy, fg_native);
                } else {
                    const uint32_t dval = bj_bitmap_pixel(dst, dx, dy);
                    uint8_t dst_r, dst_g, dst_b;
                    bj_make_pixel_rgb(dst->mode, dval, &dst_r, &dst_g, &dst_b);
                    uint8_t out_r, out_g, out_b;
                    generic_src_over_rgb(alpha, fr, fg, fb, dst_r, dst_g, dst_b, &out_r, &out_g, &out_b);
                    bj_put_pixel(dst, dx, dy, bj_make_bitmap_pixel(dst, out_r, out_g, out_b));
                }
                break;

            case BJ_MASK_BG_OPAQUE:
                if (alpha == 0) {
                    bj_put_pixel(dst, dx, dy, bg_native);
                } else if (alpha == 255) {
                    bj_put_pixel(dst, dx, dy, fg_native);
                } else {
                    uint8_t out_r = generic_mix_u8(alpha, fr, br);
                    uint8_t out_g = generic_mix_u8(alpha, fg, bg);
                    uint8_t out_b = generic_mix_u8(alpha, fb, bb);
                    bj_put_pixel(dst, dx, dy, bj_make_bitmap_pixel(dst, out_r, out_g, out_b));
                }
                break;

            case BJ_MASK_BG_REV_TRANSPARENT: {
                const uint8_t alpha_bg = (uint8_t)(255u - alpha);
                if (alpha_bg == 0) {
                    // keep dst
                } else if (alpha_bg == 255) {
                    bj_put_pixel(dst, dx, dy, bg_native);
                } else {
                    const uint32_t dval = bj_bitmap_pixel(dst, dx, dy);
                    uint8_t dst_r, dst_g, dst_b;
                    bj_make_pixel_rgb(dst->mode, dval, &dst_r, &dst_g, &dst_b);
                    uint8_t out_r, out_g, out_b;
                    generic_src_over_rgb(alpha_bg, br, bg, bb, dst_r, dst_g, dst_b, &out_r, &out_g, &out_b);
                    bj_put_pixel(dst, dx, dy, bj_make_bitmap_pixel(dst, out_r, out_g, out_b));
                }
            } break;
            }
        }
    }
}

// ----------------------------------------------------------------------------
// Mask Blit (stretched) - Generic
// ----------------------------------------------------------------------------

void bj__blit_mask_stretched_generic(
    const struct bj_bitmap* mask,
    const struct bj_rect*   ms,
    struct bj_bitmap*       dst,
    const struct bj_rect*   ds,
    uint32_t                fg_native,
    uint32_t                bg_native,
    uint8_t fr, uint8_t fg, uint8_t fb,
    uint8_t br, uint8_t bg, uint8_t bb,
    bj_mask_bg_mode         mode
) {
    const size_t sw = ms->w, sh = ms->h;
    const size_t dw = ds->w, dh = ds->h;

    if (dw == 0 || dh == 0) return;

    // Fixed-point step values: computed ONCE before the loops
    const uint32_t y_step = ((uint32_t)sh << FRAC_BITS) / (uint32_t)dh;
    const uint32_t x_step = ((uint32_t)sw << FRAC_BITS) / (uint32_t)dw;

    uint32_t y_accum = 0;

    for (uint16_t dy = 0; dy < dh; ++dy) {
        const size_t sy = (size_t)ms->y + (y_accum >> FRAC_BITS);
        const size_t out_y = (size_t)ds->y + dy;
        y_accum += y_step;

        const uint8_t* mrow = bj__row_ptr(mask, sy);

        uint32_t x_accum = 0;

        for (uint16_t dx = 0; dx < dw; ++dx) {
            const size_t sx = (size_t)ms->x + (x_accum >> FRAC_BITS);
            const size_t out_x = (size_t)ds->x + dx;
            x_accum += x_step;

            const uint8_t alpha = mrow[sx];

            switch (mode) {
            case BJ_MASK_BG_TRANSPARENT:
                if (alpha == 0) continue;
                if (alpha == 255) {
                    bj_put_pixel(dst, out_x, out_y, fg_native);
                } else {
                    const uint32_t dval = bj_bitmap_pixel(dst, out_x, out_y);
                    uint8_t dst_r, dst_g, dst_b;
                    bj_make_pixel_rgb(dst->mode, dval, &dst_r, &dst_g, &dst_b);
                    uint8_t out_r, out_g, out_b;
                    generic_src_over_rgb(alpha, fr, fg, fb, dst_r, dst_g, dst_b, &out_r, &out_g, &out_b);
                    bj_put_pixel(dst, out_x, out_y, bj_make_bitmap_pixel(dst, out_r, out_g, out_b));
                }
                break;

            case BJ_MASK_BG_OPAQUE:
                if (alpha == 0) {
                    bj_put_pixel(dst, out_x, out_y, bg_native);
                } else if (alpha == 255) {
                    bj_put_pixel(dst, out_x, out_y, fg_native);
                } else {
                    uint8_t out_r = generic_mix_u8(alpha, fr, br);
                    uint8_t out_g = generic_mix_u8(alpha, fg, bg);
                    uint8_t out_b = generic_mix_u8(alpha, fb, bb);
                    bj_put_pixel(dst, out_x, out_y, bj_make_bitmap_pixel(dst, out_r, out_g, out_b));
                }
                break;

            case BJ_MASK_BG_REV_TRANSPARENT: {
                const uint8_t alpha_bg = (uint8_t)(255u - alpha);
                if (alpha_bg == 0) { } // keep dst
                else if (alpha_bg == 255) {
                    bj_put_pixel(dst, out_x, out_y, bg_native);
                } else {
                    const uint32_t dval = bj_bitmap_pixel(dst, out_x, out_y);
                    uint8_t dst_r, dst_g, dst_b;
                    bj_make_pixel_rgb(dst->mode, dval, &dst_r, &dst_g, &dst_b);
                    uint8_t out_r, out_g, out_b;
                    generic_src_over_rgb(alpha_bg, br, bg, bb, dst_r, dst_g, dst_b, &out_r, &out_g, &out_b);
                    bj_put_pixel(dst, out_x, out_y, bj_make_bitmap_pixel(dst, out_r, out_g, out_b));
                }
            } break;
            }
        }
    }
}

// ----------------------------------------------------------------------------
// Filled Rectangle - Generic
// ----------------------------------------------------------------------------

void bj__fill_rect_generic(
    struct bj_bitmap* dst,
    int x0, int y0,
    int x1, int y1,
    uint32_t pixel
) {
    // Clip to bitmap bounds
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > (int)dst->width)  x1 = (int)dst->width;
    if (y1 > (int)dst->height) y1 = (int)dst->height;
    if (x0 >= x1 || y0 >= y1) return;

    const size_t bpp = BJ_PIXEL_GET_BPP(dst->mode);
    const size_t width = (size_t)(x1 - x0);

    // For standard formats, use first-row + memcpy pattern like 24/16bpp fills
    switch (bpp) {
    case 32: {
        // Fill first row
        uint8_t* first_row = bj__row_ptr(dst, (size_t)y0);
        uint32_t* p = (uint32_t*)first_row + x0;
        for (size_t x = 0; x < width; ++x) {
            p[x] = pixel;
        }
        // Copy to remaining rows
        const size_t row_bytes = width * sizeof(uint32_t);
        for (int y = y0 + 1; y < y1; ++y) {
            uint8_t* dest_row = bj__row_ptr(dst, (size_t)y);
            memcpy((uint32_t*)dest_row + x0, p, row_bytes);
        }
    } break;
    case 24: {
        const uint8_t b = (uint8_t)(pixel);
        const uint8_t g = (uint8_t)(pixel >> 8);
        const uint8_t r = (uint8_t)(pixel >> 16);
        uint8_t* first_row = bj__row_ptr(dst, (size_t)y0) + (size_t)x0 * 3;
        uint8_t* p = first_row;
        for (size_t x = 0; x < width; ++x) {
            *p++ = b; *p++ = g; *p++ = r;
        }
        const size_t row_bytes = width * 3;
        for (int y = y0 + 1; y < y1; ++y) {
            uint8_t* dest_row = bj__row_ptr(dst, (size_t)y) + (size_t)x0 * 3;
            memcpy(dest_row, first_row, row_bytes);
        }
    } break;
    case 16: {
        const uint16_t p16 = (uint16_t)pixel;
        uint16_t* first_row = (uint16_t*)bj__row_ptr(dst, (size_t)y0) + x0;
        for (size_t x = 0; x < width; ++x) {
            first_row[x] = p16;
        }
        const size_t row_bytes = width * sizeof(uint16_t);
        for (int y = y0 + 1; y < y1; ++y) {
            uint16_t* dest_row = (uint16_t*)bj__row_ptr(dst, (size_t)y) + x0;
            memcpy(dest_row, first_row, row_bytes);
        }
    } break;
    case 8: {
        // 8bpp: can use memset for first row if all bytes same, otherwise fill + memcpy
        const uint8_t p8 = (uint8_t)pixel;
        uint8_t* first_row = bj__row_ptr(dst, (size_t)y0) + x0;
        memset(first_row, p8, width);
        for (int y = y0 + 1; y < y1; ++y) {
            uint8_t* dest_row = bj__row_ptr(dst, (size_t)y) + x0;
            memcpy(dest_row, first_row, width);
        }
    } break;
    default:
        // Sub-byte formats: fall back to bj_put_pixel
        for (int y = y0; y < y1; ++y) {
            for (int x = x0; x < x1; ++x) {
                bj_put_pixel(dst, (size_t)x, (size_t)y, pixel);
            }
        }
        break;
    }
}

// ----------------------------------------------------------------------------
// Stubs for 24bpp and 16bpp - delegate to generic for now
// ----------------------------------------------------------------------------

void bj__blit_mask_24(
    const struct bj_bitmap* mask,
    const struct bj_rect*   ms,
    struct bj_bitmap*       dst,
    const struct bj_rect*   ds,
    uint32_t                fg_native,
    uint32_t                bg_native,
    uint8_t fr, uint8_t fg, uint8_t fb,
    uint8_t br, uint8_t bg, uint8_t bb,
    bj_mask_bg_mode         mode
) {
    bj__blit_mask_generic(mask, ms, dst, ds, fg_native, bg_native,
                          fr, fg, fb, br, bg, bb, mode);
}

void bj__blit_mask_stretched_24(
    const struct bj_bitmap* mask,
    const struct bj_rect*   ms,
    struct bj_bitmap*       dst,
    const struct bj_rect*   ds,
    uint32_t                fg_native,
    uint32_t                bg_native,
    uint8_t fr, uint8_t fg, uint8_t fb,
    uint8_t br, uint8_t bg, uint8_t bb,
    bj_mask_bg_mode         mode
) {
    bj__blit_mask_stretched_generic(mask, ms, dst, ds, fg_native, bg_native,
                                    fr, fg, fb, br, bg, bb, mode);
}

void bj__blit_mask_16(
    const struct bj_bitmap* mask,
    const struct bj_rect*   ms,
    struct bj_bitmap*       dst,
    const struct bj_rect*   ds,
    uint32_t                fg_native,
    uint32_t                bg_native,
    uint8_t fr, uint8_t fg, uint8_t fb,
    uint8_t br, uint8_t bg, uint8_t bb,
    bj_mask_bg_mode         mode
) {
    bj__blit_mask_generic(mask, ms, dst, ds, fg_native, bg_native,
                          fr, fg, fb, br, bg, bb, mode);
}

void bj__blit_mask_stretched_16(
    const struct bj_bitmap* mask,
    const struct bj_rect*   ms,
    struct bj_bitmap*       dst,
    const struct bj_rect*   ds,
    uint32_t                fg_native,
    uint32_t                bg_native,
    uint8_t fr, uint8_t fg, uint8_t fb,
    uint8_t br, uint8_t bg, uint8_t bb,
    bj_mask_bg_mode         mode
) {
    bj__blit_mask_stretched_generic(mask, ms, dst, ds, fg_native, bg_native,
                                    fr, fg, fb, br, bg, bb, mode);
}

// Optimized 24bpp fill using first-row + memcpy pattern.
void bj__fill_rect_24(
    struct bj_bitmap* dst,
    int x0, int y0,
    int x1, int y1,
    uint32_t pixel
) {
    // Clip to bitmap bounds
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > (int)dst->width)  x1 = (int)dst->width;
    if (y1 > (int)dst->height) y1 = (int)dst->height;
    if (x0 >= x1 || y0 >= y1) return;

    const size_t width = (size_t)(x1 - x0);

    // Unpack pixel to BGR bytes
    const uint8_t b = (uint8_t)(pixel);
    const uint8_t g = (uint8_t)(pixel >> 8);
    const uint8_t r = (uint8_t)(pixel >> 16);

    // Fill first row
    uint8_t* first_row = bj__row_ptr(dst, (size_t)y0) + (size_t)x0 * 3;
    uint8_t* p = first_row;
    for (size_t x = 0; x < width; ++x) {
        *p++ = b;
        *p++ = g;
        *p++ = r;
    }

    // Copy first row to remaining rows
    const size_t row_bytes = width * 3;
    for (int y = y0 + 1; y < y1; ++y) {
        uint8_t* dest_row = bj__row_ptr(dst, (size_t)y) + (size_t)x0 * 3;
        memcpy(dest_row, first_row, row_bytes);
    }
}

// Optimized 16bpp fill using first-row + memcpy pattern.
void bj__fill_rect_16(
    struct bj_bitmap* dst,
    int x0, int y0,
    int x1, int y1,
    uint32_t pixel
) {
    // Clip to bitmap bounds
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > (int)dst->width)  x1 = (int)dst->width;
    if (y1 > (int)dst->height) y1 = (int)dst->height;
    if (x0 >= x1 || y0 >= y1) return;

    const size_t width = (size_t)(x1 - x0);
    const uint16_t p16 = (uint16_t)pixel;

    // Fill first row
    uint8_t* first_row_base = bj__row_ptr(dst, (size_t)y0);
    uint16_t* first_row = (uint16_t*)first_row_base + x0;
    for (size_t x = 0; x < width; ++x) {
        first_row[x] = p16;
    }

    // Copy first row to remaining rows
    const size_t row_bytes = width * sizeof(uint16_t);
    for (int y = y0 + 1; y < y1; ++y) {
        uint8_t* dest_row = bj__row_ptr(dst, (size_t)y);
        memcpy((uint16_t*)dest_row + x0, first_row, row_bytes);
    }
}

// ----------------------------------------------------------------------------
// Horizontal Line - for circles/triangles
// ----------------------------------------------------------------------------

void bj__hline_generic(struct bj_bitmap* dst, int x0, int x1, int y, uint32_t pixel) {
    // Clip
    if (y < 0 || y >= (int)dst->height) return;
    if (x0 < 0) x0 = 0;
    if (x1 > (int)dst->width) x1 = (int)dst->width;
    if (x0 >= x1) return;

    // Compute row pointer once, use fast accessors (coordinates already clipped)
    uint8_t* row = bj__row_ptr(dst, (size_t)y);
    const size_t bpp = BJ_PIXEL_GET_BPP(dst->mode);

    // For 8/16/24/32 bpp, use direct access; sub-byte falls back to bj_put_pixel
    switch (bpp) {
    case 32:
        for (int x = x0; x < x1; ++x) {
            ((uint32_t*)row)[x] = pixel;
        }
        break;
    case 24:
        for (int x = x0; x < x1; ++x) {
            uint8_t* p = row + x * 3;
            p[0] = (uint8_t)(pixel);
            p[1] = (uint8_t)(pixel >> 8);
            p[2] = (uint8_t)(pixel >> 16);
        }
        break;
    case 16:
        for (int x = x0; x < x1; ++x) {
            ((uint16_t*)row)[x] = (uint16_t)pixel;
        }
        break;
    case 8:
        for (int x = x0; x < x1; ++x) {
            row[x] = (uint8_t)pixel;
        }
        break;
    default:
        // Sub-byte formats: fall back to bj_put_pixel
        for (int x = x0; x < x1; ++x) {
            bj_put_pixel(dst, (size_t)x, (size_t)y, pixel);
        }
        break;
    }
}

void bj__hline_24(struct bj_bitmap* dst, int x0, int x1, int y, uint32_t pixel) {
    // Clip
    if (y < 0 || y >= (int)dst->height) return;
    if (x0 < 0) x0 = 0;
    if (x1 > (int)dst->width) x1 = (int)dst->width;
    if (x0 >= x1) return;

    uint8_t* row = bj__row_ptr(dst, (size_t)y) + x0 * 3;
    const uint8_t b = (uint8_t)(pixel);
    const uint8_t g = (uint8_t)(pixel >> 8);
    const uint8_t r = (uint8_t)(pixel >> 16);

    for (int x = x0; x < x1; ++x) {
        *row++ = b;
        *row++ = g;
        *row++ = r;
    }
}

void bj__hline_16(struct bj_bitmap* dst, int x0, int x1, int y, uint32_t pixel) {
    // Clip
    if (y < 0 || y >= (int)dst->height) return;
    if (x0 < 0) x0 = 0;
    if (x1 > (int)dst->width) x1 = (int)dst->width;
    if (x0 >= x1) return;

    uint16_t* row = (uint16_t*)bj__row_ptr(dst, (size_t)y) + x0;
    const uint16_t p16 = (uint16_t)pixel;
    const size_t count = (size_t)(x1 - x0);

    for (size_t i = 0; i < count; ++i) {
        row[i] = p16;
    }
}


