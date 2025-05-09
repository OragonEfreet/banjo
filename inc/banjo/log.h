////////////////////////////////////////////////////////////////////////////////
/// \file
/// Logging utility functions
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// \defgroup log Logging
/// \ingroup core
///
/// \brief Logging utility functions
/// \{
#pragma once

#include <banjo/api.h>


////////////////////////////////////////////////////////////////////////////////
/// Maximum length of log messages
///
/// Log messages, including header information such as timestamp and log level,
/// cannot have a size in bytes larger than `BJ_MAXIMUM_LOG_LEN`, excluding the
/// null character.
/// In case a message is longer than this limit, the content is truncated in
/// order of priority as described in \ref bj_message.
#define BJ_MAXIMUM_LOG_LEN 120

////////////////////////////////////////////////////////////////////////////////
/// Log Levels.
/// Level values are ordered from 0.
///
enum {
    BJ_LOG_TRACE, ///< Fine-grained diagnostic details.
    BJ_LOG_DEBUG, ///< Detailed information for debugging.
    BJ_LOG_INFO,  ///< Informational messages about execution.
    BJ_LOG_WARN,  ///< Warnings for potential issues.
    BJ_LOG_ERROR, ///< Errors preventing correct function.
    BJ_LOG_FATAL  ///< Critical errors leading to termination.
};
    
////////////////////////////////////////////////////////////////////////////////
/// Log a message using the given level `LEVEL`.
///
/// Any integer value can be used for `LEVEL`, but usually any of 
/// `BJ_LOG_TRACE`, `BJ_LOG_DEBUG`, `BJ_LOG_INFO`,  `BJ_LOG_WARN`,
/// `BJ_LOG_ERROR` or `BJ_LOG_FATAL` is used.
///
/// When a message is sent with a given level, it will be reported only if the
/// value set with \ref bj_log_set_level is at least the same value.
///
/// The function effectively calls \ref bj_message, forwarding arguments to
/// format the input string.
///
/// \param LEVEL The log level to report the message in.
/// \param ...   Arguments forwarded to \ref bj_message.
///
/// \see bj_message, bj_trace, bj_debug, bj_info, bj_warn, bj_err, bj_fatal.
#ifdef BJ_BUILD_RELEASE
#   define bj_log(LEVEL, ...) bj_message(BJ_LOG_ ## LEVEL, 0, 0, __VA_ARGS__)
#else
#   define bj_log(LEVEL, ...) bj_message(BJ_LOG_ ## LEVEL, __FILE__, __LINE__, __VA_ARGS__)
#endif

////////////////////////////////////////////////////////////////////////////////
/// Log a message using the `BJ_LOG_TRACE` level.
///
/// The function effectively calls \ref bj_message, forwarding arguments to
/// format the input string.
///
/// This function is preferred over calling \ref bj_log of \ref bj_message
/// directly for clarity.
///
/// \param ... The string formatting, forwarded to the last arguments of \ref bj_message.
///
/// \see bj_log, bj_trace, bj_debug, bj_info, bj_warn, bj_err, bj_fatal.
#define bj_trace(...)      bj_log(TRACE, __VA_ARGS__) ///< Log using BJ_LOG_TRACE level.

////////////////////////////////////////////////////////////////////////////////
/// Log a message using the `BJ_LOG_DEBUG` level.
///
/// The function effectively calls \ref bj_message, forwarding arguments to
/// format the input string.
///
/// This function is preferred over calling \ref bj_log of \ref bj_message
/// directly for clarity.
///
/// \param ... The string formatting, forwarded to the last arguments of \ref bj_message.
///
/// \see bj_trace, bj_trace, bj_debug, bj_info, bj_warn, bj_err, bj_fatal.
#define bj_debug(...)      bj_log(DEBUG, __VA_ARGS__) ///< Log using BJ_LOG_DEBUG level.

////////////////////////////////////////////////////////////////////////////////
/// Log a message using the `BJ_LOG_INFO` level.
///
/// The function effectively calls \ref bj_message, forwarding arguments to
/// format the input string.
///
/// This function is preferred over calling \ref bj_log of \ref bj_message
/// directly for clarity.
///
/// \param ... The string formatting, forwarded to the last arguments of \ref bj_message.
///
/// \see bj_debug, bj_trace, bj_debug, bj_info, bj_warn, bj_err, bj_fatal.
#define bj_info(...)       bj_log(INFO, __VA_ARGS__)  ///< Log using BJ_LOG_INFO level.

////////////////////////////////////////////////////////////////////////////////
/// Log a message using the `BJ_LOG_WARN` level.
///
/// The function effectively calls \ref bj_message, forwarding arguments to
/// format the input string.
///
/// This function is preferred over calling \ref bj_log of \ref bj_message
/// directly for clarity.
///
/// \param ... The string formatting, forwarded to the last arguments of \ref bj_message.
///
/// \see bj_info, bj_trace, bj_debug, bj_info, bj_warn, bj_err, bj_fatal.
#define bj_warn(...)       bj_log(WARN, __VA_ARGS__)  ///< Log using BJ_LOG_WARN level.

