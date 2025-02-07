////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief Compile-time configuration file
///
/// This file details the complete set of compile-time options that Banjo was
/// compiled with. 
///
/// See \ref config.
///
////////////////////////////////////////////////////////////////////////////////
/// \defgroup config Configuration
/// \ingroup core
/// \brief Macro definition for customizing Banjo.
///
/// If you are compiling Banjo, you can comment/uncomment various defines to
/// customize the compilation.
/// Note that when compiling Banjo using CMake, the values in this file are
/// overridden by CMake options, so you don't need to change it directly.
///
/// If you are compiling against the Banjo API, you can rely on this header to
/// know precisely how Banjo was compiled.
///
/// Each option is documented, and if necessary, the associated documentation 
/// also provides the set of other requirements needed.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// \addtogroup config
/// \par Logging
///
/// - `BJ_CONFIG_LOG_COLOR`: When set, \ref log provides colored output.
////////////////////////////////////////////////////////////////////////////////

// #define BJ_CONFIG_LOG_COLOR

////////////////////////////////////////////////////////////////////////////////
/// \addtogroup config
/// \par Runtime checks
///
/// Runtime checks are used to provide a few checks on function calls.
/// By default, a failing check does nothing else than existing the function
/// early.
///
/// - `BJ_CONFIG_CHECKS`         : Enable runtime checks.
/// - `BJ_CONFIG_CHECKS_LOG`     : Failing checks are logged using
///                                      \ref bj_err.
/// - `BJ_CONFIG_CHECKS_ABORT`: Failing checks call `abort()`.
////////////////////////////////////////////////////////////////////////////////

// #define BJ_CONFIG_CHECKS
// #define BJ_CONFIG_CHECKS_ABORT
// #define BJ_CONFIG_CHECKS_LOG

////////////////////////////////////////////////////////////////////////////////
/// \addtogroup config
/// \par Pedantic Runtime
///
/// - `BJ_CONFIG_PEDANTIC`: When there is a compromise to make between
///                               safety and performance, this flag prioritizes
///                               safety.
////////////////////////////////////////////////////////////////////////////////

// #define BJ_CONFIG_PEDANTIC

////////////////////////////////////////////////////////////////////////////////
/// \addtogroup config
/// \par Additional Features
///
/// - `BJ_CONFIG_PEDANTIC`: When there is a compromise to make between
///                               safety and performance, this flag prioritizes
///                               safety.
////////////////////////////////////////////////////////////////////////////////


