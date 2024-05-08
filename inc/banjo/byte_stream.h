////////////////////////////////////////////////////////////////////////////////
/// \file
/// Header file for \ref byte_stream object.

////////////////////////////////////////////////////////////////////////////////
/// \defgroup byte_stream Binary Stream
/// \ingroup core
///
#pragma once

#include <banjo/api.h>

typedef struct bj_byte_stream_t bj_byte_stream; ///< Typedef for the bj_byte_stream_t struct

/// \brief The internal data structure for the \ref bj_byte_stream type.
struct bj_byte_stream_t {
    bool    weak;       //!< True is memory buffer is not managed by the object
    u8*     p_data;     //!< Array of stream data
    usize   len;        //!< Size of the stream (in bytes)
    usize   position;   //!< Current position within the stream
};

BANJO_EXPORT bj_byte_stream* bj_byte_stream_init_default(
    bj_byte_stream* p_instance,
    usize           size
);

BANJO_EXPORT bj_byte_stream* bj_byte_stream_init_read(
    bj_byte_stream* p_instance,
    u8*             p_data,
    usize           length
);

BANJO_EXPORT bj_byte_stream* bj_byte_stream_reset(
    bj_byte_stream* p_instance
);

BANJO_EXPORT usize bj_byte_stream_read_byte(
    bj_byte_stream* p_instance,
    usize           count,
    u8*             p_buffer
);

#define bj_byte_stream_read(stream, type, buffer) bj_byte_stream_read_byte(stream, sizeof(type), buffer)




