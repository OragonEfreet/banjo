/// \file error.h
/// Recoverable error handling
////////////////////////////////////////////////////////////////////////////////
/// \defgroup error Error Management
///
/// \brief Recoverable error handling facilities
///
/// This API provides a mechanism for communicating recoverable errors from
/// callee to caller, inspired by GLib's GError system.
///
/// ## Basic Usage
///
/// Functions that may fail take a `bj_error**` as their last parameter.
/// Pass a pointer to a `NULL` `bj_error*` to receive error details:
///
/// \snippet handling_errors.c Using bj_error
///
/// ## Returning Errors
///
/// Use \ref bj_set_error or \ref bj_set_error_fmt from inside a function
/// that may fail:
///
/// \snippet handling_errors.c Return Errors
///
/// ## Error Propagation
///
/// When calling a fallible function from another fallible function,
/// use \ref bj_propagate_error to forward errors up the call stack:
///
/// ```c
/// bj_error* local_err = NULL;
/// some_operation(&local_err);
/// if (local_err != NULL) {
///     bj_propagate_error(error, local_err);
///     return;
/// }
/// ```
///
/// ## Adding Context
///
/// Use \ref bj_prefix_error to add context to an existing error:
///
/// ```c
/// if (local_err != NULL) {
///     bj_prefix_error(&local_err, "While loading config: ");
///     bj_propagate_error(error, local_err);
///     return;
/// }
/// ```
///
/// ## Zero-Cost When Unused
///
/// Passing `NULL` for the error parameter indicates you don't care about
/// error details. In this case, no allocation occurs - just a pointer check.
///
/// \{
////////////////////////////////////////////////////////////////////////////////
#ifndef BJ_ERROR_H
#define BJ_ERROR_H

#include <banjo/api.h>

////////////////////////////////////////////////////////////////////////////////
/// \brief A numeric representation of an error in Banjo.
///
/// Error codes are 32-bit values with a hierarchical structure:
///
/// ```
/// +----------+------------------+------------+
/// | Origin   | Specific Code    | Kind       |
/// | (8 bits) | (16 bits)        | (8 bits)   |
/// +----------+------------------+------------+
///   MSB                              LSB
/// ```
///
/// - **Origin** (MSB): `0x00` for Banjo errors, non-zero for user-defined.
/// - **Specific Code**: Unique identifier within the kind.
/// - **Kind** (LSB): Category shared by related errors.
///
/// For example, `BJ_ERROR_INCORRECT_VALUE` is `0x00000204`:
/// - Origin: `0x00` (Banjo)
/// - Specific: `0x0002`
/// - Kind: `0x04` (same as `BJ_ERROR_INVALID_DATA`)
///
/// Use \ref bj_error_code_kind to extract the kind from any error code.
/// Use \ref bj_error_code_is_user to check if an error is user-defined.
///
////////////////////////////////////////////////////////////////////////////////
enum bj_error_code {
    /// No error occurred.
    BJ_ERROR_NONE            = 0x00000000,

    // Kind 0x01: General/Unspecified errors
    /// General unspecified error.
    BJ_ERROR                 = 0x00000001,
    /// Operation not supported on this platform or configuration.
    BJ_ERROR_UNSUPPORTED     = 0x00000101,
    /// Feature not yet implemented.
    BJ_ERROR_NOT_IMPLEMENTED = 0x00000201,

    // Kind 0x02: System/OS errors
    /// Generic operating system error.
    BJ_ERROR_SYSTEM          = 0x00000002,
    /// Requested file was not found.
    BJ_ERROR_FILE_NOT_FOUND  = 0x00000102,
    /// Memory allocation failed.
    BJ_ERROR_CANNOT_ALLOCATE = 0x00000202,
    /// System component initialization failed.
    BJ_ERROR_INITIALIZE      = 0x00000302,
    /// System component cleanup failed.
    BJ_ERROR_DISPOSE         = 0x00000402,

    // Kind 0x03: I/O errors
    /// Generic I/O error.
    BJ_ERROR_IO              = 0x00000003,
    /// Error while reading from a file or stream.
    BJ_ERROR_CANNOT_READ     = 0x00000103,
    /// Error while writing to a file or stream.
    BJ_ERROR_CANNOT_WRITE    = 0x00000203,

