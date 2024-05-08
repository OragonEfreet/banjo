////////////////////////////////////////////////////////////////////////////////
/// \file
/// Header file for \ref stream object.

////////////////////////////////////////////////////////////////////////////////
/// \defgroup stream Binary Stream
/// \ingroup core
///
#pragma once

#include <banjo/api.h>

typedef struct bj_stream_t bj_stream; ///< Typedef for the bj_stream_t struct

/// \brief The internal data structure for the \ref bj_stream type.
struct bj_stream_t {
    bool    weak;       //!< True is memory buffer is not managed by the object
    u8*     p_data;     //!< Array of stream data
    usize   len;        //!< Size of the stream (in bytes)
    usize   position;   //!< Current position within the stream
};

BANJO_EXPORT bj_stream* bj_stream_init_default(
    bj_stream* p_instance,
    usize      size
);

BANJO_EXPORT bj_stream* bj_stream_init_read(
    bj_stream* p_instance,
    void*        p_data,
    usize      length
);

BANJO_EXPORT bj_stream* bj_stream_reset(
    bj_stream* p_instance
);

BANJO_EXPORT usize bj_stream_read_byte(
    bj_stream* p_instance,
    usize      count,
    void*      p_dest
);

#define bj_stream_read(stream, type, buffer) bj_stream_read_byte(stream, sizeof(type), buffer)




