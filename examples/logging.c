//< Demonstrate the use of log facilities
#include <banjo/log.h>

#include <assert.h>

int main(int argc, char* argv[]) {

    // Usually, log levels go from 0 to 5 in that order:
    // BJ_LOG_TRACE < BJ_LOG_DEBUG < BJ_LOG_INFO,
    // BJ_LOG_WARN <  BJ_LOG_ERROR < BJ_LOG_FATAL.
    // The default log level on application start is 0 (TRACE)
    int default_level = bjGetLogLevel();
    assert(default_level == 0);

    // To set the current log level:
    bjSetLogLevel(BJ_LOG_INFO);
    // Since level are signed int, you can provide any custom log level outside
    // the range [0;5].

    // Any message sent in a level equal to higher than the current will
    // output:
    bjLog(TRACE, "Trace level won't display");
    bjLog(INFO, "INFO level will display");
    bjLog(WARN, "WARN level will display");

    // ALso ther is BjLogInfo(), BjLogTrace(), etc.
}
