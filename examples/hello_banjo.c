////////////////////////////////////////////////////////////////////////////////
/// \example window.c
/// How to open and close windows.
////////////////////////////////////////////////////////////////////////////////
#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/system.h>

int main(void) {

    bj_error* p_error = 0;

    if(!bj_begin(&p_error)) {
        bj_err("Error 0x%08X: %s", p_error->code, p_error->message);
        return 1;
    } 

    bj_info("Hello Banjo!");

    bj_end(0);
    return 0;
}
