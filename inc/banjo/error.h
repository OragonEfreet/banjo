/// \file
/// Recoverable error handling
///
/// This API provides a simple mechanism for communicating recoverable errors
/// from callee to caller.
/// 
/// When a function may fail, it takes a \ref BjError instance as a parameter.
/// The failing function may fill in this structure to communicate error while
/// the caller can check for the value of \ref BjError.code to check if 
/// any error occurred:
/// 
/// \snippet handling_errors.c Using BjError
///
/// Use \ref bj_set_error from inside a custom function that may result in an
/// error:
///
/// \snippet handling_errors.c Return Errors
#pragma once

#include <banjo/api.h>
#include <banjo/memory.h>

////////////////////////////////////////////////////////////////////////////////
/// The error object.
///
/// `domain` and `code` are user defined.
/// An error object is considered as representing an error if `code != 0`.
typedef struct {
    u32 domain; ///< The domain identifier.
    u32 code;   ///< The error code.
} BjError;

////////////////////////////////////////////////////////////////////////////////
/// Fills in a \ref BjError object with given domain and code.
///
/// \param p_error The error to fill in.
/// \param domain  Domain identifier.
/// \param code    Error code.
///
/// If `p_error` is null, the function does nothing.
///
/// If `p_error` is not null, the function only stored the error if `code == 0`.
/// Otherwise, `code` and `domain` are only reported as logs.
BANJO_EXPORT void bj_set_error(
    BjError* p_error,
    u32      domain,
    u32      code
);


