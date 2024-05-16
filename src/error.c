#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/memory.h>

void bj_set_error(
    bj_error**  p_error,
    u16         domain,
    u16         code,
    const char* message
) {
    // Don't report anything if the user is not interested
    if(p_error == NULL) {
#ifndef NDEBUG
        bj_error("Uncaught error: 0x%hx/0x%hx", domain, code);
#endif
        return; // TODO Still report in debug mode
    }

    if(*p_error == NULL) {
        *p_error           = bj_malloc(sizeof(bj_error));
        (*p_error)->domain = domain;
        (*p_error)->code   = code;
    } else {
        bj_error("Error code [0x%hx:0x%hx] overwritten by [0x%hx:0x%hx]",
            (*p_error)->domain, (*p_error)->code, domain, code
        );
    }
}

bool bj_error_check(
    const bj_error* p_error,
    u16 domain,
    u16 code
) {
    return p_error && domain == p_error->domain && code == p_error->code;
}

void bj_propagate_error(
    bj_error*  p_source,
    bj_error** p_destination
) {
    bj_assert(p_source != 0);

    if (p_destination == 0) {
        if (p_source) {
            bj_free (p_source);
        }
        return;
    } else {
        if (*p_destination != 0) {
            // TODO
            bj_error("Error code [0x%hx:0x%hx] overwritten by [0x%hx:0x%hx]",
                (*p_destination)->domain, (*p_destination)->code, p_source->domain, p_source->code
            );
            bj_free (p_source);
        } else {
            *p_destination = p_source;
        }
    }
}

void bj_clear_error(
    bj_error** p_error
) {
    if(p_error && *p_error) {
        bj_free(*p_error);
        *p_error = 0;
    }
}
