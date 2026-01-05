#include <banjo/assert.h>
#include <banjo/bitmap.h>

#define ERR_MSG                         "unsupported"
#define ERR_MSG_BAD_BIT_COUNT           "unsupported bit count"
#define ERR_MSG_BAD_BMP_SIZE            "incorrect bitmap size"
#define ERR_MSG_BAD_COMPRESSION_TYPE    "unsupported compression type"
#define ERR_MSG_BAD_HEADER_INFO_TYPE    "unsupported header info type"
#define ERR_MSG_BAD_PALETTE_SIZE        "incorrect palette size"
#define ERR_MSG_BAD_PLANES_COUNT        "unsupported planes count (expect 1)"
#define ERR_MSG_BAD_RASTER_OFFSET       "incorrect raster offset"
#define ERR_MSG_BAD_SIGNATURE           "incorrect signature"
#define ERR_MSG_BITFIELDS_BAD_BPP       "bitfields only allowed for 16bpp and 32bpp bitmaps"
#define ERR_MSG_CANNOT_ALLOC_PALETTE    "cannot allocated palette"
#define ERR_MSG_EOS                     "unexpected end of file"
#define ERR_MSG_OVERLAPPING_BITFIELDS   "overlapping bitfields"
#define ERR_MSG_RLE4_BAD_BPP            "rle4 encoding only supported for 4bpp bitmaps"
#define ERR_MSG_RLE8_BAD_BPP            "rle8 encoding only supported for 8bpp bitmaps"
#define ERR_MSG_UNSUPPORTED_COMPRESSION "unsupported compression"
#define ERR_MSG_WRITE_OUTSIDE           "rle decoding writes outside of frame"

#define _ABS(x) ((x) < 0 ? -(x) : (x))

#define DIB_SIGNATURE 0x4D42
#define DIB_INFO_HEADER_SIZE 40

#define DIB_BIT_COUNT_1 (0x01)  //!< Monochrome, 1bit per pixel.
#define DIB_BIT_COUNT_4 (0x04)  //!< 4 bits palletized. 16 colors.
#define DIB_BIT_COUNT_8 (0x08)  //!< 8 bits palletized, 256 colors.
#define DIB_BIT_COUNT_16 (0x10) //!< 16 bits RGB 65536 colors.
#define DIB_BIT_COUNT_24 (0x18) //!< 24 bits palletized, 16M colors.
#define DIB_BIT_COUNT_32 (0x20) //!< 24 bits palletized, 16M colors.

#define DIB_BI_RGB (0x00) //!< No compression.
#define DIB_BI_RLE8 (0x01) //!< 8bit RLE encoding.
#define DIB_BI_RLE4 (0x02) //!< 4bit RLE encoding.
#define DIB_BI_BITFIELD (0x03) //!< Uncompressd Color masks

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} dib_table_rgb;

static struct bj_bitmap* unpalletized(struct bj_bitmap* p_original, dib_table_rgb* p_color_table, size_t color_table_size) {
    (void)color_table_size; 
    enum bj_pixel_mode mode = bj_bitmap_mode(p_original);

