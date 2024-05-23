#include <banjo/log.h>
#include <banjo/memory.h>
#include <banjo/stream.h>

bj_stream* bj_stream_init_default(
    bj_stream* p_stream,
    usize size
) {
    bj_memset(p_stream, 0, sizeof(bj_stream));
    if(size > 0) {
        p_stream->p_data.r = bj_malloc(p_stream->len = size);
    }
    return p_stream;
}

bj_stream* bj_stream_init_read(
    bj_stream*   p_stream,
    const void*  p_data,
    usize        length
){
    bj_memset(p_stream, 0, sizeof(bj_stream));
    if(p_data != 0 && length > 0) {
        p_stream->p_data.r = p_data;
        p_stream->weak   = true;
        p_stream->len    = length;
    }
    return p_stream;
}

bj_stream* bj_stream_reset(
    bj_stream* p_stream
) {
    bj_check_or_0(p_stream != 0);
    if(!p_stream->weak && p_stream->p_data.r != 0) {
        bj_free((void*)p_stream->p_data.r);
    }
    bj_memset(p_stream, 0, sizeof(bj_stream));
    return p_stream;
}

usize bj_stream_read(
    bj_stream* p_stream,
    void*      p_buffer,
    usize      count
) {
   usize position = p_stream->position;
   usize len = p_stream->len;
   usize remaining = (position < len) ? (len - position) : 0;
   usize bytes_to_read = (remaining < count) ? remaining : count;

   if(bytes_to_read > 0 && p_buffer != 0) {
       bj_memcpy(p_buffer, p_stream->p_data.r + p_stream->position, bytes_to_read);
   }

   p_stream->position += bytes_to_read; 
   return bytes_to_read;
}


usize bj_stream_seek(
    bj_stream*     p_stream,
    size           offset,
    bj_seek_origin from
) {
    usize new_position = (from == BJ_SEEK_CURRENT) ? p_stream->position + offset :
                         (from == BJ_SEEK_BEGIN)   ? (usize)offset :
                         p_stream->len + offset;

    if ((size)new_position < 0) {
        new_position = 0;
    } else if (new_position > p_stream->len) {
        new_position = p_stream->len;
    }

    p_stream->position = new_position;
    return new_position;
}

usize bj_stream_tell(
    bj_stream*     p_stream
) {
    return p_stream->position;
}
