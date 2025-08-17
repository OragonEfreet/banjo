/// \file
/// Recoverable error handling
///
////////////////////////////////////////////////////////////////////////////////
/// \defgroup error Error Management
/// \ingroup core
///
/// \brief Recoverable error handling facilities
///
/// This API provides a simple mechanism for communicating recoverable errors
/// from callee to caller.
/// 
/// When a function may fail, it takes a \ref bj_error pointer as a parameter.
/// The failing function may fill in this value to communicate error while
/// the caller can check for the value of \ref bj_error_code to check if 
/// any error occurred:
/// 
/// \snippet handling_errors.c Using bj_error
///
/// Use \ref bj_set_error from inside a custom function that may result in an
/// error:
///
/// \snippet handling_errors.c Return Errors
///
/// \{
#ifndef BJ_ERROR_H
#define BJ_ERROR_H

#include <banjo/api.h>
#include <banjo/memory.h>

////////////////////////////////////////////////////////////////////////////////
/// \brief A numeric representation of an error in Banjo.
///
/// Within a \ref bj_error object, the first field is a `uint32_t` value indicating
/// the type of error encountered.
///
/// An error code is a 32-bit value composed as follows:
/// - The most significant byte is always _0x00_.
/// - The least significant byte represents the error kind.
/// - The two middle bytes represent the specific error code.
///
/// For example, `BJ_ERROR_INCORRECT_VALUE` is _0x204_, where:
/// - The most significant byte is _0x00_,
/// - The next two bytes are _0x0002_,
/// - The least significant byte is _0x04_.
///
/// \par Internal specifier
///
/// The most significant byte is always _0x00_ for Banjo errors.
/// User applications can set a different value to this byte to identify
/// the origin of the error code.
///
/// You can use \ref bj_error_code_is_user to check if an error code is defined
/// outside of Banjo.
///
/// \par Error Kind
///
/// The error kind is a numerical value set on the least significant byte
/// that is the same for all values of the same category.
///
/// For example, `BJ_ERROR_INVALID_FORMAT` and `BJ_ERROR_INCORRECT_VALUE` have
/// the same kind, which is equal to _0x04_.
///
/// Every error kind has its own error code, represented by a `uint32_t` with only
/// the least significant byte set.
/// For example: `BJ_ERROR_INVALID_DATA`.
///
/// You can use \ref bj_error_code_kind to get the error kind code of any error code.
///
typedef enum {
    BJ_ERROR_NONE                = 0x00000000, ///< No Error
    BJ_ERROR                     = 0x00000001, ///< General Error (unspecified)
    BJ_ERROR_UNSUPPORTED         = 0x00000101, ///< Unsupported operation error
    BJ_ERROR_NOT_IMPLEMENTED     = 0x00000201, ///< Unsupported operation error

    BJ_ERROR_SYSTEM              = 0x00000002, ///< Generic OS error
    BJ_ERROR_FILE_NOT_FOUND      = 0x00000102, ///< The requested file was not found
    BJ_ERROR_CANNOT_ALLOCATE     = 0x00000202, ///< Cannot allocate the specified memory block
    BJ_ERROR_INITIALIZE          = 0x00000302, ///< A system component cannot be initalized
    BJ_ERROR_DISPOSE             = 0x00000302, ///< A system component cannot be terminated properly

    BJ_ERROR_IO                  = 0x00000003, ///< IO-related errors
    BJ_ERROR_CANNOT_READ_FILE    = 0x00000103, ///< Error while attempting to read a file

    BJ_ERROR_INVALID_DATA        = 0x00000004, ///< Incorrect data
    BJ_ERROR_INVALID_FORMAT      = 0x00000104, ///< Data format does not match expected format
    BJ_ERROR_INCORRECT_VALUE     = 0x00000204, ///< Mismatched expected value

    BJ_ERROR_VIDEO = 0x00000005, ///< Error while running video layer
    
    BJ_ERROR_AUDIO = 0x00000006, ///< Error while running audio layer
} bj_error_code;

