#include <banjo/api.h>
#include <banjo/log.h>
#include <banjo/system.h>

int main(void) {

    bj_error* p_error = 0;

    if (bj_begin(&p_error)) {


        bj_end(p_error);
    } else {
        bj_err("while starting banjo: %s (%x)", p_error->message, p_error->code);
    }

    return 0;
}

