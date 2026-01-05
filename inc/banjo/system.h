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

enum bj_system {
    BJ_NO_SYSTEM    = 0x00,
    BJ_AUDIO_SYSTEM = 0x01,
    BJ_VIDEO_SYSTEM = 0x02,
};

////////////////////////////////////////////////////////////////////////////////
/// Initializes the system.
///
/// \param systems An OR combination of \ref bj_system to initialize.
/// \param error An optional location to an error object.
///
/// The initialization process will iteratively try to initialize a subsystem
/// among the ones available and returns on the first that succeeded.
///
/// \return _true_ if the system is properly initialized, BJ_FALSE otherswise.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_bool bj_begin(
    int               systems,
    struct bj_error** error
);

////////////////////////////////////////////////////////////////////////////////
/// De-initializes the system.
///
/// This function forcefully shuts down all subsystems and resets all reference
/// counts to zero, regardless of how many times they were retained. Errors
/// during shutdown are ignored.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_end(
    void
);

////////////////////////////////////////////////////////////////////////////////
/// Initializes a single system with reference counting.
///
/// \param system A single \ref bj_system value to initialize.
/// \param error An optional location to an error object.
///
/// This function retains the specified system, incrementing its reference count.
/// Time and event subsystems are always retained as dependencies.
/// Multiple calls to this function must be balanced with matching calls to
/// \ref bj_end_system.
///
/// \return _true_ if the system is successfully retained, BJ_FALSE otherwise.
///
/// \see bj_end_system, bj_begin
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_bool bj_begin_system(
    enum bj_system    system,
    struct bj_error** error
);

////////////////////////////////////////////////////////////////////////////////
/// De-initializes a single system with reference counting.
///
/// \param system A single \ref bj_system value to release.
/// \param error An optional location to an error object.
///
/// This function releases the specified system, decrementing its reference count.
/// Time and event subsystems are also released. When a system's reference count
/// reaches zero, it is fully shut down.
///
/// \see bj_begin_system, bj_end
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_end_system(
    enum bj_system    system,
    struct bj_error** error
);

////////////////////////////////////////////////////////////////////////////////
/// Load the provided dynamic library and returns and opaque handle to it.
///
/// This function is an abstraction over the platform specific function like
/// `dlopen` and `LoadLibrary`.
/// The provided pointer can be used with \ref bj_library_symbol to get a function
/// from the loaded library.
///
/// \param path A C-string path to the library to load.
/// \return _0_ or a handle to the loaded library.
///
/// \par Behaviour
///
/// - On Unix platforms, `path` is passed to `dlopen()` with `RTLD_LAZY` and
///   `RTLD_LOCAL` flags.
/// - On Windows, `path` is passed to `LoadLibraryA()`.
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
    const char* path
);

////////////////////////////////////////////////////////////////////////////////
/// Unload a library loaded with \ref bj_load_library from memory.
///
/// This function is an abstraction over the platform specific function like
/// `dlclose` and `FreeLibrary`.
///
/// \param handle The library to unload.
///
/// \par Behaviour
///
/// - On Unix platforms, `handle` is passed to `dlclose()`.
/// - On Windows, `path` is passed to `FreeLibrary()`.
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
    void* handle
);

////////////////////////////////////////////////////////////////////////////////
/// Get the address of a function exported by `handle` given its name.
///
/// This function is an abstraction over the platform specific function like
/// `dlsym` and `GetProcAddress`.
///
/// \param handle A library handle provided by \ref bj_load_library.
/// \param name C-String name of the function to retrieve
///
/// \return _0_ or the address of the retrieved function.
///
/// \par Behaviour
///
/// - On Unix platforms, `handle` and `name` are passed to `dlsym()`.
/// - On Windows, `handle` and `name` are passed to `GetProcAddress()`.
///
/// \par Memory Management
///
/// The caller is responsible for release the loaded function using \ref 
/// bj_unload_library with `handle`.
///
/// 
/// \see bj_load_library, bj_unload_library
/// \see [dlsym()](https://linux.die.net/man/3/dlsym),
///      [GetProcAddress()](https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getprocaddress)
///
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void* bj_library_symbol(
    void*       handle,
    const char* name
);

#endif
/// \} // End of system group
