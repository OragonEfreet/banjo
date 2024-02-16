#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/memory.h>

void bjSetError(
    BjError* pError,
    u32 domain,
    u32 code
) {
    if(pError == 0) {
        return;
    }

    if(pError->code == 0) {
        pError->domain = domain;
        pError->code = code;
    } else {
        bjLogError("Error %d/%d", domain, code);
    }
}
