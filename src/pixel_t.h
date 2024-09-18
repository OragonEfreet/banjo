#pragma once

#include <banjo/pixel.h>

#define BJ_PIXEL_TYPE_INDEX    0x01
#define BJ_PIXEL_TYPE_BITFIELD 0x02
#define BJ_PIXEL_TYPE_BYTES    0x03

#define BJ_PIXEL_ORDER_RGB  0x01
#define BJ_PIXEL_ORDER_XRGB BJ_PIXEL_ORDER_RGB
#define BJ_PIXEL_ORDER_BGR  0x02
#define BJ_PIXEL_ORDER_XBGR BJ_PIXEL_ORDER_BGR
#define BJ_PIXEL_ORDER_RGBX 0x03
#define BJ_PIXEL_ORDER_BGRX 0x04
#define BJ_PIXEL_ORDER_ARGB 0x05
#define BJ_PIXEL_ORDER_ABGR 0x06
#define BJ_PIXEL_ORDER_RGBA 0x07
#define BJ_PIXEL_ORDER_BGRA 0x08

#define BJ_PIXEL_LAYOUT_1555 0x00
#define BJ_PIXEL_LAYOUT_8888 0x01
#define BJ_PIXEL_LAYOUT_565  0x02

// Builders
#define BJ_PIXEL_MODE_MAKE(bpp, type, layout, order) (((order & 0xFF) << 24) | ((layout & 0xFF) << 16) | ((type & 0xFF) << 8) | (bpp & 0xFF))
#define BJ_PIXEL_MODE_MAKE_INDEXED(bpp)               BJ_PIXEL_MODE_MAKE(bpp, BJ_PIXEL_TYPE_INDEX, 0, 0)
#define BJ_PIXEL_MODE_MAKE_BITFIELD_16(layout, order) BJ_PIXEL_MODE_MAKE(16, BJ_PIXEL_TYPE_BITFIELD, layout, order)
#define BJ_PIXEL_MODE_MAKE_BITFIELD_32(layout, order) BJ_PIXEL_MODE_MAKE(32, BJ_PIXEL_TYPE_BITFIELD, layout, order)
#define BJ_PIXEL_MODE_MAKE_BYTES(bpp, order)          BJ_PIXEL_MODE_MAKE(bpp, BJ_PIXEL_TYPE_BYTES, 0, order)

// Extractor
#define BJ_PIXEL_GET_BPP(fmt)    ((fmt) & 0xFF)
#define BJ_PIXEL_GET_TYPE(fmt)   (((fmt) >> 8) & 0xFF)
#define BJ_PIXEL_GET_LAYOUT(fmt) (((fmt) >> 16) & 0xFF)
#define BJ_PIXEL_GET_ORDER(fmt)  (((fmt) >> 24) & 0xFF)

