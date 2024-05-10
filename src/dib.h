#pragma once

#include <banjo/error.h>
#include <banjo/api.h>

#define DIB_SIGNATURE 0x4D42
#define DIB_HEADER_SIZE 14
#define DIB_INFO_HEADER_SIZE 40

typedef struct {
    u32 file_size;
    u32 data_offset;
} dib_file_header;

typedef enum {
    BIT_COUNT_UNKNOWN = 0x00,
    BIT_COUNT_1       = 0x01, // Monochrome, 1bit per pixel.
    BIT_COUNT_4       = 0x04, // 4 bits palletized. 16 colors.
    BIT_COUNT_8       = 0x08, // 8 bits palletized, 256 colors.
    BIT_COUNT_16      = 0x10, // 16 bits RGB 65536 colors.
    BIT_COUNT_24      = 0x18, // 24 bits palletized, 16M colors.
} dib_bit_count ;

typedef enum {
    BI_RGB  = 0x00, // No compression.
    BI_RGB8 = 0x01, // 8bit RLE encoding.
    BI_RGB4 = 0x02, // 4bit RLE encoding.
} dib_compression;

typedef struct {
    u32             width;
    u32             height;
    dib_bit_count   bit_count;
    dib_compression compression;
} dib_info_header;

void dib_read_header(const u8* buffer, dib_file_header* header, bj_error* p_error);
void dib_read_info_header(const u8* buffer, dib_info_header* header, bj_error* p_error);


