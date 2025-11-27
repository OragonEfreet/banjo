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

typedef struct bj_video_layer_data_t {
    NSApplication* app;
} cocoa;

static bj_video_layer* cocoa_init_video(
    bj_error** p_error
) {
    (void)p_error;
    @autoreleasepool {
        NSApplication* app = [NSApplication sharedApplication];
        [app setActivationPolicy:NSApplicationActivationPolicyRegular];

        cocoa* p_cocoa = bj_malloc(sizeof(cocoa));
        p_cocoa->app = app;

        bj_video_layer* p_layer = bj_malloc(sizeof(bj_video_layer));
        p_layer->data                      = p_cocoa;
        p_layer->end                       = 0;
        p_layer->create_window             = 0;
        p_layer->delete_window             = 0;
        p_layer->poll_events               = 0;
        p_layer->get_window_size           = 0;
        p_layer->create_window_framebuffer = 0;
        p_layer->flush_window_framebuffer  = 0;

        bj_info("Cocoa backend initialized");
        return p_layer;
    }
}

bj_video_layer_create_info cocoa_video_layer_info = {
    .name = "cocoa",
    .create = cocoa_init_video,
};


#endif
