////////////////////////////////////////////////////////////////////////////////
/// \file A3DSDKargParse.h
/// \brief POSIX/GNU-like argc/argv parsing API
///
/// This header defines a robust, lightweight command-line argument parser
/// supporting short/long options, flags, positional arguments, chained flags,
/// and automatic help generation.
///
/// # Example Usage
///
/// ```c
/// #include <stdio.h>
/// #include "A3DSDKargParse.h"
///
/// int verbose = 0;
/// const char* input_file = "default.txt";
/// const char* output_file = NULL;
/// int count = 1;
///
/// int main(int argc, char* argv[]) {
///     HOOPSArgParserArgument args[] = {
///         {.shortname = 'v', .name = "verbose", .help = "Enable verbose output", .dest = &verbose},
///         {.shortname = 'i', .name = "input", .help = "Input file", .action = HOOPSArgParserStoreCString, .dest = &input_file, .metavar = "FILE"},
///         {.shortname = 'c', .name = "count", .help = "Number of iterations", .action = HOOPSArgParserStoreInt, .dest = &count, .metavar = "N"},
///         {.help = "Output file", .action = HOOPSArgParserStoreCString, .dest = &output_file, .metavar = "OUT"}, // positional argument
///     };
///
///     HOOPSArgParser parser = {
///         .prog = "myprogram",
///         .description = "Example program demonstrating A3DSDKargParse usage.",
///         .arguments_len = sizeof(args) / sizeof(args[0]),
///         .arguments = args,
///     };
///
///     char errbuf[256];
///     int err = HOOPSArgParserBuild(&parser, argc, argv, errbuf, sizeof(errbuf));
///     if (err) {
///         fprintf(stderr, "Error parsing arguments: %s\n", errbuf);
///         HOOPSArgParserFilePrintHelp(stderr, &parser);
///         return 1;
///     }
///
///     printf("Verbose: %d\n", verbose);
///     printf("Input File: %s\n", input_file);
///     printf("Count: %d\n", count);
///     if (output_file) {
///         printf("Output File: %s\n", output_file);
///     } else {
///         printf("No output file specified.\n");
///     }
///
///     // Your program logic here
///
///     return 0;
/// }
/// ```
///
/// This example shows:
/// - Flag: `-v` or `--verbose` sets `verbose` to 1.
/// - Named argument with value: `-i FILE` or `--input FILE` stores filename.
/// - Named argument with integer value: `-c N` or `--count N` stores an int.
/// - Positional argument: last argument treated as output filename.
/// - Automatic error and help message handling.
///
/// \par Features
///
/// - Support for tags and valued arguments
/// - Named and positional arguments
/// - Short options (e.g., `-f`)
/// - Long options (e.g., `--file`)
/// - Common syntaxes for setting values:
///   - `-finput.txt` and `-f input.txt` for short options
///   - `--file=input.txt` and `--file input.txt` for long options
/// - Chained short options (e.g., `-vtc` == `-v -t -c`)
///   - Last option in chain can have a value: `-vfinput.txt` or `-vf input.txt`
/// - Automatic help screen generation
/// - Automatic version display
/// - Built-in parsers for int, uint, double, boolean, and string values
/// - Strict parser configuration checks to prevent inconsistencies
/// - Finite-State-Machine-oriented parser for stability, streaming parsing,
///   and low memory footprint.
///
/// Usage involves defining an \ref HOOPSArgParser instance and calling \ref HOOPSArgParserBuild.
///
/// Each argument is described by an \ref HOOPSArgParserArgument, defining parsing
/// and storage behavior.
///
/// By default, the parser does not allocate heap memory.
///
#ifndef HOOPS_ARGPARSE_H
#define HOOPS_ARGPARSE_H

#include <stdio.h> // For FILE*

struct HOOPSArgParser_T;
struct HOOPSArgParserArgument_T;

