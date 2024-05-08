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
    u8*     p_data;     //!< Array of stream data
    usize   len;        //!< Size of the stream (in bytes)
    usize   position;   //!< Current position within the stream
};

BANJO_EXPORT bj_byte_stream* bj_byte_stream_init_default(
    bj_byte_stream* p_instance,
    usize size
);

/* BANJO_EXPORT bj_byte_stream* bj_byte_stream_init_read( */
/*     bj_byte_stream* p_instance, */
/*     u8*               p_data, */
/*     usize             length */
/* ); */


