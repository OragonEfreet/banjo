#include <banjo/log.h>

#include "bitmap_t.h"
#include "dib.h"

#include <stdio.h>

static usize bitmap_stride(
    usize width,
    bj_pixel_format format
) {
    assert(format == BJ_PIXEL_FORMAT_BGR24);
    return width * 3;
}

static bj_bitmap* bj_bitmap_init_with_stride(
    bj_bitmap* p_bitmap,
    usize width,
    usize height,
    bj_pixel_format format,
    usize stride
) {
    if(p_bitmap) {
        bj_memset(p_bitmap, 0, sizeof(bj_bitmap));

        if(stride > 0) {
            usize bufsize = stride * height;

            p_bitmap->width = width;
            p_bitmap->height = height;
            p_bitmap->stride = stride;
            p_bitmap->buffer = bj_malloc(bufsize);
            p_bitmap->format = format;
            bj_memset(p_bitmap->buffer, 0x00, bufsize);
        }
    }
    return p_bitmap;
}

bj_bitmap* bj_bitmap_init(
    bj_bitmap* p_bitmap,
    usize width,
    usize height,
    bj_pixel_format format
) {
    return(bj_bitmap_init_with_stride(
        p_bitmap,
        width,
        height,
        format,
        bitmap_stride(width, format)
    ));
}

bj_bitmap* bj_bitmap_init_from_buffer(
    bj_bitmap*      p_bitmap,
    usize           width,
    usize           height,
    bj_pixel_format format,
    usize           stride,
    void*           buffer,
    int             flags,
    bj_error**      p_error
) {
    if((flags & BJ_BITMAP_BUFFER_KEEP_STRIDE) == BJ_BITMAP_BUFFER_KEEP_STRIDE) {
        p_bitmap = bj_bitmap_init_with_stride(p_bitmap, width, height, format, stride);
    } else {
        p_bitmap = bj_bitmap_init(p_bitmap, width, height, format);
    }
    if(p_bitmap == 0) {
        bj_set_error(p_error, BJ_ERROR_CANNOT_ALLOCATE, "Cannot initialize bitmap from buffer");
        return 0;
    }
    bj_info("Stride -> %ld (Input is %ld)", p_bitmap->stride, stride);


    if (stride > p_bitmap->stride) {
        bj_set_error(p_error, BJ_ERROR, "Provided bitmap stride is too large");
        bj_bitmap_reset(p_bitmap);
        return 0;
    }

    const bool reverse_y = ((flags & BJ_BITMAP_BUFFER_REVERSE_Y) == BJ_BITMAP_BUFFER_REVERSE_Y);

    for(usize r = 0 ; r < height ; ++r) {
        void* dest = (byte*)p_bitmap->buffer + r * p_bitmap->stride;

        const usize y_shift = reverse_y ? (height - 1 - r) : r;
        void* src = (byte*)buffer + stride * y_shift;

        if(bj_memcpy(dest, src, p_bitmap->stride) != dest) {
            bj_set_error(p_error, BJ_ERROR, "Cannot copy buffer data");
            bj_bitmap_reset(p_bitmap);
            return 0;
        }
    }

    return p_bitmap;
}

void bj_bitmap_reset(
    bj_bitmap* p_bitmap
) {
    bj_check(p_bitmap);

    if(p_bitmap->buffer != 0) {
        bj_free(p_bitmap->buffer);
    }
#ifdef BJ_FEAT_PEDANTIC
    bj_memset(p_bitmap, 0, sizeof(bj_oldbmp));
#endif
}

bj_bitmap* bj_bitmap_new(
    usize           width,
    usize           height,
    bj_pixel_format format
) {
    bj_bitmap bitmap;
    if(bj_bitmap_init(&bitmap, width, height, format) == 0) {
        return 0;
    }
    return bj_memcpy(bj_malloc(sizeof(bj_bitmap)), &bitmap, sizeof(bj_bitmap));
}

bj_bitmap* bj_bitmap_new_from_buffer(
    usize           width,
    usize           height,
    bj_pixel_format format,
    usize           stride,
    void*           buffer,
    int             flags,
    bj_error**      p_error

) {
    bj_bitmap bitmap;
    if(bj_bitmap_init_from_buffer(&bitmap, width, height, format, stride, buffer, flags, p_error) == 0) {
        return 0;
    }
    return bj_memcpy(bj_malloc(sizeof(bj_bitmap)), &bitmap, sizeof(bj_bitmap));
}

void bj_bitmap_del(
    bj_bitmap*     p_bitmap
) {
    bj_bitmap_reset(p_bitmap);
    bj_free(p_bitmap);
}

usize bj_bitmap_width(
    bj_bitmap*     p_bitmap
) {
    bj_check_or_0(p_bitmap);
    return p_bitmap->width;
}

usize bj_bitmap_height(
    bj_bitmap*     p_bitmap
) {
    bj_check_or_0(p_bitmap);
    return p_bitmap->height;
}

int bj_bitmap_encoding( 
    bj_bitmap* p_bitmap
) {
    bj_check_or_0(p_bitmap);
    return p_bitmap->format;
}

int bj_bitmap_stride( 
    bj_bitmap* p_bitmap
) {
    bj_check_or_0(p_bitmap);
    return p_bitmap->stride;
}


void* bj_bitmap_pixels(
    bj_bitmap*     p_bitmap
) {
    bj_check_or_0(p_bitmap);
    return p_bitmap->buffer;
}

void bj_bitmap_put_rgb(
    bj_bitmap* p_bitmap,
    usize      x,
    usize      y,
    u8         r,
    u8         g,
    u8         b
) {
    bj_check(p_bitmap);
    const usize pixel_bytesize = 3;
    const usize pixel_offset = y * p_bitmap->stride + pixel_bytesize * x;
    u8* pixel = (u8*)p_bitmap->buffer + pixel_offset;
    *(pixel++) = b;
    *(pixel++) = g;
    *(pixel) = r;
}

bj_bitmap* bj_bitmap_new_from_file(
    const char*       p_path,
    bj_error**        p_error
) {
    dib dib_data;
    bj_error* p_inner_error = 0;

    dib_read_file(&dib_data, p_path, &p_inner_error);

    if(p_inner_error) {
        bj_forward_error(p_inner_error, p_error);
        dib_reset(&dib_data);
        return 0;
    }

    bj_info("Read DIB: %d / %d, BPP = %d",
        dib_data.info_header.width,
        dib_data.info_header.height,
        dib_data.info_header.bit_count
    );

    const usize dib_stride = dib_row_size(&dib_data);

    return bj_bitmap_new_from_buffer(
        dib_data.info_header.width,
        dib_data.info_header.height,
        dib_pixel_format(&dib_data),
        dib_stride,
        dib_data.storage,
        BJ_BITMAP_BUFFER_REVERSE_Y,
        p_error
    );

}