    switch(mode) {
        case BJ_PIXEL_MODE_INDEXED_1:
        case BJ_PIXEL_MODE_INDEXED_4:
        case BJ_PIXEL_MODE_INDEXED_8:
            {
                /* bj_debug("Bitmap using 0x%08X encoding converted to BGR24", mode); */

                const size_t width = bj_bitmap_width(p_original);
                const size_t height = bj_bitmap_height(p_original);

                struct bj_bitmap* p_bitmap = bj_create_bitmap(
                    width, height, BJ_PIXEL_MODE_BGR24, 0
                );

                uint8_t* p_src_row         = bj_bitmap_pixels(p_original);
                uint8_t* p_dest_row        = bj_bitmap_pixels(p_bitmap);
                const size_t src_stride  = bj_bitmap_stride(p_original);
                const size_t dest_stride = bj_bitmap_stride(p_bitmap);

                for(size_t h = 0 ; h < height ; ++h) {
                    
                    uint8_t* p_dest_byte = p_dest_row;

                    for(size_t w = 0 ; w < width ; ++w) {
                        size_t index = 0;
                        switch(mode) {
                            case BJ_PIXEL_MODE_INDEXED_1:
                                index = (p_src_row[w / 8] >> (7 - (w % 8))) & 0x01;
                                break;
                            case BJ_PIXEL_MODE_INDEXED_4:
                                index = (p_src_row[w / 2] >> (w % 2 == 0 ? 4 : 0)) & 0x0F;
                                break;
                            case BJ_PIXEL_MODE_INDEXED_8:
                                index = p_src_row[w];
                                break;
                            default:
                                break;
                        }
                        if (index >= color_table_size) {
                            bj_warn("Invalid color table index %zu >= %zu, clamping to 0", index, color_table_size);
                            index = 0;
                        }
                        *p_dest_byte++ = p_color_table[index].blue;
                        *p_dest_byte++ = p_color_table[index].green;
                        *p_dest_byte++ = p_color_table[index].red;

                        /* *p_dest_byte++ = p_color_table[index].blue; */
                        /* *p_dest_byte++ = p_color_table[index].green; */
                        /* *p_dest_byte++ = p_color_table[index].red; */
                    }

                    p_src_row += src_stride;
                    p_dest_row += dest_stride;
                }

                bj_destroy_bitmap(p_original);
                return p_bitmap;
            }
            break;
        default:
            break;
    }

    return p_original;

}

static size_t dib_uncompressed_row_size(uint32_t width, uint16_t bit_count) {
    return ((((width * bit_count) + 31) & ~31) >> 3);
}

static size_t dib_color_table_len(uint16_t bit_count, uint32_t override) {
    if (override == 0) {
        switch (bit_count) {
            case DIB_BIT_COUNT_1: return 2;
            case DIB_BIT_COUNT_4: return 16;
            case DIB_BIT_COUNT_8: return 256;
            default: return 0;
        }
    }
    return override;
}

static void dib_read_uncompressed_raster(
    struct bj_stream* p_stream,
    uint8_t*   dst_pixels,
    size_t     dst_stride,
    uint32_t   width,
    int32_t    height,
    uint16_t   dib_bit_count
) {
    const bj_bool is_top_down = height < 0;

    uint8_t* const dst_end = dst_pixels + dst_stride * _ABS(height);
    bj_assert(dst_stride > 0);

    uint8_t* p_dst_row         = is_top_down ? dst_pixels : dst_end - dst_stride;
    const size_t src_stride  = dib_uncompressed_row_size(width, dib_bit_count);
    const size_t copy_stride = src_stride < dst_stride ? src_stride : dst_stride;

    while(p_dst_row >= dst_pixels && p_dst_row < dst_end) {
        if(bj_read_stream(p_stream, p_dst_row, copy_stride) < copy_stride) {
            bj_warn("unexpected end of bitmap stream");
            return;
        }

        if(is_top_down) {
            p_dst_row += dst_stride;
        } else {
            p_dst_row -= dst_stride;
        }
    }
}

