#include "banjo/array.h"

#include "dib.h"

#include <banjo/log.h>
#include <banjo/stream.h>

// Source: https://gibberlings3.github.io/iesdp/file_formats/ie_formats/bmp.htm

void dib_read_file_header(dib_file_header* p_file_header, const u8* buffer, bj_error* p_error) {
    bj_stream* p_stream = bj_new(stream, read, buffer, BJ_DIB_HEADER_SIZE);

    u16 signature = 0;
    bj_stream_read_t(p_stream, u16, &signature);

    if (signature != BJ_DIB_SIGNATURE) {
        bj_set_error(p_error, BJ_DOMAIN_IO, BJ_INVALID_FORMAT);
        return;
    }

    bj_stream_read_t(p_stream, u32, &p_file_header->file_size);
    bj_stream_skip_t(p_stream, u32);
    bj_stream_read_t(p_stream, u32, &p_file_header->data_offset);
    bj_del(stream, p_stream);
}

void dib_read_info_header(dib_info_header* p_info_header, const u8* buffer, bj_error* p_error) {
    bj_stream* p_stream = bj_new(stream, read, buffer, BJ_DIB_HEADER_SIZE);

    u32 info_header_size = 0;
    bj_stream_read_t(p_stream, u32, &info_header_size);
    if (info_header_size != BJ_DIB_INFO_HEADER_SIZE) {
        bj_set_error(p_error, BJ_DOMAIN_IO, BJ_INVALID_FORMAT);
        return;
    }

    bj_stream_read_t(p_stream, u32, &p_info_header->width);
    bj_stream_read_t(p_stream, u32, &p_info_header->height);

    bj_stream_read_t(p_stream, u16, &p_info_header->planes);
#ifdef BANJO_PEDANTIC
    if (p_info_header->planes != 0x01) { // Planes
        bj_set_error(p_error, BJ_DOMAIN_IO, BJ_INVALID_FORMAT);
        return;
    }
#endif

    bj_stream_read_t(p_stream, u16, &p_info_header->bit_count);
    switch(p_info_header->bit_count) {
        case BJ_DIB_BIT_COUNT_1:
        case BJ_DIB_BIT_COUNT_4:
        case BJ_DIB_BIT_COUNT_8:
        case BJ_DIB_BIT_COUNT_16:
        case BJ_DIB_BIT_COUNT_24:
            break;
        default:
            bj_set_error(p_error, BJ_DOMAIN_IO, BJ_INVALID_FORMAT);
            return;
    }

    bj_stream_read_t(p_stream, u32, &p_info_header->compression);
    switch(p_info_header->compression) {
        case BJ_DIB_BI_RGB:
        case BJ_DIB_BI_RGB8:
        case BJ_DIB_BI_RGB4:
            break;
        default:
            bj_set_error(p_error, BJ_DOMAIN_IO, BJ_INVALID_FORMAT);
            return;
    }

    bj_stream_read_t(p_stream, u32, &p_info_header->image_size);
    bj_stream_read_t(p_stream, u32, &p_info_header->x_pixels_per_m);
    bj_stream_read_t(p_stream, u32, &p_info_header->y_pixels_per_m);
    bj_stream_read_t(p_stream, u32, &p_info_header->colors_used);
    bj_stream_read_t(p_stream, u32, &p_info_header->colors_important);

    bj_del(stream, p_stream);
}

void dib_read_color_table(bj_array* p_color_table, const u8* buffer, bj_error* p_error) {

}


/* static void read_color_table(bj_stream* p_stream, bj_dib* p_dib, bj_error* p_error) { */
/*     usize n_colors = 0; */
/*     switch(p_dib->info_header.bit_count) { */
/*         case BJ_DIB_BIT_COUNT_1: */
/*             n_colors = 2; */
/*             break; */
/*         case BJ_DIB_BIT_COUNT_4: */
/*             n_colors = 16; */
/*             break; */
/*         case BJ_DIB_BIT_COUNT_8: */
/*             n_colors = 256; */
/*             break; */
/*         default: */
/*             return; */
/*     } */

/*     bj_array* p_color_table = &p_dib->color_table; */

/*     const usize n_bytes = n_colors * 3; */
/*     bj_array_init_with_capacity_t(p_color_table, byte, n_bytes); */

/*     for(usize i = 0 ; i < n_colors ; ++i) { */
/*         byte value = 0; */
/*         for(usize c = 0 ; c < 3 ; ++c) { */
/*             bj_stream_read_t(p_stream, byte, &value); */
/*             bj_array_push(p_color_table, &value); */
/*         } */
/*         bj_stream_skip_t(p_stream, byte); */
/*     } */
/* } */

