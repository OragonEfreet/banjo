#include <banjo/assert.h>
#include <banjo/pixel.h>

#include <check.h>

struct bitmask {
    uint8_t shift;
    uint8_t bits;
};

struct bitfield {
    struct bitmask blue;
    struct bitmask green;
    struct bitmask red;
    struct bitmask alpha;
};

static struct bitfield bitfields[] = {
    { // BJ_PIXEL_LAYOUT_1555
      .blue =  { .shift = 0,  .bits = 5},
      .green = { .shift = 5,  .bits = 5},
      .red =   { .shift = 10, .bits = 5},
      .alpha = { .shift = 15, .bits = 1},
    },
    { // BJ_PIXEL_LAYOUT_8888
      .blue =  { .shift = 0,  .bits = 8},
      .green = { .shift = 8,  .bits = 8},
      .red =   { .shift = 16, .bits = 8},
      .alpha = { .shift = 24, .bits = 8},
    },
    { // BJ_PIXEL_LAYOUT_565
      .blue =  { .shift = 0,  .bits = 5},
      .green = { .shift = 5,  .bits = 6},
      .red =   { .shift = 11, .bits = 5},
    },
};

void bj_make_pixel_rgb(
    enum bj_pixel_mode mode,
    uint32_t      value,
    uint8_t*      p_red,
    uint8_t*      p_green,
    uint8_t*      p_blue
) {
    const uint8_t type = BJ_PIXEL_GET_TYPE(mode);

    switch(type) {
        case BJ_PIXEL_TYPE_BYTES:
        {
            bj_check(BJ_PIXEL_GET_BPP(mode) == 24);

#ifdef BJ_BIG_ENDIAN
            *p_red   = (uint8_t)(value & 0xFF);
            *p_green = (uint8_t)((value >> 8) & 0xFF);
            *p_blue  = (uint8_t)((value >> 16) & 0xFF);
#else
            *p_blue  = (uint8_t)(value & 0xFF);
            *p_green = (uint8_t)((value >> 8) & 0xFF);
            *p_red   = (uint8_t)((value >> 16) & 0xFF);
#endif
        }
        break;

        case BJ_PIXEL_TYPE_BITFIELD:
        {
            const struct bitfield bf = bitfields[BJ_PIXEL_GET_LAYOUT(mode)];

            *p_red   = (uint8_t)((uint32_t)((value >> bf.red.shift) & ((1u << bf.red.bits) - 1u)) << (8 - bf.red.bits));
            *p_green = (uint8_t)((uint32_t)((value >> bf.green.shift) & ((1u << bf.green.bits) - 1u)) << (8 - bf.green.bits));
            *p_blue  = (uint8_t)((uint32_t)((value >> bf.blue.shift) & ((1u << bf.blue.bits) - 1u)) << (8 - bf.blue.bits));
        }
        break;

        default:
            *p_red = *p_green = *p_blue = 0;
            break;
    }
}

uint32_t bj_get_pixel_value(
    enum bj_pixel_mode mode, 
    uint8_t red,
    uint8_t green,
    uint8_t blue
) {
    const uint8_t type = BJ_PIXEL_GET_TYPE(mode);
    switch(type) {
        case BJ_PIXEL_TYPE_BYTES:
        {
            bj_check_or_0(BJ_PIXEL_GET_BPP(mode) == 24);
#ifdef BJ_BIG_ENDIAN
            return ((uint32_t)red & 0xFF) | ((uint32_t)green & 0xFF) << 8 | ((uint32_t)blue & 0xFF) << 16;
#else
            return ((uint32_t)blue & 0xFF) | ((uint32_t)green & 0xFF) << 8 | ((uint32_t)red & 0xFF) << 16;
#endif
        }
        case BJ_PIXEL_TYPE_BITFIELD:
        {
            const struct bitfield bf = bitfields[BJ_PIXEL_GET_LAYOUT(mode)];

            return    ((uint32_t)(red >> (8 - bf.red.bits))) << bf.red.shift
                    | ((uint32_t)(green >> (8 - bf.green.bits))) << bf.green.shift
                    | ((uint32_t)(blue >> (8 - bf.blue.bits))) << bf.blue.shift;
        }
        break;
        default:
            break;
    }

    return 0;
}

#define RETURN_IF_MATCH(r,g,b, fmt) if(red_mask == r && green_mask == g && blue_mask == b) {return fmt;}
int bj_compute_pixel_mode(
    uint8_t bpp,
    uint32_t red_mask,
    uint32_t green_mask,
    uint32_t blue_mask
) {
    const bj_bool have_masks = (red_mask | green_mask | blue_mask) > 0;
    switch (bpp) {
    case 1:
        bj_assert(!have_masks);
        return BJ_PIXEL_MODE_INDEXED_1;

    case 4:
        bj_assert(!have_masks);
        return BJ_PIXEL_MODE_INDEXED_4;

    case 8:
        bj_assert(!have_masks);
        return BJ_PIXEL_MODE_INDEXED_8;

    case 16:
        // print_masks_instructions(red_mask, green_mask, blue_mask);

        RETURN_IF_MATCH(0x0000F800, 0x000007E0, 0x0000001F, BJ_PIXEL_MODE_RGB565);

        return have_masks ? BJ_PIXEL_MODE_UNKNOWN : BJ_PIXEL_MODE_XRGB1555;

    case 24:
        bj_assert(!have_masks);
        return BJ_PIXEL_MODE_BGR24;

    case 32:
        RETURN_IF_MATCH(0x00FF0000, 0x0000FF00, 0x000000FF, BJ_PIXEL_MODE_XRGB8888);
        return have_masks ? BJ_PIXEL_MODE_UNKNOWN : BJ_PIXEL_MODE_XRGB8888;

    default:
        break;
    }

    return BJ_PIXEL_MODE_UNKNOWN;
}
#undef RETURN_IF_MATCH

size_t bj_compute_bitmap_stride(
    size_t width,
    enum bj_pixel_mode mode
) {
    switch (mode) {
    case BJ_PIXEL_MODE_INDEXED_1:
        return ((width + 7) / 8 + 3) & ~(size_t)3;
    case BJ_PIXEL_MODE_INDEXED_4:
        return ((width + 1) / 2 + 3) & ~(size_t)3;
    case BJ_PIXEL_MODE_INDEXED_8:
        return (width + 3) & ~(size_t)3;
    case BJ_PIXEL_MODE_RGB565:
    case BJ_PIXEL_MODE_XRGB1555:
        return (width * 2 + 3) & ~(size_t)3;
    case BJ_PIXEL_MODE_BGR24:
        return (width * 3 + 3) & ~(size_t)3;
    case BJ_PIXEL_MODE_XRGB8888:
        return width * 4;
    default: break;

    }
    return 0;
}
