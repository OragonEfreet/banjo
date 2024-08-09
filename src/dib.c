#include "dib.h"

#include "oldbmp_t.h"

#include <banjo/log.h>
#include <banjo/memory.h>
#include <banjo/pixel.h>
#include <banjo/stream.h>

#include <assert.h>
#include <stdio.h>

#define _ABS(x) ((x) < 0 ? -(x) : (x))

void dib_read_file_header(dib_file_header* p_file_header, const u8* buffer, bj_error** p_error) {
    bj_stream* p_stream = bj_stream_new_read(buffer, BJ_DIB_HEADER_SIZE);

    u16 signature = 0;
    bj_stream_read_t(p_stream, u16, &signature);

    if (signature != BJ_DIB_SIGNATURE) {
        bj_set_error(p_error, BJ_ERROR_INCORRECT_VALUE, "Invalid BMP signature. Only 'BM' is supported");
        bj_stream_del(p_stream);
        return;
    }

    bj_stream_read_t(p_stream, u32, &p_file_header->file_size);
    bj_stream_skip_t(p_stream, u32);
    bj_stream_read_t(p_stream, u32, &p_file_header->data_offset);

    bj_stream_del(p_stream);
}

static void dib_read_info_header(bj_stream* p_stream, dib_info_header* p_info_header, bj_error** p_error) {
    u32 info_header_size = 0;
    bj_stream_read_t(p_stream, u32, &info_header_size);

    if (info_header_size != BJ_DIB_INFO_HEADER_SIZE) {
        bj_set_error(p_error, BJ_ERROR_INCORRECT_VALUE, "Unsupported BMP Header. Only 'BITMAPINFOHEADER' is supported");
        return;
    }

    bj_stream_read_t(p_stream, u32, &p_info_header->width);
    bj_stream_read_t(p_stream, i32, &p_info_header->height);
    bj_stream_read_t(p_stream, u16, &p_info_header->planes);

#ifdef BJ_FEAT_PEDANTIC_ENABLED
    if (p_info_header->planes != 0x01) {
        bj_set_error(p_error, BJ_ERROR_INCORRECT_VALUE, "Invalid BMP planes number");
        return;
    }
#endif

    bj_stream_read_t(p_stream, u16, &p_info_header->bit_count);
    switch (p_info_header->bit_count) {
        case BJ_DIB_BIT_COUNT_1:
        case BJ_DIB_BIT_COUNT_4:
        case BJ_DIB_BIT_COUNT_8:
        case BJ_DIB_BIT_COUNT_16:
        case BJ_DIB_BIT_COUNT_24:
        case BJ_DIB_BIT_COUNT_32:
            break;
        default:
            bj_set_error(p_error, BJ_ERROR_INCORRECT_VALUE, "Unknown bit count");
            return;
    }

    bj_stream_read_t(p_stream, u32, &p_info_header->compression);
    if (p_info_header->compression != BJ_DIB_BI_RGB) {
        bj_set_error(p_error, BJ_ERROR_INCORRECT_VALUE, "Compression is currently unsupported");
        return;
    }

    bj_stream_read_t(p_stream, u32, &p_info_header->image_size);
    bj_stream_read_t(p_stream, u32, &p_info_header->x_pixels_per_m);
    bj_stream_read_t(p_stream, u32, &p_info_header->y_pixels_per_m);
    bj_stream_read_t(p_stream, u32, &p_info_header->colors_used);
    bj_stream_read_t(p_stream, u32, &p_info_header->colors_important);
}

static usize dib_color_table_len(const dib_info_header* p_info_header) {
    if (p_info_header->colors_used == 0) {
        switch (p_info_header->bit_count) {
            case BJ_DIB_BIT_COUNT_1: return 2;
            case BJ_DIB_BIT_COUNT_4: return 16;
            case BJ_DIB_BIT_COUNT_8: return 256;
            default: return 0;
        }
    }
    return p_info_header->colors_used;
}

static void dib_read_color_table(bj_stream* p_stream, bj_array* p_color_table, usize n_colors, bj_error** p_error) {
    bj_array_init(p_color_table, sizeof(table_color), n_colors);

    for (usize c = 0; c < n_colors; ++c) {
        table_color color;
        bj_stream_read_t(p_stream, byte, &color.blue);
        bj_stream_read_t(p_stream, byte, &color.green);
        bj_stream_read_t(p_stream, byte, &color.red);
        bj_stream_skip_t(p_stream, byte);
        bj_array_push(p_color_table, &color);
    }
}

usize dib_row_size(const dib* p_dib) {
    return ((((p_dib->info_header.width * p_dib->info_header.bit_count) + 31) & ~31) >> 3);
}

usize dib_pixel_array_size(const dib* p_dib) {
    return dib_row_size(p_dib) * _ABS(p_dib->info_header.height);
}

