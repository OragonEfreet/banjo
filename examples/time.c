#include <banjo/log.h>
#include <banjo/system.h>

int main() {

    if(bj_system_init(0)) {

        for(size_t i = 0 ; i < 10 ; ++i) {
            bj_sleep(300);
            bj_trace("- %lf", bj_get_time());
        }

        bj_system_dispose(0);
    }

    return 0;
}
