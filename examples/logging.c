////////////////////////////////////////////////////////////////////////////////
/// \example logging.c
/// Using Banjo's logging system with severity levels and filtering.
///
/// Banjo provides a multi-level logging system for debugging and diagnostics.
/// Messages can be filtered by severity, formatted like printf, and queried
/// for output length.
////////////////////////////////////////////////////////////////////////////////
#include <banjo/assert.h>
#include <banjo/main.h>
#include <banjo/log.h>

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    // Banjo has six log levels in ascending severity:
    //   BJ_LOG_TRACE (0) - Detailed trace information
    //   BJ_LOG_DEBUG (1) - Debug information
    //   BJ_LOG_INFO  (2) - Informational messages
    //   BJ_LOG_WARN  (3) - Warning messages
    //   BJ_LOG_ERROR (4) - Error messages
    //   BJ_LOG_FATAL (5) - Fatal error messages
    //
    // The default level is TRACE (0), which displays all messages.
    const int default_level = bj_get_log_level();
    bj_assert(default_level == 0);
    bj_info("Default log level: %d\n", default_level);

    // Change the minimum severity level that will be displayed. Messages below
    // this level are silently discarded. Here we set it to INFO, so TRACE and
    // DEBUG messages won't appear.
    bj_set_log_level(BJ_LOG_INFO);

    // The generic bj_log_msg(level, ...) function logs at any severity.
    // Only messages at or above the current level (INFO) will display.
    bj_log_msg(TRACE, "Trace level (won't display)");
    bj_log_msg(INFO, "Information level message");
    bj_log_msg(WARN, "Warning level message");

    // Convenience functions are provided for each level: bj_trace(), bj_debug(),
    // bj_info(), bj_warn(), bj_err(), bj_fatal(). These are clearer than using
    // the generic bj_log_msg().
    bj_err("This is an error message");

    // All logging functions support printf-style formatting with variable
    // arguments. They return the number of characters written (excluding the
    // null terminator), which can be useful for buffer management or testing.
    size_t written = bj_warn("Room #%d is closed, but you have '%s'", 42, "The Key Item");

    bj_info("Previous log message was written in %ld characters (excluding '\\0')", written);
    return 0;
}
