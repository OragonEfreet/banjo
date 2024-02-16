#include <banjo/log.h>

#define BANJO_LOG_COLORS

#include <stdarg.h>
#include <time.h>
#include <stdio.h>

static struct {
    int level;
} s_context = {.level=0};

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

const char* bj_log_get_level_string(int level) {
    return level_strings[level];
}

void bj_log_set_level(int level) {
    s_context.level = level;
}

int bj_log_get_level(void) {
    return s_context.level;
}

void bj_message(int level, const char* p_file, int line, const char* p_format, ...) {
    if(level >= s_context.level) {
        time_t now = time(0);
        struct tm* pTime = localtime(&now);
        va_list ap;

        va_start(ap, p_format);
        char buffer[16];
        size_t eol_i = strftime(buffer, sizeof(buffer), "%H:%M:%S", pTime);
        buffer[eol_i] = '\0';

#ifdef BANJO_LOG_COLORS
        printf(header_fmt, buffer, level_colors[level], level_strings[level], p_file, line);
#else
        printf(header_fmt, buffer, level_strings[level], p_file, line);
#endif
        vprintf(p_format, ap);
        printf("\n");
        fflush(stdout);

        va_end(ap);
    }
}

