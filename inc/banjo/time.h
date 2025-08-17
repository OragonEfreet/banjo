////////////////////////////////////////////////////////////////////////////////
/// \file time.h
/// \brief Header file for time manipulation utilities.
////////////////////////////////////////////////////////////////////////////////
/// \defgroup time Time
///
/// High-level and low-level time management utilities, including thread sleeping,
/// time counters, and a simple stopwatch abstraction.
///
/// \{
////////////////////////////////////////////////////////////////////////////////
#ifndef BJ_TIME_H
#define BJ_TIME_H

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
void bj_sleep(
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
double bj_get_time(
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
uint64_t bj_get_time_counter(
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
uint64_t bj_get_time_frequency(
    void
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Structure representing a simple stopwatch.
///
/// A stopwatch records the time it was last reset and the time of the last
/// step. It computes elapsed time since reset, and delay since the last step.
/// A zero-initialized stopwatch is valid and will auto-reset on first use.
////////////////////////////////////////////////////////////////////////////////
typedef struct bj_stopwatch
{
    uint64_t start_counter; ///< Time when stopwatch was last reset
    uint64_t last_tick;     ///< Time of last step/checkpoint
} bj_stopwatch;

////////////////////////////////////////////////////////////////////////////////
/// \brief Resets the stopwatch to the current time.
///
/// Records the current time as both the reset point and last step.
/// Clears any prior timing information.
///
/// \param p_stopwatch Pointer to the stopwatch.
///
/// \see bj_stopwatch_step, bj_stopwatch_elapsed, bj_stopwatch_delay
////////////////////////////////////////////////////////////////////////////////
void bj_stopwatch_reset(
    bj_stopwatch* p_stopwatch
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Records a step/checkpoint in time.
///
/// Updates the internal step timestamp. This does not affect the reset time,
/// but is used to measure time deltas via \ref bj_stopwatch_delay or
/// \ref bj_stopwatch_step_delay.
///
/// \param p_stopwatch Pointer to the stopwatch.
///
/// \see bj_stopwatch_step_delay
////////////////////////////////////////////////////////////////////////////////
void bj_stopwatch_step(
    bj_stopwatch* p_stopwatch
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Returns the elapsed time in seconds since the stopwatch was reset.
///
/// If the stopwatch has never been explicitly reset, it will be reset
/// automatically on first use.
///
/// \param p_stopwatch Pointer to the stopwatch.
///
/// \return Elapsed time in seconds.
///
/// \see bj_stopwatch_reset
////////////////////////////////////////////////////////////////////////////////
double bj_stopwatch_elapsed(
    const bj_stopwatch* p_stopwatch
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Returns the time in seconds since the last step.
///
/// If the stopwatch has never been used, it will be reset automatically on
/// first use. This function does not modify the stopwatch state.
///
/// \param p_stopwatch Pointer to the stopwatch.
///
/// \return Time in seconds since the last step.
///
/// \see bj_stopwatch_step
////////////////////////////////////////////////////////////////////////////////
double bj_stopwatch_delay(
    const bj_stopwatch* p_stopwatch
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Steps the stopwatch and returns the delay since the previous step.
///
/// Equivalent to calling \ref bj_stopwatch_delay followed by
/// \ref bj_stopwatch_step, but more efficient and concise.
///
/// \param p_stopwatch Pointer to the stopwatch.
///
/// \return Time in seconds since the previous step.
///
/// \see bj_stopwatch_step, bj_stopwatch_delay
////////////////////////////////////////////////////////////////////////////////
double bj_stopwatch_step_delay(
    bj_stopwatch* p_stopwatch
);

#endif
/// \} // End of time group
