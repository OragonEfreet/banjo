////////////////////////////////////////////////////////////////////////////////
/// \example build_info.c
/// Query binary information about the Banjo API.
////////////////////////////////////////////////////////////////////////////////
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/api.h>

#include <stdio.h>

#define DESC(mode) printf("%c %s\n", info->mode ? '+' : '-', #mode)

static const char* variant_string[] = { "", "-dev", "-beta", "-rc", };

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    const bj_build_info* info = bj_build_information();

    printf("%s API Version %d.%d.%d%s (0x%08X), %s build\n",
        info->name,
        BJ_VERSION_MAJOR(info->version),
        BJ_VERSION_MINOR(info->version),
        BJ_VERSION_PATCH(info->version),
        variant_string[BJ_VERSION_VARIANT(info->version)],
        info->version,
        info->debug ? "Debug" : "Release"
    );
    printf("Compiler: %s %d\n", info->compiler_name, info->compiler_version);

    DESC(backend_alsa);
    DESC(backend_cocoa);
    DESC(backend_emscripten);
    DESC(backend_mme);
    DESC(backend_win32);
    DESC(backend_x11);

    DESC(checks_abort);
    DESC(checks_log);
    DESC(fastmath);
    DESC(log_color);
    DESC(pedantic);

    return 0;
}