static void dib_read_rle_raster(
    struct bj_stream* p_stream, 
    uint8_t*        p_dst_pixels,     
    size_t      dst_stride,
    uint32_t        width,
    int32_t        i_height,
    bj_bool       use_rle_4,
    struct bj_error** p_error
) {
    #define rle_fsm_expect_any                0x01
    #define rle_fsm_expect_escape             0x02
    #define rle_fsm_expect_padding            0x04
    #define rle_fsm_expect_delta_x            0x05
    #define rle_fsm_expect_delta_y            0x06
    #define rle_fsm_expect_and_write_index    0x03
    #define rle_fsm_keep_and_write_index      0x20

    uint8_t state = rle_fsm_expect_any;
    uint8_t last_read_byte = 0;
    uint8_t n_index = 0;
    uint8_t n_writes = 0;
    bj_bool padding_required = BJ_FALSE;

    size_t x = 0;
    size_t y = 0;
    uint32_t height = _ABS(i_height);

    while (BJ_TRUE) {
        if (state != rle_fsm_keep_and_write_index && bj_stream_read_t(p_stream, uint8_t, &last_read_byte) == 0) {
            return;
        }

        switch (state) {
            case rle_fsm_expect_any:
                if (padding_required) {
                    padding_required = BJ_FALSE;
                } else {
                    n_writes = 0;
                    if (last_read_byte == 0) {
                        state = rle_fsm_expect_escape;
                    } else {
                        n_writes = last_read_byte;
                        n_index = 1;
                        state = rle_fsm_expect_and_write_index;
                    }
                }
                break;

            case rle_fsm_expect_escape:
                switch (last_read_byte) {
                    case 0:
                        ++y;
                        x = 0;
                        state = rle_fsm_expect_any;
                        break;
                    case 1:
                        return; // End of bitmap
                    case 2:
                        state = rle_fsm_expect_delta_x;
                        break;
                    default:
                        n_index = last_read_byte;
                        n_writes = 1;
                        state = rle_fsm_expect_and_write_index;
                        padding_required = use_rle_4 ? ((n_index - 1) % 4 < 2) : (n_index % 2 == 1);
                        break;
                }
                break;

            case rle_fsm_expect_delta_x:
                x += last_read_byte;
                state = rle_fsm_expect_delta_y;
                break;

            case rle_fsm_expect_delta_y:
                y += last_read_byte;
                state = rle_fsm_expect_any;
                break;

            case rle_fsm_expect_and_write_index:
            case rle_fsm_keep_and_write_index:
                for (size_t i = 0; i < n_writes; ++i) {
                    if (x >= width || y >= height) {
                        bj_set_error(p_error, BJ_ERROR_INVALID_FORMAT, ERR_MSG_WRITE_OUTSIDE);
                        return;
                    }

                    size_t offset = (height - 1 - y) * dst_stride + x / (use_rle_4 ? 2 : 1);
                    if (use_rle_4) {
                        uint8_t index = last_read_byte;
                        if (state == rle_fsm_expect_and_write_index) index >>= 4;
                        index &= 0x0F;
                        if (x % 2 == 0) index <<= 4;
                        p_dst_pixels[offset] |= index;
                    } else {
                        p_dst_pixels[offset] = last_read_byte;
                    }
                    ++x;
                }
                
                if (--n_index == 0) {
                    state = rle_fsm_expect_any;
                } else {
                    state = (use_rle_4 && state == rle_fsm_expect_and_write_index) ? rle_fsm_keep_and_write_index : rle_fsm_expect_and_write_index;
                }
                break;

            default:
                bj_set_error(p_error, BJ_ERROR, ERR_MSG);
                return;
        }
    }

    #undef rle_fsm_expect_any
    #undef rle_fsm_expect_escape
    #undef rle_fsm_expect_padding
    #undef rle_fsm_expect_delta_x
    #undef rle_fsm_expect_delta_y
    #undef rle_fsm_expect_and_write_index
    #undef rle_fsm_keep_and_write_index
}


