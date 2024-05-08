#include "dib.h"

#include <banjo/log.h>

// Source: https://gibberlings3.github.io/iesdp/file_formats/ie_formats/bmp.htm

void dib_read_header(const u8* buffer, dib_file_header* header, bj_error* p_error) {

// Read signature
#ifdef BANJO_PEDANTIC
    if (*(u16*)buffer != BMP_SIGNATURE) {
        bj_set_error(p_error, BJ_DOMAIN_IO, BJ_INVALID_FORMAT);
        return;
    }
#endif
    buffer += sizeof(u16);

    header->file_size = *(u32*)buffer;  buffer += sizeof(u32);
    buffer += sizeof(u32); // Skip reserved
    header->data_offset = *(u32*)buffer;
}

void dib_read_info_header(const u8* buffer, dib_info_header* info_header, bj_error* p_error) {

#ifdef BANJO_PEDANTIC
    if (*(u32*)buffer != BMP_INFO_HEADER_SIZE) {
        bj_set_error(p_error, BJ_DOMAIN_IO, BJ_INVALID_FORMAT);
        return;
    }
#endif
    buffer += sizeof(u32);

    info_header->width  = *(u32*)buffer;  buffer += sizeof(u32);
    info_header->height = *(u32*)buffer;  buffer += sizeof(u32);

#ifdef BANJO_PEDANTIC
    if (*(u16*)buffer != 0x01) { // Planes
        bj_set_error(p_error, BJ_DOMAIN_IO, BJ_INVALID_FORMAT);
        return;
    }
#endif
    buffer += sizeof(u16);

    info_header->bit_count  = *(u16*)buffer;  buffer += sizeof(u16);
    switch(info_header->bit_count) {
        case BIT_COUNT_1:
        case BIT_COUNT_4:
        case BIT_COUNT_8:
        case BIT_COUNT_16:
        case BIT_COUNT_24:
            break;
        default:
            bj_set_error(p_error, BJ_DOMAIN_IO, BJ_INVALID_FORMAT);
            return;
    }

    info_header->compression  = *(u32*)buffer;  buffer += sizeof(u32);
    switch(info_header->compression) {
        case BI_RGB:
        case BI_RGB8:
        case BI_RGB4:
            break;
        default:
            bj_set_error(p_error, BJ_DOMAIN_IO, BJ_INVALID_FORMAT);
            return;
    }



}

