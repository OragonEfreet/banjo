#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/memory.h>

#define ERR(d, c) ((u32)((((u16)c & 0xFFFF) << 16) | ((u16)d & 0xFFFF)))

void bj_set_error(
    bj_error* p_error,
    u16 domain,
    u16 code
) {
    u32 err = ERR(domain, code);
    if(p_error == 0) {
#ifndef NDEBUG
        bj_debug("Error 0x%04x", err);
#endif
        return;
    }

    if(*p_error == 0) {
        *p_error = ERR(domain, code);
    } else {
        bj_error("Error 0x%04x", err);
    }
}