    // Kind 0x04: Data validation errors
    /// Generic invalid data error.
    BJ_ERROR_INVALID_DATA    = 0x00000004,
    /// Data format does not match expected format.
    BJ_ERROR_INVALID_FORMAT  = 0x00000104,
    /// Value does not match expected value.
    BJ_ERROR_INCORRECT_VALUE = 0x00000204,

    // Kind 0x05: Video subsystem errors
    /// Error in video/graphics subsystem.
    BJ_ERROR_VIDEO           = 0x00000005,

    // Kind 0x06: Audio subsystem errors
    /// Error in audio subsystem.
    BJ_ERROR_AUDIO           = 0x00000006,

    // Kind 0x07: Network subsystem errors
    /// Error in network subsystem.
    BJ_ERROR_NETWORK         = 0x00000007,
    /// Socket error
    BJ_ERROR_NETWORK_SOCKET  = 0x00000107,
};

////////////////////////////////////////////////////////////////////////////////
/// \brief Extracts the kind (category) from an error code.
///
/// The kind is the least significant byte, shared by all errors in the same
/// category. For example:
/// ```c
/// bj_error_code_kind(BJ_ERROR_CANNOT_ALLOCATE) == BJ_ERROR_SYSTEM  // true
/// bj_error_code_kind(BJ_ERROR_INVALID_FORMAT)  == BJ_ERROR_INVALID_DATA  // true
/// ```
///
/// \param code The error code to extract the kind from.
/// \return The kind portion of the error code.
////////////////////////////////////////////////////////////////////////////////
#define bj_error_code_kind(code) ((code) & 0x000000FFu)

////////////////////////////////////////////////////////////////////////////////
/// \brief Checks if an error code is user-defined (not from Banjo).
///
/// Banjo error codes have their most significant byte set to `0x00`.
/// User applications can use non-zero values in the MSB to define their
/// own error codes without conflicting with Banjo.
///
/// \param code The error code to check.
/// \return Non-zero if the error is user-defined, zero if it's a Banjo error.
////////////////////////////////////////////////////////////////////////////////
#define bj_error_code_is_user(code) (((code) >> 24) & 0xFFu)

////////////////////////////////////////////////////////////////////////////////
/// \brief Opaque error structure.
///
/// Contains an error code and a human-readable message describing what went
/// wrong. Created by \ref bj_set_error or \ref bj_set_error_fmt, and must
/// be freed with \ref bj_clear_error.
///
/// Access the code with \ref bj_error_code and the message with
/// \ref bj_error_message.
////////////////////////////////////////////////////////////////////////////////
struct bj_error;

