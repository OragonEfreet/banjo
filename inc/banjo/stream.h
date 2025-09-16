////////////////////////////////////////////////////////////////////////////////
/// \file stream.h
/// \brief Header file for bj_stream_t struct and related functions
////////////////////////////////////////////////////////////////////////////////
/// \defgroup stream Data Stream
/// \{
////////////////////////////////////////////////////////////////////////////////
#ifndef BJ_STREAM_H
#define BJ_STREAM_H

#include <banjo/api.h>
#include <banjo/error.h>

////////////////////////////////////////////////////////////////////////////////
/// \brief Structure representing a stream of data
///
typedef struct bj_stream_t bj_stream; ///< Typedef for the bj_stream_t struct

////////////////////////////////////////////////////////////////////////////////
/// \brief Position in a \ref bj_stream to use for origin
///
typedef enum {
    BJ_SEEK_BEGIN   = 0x00, //!< The beginning of the stream
    BJ_SEEK_CURRENT = 0x01, //!< The current position of the stream
    BJ_SEEK_END     = 0x02, //!< The end of the stream
} bj_seek_origin;


////////////////////////////////////////////////////////////////////////////////
/// Allocate a new \ref bj_stream object
///
/// \return A pointer to a new \ref bj_stream
///
/// \par Memory Management
///
/// The object pointed by the returned value must be deleted using bj_free.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_stream* bj_allocate_stream(
    void
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Creates a new bj_stream for reading from a memory buffer.
///
/// \param p_data Pointer to the data buffer.
/// \param length Length of the data buffer in bytes.
/// \return A pointer to the newly created bj_stream object.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_stream* bj_open_stream_read(
    const void* p_data,
    size_t length
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Creates a new bj_stream for reading from a file.
///
/// \param p_path  The file path to open
/// \param p_error Optional error object
/// \return A pointer to an error object
///
/// The file memory is entirely copied to internal memory buffer.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_stream* bj_open_stream_file(
    const char*       p_path,
    bj_error**        p_error
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Deletes a bj_stream object and releases associated memory.
///
/// \param p_stream Pointer to the bj_stream object to delete.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_close_stream(
    bj_stream* p_stream
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Reads data from the stream into a destination buffer.
///
/// \param p_stream Pointer to the stream instance.
/// \param p_dest Pointer to the destination buffer.
/// \param count Number of bytes to read.
/// \return Number of bytes actually read.
///
/// The function advances the stream position by `count` bytes.
/// If fewer bytes than `count` are read, the end of the stream is reached.
///
/// \par Memory Safety
///
/// This function does not perform any memory bounds checking.
/// It is the caller's responsibility to ensure `p_dest` has enough space
/// to hold `count` bytes.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT size_t bj_read_stream(
    bj_stream* p_stream,
    void* p_dest,
    size_t count
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Get the size of the stream.
///
/// \param p_stream Pointer to the stream instance.
/// \return The size of the stream, in bytes
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT size_t bj_get_stream_length(
    bj_stream* p_stream
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Seeks to a new position in the stream relative to a specified origin.
///
/// \param p_stream Pointer to the stream instance.
/// \param position Offset for the new position.
/// \param from Origin relative to which the position should be calculated.
/// \return The new position within the stream after seeking.
///
/// The function clamps the new position to stay within the valid range
/// of the stream, from 0 to the length of the stream.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT size_t bj_seek_stream(
    bj_stream* p_stream,
    ptrdiff_t position,
    bj_seek_origin from
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Returns the current position of the cursor in the stream.
///
/// \param p_stream Pointer to the stream instance.
/// \return The current position within the stream.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT size_t bj_tell_stream(
    bj_stream* p_stream
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Reads data of a specified type from the stream into a buffer.
///
/// \param stream Pointer to the stream instance.
/// \param type Data type to read.
/// \param buffer Pointer to the buffer to store the read data.
///
#define bj_stream_read_t(stream, type, buffer) bj_read_stream(stream, buffer, sizeof(type))

////////////////////////////////////////////////////////////////////////////////
/// \brief Skips reading data of a specified type from the stream.
///
/// \param stream Pointer to the stream instance.
/// \param type Data type to skip.
///
#define bj_stream_skip_t(stream, type) bj_read_stream(stream, 0, sizeof(type))

#endif
/// \} // End of stream group
