
////////////////////////////////////////////////////////////////////////////////
/// \file time.h
/// \brief Header file for time manipulation.
////////////////////////////////////////////////////////////////////////////////
/// \defgroup time Time
///
/// Abstraction to usual system calls related to time.
///
/// \{
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <banjo/api.h>
#include <banjo/error.h>


////////////////////////////////////////////////////////////////////////////////
/// \brief Suspends the current thread for a specified duration.
///
/// This function puts the current thread to sleep for at least the specified number
/// of milliseconds. The actual sleep duration may be longer depending on system timing.
///
/// \param milliseconds The number of milliseconds to sleep.
///
/// \see bj_get_time, bj_get_time_counter, bj_get_time_frequency
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_sleep(
    int milliseconds
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Gets the current time in seconds.
///
/// Returns the elapsed time in seconds since Banjo is initialized with
/// \ref bj_begin.
///
/// \return The current time in seconds as a double-precision value.
///
/// \see bj_get_time_counter, bj_get_time_frequency, bj_sleep
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT double bj_get_time(
    void
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Gets the current raw high-resolution time counter.
///
/// This function returns a time counter in platform-dependent ticks. To convert
/// the value to seconds, divide it by the result of \ref bj_get_time_frequency
/// or just call \ref bj_get_time.
///
/// \return The current high-resolution time counter.
///
/// \see bj_get_time, bj_get_time_frequency, bj_sleep
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT uint64_t bj_get_time_counter(
    void
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Gets the frequency of the high-resolution time counter.
///
/// This function returns the number of ticks per second of the 
/// high-resolution counter. 
/// It is used in conjunction with \ref bj_get_time_counter to compute durations.
///
/// \return The frequency in ticks per second.
///
/// \see bj_get_time, bj_get_time_counter, bj_sleep
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT uint64_t bj_get_time_frequency(
    void
);

/// \} // End of system group