////////////////////////////////////////////////////////////////////////////////
/// \brief Creates a new error with a literal message.
///
/// Allocates and initializes a new error object. Use this when the error
/// message is a compile-time constant or doesn't need formatting.
///
/// If `*error` is already non-NULL, the new error is logged but not stored,
/// preserving the original error (first error wins).
///
/// If `error` is NULL, the error is logged in debug builds but no allocation
/// occurs (zero cost).
///
/// \param error   Pointer to error location. May be NULL.
/// \param code    Error code from \ref bj_error_code or user-defined.
/// \param message Null-terminated error message.
///
/// \see bj_set_error_fmt For formatted messages.
/// \see bj_clear_error To free the error.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_set_error(
    struct bj_error** error,
    uint32_t          code,
    const char*       message
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Creates a new error with a formatted message.
///
/// Like \ref bj_set_error, but accepts printf-style format arguments.
///
/// ```c
/// bj_set_error_fmt(&error, BJ_ERROR_FILE_NOT_FOUND,
///                  "Cannot open '%s': %s", path, strerror(errno));
/// ```
///
/// \param error  Pointer to error location. May be NULL.
/// \param code   Error code.
/// \param format Printf-style format string.
/// \param ...    Format arguments.
///
/// \see bj_set_error For literal messages (slightly more efficient).
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_set_error_fmt(
    struct bj_error** error,
    uint32_t          code,
    const char*       format,
    ...
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Propagates an error to the caller's error location.
///
/// Transfers ownership of `src` to `*dest`. After this call, `src` is
/// consumed and must not be used or freed.
///
/// If `dest` is NULL, the error is logged and freed (caller doesn't want it).
/// If `*dest` is already non-NULL, `src` is logged and freed (first error wins).
///
/// Typical usage:
/// ```c
/// bj_error* local_err = NULL;
/// do_something(&local_err);
/// if (local_err != NULL) {
///     bj_propagate_error(error, local_err);
///     return FAILURE;
/// }
/// ```
///
/// \param dest Caller's error location. May be NULL.
/// \param src  Error to propagate. Consumed by this call.
///
/// \see bj_propagate_prefixed_error To add context while propagating.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_propagate_error(
    struct bj_error** dest,
    struct bj_error*  src
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Propagates an error with an added prefix.
///
/// Combines \ref bj_prefix_error_fmt and \ref bj_propagate_error in one call.
/// Adds context to the error message before propagating.
///
/// ```c
/// bj_propagate_prefixed_error(error, local_err,
///                             "While loading '%s': ", filename);
/// ```
///
/// \param dest   Caller's error location. May be NULL.
/// \param src    Error to propagate. Consumed by this call.
/// \param format Printf-style prefix format.
/// \param ...    Format arguments.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_propagate_prefixed_error(
    struct bj_error** dest,
    struct bj_error*  src,
    const char*       format,
    ...
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Adds a prefix to an existing error's message.
///
/// Prepends text to the error message to add context about where/why
/// the error occurred.
///
/// ```c
/// if (err != NULL) {
///     bj_prefix_error(&err, "In function foo: ");
///     // Message is now "In function foo: original message"
/// }
/// ```
///
/// If `*error` is NULL, this function does nothing.
///
/// \param error  Pointer to the error to modify.
/// \param prefix Null-terminated prefix string.
///
/// \see bj_prefix_error_fmt For formatted prefixes.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_prefix_error(
    struct bj_error** error,
    const char*       prefix
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Adds a formatted prefix to an existing error's message.
///
/// Like \ref bj_prefix_error, but with printf-style formatting.
///
/// ```c
/// bj_prefix_error_fmt(&err, "While processing item %d: ", index);
/// ```
///
/// \param error  Pointer to the error to modify.
/// \param format Printf-style format string for the prefix.
/// \param ...    Format arguments.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_prefix_error_fmt(
    struct bj_error** error,
    const char*       format,
    ...
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Creates a copy of an error.
///
/// Returns a newly allocated error with the same code and message.
/// The caller owns the copy and must free it with \ref bj_clear_error.
///
/// \param error The error to copy. May be NULL.
/// \return A new error copy, or NULL if `error` was NULL.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT struct bj_error* bj_copy_error(
    const struct bj_error* error
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Frees an error and sets the pointer to NULL.
///
/// Safe to call with a NULL pointer or pointer to NULL.
///
/// ```c
/// bj_clear_error(&err);  // err is now NULL
/// bj_clear_error(&err);  // Safe, does nothing
/// ```
///
/// \param error Pointer to error to free.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_clear_error(
    struct bj_error** error
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Checks if an error matches a specific error code.
///
/// Returns true if `error` is non-NULL and has the specified code.
/// Use this for exact matching of error codes.
///
/// ```c
/// if (bj_error_matches(err, BJ_ERROR_FILE_NOT_FOUND)) {
///     // Handle missing file specifically
/// }
/// ```
///
/// \param error The error to check. May be NULL.
/// \param code  The error code to match against.
/// \return BJ_TRUE if error matches the code, BJ_FALSE otherwise.
///
/// \see bj_error_matches_kind For matching error categories.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_bool bj_error_matches(
    const struct bj_error* error,
    uint32_t               code
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Checks if an error belongs to a specific error kind (category).
///
/// Returns true if `error` is non-NULL and its kind matches the specified
/// kind. Use this to handle categories of errors uniformly.
///
/// ```c
/// if (bj_error_matches_kind(err, BJ_ERROR_IO)) {
///     // Handle any I/O error (read, write, etc.)
/// }
/// ```
///
/// \param error The error to check. May be NULL.
/// \param kind  The error kind to match (e.g., BJ_ERROR_IO, BJ_ERROR_SYSTEM).
/// \return BJ_TRUE if error's kind matches, BJ_FALSE otherwise.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_bool bj_error_matches_kind(
    const struct bj_error* error,
    uint32_t               kind
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Gets the error code from an error object.
///
/// \param error The error to query. May be NULL.
/// \return The error code, or BJ_ERROR_NONE if error is NULL.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT uint32_t bj_error_code(
    const struct bj_error* error
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Gets the error message from an error object.
///
/// The returned string is owned by the error object and remains valid
/// until the error is freed or modified.
///
/// \param error The error to query. May be NULL.
/// \return The error message, or NULL if error is NULL.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT const char* bj_error_message(
    const struct bj_error* error
);

#endif
/// \} End of error group