////////////////////////////////////////////////////////////////////////////////
/// \brief Callback function prototype for argument value processing.
///
/// Set in each \ref HOOPSArgParserArgument to parse, validate and store the
/// argument's string value extracted from argv.
///
/// Predefined callbacks include:
/// - \ref HOOPSArgParserStoreCString
/// - \ref HOOPSArgParserStoreDouble
/// - \ref HOOPSArgParserStoreInt
/// - \ref HOOPSArgParserStoreUInt
/// - \ref HOOPSArgParserStoreBool
/// - \ref HOOPSArgParserPrintHelp
///
/// \param _parser  Pointer to the HOOPSArgParser instance
/// \param _arg     Pointer to the argument descriptor
/// \param _value   Argument string value from argv
/// \param _dest    Pointer to storage location
/// \param _buffer  Buffer to write error messages to
/// \param _n       Size of error buffer
/// 
/// \return _0_ on success, or nonzero error code
///
typedef int (*HOOPSArgParserAction_T)(
    const struct HOOPSArgParser_T*           _parser,
    const struct HOOPSArgParserArgument_T*   _arg,
    const char*                             _value,
    void*                                   _dest,
    char*                                   _buffer,
    size_t                                  _n
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Descriptor for a single command line argument.
///
/// Defines how an argument is parsed and stored by \ref HOOPSArgParserBuild.
///
/// Most arguments will set `.dest` and either `.name` (long option) or
/// `.shortname` (short option).
///
/// \code{.c}
/// int verbose = 0;
///
/// HOOPSArgParser parser = {
///     .arguments_len = 1,
///     .arguments     = (HOOPSArgParserArgument[]) {
///         {.shortname = 'v', .name = "verbose", .help = "verbose mode", .dest = &verbose},
///     },
/// };
///
/// HOOPSArgParserBuild(&parser, argc, argv, 0, 0);
///
/// if (verbose) {
///     printf("Verbose Mode\n");
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
/// HOOPSArgParser parser = {
///     .arguments_len = 4,
///     .arguments     = (HOOPSArgParserArgument[]) {
///         {.shortname = 'f', .name = "format", .help = "output format", .dest = &format, .action = HOOPSArgParserStoreCString},
///         {.help = "input file", .dest = &input, .action = HOOPSArgParserStoreCString},
///         {.shortname = 'v', .name = "verbose", .help = "verbose mode", .dest = &verbose},
///         {.help = "output file", .dest = &output, .action = HOOPSArgParserStoreCString},
///     },
/// };
/// \endcode
///
/// \typedef HOOPSArgParserArgument
///
typedef struct HOOPSArgParserArgument_T {
    const char*           name;       ///< Long option name (e.g., "file" for `--file`).
                                      ///< Can be _0_ if positional or only shortname used.
    char                  shortname;  ///< Short option name (e.g., 'o' for `-o`).
                                      ///< Can be _0_ if positional or only long name used.
    const char*           help;       ///< Help message shown in usage output.
                                      ///< Can be _0_ if no help needed.
    const char*           metavar;    ///< Placeholder for argument value in help output.
                                      ///< Must not be _0_ for flag arguments.
    void*                 dest;       ///< Pointer to store parsed value.
                                      ///< If _0_, program exits after argument is found (e.g., `--help`).
    HOOPSArgParserAction_T action;     ///< Callback to parse/store argument value.
                                      ///< If _0_, `.dest` is set as int flag.
    int                   required;   ///< Set to _1_ if positional argument is mandatory.
} HOOPSArgParserArgument;

////////////////////////////////////////////////////////////////////////////////
/// \brief Parser context and argument list descriptor.
///
/// Contains global parser settings, program info, and list of argument
/// descriptors.
///
typedef struct HOOPSArgParser_T {
    const char*            prog;          ///< Program name; if _0_, argv[0] is used.
    const char*            description;   ///< Description text displayed before argument list.
    const char*            epilog;        ///< Text displayed after argument list.
    size_t                 arguments_len; ///< Number of arguments in the parser.
    HOOPSArgParserArgument* arguments;     ///< Pointer to array of argument descriptors.
} HOOPSArgParser;

////////////////////////////////////////////////////////////////////////////////
/// \brief Arguments common to all HOOPS Exchange example applications
typedef struct HOOPSArgParserStandardArgs_T {
    int verbose;  ///< Application will display extra information (-v / --verbose)
} HOOPSArgParserStandardArgs;

////////////////////////////////////////////////////////////////////////////////
/// \brief Parses argc/argv for common arguments
///
/// This function prepares an internal \ref argpase and uses it to parse
/// argc/argv for HOOPS Exchange samples arguments.
/// The resulting argument values are stored into `args`.
/// 
/// This function is the simplest way to parse arguments, since the required
/// code becomes minimal:
/// \code{.c}
/// HOOPSArgParserStandardArgs args = {0};
/// HOOPSParseStandardArguments(&args, argc, argv);
/// \endcode
///
/// If the parsing fails, the function prints the error in stderr and `*args`
/// is left in an undetermined state.
/// The function _may_ allocate in case of failed parsing.
///
/// \param args Location to an \ref HOOPSArgParserStandardArgs object
/// \param argc Size of argv, typically passed to main()
/// \param argv Argument list, typically passed to main()
/// \return _1_ if the parsing succeeded, _0_ otherwise.
/// 
int HOOPSParseStandardArguments(
    HOOPSArgParserStandardArgs* args, 
    int                         argc, 
    char*                       argv[]
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Parses argc/argv for common arguments, allowing additional arguments.
///
/// The function is an extension of \ref HOOPSParseStandardArguments where it is
/// possible to add another set of arguments.
///
/// It prepares an internal \ref argpase and combines it with `user_parser` to
/// parse argc/argv for HOOPS Exchange samples arguments.
/// The resulting argument values are stored into `args` and any other
/// destination spefifier by `user_parser`.
/// 
/// This function is one of the simplest way to parse arguments, since the 
/// required code becomes minimal:
/// \code{.c}
/// HOOPSArgParserStandardArgs args = {0};
/// double tolerance = 0.0;
///
/// HOOPSParseExtendedArguments(&args, argc, argv, &(HOOPSArgParser){
///     .arguments_len = 1,
///     .arguments     = (HOOPSArgParserArgument[]) {
///         {.shortname='t', .name = "tolerance", .help = "Tolerance level",  .action=HOOPSArgParserStoreDouble, .dest=&tolerance},
///     }
/// });
/// \endcode
///
/// If the parsing fails, the function prints the error in stderr and `*args`
/// is left in an undetermined state.
/// The function _may_ allocate in case of failed parsing.
///
/// \param args Location to an \ref HOOPSArgParserStandardArgs object
/// \param argc Size of argv, typically passed to main()
/// \param argv Argument list, typically passed to main()
/// \return _1_ if the parsing succeeded, _0_ otherwise.
int HOOPSParseExtendedArguments(
    HOOPSArgParserStandardArgs* args, 
    int                        argc, 
    char*                      argv[], 
    HOOPSArgParser*            user_parser
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Print help message to a stream.
///
/// \param stream Output FILE stream.
/// \param parser Parser instance.
/// \return 0 on success, nonzero on error.
///
int HOOPSArgParserFilePrintHelp(
    FILE*                 stream, 
    const HOOPSArgParser* parser
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Check for configuration errors in the parser.
///
/// Ensures no conflicts or inconsistencies in argument definitions.
///
/// \param parser Parser instance.
/// \param buffer Buffer to write error messages to.
/// \param n Size of buffer.
/// \return 0 if no error; otherwise, length of error message written.
///
int HOOPSArgParserValidate(
    const HOOPSArgParser* parser, 
    char*                 buffer, 
    size_t                n
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Parse command line arguments.
///
/// Parses argv according to parser description and stores results.
///
/// If any error occurs, the function will print to `output`.
/// If `output` is empty, it's defaulted to `stderr`.
///
/// This function _may_ allocate memory when an error happens while parsing.
/// See also \ref HOOPSArgParserBuild_SN.
///
/// \param parser Parser instance.
/// \param argc Argument count.
/// \param argv Argument vector.
/// \param output `FILE*` destination for writing error if any.
/// \return 0 on success, or nonzero error code.
///
int HOOPSArgParserBuild(
    HOOPSArgParser* parser,
    int            argc,
    char*          argv[],
    FILE*          output
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Parse command line arguments.
///
/// Parses argv according to parser description and stores results.
///
/// This function can be called with `buffer` and `n` set to 0, in which case
/// the parsing indeed occurs, but any error message will not be stored.
///
/// The function returns the size of the error message, even if not stored.
/// A typical use of this function can be to call it first with no error buffer
/// and call it again if the result of the first call was > 0:
///
/// \code{.c}
/// const int errlen = HOOPSArgParser(parser, argc, argv, 0, 0);
/// if(errlen) {
///     char err[errlen+1];
///     HOOPSArgParser(parser, argc, argv, err, errlen+1);
///     puts(err);
/// }
/// \endcode
///
/// See also \ref HOOPSArgParserBuild for a simpler version of this function.
///
/// \param parser Parser instance.
/// \param argc Argument count.
/// \param argv Argument vector.
/// \param buffer Buffer for error messages.
/// \param n Buffer size.
/// \return 0 on success, or nonzero error code.
///
int HOOPSArgParserBuild_SN(
    HOOPSArgParser*  parser,
    int             argc,
    char*           argv[],
    char*           buffer,
    size_t          n
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Store string argument value.
///
/// Stores value as a null-terminated string pointer.
///
/// \param parser Parser instance.
/// \param arg    Argument descriptor.
/// \param value  C-string value.
/// \param dest   Destination pointer.
/// \param buffer Error buffer.
/// \param n      Buffer size.
/// \return _0_ on success, or the length of the error message on error.
///
int HOOPSArgParserStoreCString(
    const struct HOOPSArgParser_T*         parser,
    const struct HOOPSArgParserArgument_T* arg,
    const char*                           value,
    void*                                 dest,
    char*                                 buffer,
    size_t                                n
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Store double argument value.
///
/// Parses and stores a double floating point value.
///
/// \param parser   Parser instance.
/// \param arg      Argument descriptor.
/// \param value    float value.
/// \param dest     Destination pointer (double*).
/// \param buffer   Error buffer.
/// \param n        Buffer size.
/// \return _0_ on success, or the length of the error message on error.
///
int HOOPSArgParserStoreDouble(
    const struct HOOPSArgParser_T*         parser, 
    const struct HOOPSArgParserArgument_T* arg,
    const char*                           value,
    void*                                 dest,
    char*                                 buffer,
    size_t                                n
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Store int argument value.
///
/// Parses and stores a signed integer value.
///
/// \param parser   Parser instance.
/// \param arg      Argument descriptor.
/// \param value    int value.
/// \param dest     Destination pointer (int*).
/// \param buffer   Error buffer.
/// \param n        Buffer size.
/// \return _0_ on success, or the length of the error message on error.
///
int HOOPSArgParserStoreInt(
    const struct HOOPSArgParser_T*         parser,
    const struct HOOPSArgParserArgument_T* arg,
    const char*                           value,
    void*                                 dest,
    char*                                 buffer,
    size_t                                n
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Store unsigned int argument value.
///
/// Parses and stores an unsigned integer value.
///
/// \param parser   Parser instance.
/// \param arg      Argument descriptor.
/// \param value    unsigned int value.
/// \param dest     Destination pointer (unsigned int*).
/// \param buffer   Error buffer.
/// \param n        Buffer size.
/// \return _0_ on success, or the length of the error message on error.
///
int HOOPSArgParserStoreUInt(
    const struct HOOPSArgParser_T*         parser,
    const struct HOOPSArgParserArgument_T* arg,
    const char*                           value,
    void*                                 dest,
    char*                                 buffer,
    size_t                                n
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Store boolean argument value.
///
/// Accepts _true_, _false_, _1_, _0_, _yes_, _no_ (case-insensitive).
///
/// \param parser   Parser instance.
/// \param arg      Argument descriptor.
/// \param value    bool value.
/// \param dest     Destination pointer (int*).
/// \param buffer   Error buffer.
/// \param n        Buffer size.
/// \return 0 on success, or the length of the error message on error.
///
int HOOPSArgParserStoreBool(
    const struct HOOPSArgParser_T*         parser,
    const struct HOOPSArgParserArgument_T* arg,
    const char*                           value,
    void*                                 dest,
    char*                                 buffer,
    size_t                                n
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Print help argument action.
///
/// Special action function to trigger printing help and exiting.
///
/// \param parser   Parser instance.
/// \param arg      Argument descriptor.
/// \param value    Argument string (usually `NULL`).
/// \param dest     Destination (unused).
/// \param buffer   Error buffer.
/// \param n         Buffer size.
/// \return 0 always.
///
int HOOPSArgParserPrintHelp(
    const struct HOOPSArgParser_T*         parser,
    const struct HOOPSArgParserArgument_T* arg,
    const char*                           value,
    void*                                 dest,
    char*                                 buffer,
    size_t                                n
);

#endif // HOOPS_ARGPARSE_H
