#include <banjo/stream.h>
#include <banjo/memory.h>

bj_stream* bj_stream_init_default(
    bj_stream* p_stream,
    usize size
) {
    bj_memset(p_stream, 0, sizeof(bj_stream));
    if(size > 0) {
        p_stream->p_data = bj_malloc(p_stream->len = size);
    }
    return p_stream;
}

bj_stream* bj_stream_init_read(
    bj_stream* p_stream,
    void*      p_data,
    usize      length
){
    bj_memset(p_stream, 0, sizeof(bj_stream));
    if(p_data != 0 && length > 0) {
        p_stream->p_data = p_data;
        p_stream->weak   = true;
        p_stream->len    = length;
    }
    return p_stream;
}

bj_stream* bj_stream_reset(
    bj_stream* p_stream
) {
    bj_assert(p_stream != 0);
    if(!p_stream->weak && p_stream->p_data != 0) {
        bj_free(p_stream->p_data);
    }
    bj_memset(p_stream, 0, sizeof(bj_stream));
    return p_stream;
}

usize bj_stream_read_byte(
    bj_stream* p_stream,
    usize      count,
    void*      p_buffer
) {
   usize position = p_stream->position;
   usize len = p_stream->len;
   usize remaining = (position < len) ? (len - position) : 0;
   usize bytes_to_read = (remaining < count) ? remaining : count;

   if(p_buffer != 0) {
       bj_memcpy(p_buffer, p_stream->p_data + p_stream->position, bytes_to_read);
   }

   p_stream->position += bytes_to_read; 
   return bytes_to_read;
}

