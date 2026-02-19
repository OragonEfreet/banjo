#include <banjo/error.h>
#include <banjo/memory.h>

#include <check.h>

#include <stdarg.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////
// Internal structure definition (separate pointer)
////////////////////////////////////////////////////////////////////////////////

struct bj_error {
    uint32_t  code;
    char* message;
};

////////////////////////////////////////////////////////////////////////////////
// Internal helpers
////////////////////////////////////////////////////////////////////////////////

/// Returns the length of a string
static size_t str_len(const char* str) {
    size_t len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

/// Allocates and initializes a new error with exact message size
static struct bj_error* error_new(uint32_t code, const char* message) {
    size_t msg_len = str_len(message);

    struct bj_error* err = bj_malloc(sizeof(struct bj_error));
    if (err == 0) {
        return 0;
    }

    err->message = bj_malloc(msg_len + 1);
    if (err->message == 0) {
        bj_free(err);
        return 0;
    }

    err->code = code;
    bj_memcpy(err->message, message, msg_len + 1);
    return err;
}

/// Frees an error and its message
static void error_free(struct bj_error* err) {
    if (err == 0) {
        return;
    }
    bj_free(err->message);
    bj_free(err);
}

/// Logs an error (used when error is discarded)
static void log_discarded_error(const struct bj_error* err, const char* reason) {
#ifdef BJ_BUILD_DEBUG
    bj_err("%s: [0x%08X] %s", reason, err->code, err->message);
#else
    (void)err;
    (void)reason;
#endif
}

////////////////////////////////////////////////////////////////////////////////
// Public API implementation
////////////////////////////////////////////////////////////////////////////////

void bj_set_error(
    struct bj_error** error,
    uint32_t          code,
    const char* message
) {
    // Zero cost path: caller doesn't want error details
    if (error == 0) {
#ifdef BJ_BUILD_DEBUG
        bj_err("Uncaught error: [0x%08X] %s", code, message);
#endif
        return;
    }

    // First error wins: don't overwrite existing error
    if (*error != 0) {
        bj_err("Error [0x%08X] discarded, keeping [0x%08X]: %s",
            code, (*error)->code, message);
        return;
    }

    // Allocate and store the error
    *error = error_new(code, message);
}

void bj_set_error_fmt(
    struct bj_error** error,
    uint32_t          code,
    const char* format,
    ...
) {
    va_list args;

    // First pass: determine required size
    va_start(args, format);
    int len = vsnprintf(0, 0, format, args);
    va_end(args);

    if (len < 0) {
        // Format error - fall back to format string itself
        bj_set_error(error, code, format);
        return;
    }

    // Zero cost path: caller doesn't want error details
    if (error == 0) {
#ifdef BJ_BUILD_DEBUG
        char* buf = bj_malloc((size_t)len + 1);
        if (buf) {
            va_start(args, format);
            vsnprintf(buf, (size_t)len + 1, format, args);
            va_end(args);
            bj_err("Uncaught error: [0x%08X] %s", code, buf);
            bj_free(buf);
        }
#endif
        return;
    }

    // First error wins: don't overwrite existing error
    if (*error != 0) {
#ifdef BJ_BUILD_DEBUG
        char* buf = bj_malloc((size_t)len + 1);
        if (buf) {
            va_start(args, format);
            vsnprintf(buf, (size_t)len + 1, format, args);
            va_end(args);
            bj_err("Error [0x%08X] discarded, keeping [0x%08X]: %s",
                code, (*error)->code, buf);
            bj_free(buf);
        }
#endif
        return;
    }

    // Allocate error struct and message separately
    struct bj_error* err = bj_malloc(sizeof(struct bj_error));
    if (err == 0) {
        return;
    }

    err->message = bj_malloc((size_t)len + 1);
    if (err->message == 0) {
        bj_free(err);
        return;
    }

    err->code = code;
    va_start(args, format);
    vsnprintf(err->message, (size_t)len + 1, format, args);
    va_end(args);

    *error = err;
}

void bj_propagate_error(
    struct bj_error** dest,
    struct bj_error* src
) {
    // Nothing to propagate
    if (src == 0) {
        return;
    }

    // Caller doesn't want error details - log and free
    if (dest == 0) {
        log_discarded_error(src, "Propagated error discarded");
        error_free(src);
        return;
    }

    // First error wins - log and free the new one
    if (*dest != 0) {
        log_discarded_error(src, "Propagated error discarded, keeping original");
        error_free(src);
        return;
    }

    // Transfer ownership
    *dest = src;
}

void bj_propagate_prefixed_error(
    struct bj_error** dest,
    struct bj_error* src,
    const char* format,
    ...
) {
    // Nothing to propagate
    if (src == 0) {
        return;
    }

    // Format the prefix
    va_list args;
    va_start(args, format);
    int prefix_len = vsnprintf(0, 0, format, args);
    va_end(args);

    if (prefix_len < 0) {
        prefix_len = 0;
    }

    size_t msg_len = str_len(src->message);
    size_t total_len = (size_t)prefix_len + msg_len;

    struct bj_error* new_err = bj_malloc(sizeof(struct bj_error));
    if (new_err == 0) {
        // Propagate without prefix on allocation failure
        bj_propagate_error(dest, src);
        return;
    }

    new_err->message = bj_malloc(total_len + 1);
    if (new_err->message == 0) {
        bj_free(new_err);
        bj_propagate_error(dest, src);
        return;
    }

    new_err->code = src->code;

    // Write prefix
    va_start(args, format);
    vsnprintf(new_err->message, (size_t)prefix_len + 1, format, args);
    va_end(args);

    // Append original message
    bj_memcpy(new_err->message + prefix_len, src->message, msg_len + 1);

    // Free original
    error_free(src);

    // Propagate the new prefixed error
    bj_propagate_error(dest, new_err);
}

void bj_prefix_error(
    struct bj_error** error,
    const char* prefix
) {
    if (error == 0 || *error == 0) {
        return;
    }

    struct bj_error* err = *error;
    size_t prefix_len = str_len(prefix);
    size_t msg_len = str_len(err->message);
    size_t total_len = prefix_len + msg_len;

    char* new_message = bj_malloc(total_len + 1);
    if (new_message == 0) {
        // Keep original on allocation failure
        return;
    }

    bj_memcpy(new_message, prefix, prefix_len);
    bj_memcpy(new_message + prefix_len, err->message, msg_len + 1);

    bj_free(err->message);
    err->message = new_message;
}

void bj_prefix_error_fmt(
    struct bj_error** error,
    const char* format,
    ...
) {
    if (error == 0 || *error == 0) {
        return;
    }

    // Format the prefix
    va_list args;
    va_start(args, format);
    int prefix_len = vsnprintf(0, 0, format, args);
    va_end(args);

    if (prefix_len < 0) {
        return;
    }

    struct bj_error* err = *error;
    size_t msg_len = str_len(err->message);
    size_t total_len = (size_t)prefix_len + msg_len;

    char* new_message = bj_malloc(total_len + 1);
    if (new_message == 0) {
        return;
    }

    // Write prefix
    va_start(args, format);
    vsnprintf(new_message, (size_t)prefix_len + 1, format, args);
    va_end(args);

    // Append original message
    bj_memcpy(new_message + prefix_len, err->message, msg_len + 1);

    bj_free(err->message);
    err->message = new_message;
}

struct bj_error* bj_copy_error(
    const struct bj_error* error
) {
    if (error == 0) {
        return 0;
    }
    return error_new(error->code, error->message);
}

void bj_clear_error(
    struct bj_error** error
) {
    if (error != 0 && *error != 0) {
        error_free(*error);
        *error = 0;
    }
}

bj_bool bj_error_matches(
    const struct bj_error* error,
    uint32_t               code
) {
    if (error == 0) {
        return BJ_FALSE;
    }
    return error->code == code ? BJ_TRUE : BJ_FALSE;
}

bj_bool bj_error_matches_kind(
    const struct bj_error* error,
    uint32_t               kind
) {
    if (error == 0) {
        return BJ_FALSE;
    }
    return bj_error_code_kind(error->code) == bj_error_code_kind(kind)
        ? BJ_TRUE : BJ_FALSE;
}

uint32_t bj_error_code(
    const struct bj_error* error
) {
    if (error == 0) {
        return BJ_ERROR_NONE;
    }
    return error->code;
}

const char* bj_error_message(
    const struct bj_error* error
) {
    if (error == 0) {
        return 0;
    }
    return error->message;
}
