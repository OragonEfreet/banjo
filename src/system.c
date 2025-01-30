#include <banjo/error.h>
#include <banjo/system.h>

void system_init_window(bj_error** p_error);
void system_dispose_window(bj_error** p_error);

bool bj_system_init(
    bj_error** p_error
) {
    system_init_window(p_error);
    return *p_error == 0;
}

void bj_system_dispose(
    bj_error** p_error
) {
    system_dispose_window(p_error);
}

