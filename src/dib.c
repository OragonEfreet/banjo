#include "dib.h"

#include <banjo/log.h>
#include <banjo/stream.h>

#include <stdio.h>

// Source: https://gibberlings3.github.io/iesdp/file_formats/ie_formats/bmp.htm

static void read_header(bj_stream* p_stream, dib* p_dib, bj_error* p_error) {

    // Read signature
        u16 signature = 0;
        bj_stream_read_t(p_stream, u16, &signature);

        if (signature != DIB_SIGNATURE) {
            bj_set_error(p_error, BJ_DOMAIN_IO, BJ_INVALID_FORMAT);
            return;
        }

        bj_stream_read_t(p_stream, u32, &p_dib->header.file_size);
        bj_stream_skip_t(p_stream, u32);
        bj_stream_read_t(p_stream, u32, &p_dib->header.data_offset);

}

static void read_info_header(bj_stream* p_stream, dib* p_dib, bj_error* p_error) {


    u32 info_header_size = 0;
    bj_stream_read_t(p_stream, u32, &info_header_size);
    if (info_header_size != DIB_INFO_HEADER_SIZE) {
        bj_set_error(p_error, BJ_DOMAIN_IO, BJ_INVALID_FORMAT);
        return;
    }

    bj_stream_read_t(p_stream, u32, &p_dib->info_header.width);
    bj_stream_read_t(p_stream, u32, &p_dib->info_header.height);

    bj_stream_read_t(p_stream, u16, &p_dib->info_header.planes);
#ifdef BANJO_PEDANTIC
    if (p_dib->info_header.planes != 0x01) { // Planes
        bj_set_error(p_error, BJ_DOMAIN_IO, BJ_INVALID_FORMAT);
        return;
    }
#endif

    bj_stream_read_t(p_stream, u16, &p_dib->info_header.bit_count);
    switch(p_dib->info_header.bit_count) {
        case DIB_BIT_COUNT_1:
        case DIB_BIT_COUNT_4:
        case DIB_BIT_COUNT_8:
        case DIB_BIT_COUNT_16:
        case DIB_BIT_COUNT_24:
            break;
        default:
            bj_set_error(p_error, BJ_DOMAIN_IO, BJ_INVALID_FORMAT);
            return;
    }

    bj_stream_read_t(p_stream, u32, &p_dib->info_header.compression);
    switch(p_dib->info_header.compression) {
        case DIB_BI_RGB:
        case DIB_BI_RGB8:
        case DIB_BI_RGB4:
            break;
        default:
            bj_set_error(p_error, BJ_DOMAIN_IO, BJ_INVALID_FORMAT);
            return;
    }

    bj_stream_read_t(p_stream, u32, &p_dib->info_header.image_size);
    bj_stream_read_t(p_stream, u32, &p_dib->info_header.x_pixels_per_m);
    bj_stream_read_t(p_stream, u32, &p_dib->info_header.y_pixels_per_m);
    bj_stream_read_t(p_stream, u32, &p_dib->info_header.colors_used);
    bj_stream_read_t(p_stream, u32, &p_dib->info_header.colors_important);

}

static void read_color_table(bj_stream* p_stream, dib* p_dib, bj_error* p_error) {
    /* usize n_colors = 0; */
    /* switch(p_dib->info_header.bit_count) { */
    /*     case DIB_BIT_COUNT_1: */
    /*         n_colors = 1; */
    /*         break; */
    /*     case DIB_BIT_COUNT_4: */
    /*         n_colors = 16; */
    /*         break; */
    /*     case DIB_BIT_COUNT_8: */
    /*         n_colors = 256; */
    /*         break; */
    /*     default: */
    /*         break; */
    /* } */


    
}

static void dib_read(const u8* buffer, usize buflen, dib* p_dib, bj_error* p_error) {
    bj_stream* stream = bj_new(stream, read, buffer, buflen);

    read_header(stream, p_dib, p_error);
    read_info_header(stream, p_dib, p_error);
    read_color_table(stream, p_dib, p_error);

    bj_del(stream, stream);
}

void dib_read_file(const char* p_path, dib* p_dib, bj_error* p_error) {

    FILE* bmp_file = fopen(p_path, "rb");
    if (!bmp_file) {
        bj_set_error(p_error, BJ_DOMAIN_IO, BJ_CANNOT_OPEN_FILE);
        return;
    }

    fseek(bmp_file, 0, SEEK_END);
    usize len = ftell(bmp_file);
    rewind(bmp_file);

    u8* buffer = bj_malloc(len);
    if(buffer == 0) {
        fclose(bmp_file);
        bj_set_error(p_error, BJ_DOMAIN_SYSTEM, BJ_CANNOT_ALLOCATE);
        return;
    }

    fread(buffer, len, 1, bmp_file);

    dib_read(buffer, len, p_dib, p_error);

    fclose(bmp_file);
    bj_free(buffer);
}
