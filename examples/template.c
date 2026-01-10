////////////////////////////////////////////////////////////////////////////////
/// \example template.c
/// A do-nothing template file used for creating examples.
////////////////////////////////////////////////////////////////////////////////
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/system.h>

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    if(!bj_begin(BJ_VIDEO_SYSTEM | BJ_AUDIO_SYSTEM, 0)) {
        return 1;
    } 

    bj_info("Hello Banjo!");

    bj_end();
    return 0;
}
