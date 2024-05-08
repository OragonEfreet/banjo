#include <banjo/byte_stream.h>
#include <banjo/memory.h>

bj_byte_stream* bj_byte_stream_init_default(
    bj_byte_stream* p_stream,
    usize size
) {
    bj_memset(p_stream, 0, sizeof(bj_byte_stream));
    if(size > 0) {
        p_stream->p_data = bj_malloc(p_stream->len = size);
    }
    return p_stream;
}

bj_byte_stream* bj_byte_stream_init_read(
    bj_byte_stream* p_stream,
    u8*             p_data,
    usize           length
){
    bj_memset(p_stream, 0, sizeof(bj_byte_stream));
    if(p_data != 0 && length > 0) {
        p_stream->p_data = p_data;
        p_stream->weak   = true;
        p_stream->len    = length;
    }
    return p_stream;
}

bj_byte_stream* bj_byte_stream_reset(
    bj_byte_stream* p_stream
) {
    bj_assert(p_stream != 0);
    if(!p_stream->weak && p_stream->p_data != 0) {
        bj_free(p_stream->p_data);
    }
    bj_memset(p_stream, 0, sizeof(bj_byte_stream));
    return p_stream;
}

usize bj_byte_stream_read_byte(
    bj_byte_stream* p_instance,
    usize           count,
    u8*             p_buffer
) {
    return 0;
}

