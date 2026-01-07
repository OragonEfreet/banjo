////////////////////////////////////////////////////////////////////////////////
/// \file cli.h
/// \brief POSIX/GNU-like command-line argument parser
////////////////////////////////////////////////////////////////////////////////
/// \defgroup cli Argument Parsing
///
/// \brief Command-line argument parsing utilities
///
/// This module provides a robust, lightweight command-line argument parser
/// supporting short/long options, flags, positional arguments, chained flags,
/// and automatic help generation.
///
/// # Features
///
/// - Short options (e.g., `-f`) and long options (e.g., `--file`)
/// - Positional arguments with required/optional flags
/// - Value arguments with type validation (int, uint, double, bool, string)
/// - Flag arguments (boolean switches)
/// - Chained short options (e.g., `-abc` == `-a -b -c`)
/// - Multiple value syntaxes:
///   - `-finput.txt` and `-f input.txt` for short options
///   - `--file=input.txt` and `--file input.txt` for long options
/// - Automatic help generation
/// - Zero-heap-allocation design (except on error)
/// - FSM-based parser for stability and low memory footprint
///
/// Usage involves defining a \ref bj_cli instance and calling 
/// \ref bj_cli_parse.
///
/// Each argument is described by a \ref bj_cli_argument, defining parsing
/// and storage behavior.
///
/// By default, the parser does not allocate heap memory.
///
/// \{
////////////////////////////////////////////////////////////////////////////////
#ifndef BJ_CLI_H
#define BJ_CLI_H

#include <banjo/api.h>
#include <banjo/error.h>

struct bj_cli;
struct bj_cli_argument;

