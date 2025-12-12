#include <banjo/error.h>

#include <check.h>

void bj_set_error(
    struct bj_error**  error,
    uint32_t         code,
    const char* message
) {
    // Don't report anything if the user is not interested
    if(error == 0) {
#ifdef BJ_BUILD_DEBUG
        bj_err("Uncaught error: 0x%08X", code);
#endif
        return;
    }

    if(*error == 0) {
        *error = bj_malloc(sizeof(struct bj_error));
        if (*error == 0) {
            return;
        }
        (*error)->code = code;

        // Safe string copy with bounds checking
        size_t msg_len = 0;
        while (msg_len < BJ_ERROR_MESSAGE_MAX_LEN && message[msg_len] != '\0') {
            (*error)->message[msg_len] = message[msg_len];
            msg_len++;
        }
        (*error)->message[msg_len] = '\0';
    } else {
        bj_err("Error code 0x%08X overwritten by 0x%08X",
            (*error)->code, code
        );
    }
}

bj_bool bj_check_error(
    const struct bj_error* error,
    uint32_t code
) {
    return error && code == error->code;
}

bj_bool bj_forward_error(
    struct bj_error*  source,
    struct bj_error** destination
) {
    bj_check_or_0(source != 0);

    if (destination == 0) {
        if (source) {
            bj_free (source);
        }
        return BJ_FALSE;
    }

    if (*destination != 0) {
        bj_err("Error code 0x%08X overwritten by 0x%08X",
            (*destination)->code, source->code
        );
        bj_free (source);
    } else {
        *destination = source;
    }
    return BJ_TRUE;
}

void bj_clear_error(
    struct bj_error** error
) {
    if(error && *error) {
        bj_free(*error);
        *error = 0;
    }
}
