#include "config.h"

#if BJ_HAS_FEATURE(COCOA)

#include <banjo/assert.h>
#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/memory.h>
#include <banjo/system.h>
#include <banjo/time.h>
#include <banjo/video.h>
#include <banjo/window.h>

#import <Cocoa/Cocoa.h>

static bj_video_layer* cocoa_init_video(
    bj_error** p_error
) {

    return 0;
}

bj_video_layer_create_info cocoa_video_layer_info = {
    .name = "cocoa",
    .create = cocoa_init_video,
};


#endif
