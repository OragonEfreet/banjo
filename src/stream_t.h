#pragma once

#include <banjo/stream.h>

struct bj_stream_t {
    usize   len;        //!< Size of the stream (in bytes)
    usize   position;   //!< Current position within the stream
    /// Array of stream data
    union {
        const u8* r;    //!< Read-only access
    } p_data;
    bool    weak;       //!< True if memory buffer is not managed by the object
};

