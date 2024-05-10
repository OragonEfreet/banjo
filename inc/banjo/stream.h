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
/// \todo Add write features
struct bj_stream_t {
    bool    weak;       //!< True if memory buffer is not managed by the object
    usize   len;        //!< Size of the stream (in bytes)
    usize   position;   //!< Current position within the stream
    /// Array of stream data
    union {
        const u8* r;    //!< Read-only access
    } p_data;
};

////////////////////////////////////////////////////////////////////////////////
/// \brief Initializes a stream with default settings
///
/// The stream is filled with and uninitialized buffer of `size` bytes.
///
/// \param p_stream Pointer to the stream instance
/// \param size       Size of the stream
/// \return Pointer to the initialized stream instance
///
BANJO_EXPORT bj_stream* bj_stream_init_default(
    bj_stream* p_stream,
    usize      size
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Initializes a stream for reading data
///
/// The underlying buffer is not copied into the object.
///
/// \param p_stream Pointer to the stream instance
/// \param p_data     Pointer to the data to be read
/// \param length     Length of the data in bytes
/// \return Pointer to the initialized stream instance
///
BANJO_EXPORT bj_stream* bj_stream_init_read(
    bj_stream*   p_stream,
    const void*  p_data,
    usize        length
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Resets the stream to nil state
///
/// \param p_stream Pointer to the stream instance
/// \return Pointer to the stream instance after resetting
///
BANJO_EXPORT bj_stream* bj_stream_reset(
    bj_stream* p_stream
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
/// \par Memory Safety
///
/// This function does not perform any memory checking, meaning that is it up
/// to the caller to ensure that `p_dest` if large enough to hold the read
/// data.
/// If `count` is larger than the data buffer, there wwill be buffer overflow.
///
/// \param p_stream Pointer to the stream instance
/// \param p_dest     Pointer to the destination buffer
/// \param count      Number of bytes to read
///
/// \return Number of bytes read
///
BANJO_EXPORT usize bj_stream_read(
    bj_stream* p_stream,
    void*      p_dest,
    usize      count
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Seeks to a new position in the stream, based on the specified origin.
///
/// This function adjusts the current position of the stream by a given offset,
/// starting from a specified origin.
/// The new position is clamped to ensure it remains within the valid range of
/// the stream, from 0 to the length of the stream.
///
/// \param p_stream Pointer to the instance of bj_stream.
/// \param position   Offset for the new position. Can be positive or negative.
/// \param from       Origin relative to which the position should be calculated.
///
/// \return The new position within the stream after seeking.
///
BANJO_EXPORT usize bj_stream_seek(
    bj_stream*     p_stream,
    size           position,
    bj_seek_origin from
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Returns the position of the cursor in the given stream.
///
/// \param p_stream The stream to get positiion from.
///
/// \return The position within the stream.
BANJO_EXPORT usize bj_stream_tell(
    bj_stream*     p_stream
);

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
