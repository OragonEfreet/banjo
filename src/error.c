#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/memory.h>

void bj_set_error(
    BjError* p_error,
    u32 domain,
    u32 code
) {
    if(p_error == 0) {
        return;
    }

    if(p_error->code == 0) {
        p_error->domain = domain;
        p_error->code = code;
    } else {
        bj_error("Error %d/%d", domain, code);
    }
}
