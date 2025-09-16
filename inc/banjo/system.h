////////////////////////////////////////////////////////////////////////////////
/// \file system.h
/// \brief Header file for system interactions
////////////////////////////////////////////////////////////////////////////////
/// \defgroup system System
///
/// Abstraction to usual system calls such as library loading and time.
///
/// \{
////////////////////////////////////////////////////////////////////////////////
#ifndef BJ_SYSTEM_H
#define BJ_SYSTEM_H

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
/// \return _true_ if the system is properly initialized, BJ_FALSE otherswise.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_bool bj_initialize(
    bj_error** p_error
);

////////////////////////////////////////////////////////////////////////////////
/// De-initializes the system.
///
/// \param p_error An optional location to an error object.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_shutdown(
    bj_error** p_error
);

////////////////////////////////////////////////////////////////////////////////
/// Load the provided dynamic library and returns and opaque handle to it.
///
/// This function is an abstraction over the platform specific function like
/// `dlopen` and `LoadLibrary`.
/// The provided pointer can be used with \ref bj_library_symbol to get a function
/// from the loaded library.
///
/// \param p_path A C-string path to the library to load.
/// \return _0_ or a handle to the loaded library.
///
/// \par Behaviour
///
/// - On Unix platforms, `p_path` is passed to `dlopen()` with `RTLD_LAZY` and
///   `RTLD_LOCAL` flags.
/// - On Windows, `p_path` is passed to `LoadLibraryA()`.
///
/// \par Memory Management
///
/// The caller is responsible for release the loaded library using \ref 
/// bj_unload_library.
///
/// 
/// \see bj_library_symbol, bj_unload_library
/// \see [dlopen()](https://linux.die.net/man/3/dlopen),
///      [LoadLibraryA()](https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-loadlibrarya)
///
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void* bj_load_library(
    const char* p_path
);

////////////////////////////////////////////////////////////////////////////////
/// Unload a library loaded with \ref bj_load_library from memory.
///
/// This function is an abstraction over the platform specific function like
/// `dlclose` and `FreeLibrary`.
///
/// \param p_handle The library to unload.
///
/// \par Behaviour
///
/// - On Unix platforms, `p_handle` is passed to `dlclose()`.
/// - On Windows, `p_path` is passed to `FreeLibrary()`.
///
/// \par Memory Management
///
/// The caller is responsible for release the loaded library using \ref 
/// bj_unload_library.
///
/// 
/// \see bj_library_symbol, bj_unload_library
/// \see [dlclose()](https://linux.die.net/man/3/dlclose),
///      [FreeLibrary()](https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-freelibrary)
///
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_unload_library(
    void* p_handle
);

////////////////////////////////////////////////////////////////////////////////
/// Get the address of a function exported by `p_handle` given its name.
///
/// This function is an abstraction over the platform specific function like
/// `dlsym` and `GetProcAddress`.
///
/// \param p_handle A library handle provided by \ref bj_load_library.
/// \param p_name C-String name of the function to retrieve
///
/// \return _0_ or the address of the retrieved function.
///
/// \par Behaviour
///
/// - On Unix platforms, `p_handle` and `p_name` are passed to `dlsym()`.
/// - On Windows, `p_handle` and `p_name` are passed to `GetProcAddress()`.
///
/// \par Memory Management
///
/// The caller is responsible for release the loaded function using \ref 
/// bj_unload_library with `p_handle`.
///
/// 
/// \see bj_load_library, bj_unload_library
/// \see [dlsym()](https://linux.die.net/man/3/dlsym),
///      [GetProcAddress()](https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getprocaddress)
///
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void* bj_library_symbol(
    void*       p_handle,
    const char* p_name
);

#endif
/// \} // End of system group
