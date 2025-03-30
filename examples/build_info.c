////////////////////////////////////////////////////////////////////////////////
/// \example build_info.c
/// Query binary information about the Banjo API.
////////////////////////////////////////////////////////////////////////////////
#include <banjo/api.h>

#include <stdio.h>

#define DESC(mode) printf("%c %s\n", info->mode ? '+' : '-', #mode)

int main() {
    const bj_build_info* info = bj_get_build_info();

    printf("%s API Version %d.%d.%d (0x%08X), %s build\n",
        info->p_name,
        BJ_VERSION_MAJOR(info->version),
        BJ_VERSION_MINOR(info->version),
        BJ_VERSION_PATCH(info->version),
        info->version,
        info->debug ? "Debug" : "Release"
    );
    printf("Compiler: %s %d\n", info->compiler_name, info->compiler_version);

    DESC(feature_win32);
    DESC(feature_x11);
    DESC(config_checks_abort);
    DESC(config_checks_log);
    DESC(config_log_color);
    DESC(config_pedantic);

}
