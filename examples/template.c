////////////////////////////////////////////////////////////////////////////////
/// \example template.c
/// Minimal Banjo program demonstrating the init-work-cleanup lifecycle pattern.
///
/// Every Banjo program follows this structure: initialize subsystems with
/// bj_begin(), do your work, then cleanup with bj_end().
////////////////////////////////////////////////////////////////////////////////

#include <banjo/log.h>     // Logging: bj_info, bj_warn, bj_error, bj_debug
#include <banjo/main.h>    // Lifecycle: bj_begin, bj_end
#include <banjo/system.h>  // Subsystem flags: BJ_VIDEO_SYSTEM, BJ_AUDIO_SYSTEM

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    // Initialize Banjo subsystems. Choose what you need:
    //   BJ_VIDEO_SYSTEM - windows, rendering, events
    //   BJ_AUDIO_SYSTEM - audio playback
    // Returns false on failure (unsupported platform, insufficient resources).
    if(!bj_begin(BJ_VIDEO_SYSTEM | BJ_AUDIO_SYSTEM, 0)) {
        return 1;
    }

    // With Banjo initialized, use its features. Here we just log a message.
    bj_info("Hello Banjo!");

    // Always cleanup before exit to release all Banjo resources.
    bj_end();
    return 0;
}