struct bj_bitmap* dib_create_bitmap_from_stream(
    struct bj_stream*        p_stream, 
    struct bj_error**        p_error
) {

    // Read file header
    uint16_t dib_signature = 0;
    if (bj_stream_read_t(p_stream, uint16_t, &dib_signature) != sizeof(uint16_t)) {
        bj_set_error(p_error, BJ_ERROR_INVALID_FORMAT, ERR_MSG_EOS);
        return 0;
    }

    if (dib_signature != DIB_SIGNATURE) {
        bj_set_error(p_error, BJ_ERROR_INCORRECT_VALUE, ERR_MSG_BAD_SIGNATURE);
        return 0;
    }

    uint32_t dib_file_size   = 0;
    uint32_t dib_data_offset = 0;

    if (bj_stream_read_t(p_stream, uint32_t, &dib_file_size) != sizeof(uint32_t)) {
        bj_set_error(p_error, BJ_ERROR_INVALID_FORMAT, ERR_MSG_EOS);
        return 0;
    }
    bj_stream_skip_t(p_stream, uint32_t);
    if (bj_stream_read_t(p_stream, uint32_t, &dib_data_offset) != sizeof(uint32_t)) {
        bj_set_error(p_error, BJ_ERROR_INVALID_FORMAT, ERR_MSG_EOS);
        return 0;
    }

    if(dib_data_offset == 0 || dib_data_offset >= dib_file_size || dib_data_offset > bj_get_stream_length(p_stream)) {
        bj_set_error(p_error, BJ_ERROR_INVALID_FORMAT, ERR_MSG_BAD_RASTER_OFFSET);
        return 0;
    }

    uint32_t info_header_size = 0;
    if (bj_stream_read_t(p_stream, uint32_t, &info_header_size) != sizeof(uint32_t)) {
        bj_set_error(p_error, BJ_ERROR_INVALID_FORMAT, ERR_MSG_EOS);
        return 0;
    }

    if (info_header_size != DIB_INFO_HEADER_SIZE) {
        bj_set_error(p_error, BJ_ERROR_INCORRECT_VALUE, ERR_MSG_BAD_HEADER_INFO_TYPE);
        return 0;
    }

    int32_t dib_height           = 0;
    uint32_t dib_width            = 0;
    uint16_t dib_planes           = 0;
    uint16_t dib_bit_count        = 0;
    uint32_t dib_compression      = 0;
    uint32_t dib_image_size       = 0;
    uint32_t dib_x_pixels_per_m   = 0;
    uint32_t dib_y_pixels_per_m   = 0;
    uint32_t dib_colors_used      = 0;
    uint32_t dib_colors_important = 0;

    if (bj_stream_read_t(p_stream, uint32_t, &dib_width) != sizeof(uint32_t)) {
        bj_set_error(p_error, BJ_ERROR_INVALID_FORMAT, ERR_MSG_EOS);
        return 0;
    }

    if (bj_stream_read_t(p_stream, int32_t, &dib_height) != sizeof(int32_t)) {
        bj_set_error(p_error, BJ_ERROR_INVALID_FORMAT, ERR_MSG_EOS);
        return 0;
    }


    if (dib_width * dib_height == 0x00) {
        bj_set_error(p_error, BJ_ERROR_INCORRECT_VALUE, ERR_MSG_BAD_BMP_SIZE);
        return 0;
    }

    if (dib_width != 0 && (uint32_t)(_ABS(dib_height)) > UINT32_MAX / dib_width) {
        bj_set_error(p_error, BJ_ERROR_INCORRECT_VALUE, ERR_MSG_BAD_BMP_SIZE);
        return 0;
    }

    if (bj_stream_read_t(p_stream, uint16_t, &dib_planes) != sizeof(uint16_t)) {
        bj_set_error(p_error, BJ_ERROR_INVALID_FORMAT, ERR_MSG_EOS);
        return 0;
    }

    if (dib_planes != 0x01) {
        bj_set_error(p_error, BJ_ERROR_INCORRECT_VALUE, ERR_MSG_BAD_PLANES_COUNT);
        return 0;
    }

    if (bj_stream_read_t(p_stream, uint16_t, &dib_bit_count) != sizeof(uint16_t)) {
        bj_set_error(p_error, BJ_ERROR_INVALID_FORMAT, ERR_MSG_EOS);
        return 0;
    }
    switch (dib_bit_count) {
        case DIB_BIT_COUNT_1:
        case DIB_BIT_COUNT_4:
        case DIB_BIT_COUNT_8:
        case DIB_BIT_COUNT_16:
        case DIB_BIT_COUNT_24:
        case DIB_BIT_COUNT_32:
            break;
        default:
            bj_set_error(p_error, BJ_ERROR_INCORRECT_VALUE, ERR_MSG_BAD_BIT_COUNT);
            return 0;
    }

    if (bj_stream_read_t(p_stream, uint32_t, &dib_compression) != sizeof(uint32_t)) {
        bj_set_error(p_error, BJ_ERROR_INVALID_FORMAT, ERR_MSG_EOS);
        return 0;
    }
    switch(dib_compression) {
        case DIB_BI_BITFIELD:
        case DIB_BI_RGB:
            break;
        case DIB_BI_RLE8:
            if(dib_bit_count != DIB_BIT_COUNT_8) {
                bj_set_error(p_error, BJ_ERROR_INCORRECT_VALUE, ERR_MSG_RLE8_BAD_BPP);
                return 0;
            }
            break;
        case DIB_BI_RLE4:
            if(dib_bit_count != DIB_BIT_COUNT_4) {
                bj_set_error(p_error, BJ_ERROR_INCORRECT_VALUE, ERR_MSG_RLE4_BAD_BPP);
                return 0;
            }
            break;
        default:
            bj_set_error(p_error, BJ_ERROR_INVALID_FORMAT, ERR_MSG_BAD_COMPRESSION_TYPE);
            return 0;
    }

    if (bj_stream_read_t(p_stream, uint32_t, &dib_image_size) != sizeof(uint32_t)) {
        bj_set_error(p_error, BJ_ERROR_INVALID_FORMAT, ERR_MSG_EOS);
        return 0;
    }
    if (bj_stream_read_t(p_stream, uint32_t, &dib_x_pixels_per_m) != sizeof(uint32_t)) {
        bj_set_error(p_error, BJ_ERROR_INVALID_FORMAT, ERR_MSG_EOS);
        return 0;
    }
    if (bj_stream_read_t(p_stream, uint32_t, &dib_y_pixels_per_m) != sizeof(uint32_t)) {
        bj_set_error(p_error, BJ_ERROR_INVALID_FORMAT, ERR_MSG_EOS);
        return 0;
    }
    if (bj_stream_read_t(p_stream, uint32_t, &dib_colors_used) != sizeof(uint32_t)) {
        bj_set_error(p_error, BJ_ERROR_INVALID_FORMAT, ERR_MSG_EOS);
        return 0;
    }

    if(dib_colors_used > (dib_bit_count <= DIB_BIT_COUNT_8 ? 0x01u << dib_bit_count : 256u)) {
        bj_set_error(p_error, BJ_ERROR_INVALID_FORMAT, ERR_MSG_BAD_PALETTE_SIZE);
        return 0;
    }

    if (bj_stream_read_t(p_stream, uint32_t, &dib_colors_important) != sizeof(uint32_t)) {
        bj_set_error(p_error, BJ_ERROR_INVALID_FORMAT, ERR_MSG_EOS);
        return 0;
    }

    if(dib_colors_important > dib_colors_used) {
        bj_set_error(p_error, BJ_ERROR_INVALID_FORMAT, ERR_MSG_BAD_PALETTE_SIZE);
        return 0;
    }

    uint32_t red_mask   = 0;
    uint32_t green_mask = 0;
    uint32_t blue_mask  = 0;
   
    // Read the bitfields
    if (info_header_size == DIB_INFO_HEADER_SIZE && dib_compression == DIB_BI_BITFIELD) {
        switch(dib_bit_count) {
            case DIB_BIT_COUNT_16:
            case DIB_BIT_COUNT_32:
                break;
            default:
                bj_set_error(p_error, BJ_ERROR_INVALID_FORMAT, ERR_MSG_BITFIELDS_BAD_BPP);
                return 0;
        }
        if (bj_stream_read_t(p_stream, uint32_t, &red_mask) != sizeof(uint32_t)) {
            bj_set_error(p_error, BJ_ERROR_INVALID_FORMAT, ERR_MSG_EOS);
            return 0;
        }
        if (bj_stream_read_t(p_stream, uint32_t, &green_mask) != sizeof(uint32_t)) {
            bj_set_error(p_error, BJ_ERROR_INVALID_FORMAT, ERR_MSG_EOS);
            return 0;
        }
        if (bj_stream_read_t(p_stream, uint32_t, &blue_mask) != sizeof(uint32_t)) {
            bj_set_error(p_error, BJ_ERROR_INVALID_FORMAT, ERR_MSG_EOS);
            return 0;
        }

        if (    (red_mask & green_mask & blue_mask)
              | (red_mask & green_mask) 
              | (red_mask & blue_mask) 
              | (green_mask & blue_mask)) {
            bj_set_error(p_error, BJ_ERROR_INVALID_FORMAT, ERR_MSG_OVERLAPPING_BITFIELDS);
            return 0;
        }
    }

    // Now we got the bitmasks and all, we can get the mode.
    const enum bj_pixel_mode src_mode = bj_compute_pixel_mode(dib_bit_count, red_mask, green_mask, blue_mask);

    // Read the color table
    dib_table_rgb* color_table = 0;
    size_t color_table_len = dib_color_table_len(dib_bit_count, dib_colors_used);

    if (color_table_len > 0) {
        color_table = bj_malloc(sizeof(dib_table_rgb) * color_table_len);

        if (color_table == NULL) {
            bj_set_error(p_error, BJ_ERROR_CANNOT_ALLOCATE, ERR_MSG_CANNOT_ALLOC_PALETTE);
            return 0;
        }

        // Sometimes, the color table is not encoded, we know it because
        if ((bj_tell_stream(p_stream) == dib_data_offset)) {
            bj_warn("%dbpp bitmap stream contains no color table", dib_bit_count);
            // In this case, we fill the color table with white, with exception
            // to the first index, that is set to black
            bj_memset(color_table, 0xFF, sizeof(dib_table_rgb) * color_table_len);
            bj_memset(color_table, 0x00, sizeof(dib_table_rgb));
        } else {
            for (size_t i = 0; i < color_table_len; ++i) {
                bj_stream_read_t(p_stream, char, &color_table[i].blue);
                bj_stream_read_t(p_stream, char, &color_table[i].green);
                bj_stream_read_t(p_stream, char, &color_table[i].red);
                bj_stream_skip_t(p_stream, char);
            }
        }
    }

    // Check the current position is the same as the data offset
    if(bj_tell_stream(p_stream) != dib_data_offset) {
        bj_set_error(p_error, BJ_ERROR_INVALID_FORMAT, ERR_MSG_BAD_RASTER_OFFSET);
        bj_free(color_table);
        return 0;
    }
    bj_seek_stream(p_stream, dib_data_offset, BJ_SEEK_BEGIN);

    // Stride of the bitmap is either the computed dib size (if mode is unknown) or 0.
    // If 0, the bitmap initialized will set to the best choice for us.
    struct bj_bitmap* p_bitmap = bj_create_bitmap(
        dib_width, _ABS(dib_height),
        src_mode,
        src_mode == BJ_PIXEL_MODE_UNKNOWN ? dib_uncompressed_row_size(dib_width, dib_bit_count) : 0
    );

    if (p_bitmap == 0) {
        bj_free(color_table);
        bj_set_error(p_error, BJ_ERROR_CANNOT_ALLOCATE, "cannot create bitmap");
        return 0;
    }

    struct bj_error* p_inner_error = 0;
    switch(dib_compression) {
        case DIB_BI_BITFIELD:
        case DIB_BI_RGB:
            dib_read_uncompressed_raster(
                p_stream,
                bj_bitmap_pixels(p_bitmap), // dst_pixels
                bj_bitmap_stride(p_bitmap), // dst_stride
                dib_width, dib_height,
                dib_bit_count
            );
            break;
        case DIB_BI_RLE4:
        case DIB_BI_RLE8:
            dib_read_rle_raster(
                p_stream,
                bj_bitmap_pixels(p_bitmap), // dst_pixels
                bj_bitmap_stride(p_bitmap), // dst_stride
                dib_width, dib_height,
                dib_compression == DIB_BI_RLE4,
                p_error
            );
            break;
        default:
            bj_set_error(p_error, BJ_ERROR, ERR_MSG_UNSUPPORTED_COMPRESSION);
            bj_destroy_bitmap(p_bitmap);
            bj_free(color_table);
            return 0;
    }
    if(p_inner_error) {
        bj_forward_error(p_inner_error, p_error);
        bj_destroy_bitmap(p_bitmap);
        bj_free(color_table);
        return 0;
    }


    p_bitmap = unpalletized(p_bitmap, color_table, color_table_len);
    bj_free(color_table);
    return p_bitmap;
}





                

