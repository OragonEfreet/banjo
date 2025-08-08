#include <banjo/time.h>
#include "check.h"

uint64_t bj_time_frequency = 0;

static void bj__stopwatch_autoreset(bj_stopwatch* p_stopwatch)
{
    if (p_stopwatch->start_counter == 0) {
        const uint64_t now = bj_get_time_counter();
        p_stopwatch->start_counter = now;
        p_stopwatch->last_tick     = now;
    }
}

void bj_stopwatch_reset(bj_stopwatch* p_stopwatch)
{
    bj_check(p_stopwatch);
    const uint64_t now = bj_get_time_counter();
    p_stopwatch->start_counter = now;
    p_stopwatch->last_tick     = now;
}

void bj_stopwatch_step(bj_stopwatch* p_stopwatch)
{
    bj_check(p_stopwatch);
    bj__stopwatch_autoreset(p_stopwatch);
    p_stopwatch->last_tick = bj_get_time_counter();
}

double bj_stopwatch_elapsed(const bj_stopwatch* p_stopwatch)
{
    bj_check_or_return(p_stopwatch, 0.0);

    if (p_stopwatch->start_counter == 0) {
        // auto-reset for read-only
        bj_stopwatch* writable = (bj_stopwatch*)p_stopwatch;
        bj_stopwatch_reset(writable);
        return 0.0;
    }

    const uint64_t now = bj_get_time_counter();
    return (double)(now - p_stopwatch->start_counter) / (double)bj_get_time_frequency();
}

double bj_stopwatch_delay(const bj_stopwatch* p_stopwatch)
{
    bj_check_or_return(p_stopwatch, 0.0);

    if (p_stopwatch->last_tick == 0) {
        // auto-reset for read-only
        bj_stopwatch* writable = (bj_stopwatch*)p_stopwatch;
        bj_stopwatch_reset(writable);
        return 0.0;
    }

    const uint64_t now = bj_get_time_counter();
    return (double)(now - p_stopwatch->last_tick) / (double)bj_get_time_frequency();
}

double bj_stopwatch_step_delay(bj_stopwatch* p_stopwatch)
{
    bj_check_or_return(p_stopwatch, 0.0);
    bj__stopwatch_autoreset(p_stopwatch);

    const uint64_t now = bj_get_time_counter();
    const uint64_t delta = now - p_stopwatch->last_tick;
    p_stopwatch->last_tick = now;

    return (double)delta / (double)bj_get_time_frequency();
}
