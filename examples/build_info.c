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
    const bj_build_info* info = bj_get_build_info();

    bj_info("%s API Version %d.%d.%d (0x%08X), %s build",
        info->p_name,
        BJ_VERSION_MAJOR(info->version),
        BJ_VERSION_MINOR(info->version),
        BJ_VERSION_PATCH(info->version),
        info->version,
        info->debug ? "Debug" : "Release"
    );
    bj_info("Compiler: %s %d", info->compiler_name, info->compiler_version);

    DESC(config_checks_abort);
    DESC(config_checks_log);
    DESC(config_log_color);
    DESC(config_pedantic);
    DESC(feature_alsa);
    DESC(feature_emscripten);
    DESC(feature_mme);
    DESC(feature_win32);
    DESC(feature_x11);

    return 0;
}
