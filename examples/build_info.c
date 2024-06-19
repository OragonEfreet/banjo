////////////////////////////////////////////////////////////////////////////////
/// \example build_info.c
/// Query binary information about the Banjo API.
////////////////////////////////////////////////////////////////////////////////
#include <banjo/api.h>

#include <stdio.h>

#define DESC_FEAT(mode) printf("%c %s\n", info->mode ? '+' : '-', #mode)

int main(int argc, char* argv[]) {
    const bj_build_info* info = bj_get_build_info();

    printf("%s API Version %d.%d.%d (0x%08X), %s build\n",
        info->p_name,
        BJ_VERSION_MAJOR(info->version),
        BJ_VERSION_MINOR(info->version),
        BJ_VERSION_PATCH(info->version),
        info->version,
        info->debug ? "Debug" : "Release"
    );

    DESC_FEAT(abort_on_checks);
    DESC_FEAT(checks);
    DESC_FEAT(log_color);
    DESC_FEAT(pedantic);
}
