////////////////////////////////////////////////////////////////////////////////
/// \example build_info.c
/// Query binary information about the Banjo API.
////////////////////////////////////////////////////////////////////////////////
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/api.h>

#define DESC(mode) bj_info("%c %s", info->mode ? '+' : '-', #mode)

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    bj_info("%s", BANJO_ASSETS_DIR);
    const bj_build_info* info = bj_build_information();

    bj_info("%s API Version %d.%d.%d (0x%08X), %s build",
        info->name,
        BJ_VERSION_MAJOR(info->version),
        BJ_VERSION_MINOR(info->version),
        BJ_VERSION_PATCH(info->version),
        info->version,
        info->debug ? "Debug" : "Release"
    );
    bj_info("Compiler: %s %d", info->compiler_name, info->compiler_version);

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
