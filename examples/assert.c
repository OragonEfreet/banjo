////////////////////////////////////////////////////////////////////////////////
/// \example assert.c
////////////////////////////////////////////////////////////////////////////////
#include <banjo/assert.h>
#include <banjo/main.h>

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    bj_assert(BJ_FALSE);

    return 0;
}
