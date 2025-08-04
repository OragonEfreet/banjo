////////////////////////////////////////////////////////////////////////////////
/// \file time.h
/// \brief Header file for time manipulation utilities.
////////////////////////////////////////////////////////////////////////////////
/// \defgroup time Time
///
/// High-level and low-level time management utilities, including thread sleeping,
/// time counters, and stopwatch abstraction.
///
/// \{
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <banjo/api.h>
#include <banjo/error.h>

////////////////////////////////////////////////////////////////////////////////
/// \brief Suspends the current thread for a specified duration.
///
/// This function puts the current thread to sleep for at least the specified
/// number of milliseconds. The actual sleep duration may be longer depending
/// on the system.
///
/// \param milliseconds Number of milliseconds to sleep.
///
/// \see bj_get_time, bj_get_time_counter, bj_get_time_frequency
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_sleep(
    int milliseconds
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Gets the current time in seconds since Banjo initialization.
///
/// This function returns the time in seconds since \ref bj_begin was called.
/// It is suitable for general-purpose timing, but not for high-resolution use.
///
/// \return The current time in seconds.
///
/// \see bj_get_time_counter, bj_get_time_frequency
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT double bj_get_time(
    void
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Returns the current high-resolution time counter.
///
/// This value is in platform-dependent ticks and is suitable for precise
/// timing and performance measurements. To convert ticks to seconds, divide
/// by the result of \ref bj_get_time_frequency.
///
/// \return The current time counter in ticks.
///
/// \see bj_get_time, bj_get_time_frequency
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT uint64_t bj_get_time_counter(
    void
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Returns the frequency of the high-resolution counter.
///
/// This is the number of ticks per second returned by \ref bj_get_time_counter.
/// Use this to convert tick counts to seconds.
///
/// \return Ticks per second (frequency).
///
/// \see bj_get_time_counter
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT uint64_t bj_get_time_frequency(
    void
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Structure representing a high-resolution stopwatch.
///
/// A stopwatch tracks time intervals based on tick counts. It supports starting,
/// pausing, resetting, stopping, and ticking to update internal deltas. Elapsed
/// and delta times can be retrieved in seconds when needed.
////////////////////////////////////////////////////////////////////////////////
typedef struct bj_stopwatch
{
    uint64_t start_counter;   ///< Counter value at stopwatch start
    uint64_t last_tick;       ///< Counter value at last tick
    uint64_t elapsed_ticks;   ///< Ticks since start
    uint64_t delay_ticks;     ///< Ticks since last tick
    bj_bool  running;         ///< Whether the stopwatch is currently running
} bj_stopwatch;

////////////////////////////////////////////////////////////////////////////////
/// \brief Starts the stopwatch.
///
/// Initializes internal counters and sets the stopwatch as running.
/// Elapsed and delay tick counts are reset to zero.
///
/// \param sw Pointer to the stopwatch.
///
/// \see bj_stopwatch_tick, bj_stopwatch_pause, bj_stopwatch_reset, bj_stopwatch_stop
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_stopwatch_start(
    bj_stopwatch* sw
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Updates the stopwatch tick values.
///
/// Updates the internal tick counters used to compute elapsed and delay times.
/// Has no effect if the stopwatch is not running.
///
/// \param sw Pointer to the stopwatch.
///
/// \see bj_stopwatch_elapsed, bj_stopwatch_delay
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_stopwatch_tick(
    bj_stopwatch* sw
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Pauses the stopwatch.
///
/// Marks the stopwatch as not running. Tick counters and timing state are preserved.
///
/// \param sw Pointer to the stopwatch.
///
/// \see bj_stopwatch_start, bj_stopwatch_stop
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_stopwatch_pause(
    bj_stopwatch* sw
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Resets the stopwatch tick counters.
///
/// Sets elapsed and delay tick values to zero. The stopwatch's running state is unchanged.
///
/// \param sw Pointer to the stopwatch.
///
/// \see bj_stopwatch_start, bj_stopwatch_tick
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_stopwatch_reset(
    bj_stopwatch* sw
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Stops the stopwatch.
///
/// Resets all internal timing state and marks the stopwatch as not running.
///
/// \param sw Pointer to the stopwatch.
///
/// \see bj_stopwatch_start, bj_stopwatch_pause, bj_stopwatch_reset
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_stopwatch_stop(
    bj_stopwatch* sw
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Checks whether the stopwatch is currently running.
///
/// \param sw Pointer to the stopwatch.
///
/// \return BJ_TRUE if the stopwatch is running, BJ_FALSE otherwise.
///
/// \see bj_stopwatch_start, bj_stopwatch_pause
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_bool bj_stopwatch_running(
    const bj_stopwatch* sw
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Returns the elapsed time in seconds since the stopwatch was started.
///
/// This function converts the stored tick count to seconds. It does not update
/// the stopwatch; call \ref bj_stopwatch_tick beforehand to refresh values.
///
/// \param sw Pointer to the stopwatch.
///
/// \return Elapsed time in seconds.
///
/// \see bj_stopwatch_tick
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT double bj_stopwatch_elapsed(
    const bj_stopwatch* sw
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Returns the time in seconds since the last tick.
///
/// This function converts the stored delay tick count to seconds. It does not
/// update the stopwatch; call \ref bj_stopwatch_tick beforehand to refresh values.
///
/// \param sw Pointer to the stopwatch.
///
/// \return Time in seconds since the last tick.
///
/// \see bj_stopwatch_tick
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT double bj_stopwatch_delay(
    const bj_stopwatch* sw
);

/// \} // End of time group
