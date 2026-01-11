////////////////////////////////////////////////////////////////////////////////
/// \example build_info.c
/// Query binary information about the Banjo API.
////////////////////////////////////////////////////////////////////////////////
#include <banjo/api.h>
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/version.h>

#include <stdio.h>

#define DESC(mode) printf("%c %s\n", info->mode ? '+' : '-', #mode)

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;


    const bj_build_info* info = bj_build_information();

    char version_string[32] = {0};
    bj_format_version(version_string, 32, info->version);


    printf("%s version %s (0x%08X) [%s] %s build\n",
        info->name,
        version_string, info->version,
        info->variant,
        info->debug ? "Debug" : "Release"
    );
    /* printf("Compiler: %s %d\n", info->compiler_name, info->compiler_version); */

    /* DESC(backend_alsa); */
    /* DESC(backend_cocoa); */
    /* DESC(backend_emscripten); */
    /* DESC(backend_mme); */
    /* DESC(backend_win32); */
    /* DESC(backend_x11); */

    /* DESC(checks_abort); */
    /* DESC(checks_log); */
    /* DESC(fastmath); */
    /* DESC(log_color); */
    /* DESC(pedantic); */

    return 0;
}
