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

typedef struct {
    u32 width;
    u32 height;
} bmp_info_header;

void bmp_read_header(const u8* buffer, bmp_header* header, bj_error* p_error);
void bmp_read_info_header(const u8* buffer, bmp_info_header* header, bj_error* p_error);


