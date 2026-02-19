////////////////////////////////////////////////////////////////////////////////
/// \file stream.h
/// \brief Header file for struct bj_stream struct and related functions
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
struct bj_stream;

////////////////////////////////////////////////////////////////////////////////
/// \brief Position in a \ref bj_stream to use for origin
///
enum bj_seek_origin {
    BJ_SEEK_BEGIN   = 0x00, //!< The beginning of the stream
    BJ_SEEK_CURRENT = 0x01, //!< The current position of the stream
    BJ_SEEK_END     = 0x02, //!< The end of the stream
};
#ifndef BJ_NO_TYPEDEF
typedef enum bj_seek_origin bj_seek_origin;
#endif


////////////////////////////////////////////////////////////////////////////////
/// Allocate a new bj_stream object
///
/// \return A pointer to a new  bj_stream
///
/// \par Memory Management
///
/// The object pointed by the returned value must be deleted using bj_free.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT struct bj_stream* bj_allocate_stream(
    void
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Creates a new struct bj_stream for reading from a memory buffer.
///
/// \param data Pointer to the data buffer.
/// \param length Length of the data buffer in bytes.
/// \return A pointer to the newly created struct bj_stream object.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT struct bj_stream* bj_open_stream_read(
    const void* data,
    size_t length
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Creates a new struct bj_stream for reading from a file.
///
/// \param path  The file path to open
/// \param error Optional error object
/// \return A pointer to an error object
///
/// The file memory is entirely copied to internal memory buffer.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT struct bj_stream* bj_open_stream_file(
    const char*       path,
    struct bj_error**        error
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Deletes a struct bj_stream object and releases associated memory.
///
/// \param stream Pointer to the struct bj_stream object to delete.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_close_stream(
    struct bj_stream* stream
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Reads data from the stream into a destination buffer.
///
/// \param stream Pointer to the stream instance.
/// \param dest Pointer to the destination buffer.
/// \param count Number of bytes to read.
/// \return Number of bytes actually read.
///
/// The function advances the stream position by `count` bytes.
/// If fewer bytes than `count` are read, the end of the stream is reached.
///
/// \par Memory Safety
///
/// This function does not perform any memory bounds checking.
/// It is the caller's responsibility to ensure `dest` has enough space
/// to hold `count` bytes.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT size_t bj_read_stream(
    struct bj_stream* stream,
    void* dest,
    size_t count
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Get the size of the stream.
///
/// \param stream Pointer to the stream instance.
/// \return The size of the stream, in bytes
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT size_t bj_get_stream_length(
    struct bj_stream* stream
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Seeks to a new position in the stream relative to a specified origin.
///
/// \param stream Pointer to the stream instance.
/// \param position Offset for the new position.
/// \param from Origin relative to which the position should be calculated.
/// \return The new position within the stream after seeking.
///
/// The function clamps the new position to stay within the valid range
/// of the stream, from 0 to the length of the stream.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT size_t bj_seek_stream(
    struct bj_stream* stream,
    ptrdiff_t position,
    enum bj_seek_origin from
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Returns the current position of the cursor in the stream.
///
/// \param stream Pointer to the stream instance.
/// \return The current position within the stream.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT size_t bj_tell_stream(
    struct bj_stream* stream
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
