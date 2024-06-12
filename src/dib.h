#pragma once

#include <banjo/api.h>
#include <banjo/array.h>
#include <banjo/bitmap.h>
#include <banjo/error.h>
#include <banjo/stream.h>

#define BJ_DIB_SIGNATURE 0x4D42
#define BJ_DIB_HEADER_SIZE 14
#define BJ_DIB_INFO_HEADER_SIZE 40

#define BJ_DIB_BIT_COUNT_1 (0x01)  //!< Monochrome, 1bit per pixel.
#define BJ_DIB_BIT_COUNT_4 (0x04)  //!< 4 bits palletized. 16 colors.
#define BJ_DIB_BIT_COUNT_8 (0x08)  //!< 8 bits palletized, 256 colors.
#define BJ_DIB_BIT_COUNT_16 (0x10) //!< 16 bits RGB 65536 colors.
#define BJ_DIB_BIT_COUNT_24 (0x18) //!< 24 bits palletized, 16M colors.
#define BJ_DIB_BIT_COUNT_32 (0x20) //!< 24 bits palletized, 16M colors.

#define BJ_DIB_BI_RGB (0x00) //!< No compression.
#define BJ_DIB_BI_RGB8 (0x01) //!< 8bit RLE encoding.
#define BJ_DIB_BI_RGB4 (0x02) //!< 4bit RLE encoding.

typedef struct {
    u32 file_size;
    u32 data_offset;
} dib_file_header;

typedef struct {
    u32 width;
    i32 height; // Signed because <0 means Y coordinate is upside down
    u16 planes;
    u16 bit_count;
    u32 compression;
    u32 image_size;
    u32 x_pixels_per_m;
    u32 y_pixels_per_m;
    u32 colors_used;
    u32 colors_important;
} dib_info_header;

typedef struct {
    u8 red;
    u8 green;
    u8 blue;
} table_color;

typedef struct {
    dib_info_header info_header;
    bj_array        color_table;
} dib;

void dib_read_file_header(dib_file_header* p_file_header, const u8* buffer, bj_error** p_error);
void dib_read_bitmap(bj_bitmap* p_bmp, const u8* buffer, usize buffer_size, usize data_offset, bj_error** p_error);


