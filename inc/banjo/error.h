/// \file
/// Recoverable error handling
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
#pragma once

#include <banjo/api.h>
#include <banjo/memory.h>

#define BJ_ERROR_MESSAGE_MAX_LEN 127

typedef enum {
    BJ_ERROR_NONE                = 0x00000000, ///< No Error
    BJ_ERROR                     = 0x00000001, ///< General Error (unspecified)

    BJ_ERROR_OS                  = 0x00000002, ///< Generic OS error
    BJ_ERROR_FILE_NOT_FOUND      = 0x00000102, ///< The request file was not found
    BJ_ERROR_CANNOT_ALLOCATE     = 0x00000202, ///< Cannot allocate the specified memory block

    BJ_ERROR_IO                  = 0x00000003, ///< IO-related errors
    BJ_ERROR_CANNOT_READ_FILE    = 0x00000103, ///< IO-related errors

    BJ_ERROR_INVALID_DATA        = 0x00000004, ///< Incorrect data
    BJ_ERROR_INVALID_FORMAT      = 0x00000104, ///< Associated data does not fit expected format
    BJ_ERROR_INCORRECT_VALUE     = 0x00000204, ///< Expected value mismatch
} bj_error_code;

#define bj_error_code_is_user (c) (((c >> 24) & 0xFF) > 0x00)
#define bj_error_code_category(c) (c & 0x000000FF)

typedef struct {
    u32         code;
    char        message[BJ_ERROR_MESSAGE_MAX_LEN+1];
} bj_error;

////////////////////////////////////////////////////////////////////////////////
/// Fills in a \ref bj_error object with given code and message
///
/// \param p_error The error to fill in.
/// \param code    Error code.
///
/// If `p_error` is null, the function does nothing.
///
/// If `p_error` is not null, the function only stored the error if `code == 0`.
/// Otherwise, `code` and `message` are only reported as logs.
BANJO_EXPORT void bj_set_error(
    bj_error**  p_error,
    u32         code,
    const char* message
);

BANJO_EXPORT bool bj_error_check(
    const bj_error* p_error,
    u32 code
);

BANJO_EXPORT void bj_forward_error(
    bj_error*  p_source,
    bj_error** p_destination
);

BANJO_EXPORT void bj_clear_error(
    bj_error** p_error
);
