#include <banjo/array.h>
#include <banjo/log.h>
#include <banjo/stream.h>

#include "dib.h"

#define _ABS(x) ((x) < 0 ? -(x) : (x))
#define _TOP_DOWN(x) ((x) < 0)

void dib_read_file_header(dib_file_header* p_file_header, const u8* buffer, bj_error** p_error) {
    bj_stream* p_stream = bj_new(stream, read, buffer, BJ_DIB_HEADER_SIZE);

    u16 signature = 0;
    bj_stream_read_t(p_stream, u16, &signature);

    if (signature != BJ_DIB_SIGNATURE) {
        bj_set_error(p_error, BJ_ERROR_INCORRECT_VALUE, "Invalid BMP signature. Only 'BM' is supported");
        bj_del(stream, p_stream);
        return;
    }

    bj_stream_read_t(p_stream, u32, &p_file_header->file_size);
    bj_stream_skip_t(p_stream, u32);
    bj_stream_read_t(p_stream, u32, &p_file_header->data_offset);
    bj_del(stream, p_stream);
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

static void dib_read_color_table(bj_stream* p_stream, bj_array* p_color_table, usize n_colors, bj_error** p_error) {
    bj_array_init_with_capacity_t(p_color_table, table_color, n_colors);

    for (usize c = 0; c < n_colors; ++c) {
        table_color color;
        bj_stream_read_t(p_stream, byte, &color.blue);
        bj_stream_read_t(p_stream, byte, &color.green);
        bj_stream_read_t(p_stream, byte, &color.red);
        bj_stream_skip_t(p_stream, byte);
        bj_array_push(p_color_table, &color);
    }
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

static void dib_read(dib* p_dib, const u8* buffer, usize buffer_size, usize offset_check, bj_error** p_error) {
    bj_stream* p_stream = bj_new(stream, read, buffer, buffer_size);

    bj_error* p_inner_error = 0;
    dib_read_info_header(p_stream, &p_dib->info_header, &p_inner_error);
    if (p_inner_error) {
        bj_del(stream, p_stream);
        bj_forward_error(p_inner_error, p_error);
        return;
    }

    usize n_colors = dib_color_table_len(&p_dib->info_header);
    dib_read_color_table(p_stream, &p_dib->color_table, n_colors, &p_inner_error);
    if (p_inner_error) {
        bj_array_reset(&p_dib->color_table);
        bj_del(stream, p_stream);
        bj_forward_error(p_inner_error, p_error);
        return;
    }

    if (bj_stream_tell(p_stream) != offset_check) {
        bj_set_error(p_error, BJ_ERROR_INVALID_FORMAT, "Unexpected DIB size");
        bj_array_reset(&p_dib->color_table);
        bj_del(stream, p_stream);
        return;
    }

    bj_del(stream, p_stream);
}

static usize dib_uncompressed_stride(u32 width, u16 bit_count) {
    return ((((width * bit_count) + 31) & ~31) >> 3);
}

static void dib_read_raster_1bpp(bj_bitmap* p_bmp, const u8* buffer, usize row_size, const dib* p_dib, bj_error** p_error) {
    bj_color palette[2] = { BJ_COLOR_BLACK, BJ_COLOR_WHITE };
    usize palette_size = bj_array_len(&p_dib->color_table);
    if (palette_size >= 2) {
        const table_color* color = bj_array_at(&p_dib->color_table, 0);
        palette[0] = BJ_RGB(color->red, color->green, color->blue);
        color = bj_array_at(&p_dib->color_table, 1);
        palette[1] = BJ_RGB(color->red, color->green, color->blue);
    }

    const u8* row = buffer;
    for (usize y = 0; y < p_bmp->height; ++y) {
        const usize py = p_dib->info_header.height < 0 ? y : p_bmp->height - y - 1;
        const u8* pixel8_data = row;

        usize x = 0;
        while (x < p_bmp->width) {
            u8 byte = *pixel8_data++;
            for (usize b = 0; b < 8 && x < p_bmp->width; ++b, ++x) {
                bj_bitmap_put(p_bmp, x, py, palette[(byte >> (7 - b)) & 0x01]);
            }
        }
        row += row_size;
    }
}

static void dib_read_raster_4bpp(bj_bitmap* p_bmp, const u8* buffer, usize row_size, const dib* p_dib, bj_error** p_error) {
    bj_color palette[16] = { BJ_COLOR_BLACK };
    usize palette_size = bj_array_len(&p_dib->color_table);
    for(usize c = 0 ; c < palette_size ; ++c) {
        const table_color* color = bj_array_at(&p_dib->color_table, c);
        palette[c] = BJ_RGB(color->red, color->green, color->blue);
    }

    const u8* row = buffer;
    for (usize y = 0; y < p_bmp->height; ++y) {
        const usize py = p_dib->info_header.height < 0 ? y : p_bmp->height - y - 1;
        const u8* pixel2_data = row;

        usize x = 0;
        while (x < p_bmp->width) {
            u8 byte = *pixel2_data++;
            bj_bitmap_put(p_bmp, x++, py, palette[(byte >> 4) & 0x0F]);
            bj_bitmap_put(p_bmp, x++, py, palette[byte & 0x0F]);
        }
        row += row_size;
    }
}

static void dib_read_raster_8bpp(bj_bitmap* p_bmp, const u8* buffer, usize row_size, const dib* p_dib, bj_error** p_error) {
    const u8* row = buffer;
    for (usize y = 0; y < p_bmp->height; ++y) {
        const usize py = p_dib->info_header.height < 0 ? y : p_bmp->height - y - 1;
        const u8* pixel_data = row;
        for (usize x = 0; x < p_bmp->width; ++x) {
            const table_color* color = bj_array_at(&p_dib->color_table, *pixel_data++);
            bj_bitmap_put(p_bmp, x, py, BJ_RGB(color->red, color->green, color->blue));
        }
        row += row_size;
    }
}

static void dib_read_raster_24bpp(bj_bitmap* p_bmp, const u8* buffer, usize row_size, const dib* p_dib, bj_error** p_error) {
    const u8* row = buffer;
    for (usize y = 0; y < p_bmp->height; ++y) {
        const usize py = p_dib->info_header.height < 0 ? y : p_bmp->height - y - 1;
        const u8* pixel_data = row;
        for (usize x = 0; x < p_bmp->width; ++x) {
            bj_bitmap_put(p_bmp, x, py, BJ_RGB(pixel_data[2], pixel_data[1], pixel_data[0]));
            pixel_data += 3;
        }
        row += row_size;
    }
}

static void dib_read_raster_32bpp(bj_bitmap* p_bmp, const u8* buffer, usize row_size, const dib* p_dib, bj_error** p_error) {
    const u8* row = buffer;
    for (usize y = 0; y < p_bmp->height; ++y) {
        const usize py = p_dib->info_header.height < 0 ? y : p_bmp->height - y - 1;
        const u8* pixel_data = row;
        for (usize x = 0; x < p_bmp->width; ++x) {
            bj_bitmap_put(p_bmp, x, py, BJ_RGBA(
                pixel_data[0], pixel_data[1],
                pixel_data[2], pixel_data[3]
            ));
            pixel_data += 4;
        }
        row += row_size;
    }
}

static void dib_read_raster(bj_bitmap* p_bmp, const u8* buffer, usize buffer_size, const dib* p_dib, bj_error** p_error) {
    const usize row_size = dib_uncompressed_stride(p_dib->info_header.width, p_dib->info_header.bit_count);
    switch (p_dib->info_header.bit_count) {
        case BJ_DIB_BIT_COUNT_1:
            dib_read_raster_1bpp(p_bmp, buffer, row_size, p_dib, p_error);
            break;
        case BJ_DIB_BIT_COUNT_4:
            dib_read_raster_4bpp(p_bmp, buffer, row_size, p_dib, p_error);
            break;
        case BJ_DIB_BIT_COUNT_8:
            dib_read_raster_8bpp(p_bmp, buffer, row_size, p_dib, p_error);
            break;
        case BJ_DIB_BIT_COUNT_24:
            dib_read_raster_24bpp(p_bmp, buffer, row_size, p_dib, p_error);
            break;
        case BJ_DIB_BIT_COUNT_32:
            dib_read_raster_32bpp(p_bmp, buffer, row_size, p_dib, p_error);
            break;
        default:
            bj_set_error(p_error, BJ_ERROR_INVALID_FORMAT, "Unsupported DIB raster bit count");
    }
}

void dib_read_bitmap(bj_bitmap* p_bmp, const u8* buffer, usize buffer_size, usize data_offset, bj_error** p_error) {
    dib dib_data = {0};
    bj_error* p_inner_error = 0;

    dib_read(&dib_data, buffer, buffer_size, data_offset, &p_inner_error);
    if (p_inner_error) {
        bj_forward_error(p_inner_error, p_error);
        return;
    }

    const u32 dib_height = _ABS(dib_data.info_header.height);
    bj_bitmap_init_default(p_bmp, dib_data.info_header.width, dib_height);
    bj_bitmap_set_clear_color(p_bmp, BJ_COLOR_BLACK);

    const usize raster_size = dib_uncompressed_stride(dib_data.info_header.width, dib_data.info_header.bit_count) * dib_height;

    if (buffer_size != data_offset + raster_size) {
        bj_array_reset(&dib_data.color_table);
        bj_set_error(p_error, BJ_ERROR_INVALID_FORMAT, "Unexpected raster size in DIB data");
        return;
    }

    dib_read_raster(p_bmp, buffer + data_offset, raster_size, &dib_data, &p_inner_error);
    if (p_inner_error) {
        bj_forward_error(p_inner_error, p_error);
    }

    bj_array_reset(&dib_data.color_table);
}
