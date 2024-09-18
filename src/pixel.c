#include <banjo/log.h>

#include "pixel_t.h"

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

BANJO_EXPORT uint32_t bj_pixel_value(
    bj_pixel_mode mode, 
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
