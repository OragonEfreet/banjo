#include <banjo/time.h>

#include "check.h"

uint64_t bj_time_frequency = 0;

static void stopwatch_clear(bj_stopwatch* sw, uint64_t now)
{
    sw->start_counter = now;
    sw->last_tick     = now;
    sw->elapsed_ticks = 0;
    sw->delay_ticks   = 0;
}

static void stopwatch_zero(bj_stopwatch* sw)
{
    sw->start_counter = 0;
    sw->last_tick     = 0;
    sw->elapsed_ticks = 0;
    sw->delay_ticks   = 0;
    sw->running       = BJ_FALSE;
}

void bj_stopwatch_start(bj_stopwatch* sw)
{
    bj_check(sw);
    bj_check(sw->running == BJ_FALSE);

    const uint64_t now = bj_get_time_counter();

    if (sw->start_counter == 0 && sw->last_tick == 0) {
        stopwatch_clear(sw, now);
    } else {
        sw->start_counter += (now - sw->last_tick);
        sw->last_tick = now;
    }

    sw->running = BJ_TRUE;
}

void bj_stopwatch_tick(bj_stopwatch* sw)
{
    bj_check(sw);
    bj_check(sw->running);

    const uint64_t now = bj_get_time_counter();
    sw->delay_ticks    = now - sw->last_tick;
    sw->elapsed_ticks  = now - sw->start_counter;
    sw->last_tick      = now;
}

void bj_stopwatch_pause(bj_stopwatch* sw) {
    bj_check(sw);
    if (sw->running) {
        const uint64_t now = bj_get_time_counter();
        sw->delay_ticks   = now - sw->last_tick;
        sw->elapsed_ticks = now - sw->start_counter;
        sw->last_tick     = now;
    }
    sw->running = BJ_FALSE;
}

void bj_stopwatch_reset(bj_stopwatch* sw)
{
    bj_check(sw);
    const uint64_t now = bj_get_time_counter();
    stopwatch_clear(sw, now);
}

void bj_stopwatch_stop(bj_stopwatch* sw)
{
    bj_check(sw);
    stopwatch_zero(sw);
}

bj_bool bj_stopwatch_running(const bj_stopwatch* sw)
{
    bj_check_or_return(sw, BJ_FALSE);
    return sw->running;
}

double bj_stopwatch_elapsed(const bj_stopwatch* sw)
{
    bj_check_or_return(sw, 0.0);
    bj_check_or_return(bj_time_frequency, 0.0);
    return (double)(sw->elapsed_ticks) / (double)bj_time_frequency;
}

double bj_stopwatch_delay(const bj_stopwatch* sw)
{
    bj_check_or_return(sw, 0.0);
    bj_check_or_return(bj_time_frequency, 0.0);
    return (double)(sw->delay_ticks) / (double)bj_time_frequency;
}

double bj_stopwatch_tick_delay(
    bj_stopwatch* p_stopwatch
) {
    bj_stopwatch_tick(p_stopwatch);
    return bj_stopwatch_delay(p_stopwatch);
}
