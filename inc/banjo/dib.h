#pragma once

#include <banjo/api.h>
#include <banjo/array.h>
#include <banjo/error.h>
#include <banjo/stream.h>

#define BJ_DIB_SIGNATURE 0x4D42
#define BJ_DIB_HEADER_SIZE 14
#define BJ_DIB_INFO_HEADER_SIZE 40



#define BJ_DIB_BIT_COUNT_1 (0x01) //!< Monochrome, 1bit per pixel.
#define BJ_DIB_BIT_COUNT_4 (0x04) //!< 4 bits palletized. 16 colors.
#define BJ_DIB_BIT_COUNT_8 (0x08) //!< 8 bits palletized, 256 colors.
#define BJ_DIB_BIT_COUNT_16 (0x10) //!< 16 bits RGB 65536 colors.
#define BJ_DIB_BIT_COUNT_24 (0x18) //!< 24 bits palletized, 16M colors.

#define BJ_DIB_BI_RGB (0x00) //!< No compression.
#define BJ_DIB_BI_RGB8 (0x01) //!< 8bit RLE encoding.
#define BJ_DIB_BI_RGB4 (0x02) //!< 4bit RLE encoding.

typedef struct {
    struct {
        u32 file_size;
        u32 data_offset;
    } header;

    struct {
        u32 width;
        u32 height;
        u16 planes;
        u16 bit_count;
        u32 compression;
        u32 image_size;
        u32 x_pixels_per_m;
        u32 y_pixels_per_m;
        u32 colors_used;
        u32 colors_important;
    } info_header;

    bj_array p_color_table;
} bj_dib;

BANJO_EXPORT bj_dib* bj_dib_init_from_file(
    bj_dib* p_dib,
    const char* p_path,
    bj_error* p_error
);

BANJO_EXPORT bj_dib* bj_dib_reset(
    bj_dib* p_dib
);


