#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/system.h>
#include <banjo/time.h>
#include <banjo/video.h>

#include "config.h"

#include <assert.h>

bj_video_layer* s_video = 0;

bj_video_layer* bj_init_video(bj_error**);
void bj_dispose_video(bj_video_layer*, bj_error**);
void bj_init_time(void);

bool bj_begin(
    bj_error** p_error
) {
    assert(s_video == 0);

    s_video = bj_init_video(p_error);
    if(!s_video) {
        return false;
    }

    bj_init_time();

    return true;
}

void bj_end(
    bj_error** p_error
) {
    assert(s_video != 0);

    bj_dispose_video(s_video, p_error);
    bj_info("Disposed system");
}

