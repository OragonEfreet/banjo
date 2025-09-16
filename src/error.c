#include <banjo/error.h>

#include "check.h"

void bj_set_error(
    bj_error**  p_error,
    uint32_t         code,
    const char* message
) {
    // Don't report anything if the user is not interested
    if(p_error == 0) {
#ifdef BJ_BUILD_DEBUG
        bj_err("Uncaught error: 0x%08X", code);
#endif
        return;
    }

    if(*p_error == 0) {
        *p_error           = bj_malloc(sizeof(bj_error));
        (*p_error)->code   = code;
        bj_memcpy((*p_error)->message, message, BJ_ERROR_MESSAGE_MAX_LEN + 1);
        ((*p_error)->message)[BJ_ERROR_MESSAGE_MAX_LEN] = '\0';
    } else {
        bj_err("Error code 0x%08X overwritten by 0x%08X",
            (*p_error)->code, code
        );
    }
}

bj_bool bj_check_error(
    const bj_error* p_error,
    uint32_t code
) {
    return p_error && code == p_error->code;
}

bj_bool bj_forward_error(
    bj_error*  p_source,
    bj_error** p_destination
) {
    bj_check_or_0(p_source != 0);

    if (p_destination == 0) {
        if (p_source) {
            bj_free (p_source);
        }
        return BJ_FALSE;
    }

    if (*p_destination != 0) {
        bj_err("Error code 0x%08X overwritten by 0x%08X",
            (*p_destination)->code, p_source->code
        );
        bj_free (p_source);
    } else {
        *p_destination = p_source;
    }
    return BJ_TRUE;
}

void bj_clear_error(
    bj_error** p_error
) {
    if(p_error && *p_error) {
        bj_free(*p_error);
        *p_error = 0;
    }
}
