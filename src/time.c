#include <banjo/time.h>
#include <check.h>

uint64_t bj_time_frequency_value = 0;

static void bj_stopwatch_autoreset(struct bj_stopwatch* p_stopwatch)
{
    if (p_stopwatch->start_counter == 0) {
        const uint64_t now = bj_time_counter();
        p_stopwatch->start_counter = now;
        p_stopwatch->last_tick     = now;
    }
}

void bj_reset_stopwatch(struct bj_stopwatch* p_stopwatch)
{
    bj_check(p_stopwatch);
    const uint64_t now = bj_time_counter();
    p_stopwatch->start_counter = now;
    p_stopwatch->last_tick     = now;
}

void bj_step_stopwatch(struct bj_stopwatch* p_stopwatch)
{
    bj_check(p_stopwatch);
    bj_stopwatch_autoreset(p_stopwatch);
    p_stopwatch->last_tick = bj_time_counter();
}

double bj_stopwatch_elapsed(const struct bj_stopwatch* p_stopwatch)
{
    bj_check_or_return(p_stopwatch, 0.0);

    if (p_stopwatch->start_counter == 0) {
        // auto-reset for read-only
        struct bj_stopwatch* writable = (struct bj_stopwatch*)p_stopwatch;
        bj_reset_stopwatch(writable);
        return 0.0;
    }

    const uint64_t now = bj_time_counter();
    return (double)(now - p_stopwatch->start_counter) / (double)bj_time_frequency();
}

double bj_stopwatch_delay(const struct bj_stopwatch* p_stopwatch)
{
    bj_check_or_return(p_stopwatch, 0.0);

    if (p_stopwatch->last_tick == 0) {
        // auto-reset for read-only
        struct bj_stopwatch* writable = (struct bj_stopwatch*)p_stopwatch;
        bj_reset_stopwatch(writable);
        return 0.0;
    }

    const uint64_t now = bj_time_counter();
    return (double)(now - p_stopwatch->last_tick) / (double)bj_time_frequency();
}

double bj_step_delay_stopwatch(struct bj_stopwatch* p_stopwatch)
{
    bj_check_or_return(p_stopwatch, 0.0);
    bj_stopwatch_autoreset(p_stopwatch);

    const uint64_t now = bj_time_counter();
    const uint64_t delta = now - p_stopwatch->last_tick;
    p_stopwatch->last_tick = now;

    return (double)delta / (double)bj_time_frequency();
}
