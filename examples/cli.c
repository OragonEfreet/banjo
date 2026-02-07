////////////////////////////////////////////////////////////////////////////////
/// \example cli.c
/// Command-line argument parsing with type conversion and validation.
///
/// Banjo's CLI parser automatically handles:
/// - Named options: --flag or -f (short form)
/// - Type conversion: strings, integers, doubles, booleans
/// - Help generation: --help prints auto-generated usage
/// - Positional arguments: arguments without dashes
/// - Short option combining: -abc = -a -b -c
///
/// The pattern: declare storage variables, define argument specs with actions,
/// create parser, parse, then use the values. Actions automatically convert
/// and store parsed values into your variables.
////////////////////////////////////////////////////////////////////////////////

#include <banjo/cli.h>
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/memory.h>

#include <stdio.h>

int main(int argc, char* argv[]) {
    // Declare variables to store parsed values. Set defaults here - if the
    // user doesn't provide a value, these defaults remain unchanged.
    int verbose             = 0;
    const char* input_file  = "default.txt";
    const char* output_file = NULL;
    int count               = 1;
    int threads             = 4;
    double tolerance        = 0.001;
    int enable_feature      = 0;

    // Define argument specifications. Each bj_cli_argument describes one option.
    // Fields:
    //   - name: long option name (--name)
    //   - shortname: single-character shortcut (-n)
    //   - help: description shown in --help
    //   - metavar: placeholder for value display (--count <N>)
    //   - action: function to parse and store the value
    //   - dest: pointer to variable that receives the value
    //   - required: whether this argument must be provided
    struct bj_cli_argument args[] = {
        // bj_cli_print_help_action: special action that prints help and exits.
        // Use this for --help flags. No dest needed.
        {
            .shortname = 'h',
            .name      = "help",
            .help      = "Show this help message and exit",
            .action    = bj_cli_print_help_action
        },

        // Boolean flag (no action specified): toggles the variable to 1 if present.
        // Usage: -v or --verbose sets verbose=1
        {
            .shortname = 'v',
            .name      = "verbose",
            .help      = "Enable verbose output",
            .dest      = &verbose
        },

        // bj_cli_store_cstring: parses the next argument as a string.
        // Usage: --input file.txt or -i file.txt
        {
            .shortname = 'i',
            .name      = "input",
            .help      = "Input file path",
            .metavar   = "FILE",
            .action    = bj_cli_store_cstring,
            .dest      = &input_file
        },

        // bj_cli_store_int: parses the next argument as an integer.
        // Usage: --count 42 or -c 42
        {
            .shortname = 'c',
            .name      = "count",
            .help      = "Number of iterations to perform",
            .metavar   = "N",
            .action    = bj_cli_store_int,
            .dest      = &count
        },

        // Another integer option. Can have shortname or not.
        {
            .shortname = 't',
            .name      = "threads",
            .help      = "Number of worker threads",
            .metavar   = "NUM",
            .action    = bj_cli_store_int,
            .dest      = &threads
        },

        // bj_cli_store_double: parses the next argument as a floating-point number.
        // Usage: --tolerance 0.001
        {
            .name    = "tolerance",
            .help    = "Tolerance level for calculations",
            .metavar = "TOL",
            .action  = bj_cli_store_double,
            .dest    = &tolerance
        },

        // bj_cli_store_bool: parses true/false, yes/no, 1/0 as boolean.
        // Usage: --enable-feature true
        {
            .name    = "enable-feature",
            .help    = "Enable experimental feature (true/false)",
            .metavar = "BOOL",
            .action  = bj_cli_store_bool,
            .dest    = &enable_feature
        },

        // Positional argument: no name or shortname, just help and metavar.
        // Matches arguments that don't start with dashes, in order.
        // Usage: program [options] output.txt
        {
            .help     = "Output file path (optional)",
            .metavar  = "OUTPUT",
            .action   = bj_cli_store_cstring,
            .dest     = &output_file,
            .required = 0
        },
    };

    // Create the parser with program metadata.
    // description: shown in help before options list
    // epilog: shown in help after options list (good for examples)
    // The parser auto-generates help text from argument definitions.
    struct bj_cli parser = {
        .prog          = "example_cli",
        .description   = "Example program demonstrating Banjo's argument parsing.\n"
                         "Shows flags, named options, type conversion, and positional arguments.",
        .epilog        = "Examples:\n"
                         "  example_cli -v -i data.txt output.txt\n"
                         "  example_cli --count 10 --threads 8 --tolerance 0.01\n"
                         "  example_cli -vci input.txt -t 4 result.txt",
        .arguments_len = sizeof(args) / sizeof(args[0]),
        .arguments     = args,
    };

    // Parse command-line arguments. This processes argv, calls actions to
    // convert and store values, and validates required arguments.
    // Returns false if parsing fails (unknown option, missing value, etc.)
    // On failure, print help and exit with error code.
    if (!bj_cli_parse(&parser, argc, argv, 0)) {
        bj_cli_print_help(&parser);
        return 1;
    }

    // After successful parsing, all variables contain the parsed values.
    // Just use them normally - the parser has already converted types and
    // validated everything. If a value wasn't provided, it still has the
    // default you initialized it with.
    bj_info("=== Parsed Arguments ===");
    bj_info("Verbose: %s", verbose ? "enabled" : "disabled");
    bj_info("Input File: %s", input_file);
    bj_info("Count: %d", count);
    bj_info("Threads: %d", threads);
    bj_info("Tolerance: %.6f", tolerance);
    bj_info("Feature Enabled: %s", enable_feature ? "yes" : "no");

    if (output_file) {
        bj_info("Output File: %s", output_file);
    } else {
        bj_info("Output File: (not specified)");
    }

    bj_info("");
    bj_info("=== Simulating Work ===");
    if (verbose) {
        for (int i = 0; i < count; i++) {
            bj_info("Processing iteration %d/%d...", i + 1, count);
        }
    } else {
        bj_info("Processing %d iterations...", count);
    }

    bj_info("Done!");

    return 0;
}
