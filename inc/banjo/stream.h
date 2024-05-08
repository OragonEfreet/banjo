////////////////////////////////////////////////////////////////////////////////
/// \file bj_stream.h
/// \brief Header file for bj_stream_t struct and related functions
///
#pragma once

#include <banjo/api.h>

////////////////////////////////////////////////////////////////////////////////
/// \brief Structure representing a stream of data
///
typedef struct bj_stream_t bj_stream; ///< Typedef for the bj_stream_t struct

////////////////////////////////////////////////////////////////////////////////
/// \brief Position in a \ref bj_stream to use for origin
///
typedef enum {
    BJ_SEEK_BEGIN   = 0x00, //!< The begining of the stream
    BJ_SEEK_CURRENT = 0x01, //!< The current position of the stream
    BJ_SEEK_END     = 0x02, //!< The end of the stream
} bj_seek_origin;

////////////////////////////////////////////////////////////////////////////////
/// \brief Structure representing a stream of data
///
struct bj_stream_t {
    bool    weak;       //!< True if memory buffer is not managed by the object
    u8*     p_data;     //!< Array of stream data
    usize   len;        //!< Size of the stream (in bytes)
    usize   position;   //!< Current position within the stream
};

////////////////////////////////////////////////////////////////////////////////
/// \brief Initializes a stream with default settings
///
/// The stream is filled with and uninitialized buffer of `size` bytes.
///
/// \param p_instance Pointer to the stream instance
/// \param size       Size of the stream
/// \return Pointer to the initialized stream instance
///
BANJO_EXPORT bj_stream* bj_stream_init_default(
    bj_stream* p_instance,
    usize      size
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Initializes a stream for reading data
///
/// The underlying buffer is not copied into the object.
///
/// \param p_instance Pointer to the stream instance
/// \param p_data     Pointer to the data to be read
/// \param length     Length of the data in bytes
/// \return Pointer to the initialized stream instance
///
BANJO_EXPORT bj_stream* bj_stream_init_read(
    bj_stream* p_instance,
    void*      p_data,
    usize      length
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Resets the stream to nil state
///
/// \param p_instance Pointer to the stream instance
/// \return Pointer to the stream instance after resetting
///
BANJO_EXPORT bj_stream* bj_stream_reset(
    bj_stream* p_instance
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Reads data from the stream
///
/// The function effectively progresses the buffer pointer to `count` bytes
/// forward.
///
/// If the return value is not `count`, it means the buffer reached end while
/// reading.
/// In this case, if `size > 0`, this means the expected value is probably
/// incomplete.
///
/// \param p_instance Pointer to the stream instance
/// \param p_dest     Pointer to the destination buffer
/// \param count      Number of bytes to read
///
/// \return Number of bytes read
///
BANJO_EXPORT usize bj_stream_read(
    bj_stream* p_instance,
    void*      p_dest,
    usize      count
);

BANJO_EXPORT usize bj_stream_seek(
    bj_stream*     p_instance,
    size           position,
    bj_seek_origin from
);

/// Returns a new position
////////////////////////////////////////////////////////////////////////////////
/// \brief Reads data of a specified type from the stream
///
/// The function effectively progresses the buffer pointer to `count` bytes
/// forward.
///
/// If the return value is not `count`, it means the buffer reached end while
/// reading.
/// In this case, if `size > 0`, this means the expected value is probably
/// incomplete.
///
/// \param stream Pointer to the stream instance
/// \param type   Data type to read
/// \param buffer Pointer to the buffer to store the read data
///
#define bj_stream_read_t(stream, type, buffer) bj_stream_read(stream, buffer, sizeof(type))

////////////////////////////////////////////////////////////////////////////////
/// \brief Skips reading data of a specified type from the stream
///
/// The function effectively progresses the buffer pointer to `count` bytes
/// forward.
///
/// If the return value is not `count`, it means the buffer reached end while
/// reading.
/// In this case, if `size > 0`, this means the expected value is probably
/// incomplete.
///
/// \param stream Pointer to the stream instance
/// \param type   Data type to skip
///
#define bj_stream_skip_t(stream, type) bj_stream_read(stream, 0, sizeof(type))

/// \} End of array group
