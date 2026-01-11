#include <banjo/version.h>

#include <stdio.h>
#include <stdint.h>

#define BJ_STAGE_TYPE(stage) ((stage) & 0xC0U)
#define BJ_STAGE_NUMBER(stage) ((stage) & 0x3FU)

size_t bj_format_version(char *buffer, size_t bufsize, uint32_t version) {
    uint8_t major = BJ_VERSION_MAJOR(version);
    uint8_t minor = BJ_VERSION_MINOR(version);
    uint8_t patch = BJ_VERSION_PATCH(version);
    uint8_t stage = BJ_VERSION_STAGE(version);

    size_t written = 0;
    int n;

    // Print major.minor.patch
    n = snprintf(buffer, bufsize, "%u.%u.%u", major, minor, patch);
    if (n < 0) return 0;
    written += (size_t)n;

    if (bufsize > 0) {
        size_t n_size = (n > 0) ? (size_t)n : 0;
        if (n_size < bufsize) {
            buffer += n_size;
            bufsize -= n_size;
        } else {
            buffer += bufsize;
            bufsize = 0;
        }
    }

    // Print pre-release if not stable
    if (stage != BJ_VERSION_STABLE) {
        const char *stage_str = NULL;
        uint8_t type = BJ_STAGE_TYPE(stage);
        uint8_t num  = BJ_STAGE_NUMBER(stage);

        if (type == BJ_VERSION_ALPHA) stage_str = "-alpha";
        else if (type == BJ_VERSION_BETA) stage_str = "-beta";
        else if (type == BJ_VERSION_RC) stage_str = "-rc";
        else if (type == BJ_VERSION_DEV) stage_str = "-dev";

        if (stage_str) {
            n = snprintf(buffer, bufsize, "%s", stage_str);
            if (n > 0) {
                size_t n_size = (size_t)n;
                written += n_size;
                if (bufsize > n_size) {
                    buffer += n_size;
                    bufsize -= n_size;
                } else {
                    buffer += bufsize;
                    bufsize = 0;
                }
            }

            // Append stage number if > 0
            if (num > 0 && bufsize > 0) {
                n = snprintf(buffer, bufsize, ".%u", num);
                if (n > 0) {
                    written += (size_t)n;
                }
            }
        }
    }

    return written;
}
