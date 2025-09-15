////////////////////////////////////////////////////////////////////////////////
/// \example random.c
/// Demonstrates various way for generating pseudo-randomnes
////////////////////////////////////////////////////////////////////////////////
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/random.h>
#include <banjo/time.h>

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;

    // std-style functions are available and work the same way:
    bj_info("Before srand():");
    for(size_t i = 0 ; i < 5 ; ++i) {
        bj_info("\tbj_rand() -> %d", bj_rand());
    }
    bj_srand(bj_time());
    bj_info("After srand():");
    for(size_t i = 0 ; i < 5 ; ++i) {
        bj_info("\tbj_rand() -> %d", bj_rand());
    }



    return 0;
}

