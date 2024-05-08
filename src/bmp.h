#pragma once

#include <banjo/error.h>
#include <banjo/api.h>

#define BMP_SIGNATURE 0x4D42
#define BMP_HEADER_SIZE 14
#define BMP_INFO_HEADER_SIZE 40

typedef struct {
    u16 file_size;
    u16 data_offset;
} bmp_header;

typedef enum {
    BIT_COUNT_UNKNOWN = 0x00,
    BIT_COUNT_1       = 0x01, // Monochrome, 1bit per pixel.
    BIT_COUNT_4       = 0x04, // 4 bits palletized. 16 colors.
    BIT_COUNT_8       = 0x08, // 8 bits palletized, 256 colors.
    BIT_COUNT_16      = 0x10, // 16 bits RGB 65536 colors.
    BIT_COUNT_24      = 0x18, // 24 bits palletized, 16M colors.
} bmp_bit_count ;

typedef enum {
    BI_RGB  = 0x00, // No compression.
    BI_RGB8 = 0x01, // 8bit RLE encoding.
    BI_RGB4 = 0x02, // 4bit RLE encoding.
} bmp_compression;

typedef struct {
    u32             width;
    u32             height;
    bmp_bit_count   bit_count;
    bmp_compression compression;
} bmp_info_header;

void bmp_read_header(const u8* buffer, bmp_header* header, bj_error* p_error);
void bmp_read_info_header(const u8* buffer, bmp_info_header* header, bj_error* p_error);


