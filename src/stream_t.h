#pragma once

#include <banjo/stream.h>

struct bj_stream {
    size_t   len;        //!< Size of the stream (in bytes)
    size_t   position;   //!< Current position within the stream
    /// Array of stream data
    union {
        const uint8_t* r;    //!< Read-only access
    } data;
    bj_bool    weak;       //!< True if memory buffer is not managed by the object
};

