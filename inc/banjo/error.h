/// \file
/// Recoverable error handling
///
/// This API provides a simple mechanism for communicating recoverable errors
/// from callee to caller.
/// 
/// When a function may fail, it takes a \ref bj_error pointer as a parameter.
/// The failing function may fill in this value to communicate error while
/// the caller can check for the value of \ref bj_error.code to check if 
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

////////////////////////////////////////////////////////////////////////////////
/// \brief Erorr domain values
enum bj_error_domain {
    BJ_NO_DOMAIN,
    BJ_DOMAIN_IO = 4,
};

////////////////////////////////////////////////////////////////////////////////
/// \brief Erorr domain values
enum bj_error_code {
    BJ_NO_ERROR,
    BJ_CANNOT_OPEN_FILE = 7,
};


////////////////////////////////////////////////////////////////////////////////
/// The error object.
///
/// `domain` and `code` are user defined.
/// An error object is considered as representing an error if `code != 0`.
typedef u32 bj_error;

////////////////////////////////////////////////////////////////////////////////
/// Fills in a \ref bj_error object with given domain and code.
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
    bj_error* p_error,
    u16      domain,
    u16      code
);


