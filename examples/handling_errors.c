////////////////////////////////////////////////////////////////////////////////
/// \example handling_errors.c
/// Recoverable error handling with Banjo's error system.
///
/// Banjo uses an out-parameter pattern for error handling inspired by GLib.
/// Functions that can fail take a `bj_error**` parameter to return error
/// information. This allows checking error codes, reading messages, and
/// propagating errors up the call stack without exceptions.
////////////////////////////////////////////////////////////////////////////////
#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/main.h>

#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////
// Example: Basic error reporting
////////////////////////////////////////////////////////////////////////////////

// [Return Errors]
// Functions that can fail take a bj_error** as their last parameter.
// Use bj_set_error for literal messages.
void load_config_file(const char* path, bj_error** error) {
    (void)path;
    // Simulate a failure
    int file_exists = 0;

    if (!file_exists) {
        bj_set_error(error, BJ_ERROR_FILE_NOT_FOUND, "configuration file missing");
        return;
    }
    // ... normal processing ...
}

// Use bj_set_error_fmt for formatted messages with runtime values.
void open_network_port(int port, bj_error** error) {
    // Simulate a failure
    int port_available = 0;

    if (!port_available) {
        bj_set_error_fmt(error, BJ_ERROR_SYSTEM,
                         "port %d is already in use", port);
        return;
    }
    // ... normal processing ...
}
// [Return Errors]

////////////////////////////////////////////////////////////////////////////////
// Example: Error propagation with context
////////////////////////////////////////////////////////////////////////////////

void initialize_server(const char* config_path, int port, bj_error** error) {
    bj_error* local_err = 0;

    // Try to load config
    load_config_file(config_path, &local_err);
    if (local_err != 0) {
        // Add context and propagate - local_err is consumed
        bj_propagate_prefixed_error(error, local_err,
                                    "While initializing server: ");
        return;
    }

    // Try to open port
    open_network_port(port, &local_err);
    if (local_err != 0) {
        // Alternative: prefix then propagate separately
        bj_prefix_error_fmt(&local_err, "Cannot bind to port %d: ", port);
        bj_propagate_error(error, local_err);
        return;
    }

    bj_info("Server initialized successfully");
}

////////////////////////////////////////////////////////////////////////////////
// Example: Error matching by code and kind
////////////////////////////////////////////////////////////////////////////////

void demonstrate_error_matching(void) {
    bj_error* err = 0;

    load_config_file("missing.cfg", &err);

    if (err != 0) {
        // Match specific error code
        if (bj_error_matches(err, BJ_ERROR_FILE_NOT_FOUND)) {
            bj_info("Specific match: file not found");
        }

        // Match error kind (category) - catches all system errors
        if (bj_error_matches_kind(err, BJ_ERROR_SYSTEM)) {
            bj_info("Kind match: this is a system error");
        }

        // Access error details
        bj_info("Error code: 0x%08X", bj_error_code(err));
        bj_info("Error message: %s", bj_error_message(err));

        bj_clear_error(&err);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Example: Copying errors
////////////////////////////////////////////////////////////////////////////////

void demonstrate_error_copy(void) {
    bj_error* original = 0;
    bj_error* copy = 0;

    open_network_port(8080, &original);

    if (original != 0) {
        // Create a copy for logging/reporting while keeping original
        copy = bj_copy_error(original);

        bj_info("Original: %s", bj_error_message(original));
        bj_info("Copy: %s", bj_error_message(copy));

        // Both must be freed
        bj_clear_error(&original);
        bj_clear_error(&copy);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Example: Ignoring errors (zero-cost path)
////////////////////////////////////////////////////////////////////////////////

void demonstrate_zero_cost(void) {
    // Pass NULL to indicate you don't care about error details.
    // No allocation occurs - just a pointer check and early return.
    load_config_file("optional.cfg", 0);

    bj_info("Continued despite potential error (zero cost)");
}

////////////////////////////////////////////////////////////////////////////////
// Main
////////////////////////////////////////////////////////////////////////////////

// [Using bj_error]
int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    bj_info("=== Basic Error Handling ===");

    // To receive error information, pass a pointer to a NULL bj_error*
    bj_error* error = 0;

    initialize_server("/etc/myapp.conf", 8080, &error);

    if (error != 0) {
        // The error message now includes context from the call chain
        bj_err("Startup failed: %s", bj_error_message(error));

        // Always clear errors when done to free memory
        bj_clear_error(&error);
    }

    bj_info("\n=== Error Matching ===");
    demonstrate_error_matching();

    bj_info("\n=== Error Copying ===");
    demonstrate_error_copy();

    bj_info("\n=== Zero-Cost Path ===");
    demonstrate_zero_cost();

    return 0;
}
// [Using bj_error]
