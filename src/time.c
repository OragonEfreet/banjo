#include <banjo/time.h>

#include "system_t.h"

extern bj_system_backend* s_backend;

double bj_get_time(
    void
) {
    return (double)(bj_get_time_counter() - s_backend->timer_base) / bj_get_time_frequency();
}
