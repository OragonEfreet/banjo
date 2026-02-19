#include <banjo/memory.h>

#include <stream.h>

#include <check.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

struct bj_stream* bj_allocate_stream(
    void
) {
    return bj_malloc(sizeof(struct bj_stream));
}

struct bj_stream* bj_open_stream_read(
    const void*  p_data,
    size_t        length
){
    struct bj_stream* p_stream = bj_allocate_stream();
    if(p_stream != 0) {
        bj_memset(p_stream, 0, sizeof(struct bj_stream));
        p_stream->data.r = p_data;
        p_stream->weak   = BJ_TRUE;
        p_stream->len    = length;
    }

    return p_stream;
}

struct bj_stream* bj_open_stream_file(
    const char*       p_path,
    struct bj_error**        p_error
) {
    bj_check_or_0(p_path);

    FILE* fstream  = fopen(p_path, "rb");

    if (!fstream ) {
        bj_set_error_fmt(p_error, BJ_ERROR_FILE_NOT_FOUND,
            "Cannot open '%s': %s", p_path, strerror(errno));
        return 0;
    }

    fseek(fstream, 0, SEEK_END);
    const long file_size = ftell(fstream);
    if(file_size == -1L) {
        bj_set_error_fmt(p_error, BJ_ERROR_IO,
            "Cannot get size of '%s': %s", p_path, strerror(errno));
        fclose(fstream);
        return 0;
    }
    fseek(fstream, 0, SEEK_SET);

    if(file_size > 0) {
        size_t file_byte_size = (size_t)file_size;

        void *buffer = bj_malloc(file_byte_size);
        if (buffer == 0) {
            bj_set_error_fmt(p_error, BJ_ERROR_CANNOT_ALLOCATE,
                "Cannot allocate %zu bytes for '%s'", file_byte_size, p_path);
            fclose(fstream);
            return 0;
        }

        size_t bytes_read = fread(buffer, 1, file_byte_size, fstream);
        fclose(fstream);
        if (bytes_read != file_byte_size) {
            bj_free(buffer);
            bj_set_error_fmt(p_error, BJ_ERROR_CANNOT_READ,
                "Read %zu of %zu bytes from '%s'", bytes_read, file_byte_size, p_path);
            return 0;
        }

        struct bj_stream* p_stream = bj_open_stream_read(buffer, bytes_read);
        p_stream->weak   = BJ_FALSE;
        return p_stream;
    }

    return bj_open_stream_read(0, 0);
}

void bj_close_stream(
    struct bj_stream* p_stream
) {
    bj_check(p_stream != 0);

    if(!p_stream->weak && p_stream->data.r != 0) {
        bj_free((void*)p_stream->data.r);
    }
    bj_memset(p_stream, 0, sizeof(struct bj_stream));
    bj_free(p_stream);
}

size_t bj_read_stream(
    struct bj_stream* p_stream,
    void*      p_buffer,
    size_t      count
) {
   size_t position = p_stream->position;
   size_t len = p_stream->len;
   size_t remaining = (position < len) ? (len - position) : 0;
   size_t bytes_to_read = (remaining < count) ? remaining : count;

   if(bytes_to_read > 0 && p_buffer != 0) {
       bj_memcpy(p_buffer, p_stream->data.r + p_stream->position, bytes_to_read);
   }

   p_stream->position += bytes_to_read; 
   return bytes_to_read;
}

BANJO_EXPORT size_t bj_get_stream_length(
    struct bj_stream* p_stream
) {
    bj_check_or_0(p_stream);
    return p_stream->len;
}


size_t bj_seek_stream(
    struct bj_stream*     p_stream,
    ptrdiff_t           offset,
    enum bj_seek_origin from
) {
    size_t new_position = (from == BJ_SEEK_CURRENT) ? (size_t)((ptrdiff_t)p_stream->position + offset) :
                         (from == BJ_SEEK_BEGIN)   ? (size_t)offset :
                         (size_t)((ptrdiff_t)p_stream->len + offset);

    if ((ptrdiff_t)new_position < 0) {
        new_position = 0;
    } else if (new_position > p_stream->len) {
        new_position = p_stream->len;
    }

    p_stream->position = new_position;
    return new_position;
}

size_t bj_tell_stream(
    struct bj_stream*     p_stream
) {
    return p_stream->position;
}
