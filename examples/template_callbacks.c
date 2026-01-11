////////////////////////////////////////////////////////////////////////////////
/// \example template_callbacks.c
/// Callback-based program structure using Banjo's automain feature.
///
/// This demonstrates an alternative to writing main() yourself. Instead of
/// managing the program loop manually, you define three callbacks that Banjo
/// invokes at the right times. This pattern is required for Emscripten/web
/// builds and provides cleaner separation of initialization, iteration, and
/// cleanup phases.
////////////////////////////////////////////////////////////////////////////////

// This macro tells Banjo to generate main() for you. Banjo will call your
// three callback functions (bj_app_begin, bj_app_iterate, bj_app_end) in
// the correct order and manage the program lifecycle.
#define BJ_AUTOMAIN_CALLBACKS
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/system.h>

// Called once at program startup. Initialize your resources here.
//
// user_data: Pointer to store application state (allocated here, passed to
//            other callbacks). Use *user_data = bj_malloc(...) to allocate.
// Returns: bj_callback_continue to proceed to iterate phase
//          bj_callback_exit_error to abort with error code
int bj_app_begin(void** user_data, int argc, char* argv[]) {
    (void)user_data; (void)argc; (void)argv;

    // Initialize Banjo subsystems. Same as in template.c.
    if(!bj_begin(BJ_VIDEO_SYSTEM | BJ_AUDIO_SYSTEM, 0)) {
        return bj_callback_exit_error;
    }

    return bj_callback_continue;
}

// Called repeatedly in a loop (like while(1) { ... }). Your main logic goes here.
//
// user_data: Application state from bj_app_begin (or NULL if none allocated)
// Returns: bj_callback_continue to keep looping
//          bj_callback_exit_success to stop and cleanup normally
//          bj_callback_exit_error to stop and cleanup with error
int bj_app_iterate(void* user_data) {
    (void)user_data;

    bj_info("Hello Banjo!");

    // Exit after one iteration for this simple example. In a real program,
    // you'd return bj_callback_continue until the user closes the window or
    // some exit condition is met.
    return bj_callback_exit_success;
}

// Called once when iterate requests exit. Cleanup your resources here.
//
// user_data: Application state from bj_app_begin
// status: The return code from bj_app_iterate (success or error)
// Returns: Exit code for the program (typically just pass through status)
int bj_app_end(void* user_data, int status) {
    (void)user_data;

    // Shutdown Banjo and release resources. If you allocated user_data in
    // bj_app_begin, free it here with bj_free(user_data).
    bj_end();

    return status;
}

