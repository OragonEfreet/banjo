#include "banjo/array.h"
#include <banjo/dib.h>

#include <banjo/log.h>
#include <banjo/stream.h>

#include <stdio.h>

// Source: https://gibberlings3.github.io/iesdp/file_formats/ie_formats/bmp.htm

static void read_header(bj_stream* p_stream, bj_dib* p_dib, bj_error* p_error) {
    u16 signature = 0;
    bj_stream_read_t(p_stream, u16, &signature);

    if (signature != BJ_DIB_SIGNATURE) {
        bj_set_error(p_error, BJ_DOMAIN_IO, BJ_INVALID_FORMAT);
        return;
    }

    bj_stream_read_t(p_stream, u32, &p_dib->header.file_size);
    bj_stream_skip_t(p_stream, u32);
    bj_stream_read_t(p_stream, u32, &p_dib->header.data_offset);
}

static void read_info_header(bj_stream* p_stream, bj_dib* p_dib, bj_error* p_error) {
    u32 info_header_size = 0;
    bj_stream_read_t(p_stream, u32, &info_header_size);
    if (info_header_size != BJ_DIB_INFO_HEADER_SIZE) {
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

    bj_stream_read_t(p_stream, u32, &p_dib->info_header.compression);
    switch(p_dib->info_header.compression) {
        case BJ_DIB_BI_RGB:
        case BJ_DIB_BI_RGB8:
        case BJ_DIB_BI_RGB4:
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

static void read_color_table(bj_stream* p_stream, bj_dib* p_dib, bj_error* p_error) {
    usize n_colors = 0;
    switch(p_dib->info_header.bit_count) {
        case BJ_DIB_BIT_COUNT_1:
            n_colors = 2;
            break;
        case BJ_DIB_BIT_COUNT_4:
            n_colors = 16;
            break;
        case BJ_DIB_BIT_COUNT_8:
            n_colors = 256;
            break;
        default:
            return;
    }

    bj_array* p_color_table = &p_dib->color_table;

    const usize n_bytes = n_colors * 3;
    bj_array_init_with_capacity_t(p_color_table, byte, n_bytes);

    for(usize i = 0 ; i < n_colors ; ++i) {
        byte value = 0;
        for(usize c = 0 ; c < 3 ; ++c) {
            bj_stream_read_t(p_stream, byte, &value);
            bj_array_push(p_color_table, &value);
        }
        bj_stream_skip_t(p_stream, byte);
    }
}

bj_dib* bj_dib_init_from_file(
    bj_dib* p_dib,
    const char* p_path,
    bj_error* p_error
) {
    bj_memset(p_dib, 0, sizeof(bj_dib));

    FILE* bmp_file = fopen(p_path, "rb");
    if (!bmp_file) {
        bj_set_error(p_error, BJ_DOMAIN_IO, BJ_CANNOT_OPEN_FILE);
        return p_dib;
    }

    fseek(bmp_file, 0, SEEK_END);
    usize len = ftell(bmp_file);
    rewind(bmp_file);

    u8* buffer = bj_malloc(len);
    if(buffer == 0) {
        fclose(bmp_file);
        bj_set_error(p_error, BJ_DOMAIN_SYSTEM, BJ_CANNOT_ALLOCATE);
        return p_dib;
    }

    fread(buffer, len, 1, bmp_file);
    bj_stream* stream = bj_new(stream, read, buffer, len);

    read_header(stream, p_dib, p_error);
    read_info_header(stream, p_dib, p_error);
    read_color_table(stream, p_dib, p_error);

#ifdef BANJO_PEDANTIC
    if(p_dib->header.data_offset != bj_stream_tell(stream)) {
        fclose(bmp_file);
        bj_set_error(p_error, BJ_DOMAIN_IO, BJ_INVALID_FORMAT);
        return p_dib;
    }
#endif

    bj_del(stream, stream);

    fclose(bmp_file);
    bj_free(buffer);

    return p_dib;
}

BANJO_EXPORT bj_dib* bj_dib_reset(
    bj_dib* p_dib
) {
    bj_array_reset(&p_dib->color_table);
    bj_memset(p_dib, 0, sizeof(bj_dib));
    return p_dib;
}
