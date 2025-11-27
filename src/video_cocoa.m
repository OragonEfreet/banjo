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

#include "window_t.h"

#import <Cocoa/Cocoa.h>

typedef struct bj_video_layer_data_t {
    NSApplication* app;
} cocoa;

@interface BanjoView : NSView
@end

@implementation BanjoView
- (BOOL)acceptsFirstResponder {
    return YES;
}
@end

typedef struct {
    struct bj_window_t common;
    NSWindow*          window;
    BanjoView*         view;
} cocoa_window;

static bj_window* cocoa_create_window(
    bj_video_layer* p_video,
    const char* p_title,
    uint16_t x,
    uint16_t y,
    uint16_t width,
    uint16_t height,
    uint8_t  flags
) {
    @autoreleasepool {
        /* cocoa* p_cocoa = p_video->data; */

        // Flip Y coordinate (Cocoa uses bottom-left origin)
        NSScreen* screen = [NSScreen mainScreen];
        NSRect screenFrame = [screen frame];
        CGFloat flippedY = screenFrame.size.height - y - height;

        // Create window
        NSRect contentRect = NSMakeRect(x, flippedY, width, height);
        NSUInteger styleMask = NSWindowStyleMaskTitled
                             | NSWindowStyleMaskClosable
                             | NSWindowStyleMaskMiniaturizable;

        NSWindow* nsWindow = [[NSWindow alloc]
            initWithContentRect:contentRect
                      styleMask:styleMask
                        backing:NSBackingStoreBuffered
                          defer:NO];

        [nsWindow setTitle:[NSString stringWithUTF8String:p_title]];

        BanjoView* view = [[BanjoView alloc] initWithFrame:contentRect];
        [nsWindow setContentView:view];

        cocoa_window* window = bj_malloc(sizeof(cocoa_window));
        window->common.flags = flags;
        window->window       = nsWindow;
        window->view         = view;

        [nsWindow makeKeyAndOrderFront:nil];

        bj_info("Cocoa window created: %dx%d", width, height);
        return (bj_window*)window;
    }
}

static void cocoa_end_video(
    bj_video_layer* p_video,
    bj_error** p_error
) {
    (void)p_error;

    cocoa* p_cocoa = (cocoa*)p_video->data;
    bj_free(p_cocoa);
    bj_free(p_video);
    bj_info("Cocoa backend terminated");
}

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
        p_layer->end                       = cocoa_end_video;
        p_layer->create_window             = cocoa_create_window;
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
