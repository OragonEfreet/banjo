#include <banjo/log.h>

#define BANJO_LOG_COLORS

#include <stdarg.h>
#include <time.h>
#include <stdio.h>

static struct {
    int level;
} s_context = {};

static const char* level_strings[] = {
    "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL",
};

#ifdef BANJO_LOG_COLORS
static const char* level_colors[] = {
  "\x1b[94m", "\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m", "\x1b[35m"
};
static const char* header_fmt = "%s %s%-5s\x1b[0m \x1b[90m%s:%d:\x1b[0m ";
#else
static const char* header_fmt =  "%s %-5s %s:%d: ";
#endif

const char* bjGetLogLevelString(int level) {
    return level_strings[level];
}

void bjSetLogLevel(int level) {
    s_context.level = level;
}

int bjGetLogLevel() {
    return s_context.level;
}

void bjLogMsg(int level, const char* pFile, int line, const char* pFmt, ...) {
    if(level >= s_context.level) {
        time_t now = time(0);
        struct tm* pTime = localtime(&now);
        va_list ap;

        va_start(ap, pFmt);
        char buffer[16];
        size_t eol_i = strftime(buffer, sizeof(buffer), "%H:%M:%S", pTime);
        buffer[eol_i] = '\0';

#ifdef BANJO_LOG_COLORS
        printf(header_fmt, buffer, level_colors[level], level_strings[level], pFile, line);
#else
        printf(header_fmt, buffer, level_strings[level], pFile, line);
#endif
        vprintf(pFmt, ap);
        printf("\n");
        fflush(stdout);

        va_end(ap);
    }
}