static void dib_read(dib* p_dib, const u8* buffer, usize buffer_size, usize raster_offset, bj_error** p_error) {
    assert(p_dib != 0);
    bj_memset(p_dib, 0, sizeof(dib));

    bj_stream* p_stream = bj_stream_new_read(buffer, buffer_size);

    // Read the Info header block
    bj_error* p_inner_error = 0;
    dib_read_info_header(p_stream, &p_dib->info_header, &p_inner_error);
    if (p_inner_error) {
        bj_stream_del(p_stream);
        bj_forward_error(p_inner_error, p_error);
        return;
    }

    usize n_colors = dib_color_table_len(&p_dib->info_header);
    dib_read_color_table(p_stream, &p_dib->color_table, n_colors, &p_inner_error);
    if (p_inner_error) {
        bj_stream_del(p_stream);
        bj_forward_error(p_inner_error, p_error);
        return;
    }

    bj_stream_seek(p_stream, raster_offset, BJ_SEEK_BEGIN);
    const usize raster_size = dib_pixel_array_size(p_dib);

    if (buffer_size != raster_offset + raster_size) {
        bj_stream_del(p_stream);
        bj_set_error(p_error, BJ_ERROR_INVALID_FORMAT, "Unexpected raster size in DIB data");
        return;
    }

    // TODO Remove this when we support everything
    if(p_dib->info_header.bit_count != BJ_DIB_BIT_COUNT_24) {
        bj_stream_del(p_stream);
        bj_set_error(p_error, BJ_ERROR_UNSUPPORTED, "Only 24bpp bitmaps are supported for now");
        return;
    }

    // Read raster data
    p_dib->storage  = bj_malloc(raster_size);
    bj_stream_read(p_stream, p_dib->storage, raster_size);

    bj_stream_del(p_stream);
}

void dib_read_file(dib* p_dib, const char* p_path, bj_error** p_error) {
    FILE* fstream  = fopen(p_path, "rb");
    if (!fstream ) {
        bj_set_error(p_error, BJ_ERROR_FILE_NOT_FOUND, "Cannot open BMP file");
        return;
    }

    // Allocate enough memory for the buffer
    u8* buffer = bj_malloc(BJ_DIB_HEADER_SIZE);
    if(buffer == 0) {
        bj_set_error(p_error, BJ_ERROR_CANNOT_ALLOCATE, "Cannot allocate buffer");
        bj_free(buffer);
        fclose(fstream);
        return;
    }

    // Fill in the buffer with the content that corresponds to the file header
    size_t bytes_read = fread(buffer, 1, BJ_DIB_HEADER_SIZE, fstream);
    if(bytes_read != BJ_DIB_HEADER_SIZE) {
        bj_set_error(p_error, BJ_ERROR_INVALID_FORMAT, "BMP File does not meet expected size");
        bj_free(buffer);
        fclose(fstream);
        return;
    }


    bj_error* p_inner_error = 0;

    // Read the file header
    dib_file_header file_header;
    dib_read_file_header(&file_header, buffer, &p_inner_error);
    if(p_inner_error) {
        bj_free(buffer);
        fclose(fstream);
        bj_forward_error(p_inner_error, p_error);
        return;
    }
    bj_free(buffer);

    usize raster_offset = file_header.data_offset - bytes_read;

    // Get enough memory to read the rest of the file
    const usize dib_size = file_header.file_size - BJ_DIB_HEADER_SIZE;
    buffer = bj_malloc(dib_size); 
    if(buffer == 0) {
        bj_set_error(p_error, BJ_ERROR_CANNOT_ALLOCATE, "Cannot allocate buffer");
        fclose(fstream);
        return;
    }

    // Read the rest of the file into the buffer
    bytes_read = fread(buffer, 1, dib_size, fstream);
    fclose(fstream);
    if(bytes_read != dib_size) {
        bj_set_error(p_error, BJ_ERROR_INVALID_FORMAT, "BMP File does not meet expected size");
        bj_free(buffer);
        return;
    }

    // Parse the buffer
    p_inner_error = 0;
    dib_read(p_dib, buffer, dib_size, raster_offset, &p_inner_error);
    if (p_inner_error) {
        bj_forward_error(p_inner_error, p_error);
    }
    bj_free(buffer);
}

int dib_pixel_format(dib* p_dib) {
    if(p_dib->info_header.bit_count == BJ_DIB_BIT_COUNT_24) {
        return BJ_PIXEL_FORMAT_BGR24;
    }
    return BJ_PIXEL_FORMAT_UNKNOWN;
}

void dib_reset(dib* p_dib) {
    bj_array_reset(&p_dib->color_table);
    bj_free(p_dib->storage);
    bj_memset(p_dib, 0, sizeof(dib));
}