////////////////////////////////////////////////////////////////////////////////
/// Checks if an error code is user-provided or Banjo.
///
/// All Banjo error codes are guaranted to have their most signigicant byte set
/// to 0x00.
/// This helper macro evaluates to `BJ_TRUE` if the provided error is user-defined.
///
/// \param c The error code
///
#define bj_error_code_is_user (c) (((c >> 24) & 0xFF) > 0x00)

/// Evaluates to an error code kind
///
/// Banjo error codes are hierarchical.
/// The least significant byte is the same value for all error codes of the 
/// same kind, and corresponds to the generic error code value of the said
/// kind.
///
/// This helper macro evaluates to the kind error code of any given code.
///
/// For example, `bj_error_code_kind(BJ_ERROR_CANNOT_ALLOCATE)` evaluates to
/// `BJ_ERROR_OS`.
///
/// \param c The error code
///
#define bj_error_code_kind(c) (c & 0x000000FF)

/// Maximum number of characters an error message in \ref bj_error can hold.
#define BJ_ERROR_MESSAGE_MAX_LEN 127

////////////////////////////////////////////////////////////////////////////////
/// Error structure
///
/// Described any error reported by a potentially failing function.
///
/// Banjo-internals guarantee `code` to be a value of \ref bj_error_code.
///
typedef struct {
    uint32_t         code;                                ///< Error code.
    char        message[BJ_ERROR_MESSAGE_MAX_LEN+1]; ///< Optional error description.
} bj_error;

////////////////////////////////////////////////////////////////////////////////
/// Fills in a \ref bj_error object with given code and message
///
/// \param p_error The error to fill in.
/// \param code    Error code.
/// \param message The error message to set.
///
/// If `p_error` is null, the function does nothing.
///
/// If `p_error` is not null, the function only stored the error if `code == 0`.
/// Otherwise, `code` and `message` are only reported as logs.
///
/// `message` must point to a 0-terminated C-string of at max `BJ_ERROR_MESSAGE_MAX_LEN`
/// character.
/// If the given string is larger, it is truncated to `BJ_ERROR_MESSAGE_MAX_LEN`
/// and the nul pointer is enforced.
BANJO_EXPORT void bj_set_error(
    bj_error**  p_error,
    uint32_t         code,
    const char* message
);

////////////////////////////////////////////////////////////////////////////////
/// Checks if the given error matches the error code
///
/// If `p_error` is _0_, return _false_, which must be interpreted as "no error".
/// Otherwise, returns _true_ only if `p_error->code == code`.
///
/// \param p_error The error to check
/// \param code    The error code to match
///
/// \return _true_ if `p_error` matches `code`, _false_ otherwise.
///
BANJO_EXPORT bj_bool bj_error_check(
    const bj_error* p_error,
    uint32_t code
);

////////////////////////////////////////////////////////////////////////////////
/// Forward an error into another error location
///
/// This function is used when calling an error-returning function from another
/// error-returning function.
///
/// If `p_source` contains a non-zero error, it is moved to `p_destination`.
/// If `*p_destination` already contains an error, `p_source` is only logged
/// (using \ref bj_error).
///
/// \param p_source The source error.
/// \param p_destination The destination error pointer.
///
/// \return `BJ_TRUE` if both `p_source` and `p_destination` are non-zero.
///         When this happens, this means there was an error to propagate and
///         that the caller asked for it.
///
BANJO_EXPORT bj_bool bj_forward_error(
    bj_error*  p_source,
    bj_error** p_destination
);

////////////////////////////////////////////////////////////////////////////////
/// Clears the given error location.
///
/// \param p_error The error to clear.
BANJO_EXPORT void bj_clear_error(
    bj_error** p_error
);

#endif
/// \} // End of error Error Management
