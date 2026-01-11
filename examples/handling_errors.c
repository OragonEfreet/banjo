////////////////////////////////////////////////////////////////////////////////
/// \example handling_errors.c
/// Handling recoverable errors with Banjo's error system.
///
/// Banjo uses an out-parameter pattern for error handling. Functions that can
/// fail take a bj_error** parameter to return error information. This allows
/// checking error codes, reading messages, and propagating errors up the call
/// stack without exceptions.
////////////////////////////////////////////////////////////////////////////////
#include <banjo/error.h>
#include <banjo/main.h>
#include <banjo/log.h>

#define CODE 101

// Functions that can fail take a bj_error** as an out-parameter. If the caller
// passes a non-NULL pointer, the function creates an error object on failure.
void function_returning_error(bj_error** error) {
    // Set an error with a code and message. The error object is allocated
    // internally and must be freed later with bj_clear_error.
    bj_set_error(error, CODE, "An error occured");
}

// When calling functions that might fail, check for errors and either handle
// them or forward them up the call stack.
void function_calling_failing_function(bj_error** error) {
    bj_error* sub_err = 0;

    function_returning_error(&sub_err);

    // Check if an error occurred. A non-NULL pointer means failure.
    if(sub_err != 0) {
        // Forward the error to our caller. This transfers ownership of the
        // error object, so we don't need to clear it ourselves.
        bj_forward_error(sub_err, error);
        return;
    }

    bj_info("This should not be printed\n");
}


int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    // Passing NULL (0) for the error parameter tells the function you don't
    // care about error details. The function will still fail, but won't
    // allocate an error object.
    function_returning_error(0);

    // To receive error information, pass a pointer to a bj_error* initialized
    // to NULL (0).
    bj_error* error = 0;

    function_returning_error(&error);

    // Check if an error occurred by testing for non-NULL.
    if(error != 0) {
        bj_info("There was an error");
    }

    // bj_check_error verifies an error exists and matches the expected code.
    // This is safer than comparing codes manually.
    if (bj_check_error(error, CODE)) {
        bj_info("Error domain and code match");
    }

    // Always clear errors when done to free memory. This also sets the pointer
    // back to NULL, making it safe to reuse.
    bj_clear_error(&error);

    // The same error pointer can be reused after clearing.
    function_calling_failing_function(&error);

    if (bj_check_error(error, CODE)) {
        bj_info("Error from nested function");
    }

    return 0;
}
