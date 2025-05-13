#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/system.h>
#include <banjo/time.h>
#include <banjo/video.h>

#include "config.h"

bool bj_init_video(bj_error**);
void bj_dispose_video(bj_error**);
void bj_init_time(void);

bool bj_system_init(
    bj_error** p_error
) {
    if(!bj_init_video(p_error)) {
        return false;
    }

    bj_init_time();

    return true;
}

void bj_system_dispose(
    bj_error** p_error
) {
    bj_dispose_video(p_error);
    bj_info("Disposed system");
}

