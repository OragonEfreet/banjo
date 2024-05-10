#include "dib.h"

#include <banjo/log.h>
#include <banjo/stream.h>

// Source: https://gibberlings3.github.io/iesdp/file_formats/ie_formats/bmp.htm

void dib_read_header(const u8* buffer, dib_file_header* header, bj_error* p_error) {

    bj_stream* stream = bj_new(stream, read, buffer, DIB_HEADER_SIZE);

    // Read signature
        u16 signature = 0;
        bj_stream_read_t(stream, u16, &signature);

        if (signature != DIB_SIGNATURE) {
            bj_set_error(p_error, BJ_DOMAIN_IO, BJ_INVALID_FORMAT);
            return;
        }

        bj_stream_read_t(stream, u32, &header->file_size);
        bj_stream_skip_t(stream, u32);
        bj_stream_read_t(stream, u32, &header->data_offset);

        bj_del(stream, stream);
}

void dib_read_info_header(const u8* buffer, dib_info_header* info_header, bj_error* p_error) {

#ifdef BANJO_PEDANTIC
    if (*(u32*)buffer != DIB_INFO_HEADER_SIZE) {
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