////////////////////////////////////////////////////////////////////////////////
/// \brief Callback function prototype for argument value processing.
///
/// Set in each \ref bj_cli_argument to parse, validate and store the
/// argument's string value extracted from argv.
///
/// Predefined callbacks include:
/// - \ref bj_cli_store_cstring
/// - \ref bj_cli_store_double
/// - \ref bj_cli_store_int
/// - \ref bj_cli_store_uint
/// - \ref bj_cli_store_bool
/// - \ref bj_cli_print_help_action
///
/// \param parser  Pointer to the bj_cli instance
/// \param arg     Pointer to the argument descriptor
/// \param value   Argument string value from argv
/// \param dest    Pointer to storage location
/// \param error   Error output location (may be NULL to ignore errors)
///
/// \return BJ_TRUE on success, BJ_FALSE on error
///
typedef bj_bool (*bj_cli_action_fn)(
    const struct bj_cli*          parser,
    const struct bj_cli_argument* arg,
    const char*                   value,
    void*                         dest,
    struct bj_error**             error
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Descriptor for a single command line argument.
///
/// Defines how an argument is parsed and stored by \ref bj_cli_parse.
///
/// Most arguments will set `.dest` and either `.name` (long option) or
/// `.shortname` (short option).
///
/// \code{.c}
/// int verbose = 0;
///
/// struct bj_cli parser = {
///     .arguments_len = 1,
///     .arguments     = (struct bj_cli_argument[]) {
///         {.shortname = 'v', .name = "verbose", .help = "verbose mode", .dest = &verbose},
///     },
/// };
///
/// struct bj_error* error = NULL;
/// bj_cli_parse(&parser, argc, argv, &error);
///
/// if (verbose) {
///     bj_info("Verbose Mode");
/// }
/// \endcode
///
/// \par Named arguments
///
/// Named arguments have `.name` (e.g., `--file`) or `.shortname` (e.g., `-f`).
/// They can appear anywhere in the command line, multiple times, and are
/// optional unless `.required` is set.
///
/// If the argument expects a value, `.action` and `.dest` must be set.
/// The `.action` callback validates, converts, and stores the value.
///
/// \par Flags
///
/// Flags are named arguments with no value expected.
/// To create a flag, set `.dest` but leave `.action` as _0_.
///
/// The integer pointed by `.dest` is set to 1 when the flag is present.
///
/// \par Positional arguments
///
/// Positional arguments have no `.name` or `.shortname`.
/// They are identified by their order relative to other positional arguments.
///
/// They can be declared anywhere in the argument list, mixed with named args.
///
/// Example:
/// \code{.c}
/// struct bj_cli parser = {
///     .arguments_len = 4,
///     .arguments     = (struct bj_cli_argument[]) {
///         {.shortname = 'f', .name = "format", .help = "output format",
///          .dest = &format, .action = bj_cli_store_cstring},
///         {.help = "input file", .dest = &input, .action = bj_cli_store_cstring},
///         {.shortname = 'v', .name = "verbose", .help = "verbose mode", .dest = &verbose},
///         {.help = "output file", .dest = &output, .action = bj_cli_store_cstring},
///     },
/// };
/// \endcode
///
struct bj_cli_argument {
    const char*            name;       ///< Long option name (e.g., "file" for `--file`).
                                       ///< Can be _0_ if positional or only shortname used.
    char                   shortname;  ///< Short option name (e.g., 'o' for `-o`).
                                       ///< Can be _0_ if positional or only long name used.
    const char*            help;       ///< Help message shown in usage output.
                                       ///< Can be _0_ if no help needed.
    const char*            metavar;    ///< Placeholder for argument value in help output.
                                       ///< Must not be _0_ for flag arguments.
    void*                  dest;       ///< Pointer to store parsed value.
                                       ///< If _0_, program exits after argument is found (e.g., `--help`).
    bj_cli_action_fn       action;     ///< Callback to parse/store argument value.
                                       ///< If _0_, `.dest` is set as int flag.
    int                    required;   ///< Set to _1_ if positional argument is mandatory.
};

////////////////////////////////////////////////////////////////////////////////
/// \brief Parser context and argument list descriptor.
///
/// Contains global parser settings, program info, and list of argument
/// descriptors.
///
struct bj_cli {
    const char*             prog;          ///< Program name; if _0_, argv[0] is used.
    const char*             description;   ///< Description text displayed before argument list.
    const char*             epilog;        ///< Text displayed after argument list.
    size_t                  arguments_len; ///< Number of arguments in the parser.
    struct bj_cli_argument* arguments;     ///< Pointer to array of argument descriptors.
};

////////////////////////////////////////////////////////////////////////////////
/// \brief Parse command-line arguments according to parser configuration.
///
/// Parses the provided argument vector (`argv`) according to the argument
/// descriptors in `parser`, storing results in the destinations specified
/// by each argument.
///
/// This function validates the parser configuration, processes all arguments,
/// and reports any errors through the error system.
///
/// \param parser Pointer to the argument parser configuration.
/// \param argc     Argument count (typically from main).
/// \param argv     Argument vector (typically from main).
/// \param error  Error output location (may be NULL to ignore errors).
///
/// \return BJ_TRUE on success, BJ_FALSE if parsing failed or configuration invalid.
///
/// \par Error Codes
/// - BJ_ERROR_INVALID_DATA: Parser configuration error (duplicate names, etc.)
/// - BJ_ERROR_INCORRECT_VALUE: Invalid argument value provided
/// - BJ_ERROR: General parsing error (missing required args, unknown options, etc.)
///
/// \par Zero-Heap Design
/// This function does not allocate heap memory during normal operation.
/// Memory allocation only occurs if an error is reported (for error object).
///
/// \see bj_cli_validate, bj_cli_print_help
///
BANJO_EXPORT bj_bool bj_cli_parse(
    struct bj_cli*    parser,
    int               argc,
    char*             argv[],
    struct bj_error** error
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Check for configuration errors in the parser.
///
/// Ensures no conflicts or inconsistencies in argument definitions.
/// Reports configuration errors such as:
/// - Duplicate argument names or shortnames
/// - Invalid characters in names
/// - Conflicting dest pointers
/// - Invalid metavar usage
///
/// \param parser Pointer to the argument parser configuration.
/// \param error  Error output location (may be NULL to ignore errors).
///
/// \return BJ_TRUE if configuration is valid, BJ_FALSE if errors found.
///
/// \par Error Codes
/// - BJ_ERROR_INVALID_DATA: Parser configuration has errors
///
BANJO_EXPORT bj_bool bj_cli_validate(
    const struct bj_cli* parser,
    struct bj_error**    error
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Print help message using Banjo's logging system.
///
/// Generates and prints formatted help text showing program description,
/// usage syntax, and all argument descriptions using `bj_info()`.
///
/// \param parser Pointer to the argument parser configuration.
///
/// \see bj_cli_get_help_string
///
BANJO_EXPORT void bj_cli_print_help(
    const struct bj_cli* parser
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Get help message as a string for custom output.
///
/// Generates formatted help text into the provided buffer.
///
/// This function can be called with `buffer` set to NULL and `buffer_size`
/// set to 0, in which case it returns the required buffer size without
/// writing anything.
///
/// \param parser    Pointer to the argument parser configuration.
/// \param buffer    Buffer to write help text to (may be NULL).
/// \param buffer_size Size of buffer in bytes.
///
/// \return Number of bytes written (excluding null terminator), or required
///         size if `buffer` is NULL.
///
/// \see bj_cli_print_help
///
BANJO_EXPORT size_t bj_cli_get_help_string(
    const struct bj_cli* parser,
    char*                buffer,
    size_t               buffer_size
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Store string argument value.
///
/// Stores value as a null-terminated string pointer.
/// The pointer is stored directly (no copy is made).
///
/// \param parser Parser instance.
/// \param arg    Argument descriptor.
/// \param value  C-string value.
/// \param dest   Destination pointer (const char**).
/// \param error  Error output location.
///
/// \return BJ_TRUE on success, BJ_FALSE on error.
///
BANJO_EXPORT bj_bool bj_cli_store_cstring(
    const struct bj_cli*          parser,
    const struct bj_cli_argument* arg,
    const char*                   value,
    void*                         dest,
    struct bj_error**             error
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Store double argument value.
///
/// Parses and stores a double floating point value.
///
/// \param parser Parser instance.
/// \param arg    Argument descriptor.
/// \param value  String representation of float value.
/// \param dest   Destination pointer (double*).
/// \param error  Error output location.
///
/// \return BJ_TRUE on success, BJ_FALSE on error.
///
/// \par Error Codes
/// - BJ_ERROR_INCORRECT_VALUE: Invalid or out-of-range value
///
BANJO_EXPORT bj_bool bj_cli_store_double(
    const struct bj_cli*          parser,
    const struct bj_cli_argument* arg,
    const char*                   value,
    void*                         dest,
    struct bj_error**             error
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Store int argument value.
///
/// Parses and stores a signed integer value.
///
/// \param parser Parser instance.
/// \param arg    Argument descriptor.
/// \param value  String representation of int value.
/// \param dest   Destination pointer (int*).
/// \param error  Error output location.
///
/// \return BJ_TRUE on success, BJ_FALSE on error.
///
/// \par Error Codes
/// - BJ_ERROR_INCORRECT_VALUE: Invalid or out-of-range value
///
BANJO_EXPORT bj_bool bj_cli_store_int(
    const struct bj_cli*          parser,
    const struct bj_cli_argument* arg,
    const char*                   value,
    void*                         dest,
    struct bj_error**             error
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Store unsigned int argument value.
///
/// Parses and stores an unsigned integer value.
///
/// \param parser Parser instance.
/// \param arg    Argument descriptor.
/// \param value  String representation of unsigned int value.
/// \param dest   Destination pointer (unsigned int*).
/// \param error  Error output location.
///
/// \return BJ_TRUE on success, BJ_FALSE on error.
///
/// \par Error Codes
/// - BJ_ERROR_INCORRECT_VALUE: Invalid or out-of-range value
///
BANJO_EXPORT bj_bool bj_cli_store_uint(
    const struct bj_cli*          parser,
    const struct bj_cli_argument* arg,
    const char*                   value,
    void*                         dest,
    struct bj_error**             error
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Store boolean argument value.
///
/// Accepts _true_, _false_, _1_, _0_, _yes_, _no_ (case-insensitive).
///
/// \param parser Parser instance.
/// \param arg    Argument descriptor.
/// \param value  String representation of bool value.
/// \param dest   Destination pointer (int*).
/// \param error  Error output location.
///
/// \return BJ_TRUE on success, BJ_FALSE on error.
///
/// \par Error Codes
/// - BJ_ERROR_INCORRECT_VALUE: Invalid boolean value
///
BANJO_EXPORT bj_bool bj_cli_store_bool(
    const struct bj_cli*          parser,
    const struct bj_cli_argument* arg,
    const char*                   value,
    void*                         dest,
    struct bj_error**             error
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Print help argument action.
///
/// Special action function to trigger printing help and exiting.
/// Use this as the `.action` for a `--help` argument.
///
/// \param parser Parser instance.
/// \param arg    Argument descriptor.
/// \param value  Argument string (usually NULL).
/// \param dest   Destination (if _0_, program exits after printing help).
/// \param error  Error output location.
///
/// \return BJ_TRUE always.
///
BANJO_EXPORT bj_bool bj_cli_print_help_action(
    const struct bj_cli*          parser,
    const struct bj_cli_argument* arg,
    const char*                   value,
    void*                         dest,
    struct bj_error**             error
);

#endif // BJ_CLI_H
/// \} // End of cli group
