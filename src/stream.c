#include "stream_t.h"

#include <banjo/log.h>
#include <banjo/memory.h>

#include <stdio.h>

bj_stream* bj_stream_alloc(
    void
) {
    return bj_malloc(sizeof(bj_stream));
}

bj_stream* bj_stream_new_read(
    const void*  p_data,
    size_t        length
){
    bj_stream* p_stream = bj_stream_alloc();
    if(p_stream != 0) {
        bj_memset(p_stream, 0, sizeof(bj_stream));
        p_stream->p_data.r = p_data;
        p_stream->weak   = true;
        p_stream->len    = length;
    }

    return p_stream;
}

bj_stream* bj_stream_new_read_from_file(
    const char*       p_path,
    bj_error**        p_error
) {
    // TODO Avoid dumping entire file in memory
    bj_check_or_0(p_path);

    FILE* fstream  = fopen(p_path, "rb");

    if (!fstream ) {
        bj_set_error(p_error, BJ_ERROR_FILE_NOT_FOUND, "cannot open file");
        return 0;
    }

    fseek(fstream, 0, SEEK_END);
    const long file_size = ftell(fstream);
    fseek(fstream, 0, SEEK_SET);

    if(file_size > 0) {
        void *buffer = bj_malloc(file_size);
        if (buffer == 0) {
            bj_set_error(p_error, BJ_ERROR_CANNOT_ALLOCATE, "cannot read file content");
            fclose(fstream);
            return 0;
        }

        size_t bytes_read = fread(buffer, 1, file_size, fstream);
        fclose(fstream);
        if (bytes_read != file_size) {
            bj_set_error(p_error, BJ_ERROR, "cannot read file content");
            return 0;
        }

        bj_stream* p_stream = bj_stream_new_read(buffer, bytes_read);
        p_stream->weak   = false;
        return p_stream;
    }

    return bj_stream_new_read(0, 0);
}

void bj_stream_del(
    bj_stream* p_stream
) {
    bj_check(p_stream != 0);

    if(!p_stream->weak && p_stream->p_data.r != 0) {
        bj_free((void*)p_stream->p_data.r);
    }
    bj_memset(p_stream, 0, sizeof(bj_stream));
    bj_free(p_stream);
}

size_t bj_stream_read(
    bj_stream* p_stream,
    void*      p_buffer,
    size_t      count
) {
   size_t position = p_stream->position;
   size_t len = p_stream->len;
   size_t remaining = (position < len) ? (len - position) : 0;
   size_t bytes_to_read = (remaining < count) ? remaining : count;

   if(bytes_to_read > 0 && p_buffer != 0) {
       bj_memcpy(p_buffer, p_stream->p_data.r + p_stream->position, bytes_to_read);
   }

   p_stream->position += bytes_to_read; 
   return bytes_to_read;
}

BANJO_EXPORT size_t bj_stream_len(
    bj_stream* p_stream
) {
    bj_check_or_0(p_stream);
    return p_stream->len;
}


size_t bj_stream_seek(
    bj_stream*     p_stream,
    ptrdiff_t           offset,
    bj_seek_origin from
) {
    size_t new_position = (from == BJ_SEEK_CURRENT) ? p_stream->position + offset :
                         (from == BJ_SEEK_BEGIN)   ? (size_t)offset :
                         p_stream->len + offset;

    if ((ptrdiff_t)new_position < 0) {
        new_position = 0;
    } else if (new_position > p_stream->len) {
        new_position = p_stream->len;
    }

    p_stream->position = new_position;
    return new_position;
}

size_t bj_stream_tell(
    bj_stream*     p_stream
) {
    return p_stream->position;
}
