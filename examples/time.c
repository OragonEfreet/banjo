#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/system.h>
#include <banjo/time.h>

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    if(bj_begin(0)) {

        for(size_t i = 0 ; i < 10 ; ++i) {
            bj_sleep(300);
            bj_trace("- %lf", bj_get_time());
        }

        bj_end(0);
    }

    return 0;
}
