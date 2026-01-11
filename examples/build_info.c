////////////////////////////////////////////////////////////////////////////////
/// \example build_info.c
/// Querying runtime build information about the Banjo library.
///
/// This demonstrates how to introspect the Banjo library to discover its
/// version, build configuration, available backends, and compiler information.
/// Useful for debugging, diagnostics, and feature detection.
////////////////////////////////////////////////////////////////////////////////
#include <banjo/api.h>
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/version.h>

#include <stdio.h>

// Macro to print a feature flag: '+' if enabled, '-' if disabled.
#define DESC(mode) printf("%c %s\n", info->mode ? '+' : '-', #mode)

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    // bj_build_information() returns a structure containing runtime information
    // about how the Banjo library was built. This is the actual loaded binary's
    // configuration, not compile-time constants.
    const bj_build_info* info = bj_build_information();

    // The version is stored as a packed 32-bit integer. bj_format_version()
    // converts it to a human-readable SemVer string (e.g., "0.1.0-dev").
    char version_string[32] = {0};
    bj_format_version(version_string, 32, info->version);

    // Print basic library information: name, version, variant, and build type.
    printf("%s version %s (0x%08X) [%s] %s build\n",
        info->name,
        version_string, info->version,
        info->variant,
        info->debug ? "Debug" : "Release"
    );

    // Compiler used to build the library.
    printf("Compiler: %s %d\n", info->compiler_name, info->compiler_version);

    // Platform backends that were compiled into the library. These determine
    // which windowing systems (Win32, X11, Cocoa, Emscripten) and audio
    // systems (ALSA, MME, CoreAudio) are available at runtime.
    DESC(backend_alsa);
    DESC(backend_cocoa);
    DESC(backend_emscripten);
    DESC(backend_mme);
    DESC(backend_win32);
    DESC(backend_x11);

    // Configuration flags that affect runtime behavior.
    DESC(checks_abort);   // Failed checks abort execution
    DESC(checks_log);     // Failed checks are logged
    DESC(fastmath);       // Fast math optimizations enabled
    DESC(log_color);      // Colored log output enabled
    DESC(pedantic);       // Extra runtime checks enabled

    return 0;
}