////////////////////////////////////////////////////////////////////////////////
/// Log a message using the `BJ_LOG_ERROR` level.
///
/// The function effectively calls \ref bj_message, forwarding arguments to
/// format the input string.
///
/// This function is preferred over calling \ref bj_log of \ref bj_message
/// directly for clarity.
///
/// \param ... The string formatting, forwarded to the last arguments of \ref bj_message.
///
/// \see bj_warn, bj_trace, bj_debug, bj_info, bj_warn, bj_err, bj_fatal.
#define bj_err(...)      bj_log(ERROR, __VA_ARGS__)  ///< Log using BJ_LOG_ERROR level.

////////////////////////////////////////////////////////////////////////////////
/// Log a message using the `BJ_LOG_FATAL` level.
///
/// The function effectively calls \ref bj_message, forwarding arguments to
/// format the input string.
///
/// This function is preferred over calling \ref bj_log of \ref bj_message
/// directly for clarity.
///
/// \param ... The string formatting, forwarded to the last arguments of \ref bj_message.
///
/// \see bj_err, bj_trace, bj_debug, bj_info, bj_warn, bj_err, bj_fatal.
#define bj_fatal(...)      bj_log(FATAL, __VA_ARGS__)  ///< Log using BJ_LOG_FATAL level.

////////////////////////////////////////////////////////////////////////////////
/// Returns a string describing the given `level`.
///
/// The function is only valid if called with `BJ_LOG_TRACE`, `BJ_LOG_DEBUG`,
/// `BJ_LOG_INFO`,  `BJ_LOG_WARN`, `BJ_LOG_ERROR` or `BJ_LOG_FATAL`.
///
/// The returned value is owned by the callee.
///
/// \param level The log level to get the description from.
///
/// \return A C-String describing `level`.
BANJO_EXPORT const char* bj_log_get_level_string(int level);

////////////////////////////////////////////////////////////////////////////////
/// Sets the default log level.
///
/// Once set, any call to \ref bj_message, \ref bj_log and helper macros
/// will only report a message is its level is at least `level`.
///
/// \param level The level to set.
///
/// If not set, default is \ref BJ_LOG_TRACE.
BANJO_EXPORT void bj_log_set_level(int level);

////////////////////////////////////////////////////////////////////////////////
/// Gets the current log level set by \ref bj_log_set_level.
///
/// \return An integer value describing the current log level.
BANJO_EXPORT int bj_log_get_level(void);

////////////////////////////////////////////////////////////////////////////////
/// Generic message reporting function.
///
/// Sends a message using the given `level`.
/// Message will only be reported if `level` is at least the value set as a
/// parameter to \ref bj_log_set_level (or \ref BJ_LOG_TRACE if never called).
/// 
/// The more convenient functions \ref bj_log, \ref bj_trace, ... should be
/// used for simplicity.
///
/// The string formatting follows standard `printf` signature and behaviour.
///
/// \param level           The log level the message is reported in.
/// \param p_file          The file name the message is reported from.
/// \param line            The line number the message is reported from.
/// \param p_format, ...   The formatted string to report.
///
/// \return The actual number of characters written,
///         excluding the null-terminating symbol.
///
/// \par Format and Behavior
///
/// The log message will have the following format:
/// - `TIME LEVEL: (SOURCE) MESSAGE`
/// Where:
/// - `TIME` is in the format "HH:MM:SS" and takes 8 characters
/// - `LEVEL`, in square brackets, is the result of calling
///   \ref bj_log_get_level_string. It takes up to 4 to 5 characters.
/// - `SOURCE` is in the format "FILE:LINE" and takes an undetermined number of 
///   characters. This is only present in a debug build.
/// - `MESSAGE` is the result of calling `snprintf(p_format, ...)` and the number
///   of character varies.
///
/// For example: `12:23:34 WARN: logging.c:27 Warning level message`
///
/// The maximum length a log message will take is hardcoded to the value set
/// for `BJ_MAXIMUM_LOG_LEN`.
///
/// A first buffer is filled in with the expansion of the message.
/// This message is truncated to fit the \ref BJ_MAXIMUM_LOG_LEN characters 
/// limit entirely.
/// If this limit is already reached, the message is logged as-is.
///
/// Then, in the order of below priority:
/// - If `BJ_CONFIG_LOG_COLOR` is set and but is not enough space for colored 
///   source (Debug only), `SOURCE` is not colored.
/// - If `BJ_CONFIG_LOG_COLOR` is set and but is not enough space for colored 
///   level, `LEVEL` is not colored.
/// - If there is not enough space left for `TIME`, it is not present.
/// - If there is not enough space for `SOURCE` (Debug only), it is not 
///   present.
/// - If there is not enough space for `LEVEL`, it is not present.
///
/// \see bj_log, bj_trace, bj_debug, bj_info, bj_warn, bj_err, bj_fatal
BANJO_EXPORT size_t bj_message(int level, const char* p_file, int line, const char* p_format, ...);

/// \} // End of array log
