#include "banjo/array.h"

#include "dib.h"

#include <banjo/log.h>
#include <banjo/stream.h>

// Source: https://gibberlings3.github.io/iesdp/file_formats/ie_formats/bmp.htm

void dib_read_file_header(dib_file_header* p_file_header, const u8* buffer, bj_error** p_error) {
    bj_stream* p_stream = bj_new(stream, read, buffer, BJ_DIB_HEADER_SIZE);

    u16 signature = 0;
    bj_stream_read_t(p_stream, u16, &signature);

    if (signature != BJ_DIB_SIGNATURE) {
        bj_set_error(p_error, BJ_ERROR_INCORRECT_VALUE, "Invalid BMP signature. Only 'BM' is supported");
        return;
    }

    bj_stream_read_t(p_stream, u32, &p_file_header->file_size);
    bj_stream_skip_t(p_stream, u32);
    bj_stream_read_t(p_stream, u32, &p_file_header->data_offset);
    bj_del(stream, p_stream);
}

void dib_read_info_header(dib_info_header* p_info_header, const u8* buffer, bj_error** p_error) {
    bj_stream* p_stream = bj_new(stream, read, buffer, BJ_DIB_INFO_HEADER_SIZE);

    u32 info_header_size = 0;
    bj_stream_read_t(p_stream, u32, &info_header_size);
    if (info_header_size != BJ_DIB_INFO_HEADER_SIZE) {
        bj_set_error(p_error, BJ_ERROR_INCORRECT_VALUE, "Unsupported BMP Header. Only 'BITMAPINFOHEADER' is supported");
        return;
    }

    bj_stream_read_t(p_stream, u32, &p_info_header->width);
    bj_stream_read_t(p_stream, u32, &p_info_header->height);

    bj_stream_read_t(p_stream, u16, &p_info_header->planes);
#ifdef BJ_FEAT_PEDANTIC_ENABLED
    if (p_info_header->planes != 0x01) { // Planes
        bj_set_error(p_error, BJ_ERROR_INCORRECT_VALUE, "Invalid BMP planes number");
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
            bj_set_error(p_error, BJ_ERROR_INCORRECT_VALUE, "Unknown bit count");
            return;
    }

    bj_stream_read_t(p_stream, u32, &p_info_header->compression);
    switch(p_info_header->compression) {
        case BJ_DIB_BI_RGB:
        case BJ_DIB_BI_RGB8:
        case BJ_DIB_BI_RGB4:
            break;
        default:
            bj_set_error(p_error, BJ_ERROR_INCORRECT_VALUE, "Unknown compression mode");
            return;
    }

    bj_stream_read_t(p_stream, u32, &p_info_header->image_size);
    bj_stream_read_t(p_stream, u32, &p_info_header->x_pixels_per_m);
    bj_stream_read_t(p_stream, u32, &p_info_header->y_pixels_per_m);
    bj_stream_read_t(p_stream, u32, &p_info_header->colors_used);
    bj_stream_read_t(p_stream, u32, &p_info_header->colors_important);

    bj_del(stream, p_stream);
}

usize dib_color_table_len(const dib_info_header* p_info_header) {
    if(p_info_header->colors_used == 0) {
        switch(p_info_header->bit_count) {
            case BJ_DIB_BIT_COUNT_1: return 2;
            case BJ_DIB_BIT_COUNT_4: return 16;
            case BJ_DIB_BIT_COUNT_8: return 256;
            default: break;
        }
        return 0;
    }
    return p_info_header->colors_used;
}

usize dib_color_table_memsize(const dib_info_header* p_info_header) {
    return dib_color_table_len(p_info_header) * sizeof(u8)*4;
}


void dib_read_color_table(bj_array* p_color_table, const u8* buffer, usize n_colors, bj_error** p_error) {
    bj_array_reserve(p_color_table, n_colors);

    bj_with(stream, p_stream, read, buffer, n_colors * 4) {
        for(usize c = 0 ; c < n_colors ; ++c) {
            table_color color;
            bj_stream_read_t(p_stream, byte, &color.red);
            bj_stream_read_t(p_stream, byte, &color.green);
            bj_stream_read_t(p_stream, byte, &color.blue);
            bj_stream_skip_t(p_stream, byte);
            bj_array_push(p_color_table, &color);
        }
    }
}

void dib_read_raster(bj_bitmap* p_bmp, const dib_info_header* p_info_header, const bj_array* p_color_table, bj_error** p_error) {
    for(usize x = 0 ; x < p_bmp->width ; ++x) {
        for(usize y = 0 ; y < p_bmp->height ; ++y) {
            bj_bitmap_put(p_bmp, x, y, BJ_COLOR_CRIMSON);
        }
    }
}
