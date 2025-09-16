////////////////////////////////////////////////////////////////////////////////
/// \example logging.c
/// Demonstrate the use of log facilities
////////////////////////////////////////////////////////////////////////////////
#include <banjo/assert.h>
#include <banjo/main.h>
#include <banjo/log.h>

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    // Usually, log levels go from 0 to 5 in that order:
    // BJ_LOG_TRACE < BJ_LOG_DEBUG < BJ_LOG_INFO,
    // BJ_LOG_WARN <  BJ_LOG_ERROR < BJ_LOG_FATAL.
    // The default log level on application start is 0 (TRACE)
    const int default_level = bj_get_log_level();
    bj_assert(default_level == 0);
    bj_info("Default log level: %d\n", default_level);

    // To set the current log level:
    bj_set_log_level(BJ_LOG_INFO);
    // Since level are signed int, you can provide any custom log level outside
    // the range [0;5].

    // Any message sent in a level equal to higher than the current will
    // output:
    bj_log_msg(TRACE, "Trace level (won't display)");
    bj_log_msg(INFO, "Information level message");
    bj_log_msg(WARN, "Warning level message");
    // Also ther is bj_info(), bj_trace(), etc:
    bj_err("This is an error message");


    // Logging functions accept variable arguments and string formatting a-la printf:
    size_t written = bj_warn("Room #%d is closed, but you have '%s'", 42, "The Key Item"); 

    // Written contains the number of characters actually written:
    bj_info("Previous log message was written in %ld characters (excluding '\\0')", written);
    return 0;
}
