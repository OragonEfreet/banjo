////////////////////////////////////////////////////////////////////////////////
/// \file example_cli.c
/// \brief Example demonstrating Banjo's argument parsing module
////////////////////////////////////////////////////////////////////////////////

#include <banjo/cli.h>
#include <banjo/log.h>
#include <banjo/memory.h>

#include <stdio.h>

int main(int argc, char* argv[]) {
    // Argument storage variables
    int verbose             = 0;
    const char* input_file  = "default.txt";
    const char* output_file = NULL;
    int count               = 1;
    int threads             = 4;
    double tolerance        = 0.001;
    int enable_feature      = 0;

    struct bj_cli_argument args[] = {
        // Help flag (will print help and exit)
        {
            .shortname = 'h',
            .name      = "help",
            .help      = "Show this help message and exit",
            .action    = bj_cli_print_help_action
        },

        // Verbose flag
        {
            .shortname = 'v',
            .name      = "verbose",
            .help      = "Enable verbose output",
            .dest      = &verbose
        },

        // Input file with string value
        {
            .shortname = 'i',
            .name      = "input",
            .help      = "Input file path",
            .metavar   = "FILE",
            .action    = bj_cli_store_cstring,
            .dest      = &input_file
        },

        // Count with integer value
        {
            .shortname = 'c',
            .name      = "count",
            .help      = "Number of iterations to perform",
            .metavar   = "N",
            .action    = bj_cli_store_int,
            .dest      = &count
        },

        // Threads with integer value
        {
            .shortname = 't',
            .name      = "threads",
            .help      = "Number of worker threads",
            .metavar   = "NUM",
            .action    = bj_cli_store_int,
            .dest      = &threads
        },

        // Tolerance with double value
        {
            .name    = "tolerance",
            .help    = "Tolerance level for calculations",
            .metavar = "TOL",
            .action  = bj_cli_store_double,
            .dest    = &tolerance
        },

        // Boolean feature flag
        {
            .name    = "enable-feature",
            .help    = "Enable experimental feature (true/false)",
            .metavar = "BOOL",
            .action  = bj_cli_store_bool,
            .dest    = &enable_feature
        },

        // Positional argument (output file)
        {
            .help     = "Output file path (optional)",
            .metavar  = "OUTPUT",
            .action   = bj_cli_store_cstring,
            .dest     = &output_file,
            .required = 0
        },
    };

    // Configure the parser
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

    // Parse arguments
    struct bj_error* error = NULL;
    if (!bj_cli_parse(&parser, argc, argv, &error)) {
        puts(error->message);
        bj_cli_print_help(&parser);
        bj_clear_error(&error);
        return 1;
    }

    // Display parsed values
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
