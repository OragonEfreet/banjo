////////////////////////////////////////////////////////////////////////////////
/// \file system.h
/// \brief Header file for system interactions
////////////////////////////////////////////////////////////////////////////////
/// \defgroup system System
/// \{
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <banjo/api.h>
#include <banjo/error.h>

////////////////////////////////////////////////////////////////////////////////
/// Initializes the system.
///
/// \param p_error An optional location to an error object.
///
/// The initialization process will iteratively try to initialize a subsystem
/// among the ones available and returns on the first that succeeded.
///
/// \return _true_ if the system is properly initialized, false otherswise.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bool bj_system_init(
    bj_error** p_error
);

////////////////////////////////////////////////////////////////////////////////
/// De-initializes the system.
///
/// \param p_error An optional location to an error object.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_system_dispose(
    bj_error** p_error
);

BANJO_EXPORT void* bj_load_library(
    const char* p_path
);

BANJO_EXPORT void bj_unload_library(
    void* p_handle
);

BANJO_EXPORT void* bj_get_symbol(
    void*       p_handle,
    const char* p_name
);

BANJO_EXPORT void bj_sleep(
    int milliseconds
);

BANJO_EXPORT double bj_get_time(
    void
);

BANJO_EXPORT uint64_t bj_get_time_counter(
    void
);

BANJO_EXPORT uint64_t bj_get_time_frequency(
    void
);





/// \} // End of system group
