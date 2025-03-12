#include <banjo/log.h>
#include <banjo/memory.h>

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

static struct {
    int level;
} s_context = {.level=0};

static const struct {
    const char* name;
    const char* color;
    size_t payload;
} level_info[] = { 
    { .name = "TRACE", .color = "\x1b[94m", .payload = 5, }, 
    { .name = "DEBUG", .color = "\x1b[36m", .payload = 5, }, 
    { .name = "INFO",  .color = "\x1b[32m", .payload = 4, }, 
    { .name = "WARN",  .color = "\x1b[33m", .payload = 4, }, 
    { .name = "ERROR", .color = "\x1b[31m", .payload = 5, }, 
    { .name = "FATAL", .color = "\x1b[35m", .payload = 5, }, 
};

const char* bj_log_get_level_string(int level) {
    return level_info[level].name;
}

void bj_log_set_level(int level) {
    s_context.level = level;
}

int bj_log_get_level(void) {
    return s_context.level;
}

#define COLOR_SET_PAYLOAD 5
#define COLOR_END_PAYLOAD 4
#define COLORED_CHUNK_PAYLOAD (COLOR_SET_PAYLOAD + COLOR_END_PAYLOAD)

#define BUFFER_SIZE (BJ_MAXIMUM_LOG_LEN + 1)

#ifdef BJ_CONFIG_LOG_COLOR
static const bool use_colors = true;
#else
static const bool use_colors = false;
#endif

#define BUFFER_WRAP 50

size_t bj_message(
    int level,
    const char* p_file,
    int line,
    const char* p_format, ...
) {
    if(level >= s_context.level) {
        static const char* color_end = "\x1b[0m";
        static const char* color_source = "\x1b[94m";

        char buf[BUFFER_SIZE];

        va_list ap;
        va_start(ap, p_format);
        int msg_payload = vsnprintf(buf, BUFFER_SIZE, p_format, ap);
        va_end(ap);

        if (msg_payload < 0) return 0;
        if (msg_payload > BJ_MAXIMUM_LOG_LEN) msg_payload = BJ_MAXIMUM_LOG_LEN;

        const size_t header_max_len = BJ_MAXIMUM_LOG_LEN - msg_payload;

        size_t header_size = 0;
        if (header_max_len > 0) {
            memmove(buf + header_max_len, buf, msg_payload);

            size_t source_payload = 0;
            if (p_file) {
                source_payload = snprintf(buf, header_max_len, "(%s:%d)", p_file, line) + 1;
                if (source_payload > header_max_len) source_payload = 0;
                else buf[source_payload-1] = ' ';
            }

            size_t level_payload = level_info[level].payload + 1;
            const size_t timestamp_payload = 9;

            bool with_level = header_max_len >= level_payload;
            bool with_source = with_level && ((source_payload + level_payload) <= header_max_len);
            bool with_timestamp = with_source && ((level_payload + source_payload + timestamp_payload) <= header_max_len);
            bool colored_level = use_colors && with_timestamp && ((level_payload + source_payload + timestamp_payload + COLORED_CHUNK_PAYLOAD) <= header_max_len);
            bool colored_source = use_colors && colored_level && ((level_payload + source_payload + timestamp_payload + COLORED_CHUNK_PAYLOAD * 2) <= header_max_len);

            if (with_source) {
                size_t shift = header_max_len - source_payload;
                if (colored_source) {
                    memcpy(buf + header_max_len - COLOR_END_PAYLOAD, color_end, COLOR_END_PAYLOAD);
                    shift -= COLOR_END_PAYLOAD;
                    header_size += COLOR_END_PAYLOAD;
                }
                memmove(buf + shift, buf, source_payload);
                header_size += source_payload;
                if (colored_source) {
                    memcpy(buf + header_max_len - COLOR_END_PAYLOAD - COLOR_SET_PAYLOAD - source_payload, color_source, COLOR_SET_PAYLOAD);
                    header_size += COLOR_SET_PAYLOAD;
                }
            }

            if (with_level) {
                if (colored_level) {
                    memcpy(buf + header_max_len - header_size - COLOR_END_PAYLOAD, color_end, COLOR_END_PAYLOAD);
                    header_size += COLOR_END_PAYLOAD;
                }
                snprintf(buf + header_max_len - header_size - level_payload, level_payload, "%s", level_info[level].name);
                buf[header_max_len - header_size - 1] = ' ';
                header_size += level_payload;
                if (colored_level) {
                    memcpy(buf + header_max_len - header_size - COLOR_SET_PAYLOAD, level_info[level].color, COLOR_SET_PAYLOAD);
                    header_size += COLOR_SET_PAYLOAD;
                }
            }

            if (with_timestamp) {
                time_t now = time(0);
                struct tm* pTime = localtime(&now);
                strftime(buf + header_max_len - header_size - timestamp_payload, timestamp_payload, "%H:%M:%S", pTime);
                buf[header_max_len - header_size - 1] = ' ';
                header_size += timestamp_payload;
            }
        }

        buf[header_max_len + msg_payload] = '\0';

        // TODO Should be WriteConsoleA on windows, etc.
        puts(buf + header_max_len - header_size);

        return msg_payload + header_size;
    }
    return 0;
}
